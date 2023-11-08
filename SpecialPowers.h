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
    EnduranceReserve(const QJsonObject& json): AllPowers(json) { v._end   = json["end"].toInt(0);
                                                                     v._rec   = json["rec"].toInt(0);
                                                                     v._lim   = json["lim"].toInt(0);
                                                                     v._what  = json["what"].toString();
                                                                     v._restr = json["restr"].toString();
                                                                     v._slow  = json["slow"].toInt(0);
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
    Fraction lim() override                                      { return (v._restr.isEmpty() ? Fraction(0) : Fraction(1, 4)); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   QList<Fraction> limit { { 0, 1 }, { 0, 1 },
                                                                       { 2, { 0, 1 } }, { 1, { 3, 4 } }, { 1, { 1, 2 } }, { 1, { 1, 4 } },
                                                                       { 1, { 0, 1 } },      { 3, 4 },        { 1, 2 },        { 1, 4 }
                                                                   };
                                                                   Fraction flim = (Fraction(1) + limit[v._lim + 1] + ((v._slow > 0) ? Fraction(1) * Fraction(v._slow) : Fraction(0)));
                                                                   return (v._end + 2) / 4_cp + (Fraction((2_cp * (v._rec + 1) / 3).points) / flim).toInt();
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   END->setText(QString("%1").arg(s._end));
                                                                   REC->setText(QString("%1").arg(s._rec));
                                                                   limit->setCurrentIndex(v._lim);
                                                                   what->setText(v._what);
                                                                   restr->setText(v._restr);
                                                                   slow->setCurrentIndex(v._slow);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._end   = END->text().toInt();
                                                                   v._rec   = REC->text().toInt();
                                                                   v._lim   = limit->currentIndex();
                                                                   v._what  = what->text();
                                                                   v._restr = restr->text();
                                                                   v._slow  = slow->currentIndex();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["end"]   = v._end;
                                                                   obj["rec"]   = v._rec;
                                                                   obj["lim"]   = v._lim;
                                                                   obj["what"]  = v._what;
                                                                   obj["restr"] = v._restr;
                                                                   obj["slow"]  = v._slow;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     _end   = 0;
        int     _rec   = 0;
        int     _lim   = -1;
        QString _what  = "";
        QString _restr = "";
        int     _slow  = -1;
    } v;

    QLineEdit* END;
    QLineEdit* REC;
    QComboBox* limit;
    QLineEdit* what;
    QLineEdit* restr;
    QComboBox* slow;

    QString optOut(bool showEND) {
        if (v._end < 1 || v._rec < 1 || (v._lim > 0 && v._what.isEmpty())) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1 END/%2 REC").arg(v._end).arg(v._rec) + "Endurance Reserve";
        if (v._lim > 0) res += "; Limited Recovery (" + v._what + ")";
        if (!v._restr.isEmpty()) res += "; Restricted Use (" + v._restr + ")";
        QStringList restr { "", "1 Turn", "1 Minute", "5 Minutes", "20 Minutes", "1 Hour", "6 Hours",
                            "1 Day", "1 Week", "1 Month", "1 Season", "1 Year", "5 Years", "25 Years",
                            "1 Century" };
        if (v._slow > 0) res += "; Slow Recovery (" + restr[v._slow] + ")";
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
                                                                        v._mod = Modifiers::ByName(name)->create(json["mod"].toObject());
                                                                        v._pts = json["pts"].toInt(0);
                                                                        auto power = json["power"].toObject();
                                                                        name = power["name"].toString();
                                                                        v._pow = Power::FromJson(name, power);
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
                                                                   if (v._mod == nullptr) return 0_cp;
                                                                   if (v._mod->isAdder()) return v._mod->points(Modifier::NoStore);
                                                                   else if (v._pow != nullptr) {
                                                                       Points active = v._pow->acting();
                                                                       Points newActive = active * (Fraction(1) + v._mod->fraction(Modifier::NoStore)).toInt();
                                                                       return newActive - active;
                                                                   }
                                                                   return 0_cp; // get value of power before adv, then after, then return difference
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   mod->setText(s._mod->description(Power::NoStore));
                                                                   powIdx = powerLookup(s._pow);
                                                                   pow->setCurrentIndex(powIdx);
                                                                   pts->setText(QString("%1").arg(s._pts));
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   // mod is stored in the button
                                                                   powIdx = pow->currentIndex();
                                                                   v._pow = getPower(powIdx);
                                                                   v._pts = pts->text().toInt();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["mod"]   = v._mod->toJson();
                                                                   obj["pts"]   = v._pts;
                                                                   obj["power"] = v._pow->toJson();
                                                                   return obj;
                                                                 }

private:
    struct vars {
        shared_ptr<Modifier> _mod    = nullptr;
        int                  _pts    = 0;
        shared_ptr<Power>    _pow    = nullptr;
    } v;

    int          powIdx = -1;
    QPushButton* mod;
    QLineEdit*   pts;
    QComboBox*   pow;

    QString optOut(bool showEND) {
        if (v._mod == nullptr) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += v._mod->description() + " on ";
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
            v._mod = mod;
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
    Luck(const QJsonObject& json): AllPowers(json) { v._dice = json["dice"].toInt(0);
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
                                                                   return 5_cp * v._dice;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dice->setText(QString("%1").arg(s._dice));
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._dice = dice->text().toInt();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dice"] = v._dice;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int _dice = 0;
    } v;

    QLineEdit* dice;

    QString optOut(bool showEND) {
        if (v._dice < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1d6 Luck").arg(v._dice);
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
    Regeneration(const QJsonObject& json): AllPowers(json) { v._body = json["body"].toInt(0);
                                                                 v._time = json["time"].toInt(0);
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
                                                                   return v._body * v._time * 2_cp;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   body->setText(QString("%1").arg(s._body));
                                                                   time->setCurrentIndex(s._time);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._body = body->text().toInt();
                                                                   v._time = time->currentIndex();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["body"] = v._body;
                                                                   obj["time"] = v._time;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int _body = 0;
        int _time = -1;
    } v;

    QLineEdit* body;
    QComboBox* time;

    QString optOut(bool showEND) {
        if (v._body < 1 && v._time < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        QStringList time { "", "Week", "Day", "6 Hours", "Hour", "20 Minutes", "Minute",
                           "Hour", "Turn" };
        res += QString("%1 BODY per %2 Regeneration").arg(v._body).arg(time[v._time]);
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
                                                          v._skill = SkillTalentOrPerk::FromJson(name, skill);
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
                                                                   if (v._skill) return v._skill->points(SkillTalentOrPerk::NoStore);
                                                                   else return 0_cp;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   skll->setText(s._skill->description());
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   // clicked stores the skill
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["skill"] = v._skill->toJson();
                                                                   return obj;
                                                                 }

    shared_ptr<SkillTalentOrPerk> skill() override { return v._skill; }

private:
    struct vars {
        shared_ptr<SkillTalentOrPerk> _skill = nullptr;
    } v;

    QPushButton* skll;

    QString optOut(bool showEND) {
        if (v._skill == nullptr) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += v._skill->description();
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
        v._skill = skl;
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
    TeleportLocation(const QJsonObject& json): AllPowers(json) { v._fixed = json["fixed"].toInt(0);
                                                                 v._where = json["where"].toString();
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
                                                                   return (v._fixed ? 1_cp : 5_cp);
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   fixed->setChecked(s._fixed);
                                                                   where->setText(s._where);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._fixed = fixed->isChecked();
                                                                   v._where = where->text();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["fixed"] = v._fixed;
                                                                   obj["where"] = v._where;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        bool    _fixed = 0;
        QString _where = "";
    } v;

    QCheckBox* fixed;
    QLineEdit* where;

    QString optOut(bool showEND) {
        if (v._fixed && v._where.isEmpty()) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "Teleport Location " + (v._fixed ? "(" + v._where + ")" : "(Floating)");
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
