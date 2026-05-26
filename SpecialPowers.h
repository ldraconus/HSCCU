#ifndef SPECIALPOWERS_H
#define SPECIALPOWERS_H

#include "modifier.h"
#include "powers.h"
#ifndef ISHSC
#include "skilldialog.h"
#include "modifiersdialog.h"
#include "powerdialog.h"
#include "sheet.h"
#endif

class EnduranceReserve: public AllPowers {
public:
    EnduranceReserve(): AllPowers("Endurance Reserve")         { }
    EnduranceReserve(const EnduranceReserve& s): AllPowers(s)  { }
    EnduranceReserve(EnduranceReserve&& s): AllPowers(s)       { }
    EnduranceReserve(const QJsonObject& json): AllPowers(json) { v.mEnd   = json["end"].toInt(0);
                                                                     v.mRec   = json["rec"].toInt(0);
                                                                     v.mLim   = json["lim"].toInt(0);
                                                                     v.mWhat  = json["what"].toString();
                                                                     v.mRestr = json["restr"].toString();
                                                                     v.mSlow  = json["slow"].toInt(0);
                                                                   }
    virtual EnduranceReserve& operator=(const EnduranceReserve& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual EnduranceReserve& operator=(EnduranceReserve&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   END   = createLineEdit(parent, layout, "END?", std::mem_fn(&Power::numeric));
                                                                   REC   = createLineEdit(parent, layout, "REC?", std::mem_fn(&Power::numeric));
                                                                   limit = createComboBox(parent, layout, "Limited Recovery?", { "None", "-2", "-1" + Fraction(3, 4).toString(),
                                                                                                                                 "-1" + Fraction(1, 2).toString(), "-1" + Fraction(1, 4).toString(),
                                                                                                                                 "-1", "-" + Fraction(3, 4).toString(),
                                                                                                                                 "-" + Fraction(1, 2).toString(), "-" + Fraction(1, 4).toString() });
                                                                   what  = createLineEdit(parent, layout, "Limitation?");
                                                                   restr = createLineEdit(parent, layout, "Restricted Use?");
                                                                   slow  = createComboBox(parent, layout, "Slow Recovery?",
                                                                                          { "Normal", "1 Turn", "1 Minute", "5 Minutes", "20 Minutes", "1 Hour", "6 Hours",
                                                                                            "1 Day", "1 Week", "1 Month", "1 Season", "1 Year", "5 Years", "25 Years",
                                                                                            "1 Century" });
                                                                 }
    Fraction lim() override                                      { return (v.mRestr.isEmpty() ? Fraction(0) : Fraction(1, 4)); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   QList<Fraction> limit { { 0, 1 }, { 0, 1 },
                                                                       { 2, { 0, 1 } }, { 1, { 3, 4 } }, { 1, { 1, 2 } }, { 1, { 1, 4 } },
                                                                       { 1, { 0, 1 } },      { 3, 4 },        { 1, 2 },        { 1, 4 }
                                                                   };
                                                                   Fraction flim = (Fraction(1) + limit[v.mLim + 1] + ((v.mSlow > 0) ? Fraction(1) * Fraction(v.mSlow) : Fraction(0)));
                                                                   return (v.mEnd + 2) / 4_cp + (Fraction((2_cp * (v.mRec + 1) / 3).points) / flim).toInt();
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   END->setText(QString("%1").arg(s.mEnd));
                                                                   REC->setText(QString("%1").arg(s.mRec));
                                                                   limit->setCurrentIndex(v.mLim);
                                                                   what->setText(v.mWhat);
                                                                   restr->setText(v.mRestr);
                                                                   slow->setCurrentIndex(v.mSlow);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mEnd   = END->text().toInt();
                                                                   v.mRec   = REC->text().toInt();
                                                                   v.mLim   = limit->currentIndex();
                                                                   v.mWhat  = what->text();
                                                                   v.mRestr = restr->text();
                                                                   v.mSlow  = slow->currentIndex();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["end"]   = v.mEnd;
                                                                   obj["rec"]   = v.mRec;
                                                                   obj["lim"]   = v.mLim;
                                                                   obj["what"]  = v.mWhat;
                                                                   obj["restr"] = v.mRestr;
                                                                   obj["slow"]  = v.mSlow;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     mEnd   = 0;
        int     mRec   = 0;
        int     mLim   = -1;
        QString mWhat  = "";
        QString mRestr = "";
        int     mSlow  = -1;
    } v;

    QLineEdit* END;
    QLineEdit* REC;
    QComboBox* limit;
    QLineEdit* what;
    QLineEdit* restr;
    QComboBox* slow;

    QString optOut(bool showEND) {
        if (v.mEnd < 1 || v.mRec < 1 || (v.mLim > 0 && v.mWhat.isEmpty())) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1 END/%2 REC").arg(v.mEnd).arg(v.mRec) + "Endurance Reserve";
        if (v.mLim > 0) res += "; Limited Recovery (" + v.mWhat + ")";
        if (!v.mRestr.isEmpty()) res += "; Restricted Use (" + v.mRestr + ")";
        QStringList restr { "", "1 Turn", "1 Minute", "5 Minutes", "20 Minutes", "1 Hour", "6 Hours",
                            "1 Day", "1 Week", "1 Month", "1 Season", "1 Year", "5 Years", "25 Years",
                            "1 Century" };
        if (v.mSlow > 0) res += "; Slow Recovery (" + restr[v.mSlow] + ")";
        return res;
    }

    void numeric(int) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
#ifndef ISHSC
        PowerDialog::ref().updateForm();
#endif
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }
};

class Blank: public AllPowers {
public:
    Blank(): AllPowers("Blank Line")                { }
    Blank(const Blank& s): AllPowers(s)             { }
    Blank(Blank&& s): AllPowers(s)                  { }
    Blank(const QJsonObject& json): AllPowers(json) { }

    virtual Blank& operator=(const Blank& s) { if (this != &s) AllPowers::operator=(s); return *this; }
    virtual Blank& operator=(Blank&& s)      {  AllPowers::operator=(s); return *this; }

    Fraction adv() override                        { return Fraction(0); }
    QString  description(bool) override            { return "-"; }
    QString  end() override                        { return "-"; }
    void     form(QWidget*, QVBoxLayout*) override { throw "No Form"; }
    Fraction lim() override                        { return Fraction(0); }
    Points   points(bool) override                 { return 0_cp; }
    void     restore() override                    { }
    void     store() override                      { }
    QJsonObject toJson() const override            { QJsonObject obj = AllPowers::toJson(); return obj; }
};

class IndependantAdvantage: public AllPowers {
private:
    QStringList getPowers() {
        QStringList pow;
#ifndef ISHSC
        auto list = Sheet::ref().character().powersOrEquipment();
        pow << "";
        for (const auto& power: list) {
            if (power->nickname().isEmpty()) pow << power->description();
            else pow << power->nickname();
        }
#endif
        return pow;
    }

#ifndef ISHSC
    shared_ptr<Power> getPower(int idx) {
        auto list = Sheet::ref().character().powersOrEquipment();
        return list[idx - 1];
#else
    shared_ptr<Power> getPower(int) {
        return nullptr;
#endif
    }

#ifdef ISHSC
    int powerLookup(shared_ptr<Power>) {
#else
    int powerLookup(shared_ptr<Power> pow) {
        int idx = 0;
        auto list = Sheet::ref().character().powersOrEquipment();
        for (const auto& power: list) {
            idx++;
            if (power == pow) return idx;
        }
#endif
        return -1;
    }

public:
    IndependantAdvantage(): AllPowers("Independant Advantage")        { }
    IndependantAdvantage(const IndependantAdvantage& s): AllPowers(s) { }
    IndependantAdvantage(IndependantAdvantage&& s): AllPowers(s)      { }
    IndependantAdvantage(const QJsonObject& json): AllPowers(json)    { auto mod = json["mod"].toObject();
                                                                        QString name = mod["name"].toString();
                                                                        v.mMod = Modifiers::ByName(name)->create(json["mod"].toObject());
                                                                        v.mPts = json["pts"].toInt(0);
                                                                        auto power = json["power"].toObject();
                                                                        name = power["name"].toString();
                                                                        v.mPow = Power::FromJson(name, power);
                                                                      }
    virtual IndependantAdvantage& operator=(const IndependantAdvantage& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual IndependantAdvantage& operator=(IndependantAdvantage&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   mod = createPushButton(parent, layout, "Advantage?", std::mem_fn(&Power::clicked));
                                                                   pts = createLineEdit(parent, layout, "Maximum Points Affected?", std::mem_fn(&Power::numeric));
                                                                   pow = createComboBox(parent, layout, "or Power?", getPowers());
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   if (v.mMod == nullptr) return 0_cp;
                                                                   if (v.mMod->isAdder()) return v.mMod->points(Modifier::NoStore);
                                                                   else if (v.mPow != nullptr) {
                                                                       Points active = v.mPow->acting();
                                                                       Points newActive = active * (Fraction(1) + v.mMod->fraction(Modifier::NoStore)).toInt();
                                                                       return newActive - active;
                                                                   }
                                                                   return 0_cp; // get value of power before adv, then after, then return difference
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   mod->setText(s.mMod->description(Power::NoStore));
                                                                   powIdx = powerLookup(s.mPow);
                                                                   pow->setCurrentIndex(powIdx);
                                                                   pts->setText(QString("%1").arg(s.mPts));
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   // mod is stored in the button
                                                                   powIdx = pow->currentIndex();
                                                                   v.mPow = getPower(powIdx);
                                                                   v.mPts = pts->text().toInt();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["mod"]   = v.mMod->toJson();
                                                                   obj["pts"]   = v.mPts;
                                                                   obj["power"] = v.mPow->toJson();
                                                                   return obj;
                                                                 }

private:
    struct vars {
        shared_ptr<Modifier> mMod    = nullptr;
        int                  mPts    = 0;
        shared_ptr<Power>    mPow    = nullptr;
    } v;

    int          powIdx = -1;
    QPushButton* mod;
    QLineEdit*   pts;
    QComboBox*   pow;

    QString optOut(bool showEND) {
        if (v.mMod == nullptr) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += v.mMod->description() + " on ";
        return res;
    }

    void clicked(void) override {
        QPushButton* btn = dynamic_cast<QPushButton*>(sender());
        if (btn == mod) {
#ifndef ISHSC
            ModifiersDialog dlg(ModifiersDialog::Advantage, &Sheet::ref());
            if (dlg.exec() == QDialog::Rejected) return;
            shared_ptr<Modifier> mod = dlg.modifier();
            if (!(mod->type() == ModifierBase::ModifierType::isAdvantage ||
                  (mod->type() == ModifierBase::ModifierType::isBoth && mod->fraction(Modifier::NoStore) >= 0L))) return;
            btn->setText(mod->description());
            v.mMod = mod;
#endif
        }
    }

    void numeric(int) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
#ifndef ISHSC
        PowerDialog::ref().updateForm();
#endif
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }
};

class Luck: public AllPowers {
public:
    Luck(): AllPowers("Luck")                      { }
    Luck(const Luck& s): AllPowers(s)              { }
    Luck(Luck&& s): AllPowers(s)                   { }
    Luck(const QJsonObject& json): AllPowers(json) { v.mDice = json["dice"].toInt(0);
                                                       }
    virtual Luck& operator=(const Luck& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Luck& operator=(Luck&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   dice = createLineEdit(parent, layout, "Dice?", std::mem_fn(&Power::numeric));
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return 5_cp * v.mDice;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dice->setText(QString("%1").arg(s.mDice));
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mDice = dice->text().toInt();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dice"] = v.mDice;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int mDice = 0;
    } v;

    QLineEdit* dice;

    QString optOut(bool showEND) {
        if (v.mDice < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1d6 Luck").arg(v.mDice);
        return res;
    }

    void numeric(int) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
#ifndef ISHSC
        PowerDialog::ref().updateForm();
#endif
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }
};

class Regeneration: public AllPowers {
public:
    Regeneration(): AllPowers("Regeneration")              { }
    Regeneration(const Regeneration& s): AllPowers(s)      { }
    Regeneration(Regeneration&& s): AllPowers(s)           { }
    Regeneration(const QJsonObject& json): AllPowers(json) { v.mBody = json["body"].toInt(0);
                                                                 v.mTime = json["time"].toInt(0);
                                                               }
    virtual Regeneration& operator=(const Regeneration& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Regeneration& operator=(Regeneration&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   body = createLineEdit(parent, layout, "Dice?", std::mem_fn(&Power::numeric));
                                                                   time = createComboBox(parent, layout, "Per?", { "Month", "Week", "Day", "6 Hours", "Hour", "20 Minutes", "Minute",
                                                                                                                   "Hour", "Turn" });
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v.mBody * v.mTime * 2_cp;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   body->setText(QString("%1").arg(s.mBody));
                                                                   time->setCurrentIndex(s.mTime);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mBody = body->text().toInt();
                                                                   v.mTime = time->currentIndex();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["body"] = v.mBody;
                                                                   obj["time"] = v.mTime;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int mBody = 0;
        int mTime = -1;
    } v;

    QLineEdit* body;
    QComboBox* time;

    QString optOut(bool showEND) {
        if (v.mBody < 1 && v.mTime < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        QStringList time { "", "Week", "Day", "6 Hours", "Hour", "20 Minutes", "Minute",
                           "Hour", "Turn" };
        res += QString("%1 BODY per %2 Regeneration").arg(v.mBody).arg(time[v.mTime]);
        return res;
    }

    void numeric(int) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
#ifndef ISHSC
        PowerDialog::ref().updateForm();
#endif
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }
};

class Skill: public AllPowers {
public:
    Skill(): AllPowers("Skill")                     { }
    Skill(const Skill& s): AllPowers(s)             { }
    Skill(Skill&& s): AllPowers(s)                  { }
    Skill(const QJsonObject& json): AllPowers(json) { auto skill = json["skill"].toObject();
                                                          QString name = skill["name"].toString();
                                                          v.mSkill = SkillTalentOrPerk::FromJson(name, skill);
                                                        }
    virtual Skill& operator=(const Skill& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Skill& operator=(Skill&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   skll = createPushButton(parent, layout, "Skill, Talent, or Perk?", std::mem_fn(&Power::clicked));
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   if (v.mSkill) return v.mSkill->points(SkillTalentOrPerk::NoStore);
                                                                   else return 0_cp;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   skll->setText(s.mSkill->description());
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   // clicked stores the skill
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["skill"] = v.mSkill->toJson();
                                                                   return obj;
                                                                 }

    shared_ptr<SkillTalentOrPerk> skill() override { return v.mSkill; }

private:
    struct vars {
        shared_ptr<SkillTalentOrPerk> mSkill = nullptr;
    } v;

    QPushButton* skll;

    QString optOut(bool showEND) {
        if (v.mSkill == nullptr) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += v.mSkill->description();
        return res;
    }

    void clicked(void) override {
#ifndef ISHSC
        QPushButton* btn = dynamic_cast<QPushButton*>(sender());
        SkillDialog dlg(&Sheet::ref());
        if (dlg.exec() == QDialog::Rejected) return;
        shared_ptr<SkillTalentOrPerk> skl = dlg.skilltalentorperk();
        if (!skl->isSkill()) return;
        btn->setText(skl->description());
        v.mSkill = skl;
#endif
    }

    void numeric(int) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
#ifndef ISHSC
        PowerDialog::ref().updateForm();
#endif
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }
};

class TeleportLocation: public AllPowers {
public:
    TeleportLocation(): AllPowers("Teleport Location")         { }
    TeleportLocation(const TeleportLocation& s): AllPowers(s)  { }
    TeleportLocation(TeleportLocation&& s): AllPowers(s)       { }
    TeleportLocation(const QJsonObject& json): AllPowers(json) { v.mFixed = json["fixed"].toInt(0);
                                                                 v.mWhere = json["where"].toString();
                                                               }
    virtual TeleportLocation& operator=(const TeleportLocation& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual TeleportLocation& operator=(TeleportLocation&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   fixed = createCheckBox(parent, layout, "Fixed Location?");
                                                                   where = createLineEdit(parent, layout, "Where?");
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return (v.mFixed ? 1_cp : 5_cp);
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   fixed->setChecked(s.mFixed);
                                                                   where->setText(s.mWhere);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mFixed = fixed->isChecked();
                                                                   v.mWhere = where->text();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["fixed"] = v.mFixed;
                                                                   obj["where"] = v.mWhere;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        bool    mFixed = 0;
        QString mWhere = "";
    } v;

    QCheckBox* fixed;
    QLineEdit* where;

    QString optOut(bool showEND) {
        if (v.mFixed && v.mWhere.isEmpty()) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "Teleport Location " + (v.mFixed ? "(" + v.mWhere + ")" : "(Floating)");
        return res;
    }

    void numeric(int) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
#ifndef ISHSC
        PowerDialog::ref().updateForm();
#endif
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }
};

#endif // AllPowers_H
