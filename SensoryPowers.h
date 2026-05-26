#ifndef SENSORYPOWERS_H
#define SENSORYPOWERS_H

#include "powers.h"
#include "powerdialog.h"

class Clairsentience: public AllPowers {
public:
    Clairsentience(): AllPowers("Clairsentienceϴ")           { }
    Clairsentience(const Clairsentience& s): AllPowers(s)    { }
    Clairsentience(Clairsentience&& s): AllPowers(s)         { }
    Clairsentience(const QJsonObject& json): AllPowers(json) { v.mWhat   = toStringList(json["what"].toArray());
                                                                   v.mPre    = json["pre"].toBool(false);
                                                                   v.mRetro  = json["retro"].toBool(false);
                                                                   v.mMult   = json["mult"].toInt(0);
                                                                   v.mMobile = json["mobile"].toInt(0);
                                                                   v.mAttack = json["attack"].toBool(false);
                                                                   v.mBlack  = json["black"].toBool(false);
                                                                   v.mFixed  = json["fixed"].toBool(false);
                                                                   v.mOnly   = json["only"].toInt(0);
                                                                   v.mCrit   = json["crit"].toString();
                                                                   v.mOne    = json["one"].toBool(false);
                                                                   v.mPorr   = json["porr"].toBool(false);
                                                                   v.mDreams = json["dreams"].toBool(false);
                                                                   v.mTime   = json["time"].toBool(false);
                                                                   v.mVague  = json["vague"].toBool(false);
                                                                 }
    virtual Clairsentience& operator=(const Clairsentience& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Clairsentience& operator=(Clairsentience&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* p, QVBoxLayout* l) override           { AllPowers::form(p, l);
                                                                   what    = createTreeWidget(p, l, { { "Hearing",     { "Normal Hearing",
                                                                                                                         "Active Sonar",
                                                                                                                         "Ultrasonic Perception" } },
                                                                                                      { "Mental",      { "Mental Awareness",
                                                                                                                         "Mind Scan" } },
                                                                                                      { "Radio",       { "Radio Perception",
                                                                                                                         "Radar"} },
                                                                                                      { "Sight",       { "Normal Sight",
                                                                                                                         "Nightvision",
                                                                                                                         "Infrared Pereception",
                                                                                                                         "Ultraviolet Perception" } },
                                                                                                      { "Smell/Taste", { "Normal Smell",
                                                                                                                         "Normal Taste" } },
                                                                                                      { "Touch",       { "Normal Touch" } } });
                                                                   pre    = createCheckBox(p, l, "Precognition");
                                                                   retro  = createCheckBox(p, l, "Retrocognition");
                                                                   mult   = createLineEdit(p, l, "Range Muiltiplier?", std::mem_fn(&Power::numeric));
                                                                   mobile = createLineEdit(p, l, "Mobile Movement Meters?", std::mem_fn(&Power::numeric));
                                                                   attack = createCheckBox(p, l, "Attack Roll Required");
                                                                   black  = createCheckBox(p, l, "Blackout");
                                                                   fixed  = createCheckBox(p, l, "Fixed Perception Point");
                                                                   only   = createComboBox(p, l, "Only Through The Sense Of?", { "",
                                                                                                                                 "Any Creature",
                                                                                                                                 "Limited Group Of Creatures",
                                                                                                                                 "A Single Creature" });
                                                                   crit   = createLineEdit(p, l, "Creature(s)?");
                                                                   one    = createCheckBox(p, l, "One Sense Only");
                                                                   porr   = createCheckBox(p, l, "Precognition or Retrocognition Only");
                                                                   dreams = createCheckBox(p, l, "Only Through Dreams");
                                                                   time   = createCheckBox(p, l, "Time Modifiers");
                                                                   vague  = createCheckBox(p, l, "Vague And Unclear");
                                                                 }
    Fraction lim() override                                      { return (v.mAttack      ? Fraction(1, 4)                            : Fraction(0)) +
                                                                          (v.mBlack       ? Fraction(1, 2)                            : Fraction(0)) +
                                                                          (v.mFixed       ? Fraction(1)                               : Fraction(0)) +
                                                                          ((v.mOnly > 0)  ? Fraction(1, 4) + Fraction(1, 4) * v.mOnly : Fraction(0)) +
                                                                          (v.mOne         ? Fraction(1, 4)                            : Fraction(0)) +
                                                                          (v.mPorr        ? Fraction(1)                               : Fraction(0)) +
                                                                          (v.mDreams      ? Fraction(1)                               : Fraction(0)) +
                                                                          (v.mTime        ? Fraction(1, 2)                            : Fraction(0)) +
                                                                          (v.mVague       ? Fraction(1, 2)                            : Fraction(0));
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   setTreeWidget(what, s.mWhat);
                                                                   pre->setChecked(s.mPre);
                                                                   retro->setChecked(s.mRetro);
                                                                   mult->setText(QString("%1").arg(s.mMult));
                                                                   mobile->setText(QString("%1").arg(s.mMobile));
                                                                   attack->setChecked(s.mAttack);
                                                                   black->setChecked(s.mBlack);
                                                                   fixed->setChecked(s.mFixed);
                                                                   only->setCurrentIndex(s.mOnly);
                                                                   crit->setText(s.mCrit);
                                                                   one->setChecked(s.mOne);
                                                                   porr->setChecked(s.mPorr);
                                                                   dreams->setChecked(s.mDreams);
                                                                   time->setChecked(s.mTime);
                                                                   vague->setChecked(s.mVague);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mWhat   = treeWidget(what);
                                                                   v.mPre    = pre->isChecked();
                                                                   v.mRetro  = retro->isChecked();
                                                                   v.mMult   = mult->text().toInt();
                                                                   v.mMobile = mobile->text().toInt();
                                                                   v.mAttack = attack->isChecked();
                                                                   v.mBlack  = black->isChecked();
                                                                   v.mFixed  = fixed->isChecked();
                                                                   v.mOnly   = only->currentIndex();
                                                                   v.mCrit   = crit->text();
                                                                   v.mOne    = one->isChecked();
                                                                   v.mPorr   = porr->isChecked();
                                                                   v.mDreams = dreams->isChecked();
                                                                   v.mTime   = time->isChecked();
                                                                   v.mVague  = vague->isChecked();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["what"]   = toArray(v.mWhat);
                                                                   obj["pre"]    = v.mPre;
                                                                   obj["retro"]  = v.mRetro;
                                                                   obj["mult"]   = v.mMult;
                                                                   obj["mobile"] = v.mMobile;
                                                                   obj["attack"] = v.mAttack;
                                                                   obj["black"]  = v.mBlack;
                                                                   obj["fixed"]  = v.mFixed;
                                                                   obj["only"]   = v.mOnly;
                                                                   obj["crit"]   = v.mCrit;
                                                                   obj["one"]    = v.mOne;
                                                                   obj["porr"]   = v.mPorr;
                                                                   obj["dreams"] = v.mDreams;
                                                                   obj["time"]   = v.mTime;
                                                                   obj["vague"]  = v.mVague;
                                                                   return obj;
                                                                 }

private:
    Points points(bool noStore = false) override {
        if (!noStore) store();
        Points p(0);
        QStringList groups = { "Hearing", "Mental", "Radio", "Sight", "Smell/Taste", "Touch" };
        bool first = true;
        for (int i = 0; i< v.mWhat.count(); ++i) {
            auto& str = v.mWhat[i];
            if (groups.contains(str)) {
                if (first) {
                    first = false;
                    p += 20_cp;
                } else p += 10_cp;
            } else p += 5_cp;
        }

        int mult = (int) (log((double) v.mMobile / 12) / log(2.0));
        if (v.mMobile > 0 && mult < 1) mult = 1;
        if (mult < 0) mult = 0;
        return p + ((v.mPre || v.mRetro) ? 20_cp : 0_cp) + mult * 5_cp;
    }

    struct vars {
        QStringList mWhat   { };
        bool        mPre    = false;
        bool        mRetro  = false;
        int         mMult   = 0;
        int         mMobile = 0;
        bool        mAttack = false;
        bool        mBlack  = false;
        bool        mFixed  = false;
        int         mOnly   = -1;
        QString     mCrit   = "";
        bool        mOne    = false;
        bool        mPorr   = false;
        bool        mDreams = false;
        bool        mTime   = false;
        bool        mVague  = false;
    } v;

    QTreeWidget* what;
    QCheckBox*   pre;
    QCheckBox*   retro;
    QLineEdit*   mult;
    QLineEdit*   mobile;
    QCheckBox*   attack;
    QCheckBox*   black;
    QCheckBox*   fixed;
    QComboBox*   only;
    QLineEdit*   crit;
    QCheckBox*   one;
    QCheckBox*   porr;
    QCheckBox*   dreams;
    QCheckBox*   time;
    QCheckBox*   vague;

    QString optOut(bool showEND) {
        QStringList groups = { "Hearing", "Mental", "Radio", "Sight", "Smell/Taste", "Touch" };
        bool group = false;
        for (int i = 0; i< v.mWhat.count(); ++i) {
            auto& str = v.mWhat[i];
            if (groups.contains(str)) {
                group = true; break;
            }
        }
        if (v.mWhat.size() < 1 || (v.mOne && group) || (v.mOne && v.mWhat.size() > 1) || !group || (v.mOnly > 0 && v.mCrit.isEmpty())) return "<incomplete>";
        if (v.mPorr && (v.mMult > 0 || !(v.mPre || v.mRetro))) return "<incomplete>";
        if (v.mDreams && !(v.mPre || v.mRetro)) return "<incomplete>";
        if (v.mTime && !(v.mPre || v.mRetro)) return "<incomplete>";
        if (v.mVague && !(v.mPre || v.mRetro)) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "Clairsentienceϴ with ";
        res += v.mWhat[0];
        int len = v.mWhat.length();
        for (int i = 1; i < len; ++i) {
            if (i != len - 1) res += ", ";
            else res += ", and ";
            res += v.mWhat[i];
        }
        if (v.mPre) res += "; Precognition";
        if (v.mRetro) res += "; Retrocognition";
        if (v.mMult > 0) res += QString("; x%1 Range").arg((int) pow(2, v.mMult));
        if (v.mMobile > 0) res += QString("; Mobile Perception Point (%1m)").arg(v.mMobile);
        if (v.mAttack) res += "; Attack Roll Required";
        if (v.mBlack) res += "; Blackout";
        if (v.mFixed) res += "; Fixed Perception Point";
        if (v.mOnly == 1) res += "; Only Through The Senses Of Others (Any)";
        if (v.mOnly == 2) res += "; Only Through The Senses Of Others (" + v.mCrit + ")";
        if (v.mOne) res += "; One Sense Only";
        if (v.mPorr) {
            QString sep = "; ";
            if (v.mPre) { res += sep + "Precognition"; sep = " and "; }
            if (v.mRetro) res += sep + "Retrocognition";
            res += " Only";
        }
        if (v.mDreams) res += "; Only Through Dreams";
        if (v.mTime) res += "; Time Modifiers";
        if (v.mVague) res += "; Vague and Unclear";
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

class EnhancedSenses: public AllPowers {
public:
    EnhancedSenses(): AllPowers("Enhanced Senses")           { }
    EnhancedSenses(const EnhancedSenses& s): AllPowers(s)    { }
    EnhancedSenses(EnhancedSenses&& s): AllPowers(s)         { }
    EnhancedSenses(const QJsonObject& json): AllPowers(json) { v.mWhat   = toStringList(json["what"].toArray());
                                                                   v.mDetect = json["detect"].toInt(0);
                                                                   v.mThing  = json["thing"].toString();
                                                                   v.mEnhanc = json["enhanc"].toInt(0);
                                                                   v.mSenses = json["senses"].toString();
                                                                   v.mAmount = json["amount"].toInt(0);
                                                                   v.mSpatl  = json["spatl"].toBool(false);
                                                                   v.mAdj    = json["adj"].toInt(0);
                                                                   v.mAnlz   = json["anlz"].toBool(false);
                                                                   v.mConc   = json["conc"].toInt(0);
                                                                   v.mDiscr  = json["discr"].toBool(false);
                                                                   v.mDim    = json["dim"].toInt(0);
                                                                   v.mWhich  = json["which"].toString();
                                                                   v.mIncr   = json["incr"].toInt(0);
                                                                   v.mMic    = json["mic"].toInt(0);
                                                                   v.mPen    = json["pen"].toInt(0);
                                                                   v.mRange  = json["range"].toBool(false);
                                                                   v.mRapid  = json["rapid"].toInt(0);
                                                                   v.mSense  = json["sense"].toBool(false);
                                                                   v.mTarget = json["target"].toBool(false);
                                                                   v.mTele   = json["tele"].toInt(0);
                                                                   v.mTrack  = json["track"].toBool(false);
                                                                   v.mTrans  = json["trans"].toBool(false);
                                                                   v.mNoDir  = json["nodir"].toBool(false);
                                                                 }
    virtual EnhancedSenses& operator=(const EnhancedSenses& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual EnhancedSenses& operator=(EnhancedSenses&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* p, QVBoxLayout* l) override           { AllPowers::form(p, l);
                                                                   what    = createTreeWidget(p, l, { { "Hearing",     { "Normal Hearing",
                                                                                                                         "Active Sonar",
                                                                                                                         "Ultrasonic Perception" } },
                                                                                                      { "Mental",      { "Mental Awareness" } },
                                                                                                      { "Radio",       { "Radio Perception",
                                                                                                                         "Radio Listen/Transmit",
                                                                                                                         "High Range Radio",
                                                                                                                         "Radar"} },
                                                                                                      { "Sight",       { "Normal Sight",
                                                                                                                         "Nightvision",
                                                                                                                         "Infrared Pereception",
                                                                                                                         "Ultraviolet Perception" } },
                                                                                                      { "Smell/Taste", { "Normal Smell",
                                                                                                                         "Normal Taste" } },
                                                                                                      { "Touch",       { "Normal Touch" } } });
                                                                   detect = createComboBox(p, l, "Detect?", { "",
                                                                                                              "One Thing",
                                                                                                              "A Class Of Things",
                                                                                                              "A Large Class Of Things" });
                                                                   thing  = createLineEdit(p, l, "Thing(s)?");
                                                                   spatl  = createCheckBox(p, l, "Spatial Awareness");
                                                                   enhanc = createComboBox(p, l, "Enhanced Perception?", { "",
                                                                                                                           "One Sense",
                                                                                                                           "One Sense Group",
                                                                                                                           "All Senses" });
                                                                   senses = createLineEdit(p, l, "Which sense(s)");
                                                                   amount = createLineEdit(p, l, "Plusses?", std::mem_fn(&Power::numeric));
                                                                   adj    = createComboBox(p, l, "Adjacent?", { "",
                                                                                                                "Fixed Point Within 2m",
                                                                                                                "Any Point Withing 2m" });
                                                                   anlz   = createCheckBox(p, l, "Analyize");
                                                                   conc   = createLineEdit(p, l, "Concealment Modifier?", std::mem_fn(&Power::numeric));
                                                                   discr  = createCheckBox(p, l, "Discriminatory");
                                                                   dim    = createComboBox(p, l, "Dimensional?", { "",
                                                                                                                   "Single Other Dimension",
                                                                                                                   "Related Group Of Dimensions",
                                                                                                                   "Any Dimension" });
                                                                   which  = createLineEdit(p, l, "Dimension(s)?");
                                                                   incr   = createComboBox(p, l, "Increased Arc Of Perception?", { "",
                                                                                                                                   "240 Degrees",
                                                                                                                                   "360 Degrees" });
                                                                   mic    = createLineEdit(p, l, "Microscropic x10 Magnification?", std::mem_fn(&Power::numeric));
                                                                   pen    = createComboBox(p, l, "Penetrative?", { "",
                                                                                                                   "Partially",
                                                                                                                   "Fully" });
                                                                   range  = createCheckBox(p, l, "Ranged");
                                                                   rapid  = createLineEdit(p, l, "Rapid Speed x10?", std::mem_fn(&Power::numeric));
                                                                   sense  = createCheckBox(p, l, "Sense");
                                                                   target = createCheckBox(p, l, "Targeting");
                                                                   tele   = createLineEdit(p, l, "Telescopic Modifiers?", std::mem_fn(&Power::numeric));
                                                                   track  = createCheckBox(p, l, "Tracking");
                                                                   trans  = createCheckBox(p, l, "Transmit");
                                                                   nodir  = createCheckBox(p, l, "No Direction");
                                                                 }
    Fraction lim() override                                      { return (v.mNoDir ? Fraction(1, 2) : Fraction(0));
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   setTreeWidget(what, s.mWhat);
                                                                   detect->setCurrentIndex(s.mDetect);
                                                                   thing->setText(v.mThing);
                                                                   enhanc->setCurrentIndex(s.mEnhanc);
                                                                   senses->setText(s.mSenses);
                                                                   amount->setText(QString("%1").arg(s.mAmount));
                                                                   adj->setCurrentIndex(s.mAdj);
                                                                   anlz->setChecked(s.mAnlz);
                                                                   conc->setText(QString("%1").arg(s.mConc));
                                                                   discr->setChecked(s.mDiscr);
                                                                   dim->setCurrentIndex(s.mDim);
                                                                   which->setText(s.mWhich);
                                                                   incr->setCurrentIndex(s.mIncr);
                                                                   mic->setText(QString("%1").arg(s.mMic));
                                                                   pen->setCurrentIndex(s.mPen);
                                                                   range->setChecked(s.mRange);
                                                                   rapid->setText(QString("%1").arg(s.mRapid));
                                                                   sense->setChecked(s.mSense);
                                                                   spatl->setChecked(s.mSpatl);
                                                                   target->setChecked(s.mTarget);
                                                                   tele->setText(QString("%!").arg(s.mTele));
                                                                   track->setChecked(s.mTrack);
                                                                   trans->setChecked(s.mTrans);
                                                                   nodir->setChecked(s.mNoDir);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mWhat   = treeWidget(what);
                                                                   v.mDetect = detect->currentIndex();
                                                                   v.mThing  = thing->text();
                                                                   v.mEnhanc = enhanc->currentIndex();
                                                                   v.mSenses = senses->text();
                                                                   v.mAmount = amount->text().toInt();
                                                                   v.mAdj    = adj->currentIndex();
                                                                   v.mAnlz   = anlz->isChecked();
                                                                   v.mConc   = conc->text().toInt();
                                                                   v.mDiscr  = discr->isChecked();
                                                                   v.mDim    = dim->currentIndex();
                                                                   v.mWhich  = which->text();
                                                                   v.mIncr   = incr->currentIndex();
                                                                   v.mMic    = mic->text().toInt();
                                                                   v.mPen    = pen->currentIndex();
                                                                   v.mRange  = range->isChecked();
                                                                   v.mRapid  = rapid->text().toInt();
                                                                   v.mSense  = sense->isChecked();
                                                                   v.mSpatl  = spatl->isChecked();
                                                                   v.mTarget = target->isChecked();
                                                                   v.mTele   = tele->text().toInt();
                                                                   v.mTrack  = track->isChecked();
                                                                   v.mTrans  = trans->isChecked();
                                                                   v.mNoDir  = nodir->isChecked();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["what"]   = toArray(v.mWhat);
                                                                   obj["detect"] = v.mDetect;
                                                                   obj["thing"]  = v.mThing;
                                                                   obj["enhanc"] = v.mEnhanc;
                                                                   obj["senses"] = v.mSenses;
                                                                   obj["amount"] = v.mAmount;
                                                                   obj["adj"]    = v.mAdj;
                                                                   obj["anlz"]   = v.mAnlz;
                                                                   obj["conc"]   = v.mConc;
                                                                   obj["discr"]  = v.mDiscr;
                                                                   obj["dim"]    = v.mDim;
                                                                   obj["which"]  = v.mWhich;
                                                                   obj["incr"]   = v.mIncr;
                                                                   obj["mic"]    = v.mMic;
                                                                   obj["pen"]    = v.mPen;
                                                                   obj["range"]  = v.mRange;
                                                                   obj["rapid"]  = v.mRapid;
                                                                   obj["sense"]  = v.mSense;
                                                                   obj["spatl"]  = v.mSpatl;
                                                                   obj["target"] = v.mTarget;
                                                                   obj["tele"]   = v.mTele;
                                                                   obj["track"]  = v.mTrack;
                                                                   obj["trans"]  = v.mTrans;
                                                                   obj["nodir"]  = v.mNoDir;
                                                                   return obj;
                                                                 }

private:
    Points points(bool noStore = false) override {
        if (!noStore) store();
        Points p(0);
        int group = 0;
        int groupCount = 0;
        int sense = 0;
        QStringList groups { "Hearing", "Mental", "Radio", "Sight", "Smell/Taste", "Touch" };
        QMap<QString, int> groupCounts { { "Hearing", 3}, { "Mental", 1 }, { "Radio", 2 }, { "Sight", 4 }, { "Smell/Taste", 2 }, { "Touch", 1 }};
        QMap<QString, int> cost { { "Active Sonar",           15 },
                                  { "Infrared Perception",     5 },
                                  { "Mental Awareness",        5 },
                                  { "Nightvision",             5 },
                                  { "Radar",                  15 },
                                  { "Radio Perception",        8 },
                                  { "Radio Listen/Transmit",  10 },
                                  { "High Range Radio",       12 },
                                  { "Untrasonic Perception",   3 },
                                  { "Ultraviolet Perception",  5 }
                                };
        QStringList keys = cost.keys();
        for (int i = 0; i< v.mWhat.count(); ++i) {
            auto& str = v.mWhat[i];
            if (groups.contains(str)) {
                group++;
                groupCount += groupCounts[str];
            }
            else sense++;
            if (keys.contains(str)) p += Points(cost[str]);
        }
        int totalCount = sense + groupCount;

        if (v.mEnhanc > 0) p += 1_cp * v.mEnhanc * v.mAmount;
        if (v.mDetect > 0) p += 3_cp + ((v.mDetect == 2) ? 2_cp : ((v.mDetect == 3) ? 8_cp : 0_cp));
        if (v.mSpatl) p += 32_cp;
        if (v.mAdj > 0) p += v.mAdj * sense + (3_cp + (v.mAdj - 1) * 2_cp) * group + (v.mSpatl ? v.mAdj * 1_cp : 0_cp);
        if (v.mAnlz) p += 5_cp * sense + 10_cp * group + (v.mSpatl ? 5_cp : 0_cp);
        if (v.mConc > 0) p += 1_cp * v.mConc;
        if (v.mDiscr) p += 5_cp * sense + 10_cp * group + (v.mSpatl ? 5_cp : 0_cp);
        if (v.mDim > 0) p += (v.mDim * 5_cp) * sense + ((v.mDim == 1) ? 10_cp : ((v.mDim == 2) ? 20_cp : 25_cp)) * group + (v.mSpatl ? 5_cp : 0_cp);
        if (v.mIncr > 0) {
            if (group == 6) p += (v.mIncr == 1) ? 10_cp : 25_cp;
            else p += (2_cp + (v.mIncr - 1) * 3_cp) * sense + (v.mIncr * 5_cp) * group + (v.mSpatl ? ((sense == 0) ? 2_cp : 0_cp ) + (v.mIncr - 1) * 3_cp : 0_cp);
        }
        if (v.mMic > 0) p += (3_cp * sense + 5_cp * group) * v.mMic;
        if (v.mPen > 0) p += 5_cp * v.mPen * sense + (5_cp + v.mPen * 5) * group + (v.mSpatl ? 5_cp * v.mPen : 0_cp);
        if (v.mRange) p += 5_cp * sense + 10_cp * group + (v.mSpatl ? 5_cp : 0_cp);
        if (v.mRapid > 0) p += 3_cp * v.mRapid * sense + 5_cp * v.mRapid * group + (v.mSpatl ? 3_cp * v.mRapid : 0_cp);
        if (v.mSense) p += 2_cp * (totalCount + (v.mSpatl ? 1 : 0));
        if (v.mTarget) p += 10_cp * sense + 20_cp * group + (v.mSpatl ? 10_cp : 0_cp);
        if (v.mTele) p += (v.mTele + 1) / 2_cp * (sense + (v.mSpatl ? 1 : 0)) + 3_cp * (v.mTele + 1) / 2 * group;
        if (v.mTrack) p += 5_cp * sense + 10_cp * group + (v.mSpatl ? 5_cp : 0_cp);
        if (v.mTrans) p += 5_cp * sense + 10_cp * group + (v.mSpatl ? 5_cp : 0_cp);
        return p;
    }

    struct vars {
        QStringList mWhat   { };
        int         mDetect = -1;
        QString     mThing  = "";
        int         mEnhanc = -1;
        QString     mSenses = "";
        int         mAmount = 0;
        int         mAdj    = -1;
        bool        mAnlz   = false;
        int         mConc   = 0;
        bool        mDiscr  = false;
        int         mDim    = -1;
        QString     mWhich  = "";
        int         mIncr   = -1;
        int         mMic    = 0;
        int         mPen    = -1;
        bool        mRange  = false;
        int         mRapid  = 0;
        bool        mSense  = false;
        bool        mSpatl  = false;
        bool        mTarget = false;
        int         mTele   = 0;
        bool        mTrack  = false;
        bool        mTrans  = false;
        bool        mNoDir  = false;
    } v;

    QTreeWidget* what;
    QComboBox*   detect;
    QLineEdit*   thing;
    QComboBox*   enhanc;
    QLineEdit*   senses;
    QLineEdit*   amount;
    QComboBox*   adj;
    QCheckBox*   anlz;
    QLineEdit*   conc;
    QCheckBox*   discr;
    QComboBox*   dim;
    QLineEdit*   which;
    QComboBox*   incr;
    QLineEdit*   mic;
    QComboBox*   pen;
    QCheckBox*   range;
    QLineEdit*   rapid;
    QCheckBox*   sense;
    QCheckBox*   spatl;
    QCheckBox*   target;
    QLineEdit*   tele;
    QCheckBox*   track;
    QCheckBox*   trans;
    QCheckBox*   nodir;

    QString optOut(bool showEND) {
        if (v.mWhat.size() < 1 && !v.mSpatl && v.mDetect < 1 && v.mEnhanc < 1) return "<incomplete>";
        if (v.mDetect > 0  && v.mThing.isEmpty()) return "<incomplete>";
        if ((v.mEnhanc > 0 && v.mAmount < 1 && v.mEnhanc < 3 && v.mSenses.isEmpty()) ||
            (v.mEnhanc == 3 && v.mAmount < 1)) return "<incomplete>";
        if (v.mDim > 0 && v.mDim < 2 && v.mWhich.isEmpty()) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "Enhanced Senses: ";
        bool first = true;
        if (v.mSpatl) { res += "Spatial Awareness"; first = false; }
        if (v.mDetect > 0) { res += QString("%1Detect ").arg(first ? "" : ", ") + v.mThing; first = false; }
        for (int i = 0; i < v.mWhat.count() - 1; ++i) {
            if (first) first = false;
            else res += ", ";
            res += v.mWhat[i];
        }
        if (!first && v.mWhat.count() > 0) res += ", and ";
        if (v.mWhat.count() > 0) res += v.mWhat[v.mWhat.count() - 1];
        if (v.mEnhanc > 0) {
            res += QString("; +%1 PER Roll With ").arg(v.mAmount);
            if (v.mEnhanc != 3) res += v.mSenses;
            else res += "All Senses";
        }
        if (v.mAdj > 0) {
            if (v.mAdj == 1) res += "; Fixed Perception Point ";
            else res += "; Perception Point Anywhere ";
            res += "Within 2m";
        }
        if (v.mAnlz) res += "; Analyze";
        if (v.mConc > 0) res += QString("; -%1 PER Roll To Detect").arg(v.mConc);
        if (v.mDiscr) res += "; Discriminatory";
        if (v.mDim > 0) {
            if (v.mDim != 3) res += "; Perceive Into " + v.mWhich;
            else res += "; Perceive Into Any Dimension";
        }
        if (v.mIncr > 0) {
            if (v.mIncr == 1) res += "; 240 Degree Arc Of Perception";
            else res += res += "; 360 Defgree Arc Of Perception";
        }
        if (v.mMic > 0) res += QString("; x%1 Microscopic Vision").arg((int) pow(10, v.mMic));
        if (v.mPen > 0) {
            if (v.mPen == 1) res += "; Partially Penatrative";
            else res += "; Fully Penatrative";
        }
        if (v.mRange) res += "; Ranged";
        if (v.mRapid) res += QString("; x%1 Rapid").arg((int) pow(10, v.mRapid));
        if (v.mSense) res += "; Sense";
        if (v.mTarget) res += "; Targeting";
        if (v.mTele > 0) res += QString("; x%1 Telescopic").arg((int) pow(10, v.mTele));
        if (v.mTrack) res += "; Tracking";
        if (v.mTrans) res += "; Transmit";
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
