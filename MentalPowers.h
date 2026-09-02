#ifndef MENTAL_POWERS_H
#define MENTAL_POWERS_H

#include "powers.h"
#include "powerdialog.h"

class MentalBlast: public AllPowers {
public:
    MentalBlast(): AllPowers("Mental Blast")        { }
    MentalBlast(QJsonObject& json): AllPowers(json) { v.mDice = json["dice"].toInt(0);
                                                    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  abbreviation(bool showEND = false) override         { return optOut(showEND, true); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   dice = createLineEdit(parent, layout, "Dice?", std::mem_fn(&Power::numeric));
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points   points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v.mDice * 10_cp; } // NOLINT
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dice->setText(QString("%1").arg(s.mDice));
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mDice = dice->text().toInt();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dice"] = v.mDice;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int mDice = 0;
    } v;

    QLineEdit* dice = nullptr;

    QString optOut(bool showEND, bool abbr = false) {
        if (v.mDice < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1").arg(v.mDice) + (abbr ? "d6 MB" : "d6 Mental Blast");
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

class MentalIllusions: public AllPowers {
public:
    MentalIllusions(): AllPowers("Mental Illusions")    { }
    MentalIllusions(QJsonObject& json): AllPowers(json) { v.mDice   = json["dice"].toInt(0);
                                                          v.mHarm   = json["harm"].toBool(false);
                                                          v.mDepend = json["depend"].toBool(false);
                                                          v.mSense  = json["sense"].toInt(0);
                                                          v.mSelf   = json["self"].toBool(false);
                                                        }

    Fraction adv() override                                      { return Fraction(0); }
    QString  abbreviation(bool showEND = false) override         { return optOut(showEND, true); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   dice   = createLineEdit(parent, layout, "Dice?", std::mem_fn(&Power::numeric));
                                                                   harm   = createCheckBox(parent, layout, "Cannot Cause Harm");
                                                                   depend = createCheckBox(parent, layout, "Depends On Character's Knowledge");
                                                                   sense  = createComboBox(parent, layout, "Senses Not Affected?", { "No Senses",
                                                                                                                                     "Non-Targeting Senses",
                                                                                                                                     "Targeting Senses" });
                                                                   self   = createCheckBox(parent, layout, "Self Only");
                                                                 }
    Fraction lim() override                                      { return (v.mHarm   ? Fraction(1, 4) : Fraction(0)) +
                                                                          (v.mDepend ? Fraction(1, 2) : Fraction(0)) +
                                                                          (v.mSelf   ? Fraction(1)    : Fraction(0)) +
                                                                          v.mSense * Fraction(1, 4); }
    Points   points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v.mDice * 5_cp; } // NOLINT
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dice->setText(QString("%1").arg(s.mDice));
                                                                   harm->setChecked(s.mHarm);
                                                                   depend->setChecked(s.mDepend);
                                                                   sense->setCurrentIndex(s.mSense);
                                                                   self->setChecked(s.mSelf);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mDice   = dice->text().toInt();
                                                                   v.mHarm   = harm->isChecked();
                                                                   v.mDepend = depend->isChecked();
                                                                   v.mSense  = sense->currentIndex();
                                                                   v.mSelf   = self->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dice"]   = v.mDice;
                                                                   obj["harm"]   = v.mHarm;
                                                                   obj["depend"] = v.mDepend;
                                                                   obj["sense"]  = v.mSense;
                                                                   obj["self"]   = v.mSelf;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int  mDice   = 0;
        bool mHarm   = false;
        bool mDepend = false;
        int  mSense  = -1;
        bool mSelf = false;
    } v;

    QLineEdit* dice = nullptr;
    QCheckBox* harm = nullptr;
    QCheckBox* depend = nullptr;
    QComboBox* sense = nullptr;
    QCheckBox* self = nullptr;

    QString optOut(bool showEND, bool abbr = false) {
        if (v.mDice < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1").arg(v.mDice) + (abbr ? "d6 M. Illus." : "d6 Mental Illiusions");
        if (v.mHarm) res += abbr ? "; Can't Harm" : "; Cannot Cause Harm";
        if (v.mDepend) res += abbr ? "; Dep. Char. Know." : "; Depends On Character's Knowledge";
        if (v.mSense >= 1) {
            if (v.mSense == 1) res += abbr ? "No Non-Target Senses" : "; Doesn't Affect Non-Targeting Senses";
            else res += abbr ? "No Target Senses" : "; Doesnt Affect Targeting Senses";
        }
        if (v.mSelf) res += "; Self Only";
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

class MindControl: public AllPowers {
public:
    MindControl(): AllPowers("Mind Control")        { }
    MindControl(QJsonObject& json): AllPowers(json) { v.mDice = json["dice"].toInt(0);
                                                      v.mTele = json["tele"].toBool(false);
                                                      v.mLit  = json["lit"].toBool(false);
                                                      v.mSet  = json["set"].toInt(0);
                                                      v.mCmd  = json["cmd"].toString();
                                                    }

    Fraction adv() override                                      { return v.mTele ? Fraction(1, 4) : Fraction(0); }
    QString  abbreviation(bool showEND = false) override         { return optOut(showEND, true); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   dice = createLineEdit(parent, layout, "Dice?", std::mem_fn(&Power::numeric));
                                                                   tele = createCheckBox(parent, layout, "Telepathic");
                                                                   lit  = createCheckBox(parent, layout, "Literal Interpretation");
                                                                   set  = createComboBox(parent, layout, "Set Effect?", { "Unrestricted",
                                                                                                                          "Restricted",
                                                                                                                          "Very Restricted",
                                                                                                                          "Completely Restricted" });
                                                                   cmd  = createLineEdit(parent, layout, "Command?");
                                                                 }
    Fraction lim() override                                      { return (v.mLit ? Fraction(1, 4) : Fraction(0)) +
                                                                          ((v.mSet >= 1) ? Fraction(1, 4) : Fraction(0)) +
                                                                          v.mSet * Fraction(1, 4); }
    Points   points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v.mDice * 5_cp; } // NOLINT
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dice->setText(QString("%1").arg(s.mDice));
                                                                   tele->setChecked(s.mTele);
                                                                   lit->setChecked(s.mLit);
                                                                   set->setCurrentIndex(s.mSet);
                                                                   cmd->setText(s.mCmd);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mDice = dice->text().toInt();
                                                                   v.mTele = tele->isChecked();
                                                                   v.mLit  = lit->isChecked();
                                                                   v.mSet  = set->currentIndex();
                                                                   v.mCmd  = cmd->text();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dice"] = v.mDice;
                                                                   obj["tele"] = v.mTele;
                                                                   obj["lit"]  = v.mLit;
                                                                   obj["set"]  = v.mSet;
                                                                   obj["cmd"]  = v.mCmd;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     mDice = 0;
        bool    mTele = false;
        bool    mLit  = false;
        int     mSet  = -1;
        QString mCmd  = "";
    } v;

    QLineEdit* dice = nullptr;
    QCheckBox* tele = nullptr;
    QCheckBox* lit = nullptr;
    QComboBox* set = nullptr;
    QLineEdit* cmd = nullptr;

    QString optOut(bool showEND, bool abbr = false) {
        if (v.mDice < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1").arg(v.mDice) + "d6 Mind Control";
        if (v.mTele) res += abbr ? "Telepath." : "; Telepathic";
        if (v.mLit) res += abbr ? "Literal" : "; Literal Interpretation";
        if (v.mSet != 0) res += "Set Effect (" + v.mCmd + ")";
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

class MindLink: public AllPowers {
public:
    MindLink(): AllPowers("Mind Link")           { }
    MindLink(QJsonObject& json): AllPowers(json) { v.mMinds = json["minds"].toInt(0);
                                                   v.mInv   = json["inv"].toInt(0);
                                                   v.mWho   = json["who"].toString();
                                                   v.mRange = json["range"].toInt(0);
                                                   v.mLOS   = json["los"].toBool(false);
                                                   v.mBond  = json["bond"].toString();
                                                   v.mFloat = json["float"].toBool(false);
                                                   v.mFeed  = json["feed"].toInt(0);
                                                   v.mOnly  = json["only"].toBool(false);
                                                 }

    Fraction adv() override                                      { return Fraction(0); }
    QString  abbreviation(bool showEND = false) override         { return optOut(showEND, true); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   minds    = createLineEdit(parent, layout, "Minds?", std::mem_fn(&Power::numeric));
                                                                   inv      = createComboBox(parent, layout, "Minds Involved?", { "A Single Mind",
                                                                                                                                  "A Group of Minds",
                                                                                                                                  "Any Mind" });
                                                                   who      = createLineEdit(parent, layout, "Who?");
                                                                   range    = createComboBox(parent, layout, "Range?", { "Planetary",
                                                                                                                         "Unlimited",
                                                                                                                         "Unlimited, pan-dimensional" });
                                                                   los      = createCheckBox(parent, layout, "No LOS Needed");
                                                                   bond     = createLineEdit(parent, layout, "Psychic Bond(s)");
                                                                   floating = createCheckBox(parent, layout, "Floating Psychic Bond");
                                                                   feed     = createComboBox(parent, layout, "Feedback?", { "None", "STUN", "STUN and BODY" });
                                                                   only     = createCheckBox(parent, layout, "Only With Others Who Have Mind Link");
                                                                 }
    Fraction lim() override                                      { return Fraction(v.mFeed < 0 ? 0 : v.mFeed) + (v.mOnly ? Fraction(1) : Fraction(0)); }
    Points   points(bool noStore = false) override               { if (!noStore) store();
                                                                   Points pts = 5_cp * (v.mInv < 0 ? 0 : v.mInv); // NOLINT
                                                                   int steps = 0;
                                                                   if (v.mMinds > 0) steps = (int) (log((double) v.mMinds) / log(2.0)); // NOLINT
                                                                   pts = pts + steps * 5_cp; // NOLINT
                                                                   pts = pts + (v.mLOS ? 10_cp : 0_cp); // NOLINT
                                                                   pts = pts + (v.mRange < 0 ? 0 : v.mRange) * 5; // NOLINT
                                                                   pts = pts + countCommas(v.mBond) * 5_cp; // NOLINT
                                                                   pts = pts + (v.mFloat ? 10_cp : 0_cp); // NOLINT
                                                                   return pts;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   minds->setText(QString("%1").arg(s.mMinds));
                                                                   los->setChecked(s.mLOS);
                                                                   bond->setText(s.mBond);
                                                                   inv->setCurrentIndex(s.mInv);
                                                                   who->setText(s.mWho);
                                                                   range->setCurrentIndex(s.mRange);
                                                                   floating->setChecked(s.mFloat);
                                                                   feed->setCurrentIndex(s.mFeed);
                                                                   only->setChecked(s.mOnly);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mMinds = minds->text().toInt();
                                                                   v.mLOS   = los->isChecked();
                                                                   v.mBond  = bond->text();
                                                                   v.mInv   = inv->currentIndex();
                                                                   v.mWho   = who->text();
                                                                   v.mRange = range->currentIndex();
                                                                   v.mFloat = floating->isChecked();
                                                                   v.mFeed  = feed->currentIndex();
                                                                   v.mOnly  = only->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["minds"] = v.mMinds;
                                                                   obj["inv"]   = v.mInv;
                                                                   obj["who"]   = v.mWho;
                                                                   obj["los"]   = v.mLOS;
                                                                   obj["bond"]  = v.mBond;
                                                                   obj["range"] = v.mRange;
                                                                   obj["float"] = v.mFloat;
                                                                   obj["feed"]  = v.mFeed;
                                                                   obj["only"]  = v.mOnly;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     mMinds = 0;
        int     mInv   = -1;
        QString mWho   = "";
        int     mRange = -1;
        bool    mLOS   = false;
        QString mBond  = "";
        bool    mFloat = false;
        int     mFeed  = -1;
        bool    mOnly  = false;
    } v;

    QLineEdit* minds = nullptr;
    QComboBox* inv = nullptr;
    QLineEdit* who = nullptr;
    QComboBox* range = nullptr;
    QCheckBox* los = nullptr;
    QLineEdit* bond = nullptr;
    QCheckBox* floating = nullptr;
    QComboBox* feed = nullptr;
    QCheckBox* only = nullptr;

    QString optOut(bool showEND, bool abbr = false) {
        if (v.mMinds < 1 || v.mInv < 0 || (v.mInv != 2 && v.mWho.isEmpty()) || v.mRange < 0) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += abbr ? "Mind Link w/" : "Mind Link with ";
        switch (v.mInv) {
        case 0: res += abbr ? v.mWho : ("A Single Mind (" + v.mWho + ")");     break;
        case 1: res += abbr ? v.mWho : ("A Group of Minds (" + v.mWho + ")"); break;
        case 2: res += "Any Mind";                          break;
        }
        switch (v.mRange) {
        case 0: res += abbr ? "; Planetary Rng" : "; Planetary Range";                         break;
        case 1: res += abbr ? "; Unlim. Rng" : "; Unlimited Range";                            break;
        case 2: res += abbr ? "; Unlim. Rng, Pan-Dim." : "; Unlimited Range, Pan-Dimensional"; break;
        }
        if (v.mLOS) res += "; No LOS" + QString(abbr ? "" : " Needed");
        if (!v.mBond.isEmpty()) res += (abbr ? "; Bond w/" : "; Psychic Bond with ") + v.mBond;
        if (v.mFloat) res += res += abbr ? "; Float. Bond" : "; Floating Psychic Bond";
        switch (v.mFeed) {
        case 1: res += abbr ? "; Channel STUN Dmg" : "; Link Channels STUN Damage";                 break;
        case 2: res += abbr ? "; Channel STUN & BODY Dmg" : "; Link Channels STUN and BODY Damage"; break;
        }
        if (v.mOnly) res += abbr ? "; Only w/Others w/Mind Link" : "; Only With Others Who Have Mind Link";
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

class MindScan: public AllPowers {
public:
    MindScan(): AllPowers("Mind Scan")           { }
    MindScan(QJsonObject& json): AllPowers(json) { v.mDice  = json["dice"].toInt(0);
                                                   v.mLink  = json["link"].toBool(false);
                                                   v.mLock  = json["lit"].toBool(false);
                                                   v.mBonus = json["set"].toInt(0);
                                                   v.mCant  = json["cant"].toInt(0);
                                                   v.mPart  = json["part"].toBool(false);
                                                 }

    Fraction adv() override                                      { return (v.mLink ? Fraction(1)    : Fraction(0)) +
                                                                          (v.mLock ? Fraction(1, 2) : Fraction(0)); }
    QString  abbreviation(bool showEND = false) override         { return optOut(showEND, true); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   dice  = createLineEdit(parent, layout, "Dice?", std::mem_fn(&Power::numeric));
                                                                   link  = createCheckBox(parent, layout, "One Way Link");
                                                                   lock  = createCheckBox(parent, layout, "Partial Lock-On");
                                                                   bonus = createLineEdit(parent, layout, "Bonus OMCV?", std::mem_fn(&Power::numeric));
                                                                   cant  = createComboBox(parent, layout, "Cannot Attack Through Link", { "Attacks Possible",
                                                                                                                                          "No Attack",
                                                                                                                                          "No Attack or Communication",
                                                                                                                                          "Only Target Can Attack" });
                                                                   part  = createCheckBox(parent, layout, "Partial Effect");
                                                                 }
    Fraction lim() override                                      { return ((v.mCant > 0) ? v.mCant * Fraction(1, 2) : Fraction(0)) + (v.mPart ? Fraction(1, 2) : Fraction(0)); }
    Points   points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v.mDice * 5_cp + v.mBonus * 2_cp; } // NOLINT
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dice->setText(QString("%1").arg(s.mDice));
                                                                   link->setChecked(s.mLink);
                                                                   lock->setChecked(s.mLock);
                                                                   bonus->setText(QString("%1").arg(s.mBonus));
                                                                   cant->setCurrentIndex(s.mCant);
                                                                   part->setChecked(s.mPart);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mDice  = dice->text().toInt();
                                                                   v.mLink  = link->isChecked();
                                                                   v.mLock  = lock->isChecked();
                                                                   v.mBonus = bonus->text().toInt();
                                                                   v.mCant  = cant->currentIndex();
                                                                   v.mPart  = part->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dice"]  = v.mDice;
                                                                   obj["tele"]  = v.mLink;
                                                                   obj["lock"]  = v.mLock;
                                                                   obj["bonus"] = v.mBonus;
                                                                   obj["cant"]  = v.mCant;
                                                                   obj["part"]  = v.mPart;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int  mDice  = 0;
        bool mLink  = false;
        bool mLock  = false;
        int  mBonus = 0;
        int  mCant  = -1;
        bool mPart  = false;
    } v;

    QLineEdit* dice = nullptr;
    QCheckBox* link = nullptr;
    QCheckBox* lock = nullptr;
    QLineEdit* bonus = nullptr;
    QComboBox* cant = nullptr;
    QCheckBox* part = nullptr;

    QString optOut(bool showEND, bool abbr = false) {
        if (v.mDice < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1").arg(v.mDice) + "d6 Mind Scan";
        if (v.mBonus > 0) res += QString(", +%1 OMCV").arg(v.mBonus);
        if (v.mLink) res += abbr ? "; 1-Way Link" : "; One Way Link";
        if (v.mLock) res += abbr ? "; Part. Lock." :"; Partial Lock-On";
        switch (v.mCant) {
        case 1: res += abbr ? "; No Atk Thru" : "; No Attacks Through Link";                          break;
        case 2: res += abbr ? "; No Atk or Comm. Thru" : "; No Attack or Communication Through Link"; break;
        case 3: res += abbr ? "; Only Tgt can Atk Thru" : "; Only Traget May Attack Through Link";    break;
        }
        if (v.mPart) res += abbr ? "; Part. Eff." : "; Partial Effect";
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

class Telepathy: public AllPowers {
public:
    Telepathy(): AllPowers("Telepathy")           { }
    Telepathy(QJsonObject& json): AllPowers(json) { v.mDice  = json["dice"].toInt(0);
                                                    v.mBCast = json["bcast"].toBool(false);
                                                    v.mExcl  = json["excl"].toBool(false);
                                                    v.mEmp   = json["emp"].toInt(0);
                                                    v.mEmo   = json["emo"].toString();
                                                    v.mFeed  = json["feed"].toInt(0);
                                                    v.mLang  = json["lang"].toBool(false);
                                                    v.mRecv  = json["recv"].toBool(false);
                                                  }

    Fraction adv() override                                      { return Fraction(0); }
    QString  abbreviation(bool showEND = false) override         { return optOut(showEND, true); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   dice  = createLineEdit(parent, layout, "Dice?", std::mem_fn(&Power::numeric));
                                                                   bcast = createCheckBox(parent, layout, "Broadcast Only");
                                                                   excl  = createCheckBox(parent, layout, "Communication Only");
                                                                   emp   = createComboBox(parent, layout, "Empathy?", { "Read Thoughts",
                                                                                                                        "Only Read Emptions",
                                                                                                                        "Only Read An Emotion" });
                                                                   emo   = createLineEdit(parent, layout, "Command?");
                                                                   feed  = createComboBox(parent, layout, "Feedback?", { "None",
                                                                                                                         "STUN Only",
                                                                                                                         "STUN and BODY" });
                                                                   lang  = createCheckBox(parent, layout, "Language Barrier");
                                                                   recv  = createCheckBox(parent, layout, "Receive Only");
                                                                 }
    Fraction lim() override                                      { return (v.mBCast ? Fraction(1, 2) : Fraction(0)) +
                                                                          (v.mExcl ? Fraction(1, 4) : Fraction(0)) +
                                                                          (v.mEmp + 1) * Fraction(1, 2) +
                                                                          (v.mFeed + 1) * Fraction(1) +
                                                                          (v.mLang ? Fraction(1, 2) : Fraction(0)) +
                                                                          (v.mRecv ? Fraction(1, 2) : Fraction(0)); }
    Points   points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v.mDice * 5_cp; } // NOLINT
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dice->setText(QString("%1").arg(s.mDice));
                                                                   bcast->setChecked(s.mBCast);
                                                                   excl->setChecked(s.mExcl);
                                                                   emp->setCurrentIndex(s.mEmp);
                                                                   emo->setText(s.mEmo);
                                                                   feed->setCurrentIndex(s.mFeed);
                                                                   lang->setChecked(s.mLang);
                                                                   recv->setChecked(s.mRecv);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mDice  = dice->text().toInt();
                                                                   v.mBCast = bcast->isChecked();
                                                                   v.mExcl  = excl->isChecked();
                                                                   v.mEmp   = emp->currentIndex();
                                                                   v.mEmo   = emo->text();
                                                                   v.mFeed  = feed->currentIndex();
                                                                   v.mLang  = lang->isChecked();
                                                                   v.mRecv  = recv->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dice"]  = v.mDice;
                                                                   obj["bcast"] = v.mBCast;
                                                                   obj["excl"]  = v.mExcl;
                                                                   obj["emp"]   = v.mEmp;
                                                                   obj["emo"]   = v.mEmo;
                                                                   obj["feed"]  = v.mFeed;
                                                                   obj["lang"]  = v.mLang;
                                                                   obj["recv"]  = v.mRecv;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     mDice  = 0;
        bool    mBCast = false;
        bool    mExcl  = false;
        int     mEmp   = -1;
        QString mEmo   = "";
        int     mFeed  = -1;
        bool    mLang  = false;
        bool    mRecv  = false;
    } v;

    QLineEdit* dice = nullptr;
    QCheckBox* bcast = nullptr;
    QCheckBox* excl = nullptr;
    QComboBox* emp = nullptr;
    QLineEdit* emo = nullptr;
    QComboBox* feed = nullptr;
    QCheckBox* lang = nullptr;
    QCheckBox* recv = nullptr;

    QString optOut(bool showEND, bool abbr = false) {
        if (v.mDice < 1 || (v.mEmp == 1 && v.mEmo.isEmpty())) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1").arg(v.mDice) + "d6 Telepathy";
        if (v.mBCast) res += abbr ? "; Brdcast Only" : "; Broadcast Only";
        if (v.mExcl) res += abbr ? "; Comm. Only" : "; Communication Only";
        if (v.mEmp == 1) res += "; Empathy";
        else if (v.mEmp == 2) res += abbr ? "; " + v.mEmo : ("Empathy (" + v.mEmo + " Only)");
        if (v.mFeed == 1) res += "; Feedback (STUN)";
        else if (v.mFeed == 2) res += "; Feedback (STUN & BODY)";
        if (v.mLang) res += abbr ? "; Lang. Bar." : "; Language Barrier";
        if (v.mRecv) res += abbr ? "; Rcv Only" : "; Receive Only";
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
