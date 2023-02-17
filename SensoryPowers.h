#ifndef SENSORYPOWERS_H
#define SENSORYPOWERS_H

#include "powers.h"

class Clairsentience: public AllPowers {
public:
    Clairsentience(): AllPowers("Clairsentienceϴ")           { }
    Clairsentience(const Clairsentience& s): AllPowers(s)    { }
    Clairsentience(Clairsentience&& s): AllPowers(s)         { }
    Clairsentience(const QJsonObject& json): AllPowers(json) { v._what   = toStringList(json["what"].toArray());
                                                                   v._pre    = json["pre"].toBool(false);
                                                                   v._retro  = json["retro"].toBool(false);
                                                                   v._mult   = json["mult"].toInt(0);
                                                                   v._mobile = json["mobile"].toInt(0);
                                                                   v._attack = json["attack"].toBool(false);
                                                                   v._black  = json["black"].toBool(false);
                                                                   v._fixed  = json["fixed"].toBool(false);
                                                                   v._only   = json["only"].toInt(0);
                                                                   v._crit   = json["crit"].toString();
                                                                   v._one    = json["one"].toBool(false);
                                                                   v._porr   = json["porr"].toBool(false);
                                                                   v._dreams = json["dreams"].toBool(false);
                                                                   v._time   = json["time"].toBool(false);
                                                                   v._vague  = json["vague"].toBool(false);
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
                                                                   mobile = createLineEdit(p, l, "Mobile Movement Muiltiplier?", std::mem_fn(&Power::numeric));
                                                                   attack = createCheckBox(p, l, "Attack Roll Required");
                                                                   black  = createCheckBox(p, l, "Blackout");
                                                                   fixed  = createCheckBox(p, l, "Fixed Perception Point");
                                                                   only   = createComboBox(p, l, "Only Through The Sense Of?", { "Anywhere",
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
    Fraction lim() override                                      { return (v._attack      ? Fraction(1, 4)                            : Fraction(0)) +
                                                                          (v._black       ? Fraction(1, 2)                            : Fraction(0)) +
                                                                          (v._fixed       ? Fraction(1)                               : Fraction(0)) +
                                                                          ((v._only > 0)  ? Fraction(1, 4) + Fraction(1, 4) * v._only : Fraction(0)) +
                                                                          (v._one         ? Fraction(1, 4)                            : Fraction(0)) +
                                                                          (v._porr        ? Fraction(1)                               : Fraction(0)) +
                                                                          (v._dreams      ? Fraction(1)                               : Fraction(0)) +
                                                                          (v._time        ? Fraction(1, 2)                            : Fraction(0)) +
                                                                          (v._vague       ? Fraction(1, 2)                            : Fraction(0));
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   setTreeWidget(what, s._what);
                                                                   pre->setChecked(s._pre);
                                                                   retro->setChecked(s._retro);
                                                                   mult->setText(QString("%1").arg(s._mult));
                                                                   mobile->setText(QString("%1").arg(s._mobile));
                                                                   attack->setChecked(s._attack);
                                                                   black->setChecked(s._black);
                                                                   fixed->setChecked(s._fixed);
                                                                   only->setCurrentIndex(s._only);
                                                                   crit->setText(s._crit);
                                                                   one->setChecked(s._one);
                                                                   porr->setChecked(s._porr);
                                                                   dreams->setChecked(s._dreams);
                                                                   time->setChecked(s._time);
                                                                   vague->setChecked(s._vague);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._what   = treeWidget(what);
                                                                   v._pre    = pre->isChecked();
                                                                   v._retro  = retro->isChecked();
                                                                   v._mult   = mult->text().toInt();
                                                                   v._mobile = mobile->text().toInt();
                                                                   v._attack = attack->isChecked();
                                                                   v._black  = black->isChecked();
                                                                   v._fixed  = fixed->isChecked();
                                                                   v._only   = only->currentIndex();
                                                                   v._crit   = crit->text();
                                                                   v._one    = one->isChecked();
                                                                   v._porr   = porr->isChecked();
                                                                   v._dreams = dreams->isChecked();
                                                                   v._time   = time->isChecked();
                                                                   v._vague  = vague->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["what"]   = toArray(v._what);
                                                                   obj["pre"]    = v._pre;
                                                                   obj["retro"]  = v._retro;
                                                                   obj["mult"]   = v._mult;
                                                                   obj["mobile"] = v._mobile;
                                                                   obj["attack"] = v._attack;
                                                                   obj["black"]  = v._black;
                                                                   obj["fixed"]  = v._fixed;
                                                                   obj["only"]   = v._only;
                                                                   obj["crit"]   = v._crit;
                                                                   obj["one"]    = v._one;
                                                                   obj["porr"]   = v._porr;
                                                                   obj["dreams"] = v._dreams;
                                                                   obj["time"]   = v._time;
                                                                   obj["vague"]  = v._vague;
                                                                   return obj;
                                                                 }

private:
    Points<> points(bool noStore = false) override {
        if (!noStore) store();
        Points<> p(0);
        QStringList groups = { "Hearing", "Mental", "Radio", "Sight", "Smell/Taste", "Touch" };
        bool first = true;
        for (const auto& str: v._what) {
            if (groups.contains(str)) {
                if (first) {
                    first = false;
                    p += 20_cp;
                } else p += 10_cp;
            } else p += 5_cp;
        }

        return p + ((v._pre || v._retro) ? 20_cp : 0_cp) + v._mult * 5_cp + v._mobile * 5_cp;
    }

    struct vars {
        QStringList _what   { };
        bool        _pre    = false;
        bool        _retro  = false;
        int         _mult   = 0;
        int         _mobile = 0;
        bool        _attack = false;
        bool        _black  = false;
        bool        _fixed  = false;
        int         _only   = -1;
        QString     _crit   = "";
        bool        _one    = false;
        bool        _porr   = false;
        bool        _dreams = false;
        bool        _time   = false;
        bool        _vague  = false;
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
        for (const auto& str: v._what) if (groups.contains(str)) { group = true; break; }
        if (v._what.size() < 1 || (one && group) || (one && v._what.size() > 1) || !group || (v._only > 0 && v._crit.isEmpty())) return "<incomplete>";
        if (v._porr && (v._mult > 0 || !(v._pre || v._retro))) return "<incomplete>";
        if (v._dreams && !(v._pre || v._retro)) return "<incomplete>";
        if (v._time && !(v._pre || v._retro)) return "<incomplete>";
        if (v._vague && !(v._pre || v._retro)) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "Clairsentienceϴ with ";
        res += v._what[0];
        int len = v._what.length();
        for (int i = 1; i < len; ++i) {
            if (i != len - 1) res += ", ";
            else res += ", and ";
            res += v._what[i];
        }
        if (v._pre) res += "; Precognition";
        if (v._retro) res += "; Retrocognition";
        if (v._mult > 0) res += QString("; x%1 Range").arg((int) pow(2, v._mult));
        if (v._mobile > 0) res += QString("; Mobile Perception Point (%1m)").arg(12 * pow(2, v._mobile));
        if (v._attack) res += "; Attack Roll Required";
        if (v._black) res += "; Blackout";
        if (v._fixed) res += "; Fixed Perception Point";
        if (v._only == 1) res += "; Only Through The Senses Of Others (Any)";
        if (v._only == 2) res += "; Only Through The Senses Of Others (" + v._crit + ")";
        if (v._one) res += "; One Sense Only";
        if (v._porr) {
            QString sep = "; ";
            if (v._pre) { res += sep + "Precognition"; sep = " and "; }
            if (v._retro) res += sep + "Retrocognition";
            res += " Only";
        }
        if (v._dreams) res += "; Only Through Dreams";
        if (v._time) res += "; Time Modifiers";
        if (v._vague) res += "; Vague and Unclear";
        return res;
    }

    void numeric(int) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
        PowerDialog::ref().updateForm();
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }
};

class EnhancedSenses: public AllPowers {
public:
    EnhancedSenses(): AllPowers("Enhanced Senses")           { }
    EnhancedSenses(const EnhancedSenses& s): AllPowers(s)    { }
    EnhancedSenses(EnhancedSenses&& s): AllPowers(s)         { }
    EnhancedSenses(const QJsonObject& json): AllPowers(json) { v._what   = toStringList(json["what"].toArray());
                                                                   v._detect = json["detect"].toInt(0);
                                                                   v._thing  = json["thing"].toString();
                                                                   v._enhanc = json["enhanc"].toInt(0);
                                                                   v._senses = json["senses"].toString();
                                                                   v._amount = json["amount"].toInt(0);
                                                                   v._spatl  = json["spatl"].toBool(false);
                                                                   v._adj    = json["adj"].toInt(0);
                                                                   v._anlz   = json["anlz"].toBool(false);
                                                                   v._conc   = json["conc"].toInt(0);
                                                                   v._discr  = json["discr"].toBool(false);
                                                                   v._dim    = json["dim"].toInt(0);
                                                                   v._which  = json["which"].toString();
                                                                   v._incr   = json["incr"].toInt(0);
                                                                   v._mic    = json["mic"].toInt(0);
                                                                   v._pen    = json["pen"].toInt(0);
                                                                   v._range  = json["range"].toBool(false);
                                                                   v._rapid  = json["rapid"].toInt(0);
                                                                   v._sense  = json["sense"].toBool(false);
                                                                   v._target = json["target"].toBool(false);
                                                                   v._tele   = json["tele"].toInt(0);
                                                                   v._track  = json["track"].toBool(false);
                                                                   v._trans  = json["trans"].toBool(false);
                                                                   v._nodir  = json["nodir"].toBool(false);
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
                                                                   detect = createComboBox(p, l, "Detect?", { "Not detecting",
                                                                                                              "One Thing",
                                                                                                              "A Class Of Things",
                                                                                                              "A Large Class Of Things" });
                                                                   thing  = createLineEdit(p, l, "Thing(s)?");
                                                                   spatl  = createCheckBox(p, l, "Spatial Awareness");
                                                                   enhanc = createComboBox(p, l, "Enhanced Perception?", { "Not Enhanced",
                                                                                                                           "One Sense",
                                                                                                                           "One Sense Group",
                                                                                                                           "All Senses" });
                                                                   senses = createLineEdit(p, l, "Which sense(s)");
                                                                   amount = createLineEdit(p, l, "Plusses?", std::mem_fn(&Power::numeric));
                                                                   adj    = createComboBox(p, l, "Adjacent?", { "Non-adjacesnt",
                                                                                                                "Fixed Point Within 2m",
                                                                                                                "Any Point Withing 2m" });
                                                                   anlz   = createCheckBox(p, l, "Analyize");
                                                                   conc   = createLineEdit(p, l, "Concealment Modifier?", std::mem_fn(&Power::numeric));
                                                                   discr  = createCheckBox(p, l, "Discriminatory");
                                                                   dim    = createComboBox(p, l, "Dimensional?", { "Non-Dimensional",
                                                                                                                   "Single Other Dimension",
                                                                                                                   "Related Group Of Dimensions",
                                                                                                                   "Any Dimension" });
                                                                   which  = createLineEdit(p, l, "Dimension(s)?");
                                                                   incr   = createComboBox(p, l, "Increased Arc Of Perception?", { "Normal",
                                                                                                                                   "240 Degrees",
                                                                                                                                   "360 Degrees" });
                                                                   mic    = createLineEdit(p, l, "Microscropic x10 Magnification?", std::mem_fn(&Power::numeric));
                                                                   pen    = createComboBox(p, l, "Penetrative?", { "Not",
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
    Fraction lim() override                                      { return (v._nodir ? Fraction(1, 2) : Fraction(0));
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   setTreeWidget(what, s._what);
                                                                   detect->setCurrentIndex(s._detect);
                                                                   thing->setText(v._thing);
                                                                   enhanc->setCurrentIndex(s._enhanc);
                                                                   senses->setText(s._senses);
                                                                   amount->setText(QString("%1").arg(s._amount));
                                                                   adj->setCurrentIndex(s._adj);
                                                                   anlz->setChecked(s._anlz);
                                                                   conc->setText(QString("%1").arg(s._conc));
                                                                   discr->setChecked(s._discr);
                                                                   dim->setCurrentIndex(s._dim);
                                                                   which->setText(s._which);
                                                                   incr->setCurrentIndex(s._incr);
                                                                   mic->setText(QString("%1").arg(s._mic));
                                                                   pen->setCurrentIndex(s._pen);
                                                                   range->setChecked(s._range);
                                                                   rapid->setText(QString("%1").arg(s._rapid));
                                                                   sense->setChecked(s._sense);
                                                                   spatl->setChecked(s._spatl);
                                                                   target->setChecked(s._target);
                                                                   tele->setText(QString("%!").arg(s._tele));
                                                                   track->setChecked(s._track);
                                                                   trans->setChecked(s._trans);
                                                                   nodir->setChecked(s._nodir);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._what   = treeWidget(what);
                                                                   v._detect = detect->currentIndex();
                                                                   v._thing  = thing->text();
                                                                   v._enhanc = enhanc->currentIndex();
                                                                   v._senses = senses->text();
                                                                   v._amount = amount->text().toInt();
                                                                   v._adj    = adj->currentIndex();
                                                                   v._anlz   = anlz->isChecked();
                                                                   v._conc   = conc->text().toInt();
                                                                   v._discr  = discr->isChecked();
                                                                   v._dim    = dim->currentIndex();
                                                                   v._which  = which->text();
                                                                   v._incr   = incr->currentIndex();
                                                                   v._mic    = mic->text().toInt();
                                                                   v._pen    = pen->currentIndex();
                                                                   v._range  = range->isChecked();
                                                                   v._rapid  = rapid->text().toInt();
                                                                   v._sense  = sense->isChecked();
                                                                   v._spatl  = spatl->isChecked();
                                                                   v._target = target->isChecked();
                                                                   v._tele   = tele->text().toInt();
                                                                   v._track  = track->isChecked();
                                                                   v._trans  = trans->isChecked();
                                                                   v._nodir  = nodir->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["what"]   = toArray(v._what);
                                                                   obj["detect"] = v._detect;
                                                                   obj["thing"]  = v._thing;
                                                                   obj["enhanc"] = v._enhanc;
                                                                   obj["senses"] = v._senses;
                                                                   obj["amount"] = v._amount;
                                                                   obj["adj"]    = v._adj;
                                                                   obj["anlz"]   = v._anlz;
                                                                   obj["conc"]   = v._conc;
                                                                   obj["discr"]  = v._discr;
                                                                   obj["dim"]    = v._dim;
                                                                   obj["which"]  = v._which;
                                                                   obj["incr"]   = v._incr;
                                                                   obj["mic"]    = v._mic;
                                                                   obj["pen"]    = v._pen;
                                                                   obj["range"]  = v._range;
                                                                   obj["rapid"]  = v._rapid;
                                                                   obj["sense"]  = v._sense;
                                                                   obj["spatl"]  = v._spatl;
                                                                   obj["target"] = v._target;
                                                                   obj["tele"]   = v._tele;
                                                                   obj["track"]  = v._track;
                                                                   obj["trans"]  = v._trans;
                                                                   obj["nodir"]  = v._nodir;
                                                                   return obj;
                                                                 }

private:
    Points<> points(bool noStore = false) override {
        if (!noStore) store();
        Points<> p(0);
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
        for (const auto& str: v._what) {
            if (groups.contains(str)) {
                group++;
                groupCount += groupCounts[str];
            }
            else sense++;
            if (keys.contains(str)) p += Points<>(cost[str]);
        }
        int totalCount = sense + groupCount;

        if (v._enhanc > 0) p += 1_cp * v._enhanc * v._amount;
        if (v._detect > 0) p += 3_cp + ((v._detect == 2) ? 2_cp : ((v._detect == 3) ? 8_cp : 0_cp));
        if (v._spatl) p += 32_cp;
        if (v._adj > 0) p += v._adj * sense + (3_cp + (v._adj - 1) * 2_cp) * group;
        if (v._anlz) p += 5_cp * sense + 10_cp * group;
        if (v._conc > 0) p += 1_cp * v._conc;
        if (v._discr) p += 5_cp * sense + 10_cp * group;
        if (v._dim > 0) p += (v._dim * 5_cp) * sense + ((v._dim == 1) ? 10_cp : ((v._dim == 2) ? 20_cp : 25_cp)) * group;
        if (v._incr > 0) {
            if (group == 6) p += (v._incr == 1) ? 10_cp : 25_cp;
            else p += (2_cp + (v._incr - 1) * 3_cp) * sense + (v._incr * 5_cp) * group;
        }
        if (v._mic > 0) p += (3_cp * sense + 5_cp * group) * v._mic;
        if (v._pen > 0) p += 5_cp * v._pen * sense + (5_cp + v._pen * 5) * group;
        if (v._range) p += 5_cp * sense + 10_cp * group;
        if (v._rapid > 0) p += 3_cp * v._rapid * sense + 5_cp * v._rapid * group;
        if (v._sense) p += 2_cp * totalCount;
        if (v._target) p += 10_cp * sense + 20_cp * group;
        if (v._tele) p += (v._tele + 1) / 2_cp * sense + 3_cp * (v._tele + 1) / 2 * group;
        if (v._track) p += 5_cp * sense + 10_cp * group;
        if (v._trans) p += 5_cp * sense + 10_cp * group;
        return p;
    }

    struct vars {
        QStringList _what   { };
        int         _detect = -1;
        QString     _thing  = "";
        int         _enhanc = -1;
        QString     _senses = "";
        int         _amount = 0;
        int         _adj    = -1;
        bool        _anlz   = false;
        int         _conc   = 0;
        bool        _discr  = false;
        int         _dim    = -1;
        QString     _which  = "";
        int         _incr   = -1;
        int         _mic    = 0;
        int         _pen    = -1;
        bool        _range  = false;
        int         _rapid  = 0;
        bool        _sense  = false;
        bool        _spatl  = false;
        bool        _target = false;
        int         _tele   = 0;
        bool        _track  = false;
        bool        _trans  = false;
        bool        _nodir  = false;
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
        if (v._what.size() < 1 && v._detect < 1 && v._enhanc < 1) return "<incomplete>";
        if (v._detect > 0  && v._thing.isEmpty()) return "<incomplete>";
        if ((v._enhanc > 0 && v._amount < 1 && v._enhanc < 3 && v._senses.isEmpty()) ||
            (v._enhanc == 3 && v._amount < 1)) return "<incomplete>";
        if (v._dim > 0 && v._dim < 2 && v._which.isEmpty()) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "Enhanced Senses: ";
        bool first = true;
        if (v._detect > 0) { res += "Detect " + v._thing; first = false; }
        for (int i = 0; i < v._what.count() - 1; ++i) {
            if (first) first = false;
            else res += ", ";
            res += v._what[i];
        }
        if (!first) res += ", and ";
        if (v._what.count() > 0) res += v._what[v._what.count() - 1];
        if (v._enhanc > 0) {
            res += QString("; +%1 PER Roll With ").arg(v._amount);
            if (v._enhanc != 3) res += v._senses;
            else res += "All Senses";
        }
        if (v._adj > 0) {
            if (v._adj == 1) res += "; Fixed Perception Point ";
            else res += "; Perception Point Anywhere ";
            res += "Within 2m";
        }
        if (v._anlz) res += "; Analyze";
        if (v._conc > 0) res += QString("; -%1 PER Roll To Detect").arg(v._conc);
        if (v._discr) res += "; Discriminatory";
        if (v._dim > 0) {
            if (v._dim != 3) res += "; Perceive Into " + v._which;
            else res += "; Perceive Into Any Dimension";
        }
        if (v._incr > 0) {
            if (v._incr == 1) res += "; 240 Degree Arc Of Perception";
            else res += res += "; 360 Defgree Arc Of Perception";
        }
        if (v._mic > 0) res += QString("; x%1 Microscopic Vision").arg((int) pow(10, v._mic));
        if (v._pen > 0) {
            if (v._pen == 1) res += "; Partially Penatrative";
            else res += "; Fully Penatrative";
        }
        if (v._range) res += "; Ranged";
        if (v._rapid) res += QString("; x%1 Rapid").arg((int) pow(10, v._rapid));
        if (v._sense) res += "; Sense";
        if (v._target) res += "; Targeting";
        if (v._tele > 0) res += QString("; x%1 Telescopic").arg((int) pow(10, v._tele));
        if (v._track) res += "; Tracking";
        if (v._trans) res += "; Transmit";
        return res;
    }

    void numeric(int) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
        PowerDialog::ref().updateForm();
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }
};

#endif // AllPowers_H
