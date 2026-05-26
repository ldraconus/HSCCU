#ifndef SENSEAFFECTINGPOWERS_H
#define SENSEAFFECTINGPOWERS_H
#include "powers.h"
#ifndef ISHSC
#include "powerdialog.h"
#endif

class Darkness: public AllPowers {
public:
    Darkness(): AllPowers("Darkness")                  { }
    Darkness(const Darkness& s): AllPowers(s)          { }
    Darkness(Darkness&& s): AllPowers(s)               { }
    Darkness(const QJsonObject& json): AllPowers(json) {
        v.mRad  = json["speed"].toInt(0);
        v.mWhat = toStringList(json["what"].toArray());
    }
    ~Darkness() override { }
    Darkness& operator=(const Darkness& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    Darkness& operator=(Darkness&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* p, QVBoxLayout* l) override           { AllPowers::form(p, l);
                                                                   rad  = createLineEdit(p, l, "Distance?", std::mem_fn(&Power::numeric));
                                                                   what  = createTreeWidget(p, l, { { "Hearing",     { "Normal Hearing",
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
                                                                 }
    Fraction lim() override                                      { return Fraction(0);
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   rad->setText(QString("%1").arg(s.mRad));
                                                                   setTreeWidget(what, s.mWhat);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mRad  = rad->text().toInt();
                                                                   v.mWhat = treeWidget(what);
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["rad"]  = v.mRad;
                                                                   obj["what"] = toArray(v.mWhat);
                                                                   return obj;
                                                                 }

private:
    Points points(bool noStore = false) override {
        if (!noStore) store();
        Points p(0);
        QStringList groups = { "Hearing", "Mental", "Radio", "Sight", "Smell/Taste", "Touch" };
        QStringList target = { "Sight", "Normal Sight", "Nightvision", "Infrared Perception", "Ultravioloet Perception" };
        bool targeting = false;
        bool group = false;
        QString found = "";
        for (int i = 0; i< v.mWhat.count(); ++i) {
            auto& str = v.mWhat[i];
            targeting = true;
            if (found.isEmpty()) found = str;
            if (groups.contains(str)) {
                group = true;
                found = str;
            }
        }
        if (targeting) p = 5_cp; // NOLINT
        else p = 3_cp;
        bool first = true;
        for (int i = 0; i< v.mWhat.count(); ++i) {
            auto& str = v.mWhat[i];
            if (!groups.contains(str)) continue;
            if (first) {
                if (str == found && group) first = false;
                else if (target.contains(str)) p += 10_cp; // NOLINT
                else p += 5_cp; // NOLINT
            } else if (target.contains(str)) p += 10_cp; // NOLINT
            else p += 5_cp; // NOLINT
        }
        for (int i = 0; i< v.mWhat.count(); ++i) {
            auto& str = v.mWhat[i];
            if (groups.contains(str)) continue;
            if (first) {
                if (str == found) first = false;
                else if (target.contains(str)) p += 5_cp; // NOLINT
                else p += 3_cp;
            } else if (target.contains(str)) p += 5_cp; // NOLINT
            else p += 3_cp;
        }
        return p * v.mRad;
    }

    struct vars {
        int         mRad  = 0;
        QStringList mWhat { };
    } v;

    QLineEdit*   rad = nullptr;
    QTreeWidget* what = nullptr;

    QString optOut(bool showEND) {
        if (v.mRad < 1 || v.mWhat.size() < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1m Radius Darkness to ").arg(v.mRad);
        res += v.mWhat[0];
        auto len = v.mWhat.length();
        for (auto i = 1; i < len; ++i) {
            if (i != len - 1) res += ", ";
            else res += ", and ";
            res += v.mWhat[i];
        }
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

class Images: public AllPowers {
public:
    Images(): AllPowers("Images")                    { }
    Images(const Images& s): AllPowers(s)            { }
    Images(Images&& s): AllPowers(s)                 { }
    Images(const QJsonObject& json): AllPowers(json) { v.mWhat   = toStringList(json["what"].toArray());
                                                       v.mPer    = json["per"].toInt(0);
                                                       v.mDiff   = json["diff"].toInt(0);
                                                       v.mOnly   = json["only"].toBool(false);
                                                       v.mSet    = json["set"].toBool(false);
                                                       v.mEfect = json["effect"].toString();
                                                     }
    ~Images() override { }
    Images& operator=(const Images& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    Images& operator=(Images&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* p, QVBoxLayout* l) override           { AllPowers::form(p, l);
                                                                   what   = createTreeWidget(p, l, { { "Hearing",     { "Normal Hearing",
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
                                                                   per    = createLineEdit(p, l, "Adjustment to PER Roll?", std::mem_fn(&Power::numeric));
                                                                   diff   = createComboBox(p, l, "Difficult to Alter?", { "Zero Phase",
                                                                                                                          "Half Phase",
                                                                                                                          "Full Phase" });
                                                                   only   = createCheckBox(p, l, "Only To Create Light");
                                                                   set    = createCheckBox(p, l, "Set Effect");
                                                                   effect = createLineEdit(p, l, "Set Effect?");
                                                                 }
    Fraction lim() override                                      { return (v.mDiff - 1) * Fraction(1, 4) +
                                                                          (v.mOnly ? Fraction(1) : Fraction(0)) +
                                                                          (v.mSet  ? Fraction(1) : Fraction(0));
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   setTreeWidget(what, s.mWhat);
                                                                   per->setText(QString("%2%1").arg(s.mPer).arg(s.mPer >= 0 ? "+" : ""));
                                                                   diff->setCurrentIndex(s.mDiff);
                                                                   only->setChecked(s.mOnly);
                                                                   set->setChecked(s.mSet);
                                                                   effect->setText(s.mEfect);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mWhat   = treeWidget(what);
                                                                   v.mPer    = normalize(per->text()).toInt();
                                                                   v.mDiff   = diff->currentIndex();
                                                                   v.mOnly   = only->isChecked();
                                                                   v.mSet    = set->isChecked();
                                                                   v.mEfect = effect->text();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["what"]   = toArray(v.mWhat);
                                                                   obj["per"]    = v.mPer;
                                                                   obj["diff"]   = v.mDiff;
                                                                   obj["only"]   = v.mOnly;
                                                                   obj["set"]    = v.mSet;
                                                                   obj["effect"] = v.mEfect;
                                                                   return obj;
                                                                 }

private:
    QString normalize(const QString& num) {
       if (!num.isEmpty() && num[0] == '+') return num.right(num.length() - 1);
       return num;
    }
    Points points(bool noStore = false) override {
        if (!noStore) store();
        Points p(0);
        QStringList groups = { "Hearing", "Mental", "Radio", "Sight", "Smell/Taste", "Touch" };
        QStringList target = { "Sight", "Normal Sight", "Nightvision", "Infrared Perception", "Ultravioloet Perception" };
        bool targeting = false;
        bool group = false;
        QString found = "";
        for (int i = 0; i< v.mWhat.count(); ++i) {
            auto& str = v.mWhat[i];
            if (target.contains(str)) {
                targeting = true;
                if (found.isEmpty()) found = str;
                if (groups.contains(str)) {
                    group = true;
                    found = str;
                }
            }
        }
        if (targeting) p = 10_cp; // NOLINT
        else p = 5_cp; // NOLINT
        bool first = true;
        for (int i = 0; i< v.mWhat.count(); ++i) {
            auto& str = v.mWhat[i];
            if (!groups.contains(str)) continue;
            if (first) {
                if (str == found && group) first = false;
                else if (target.contains(str)) p += 10_cp; // NOLINT
                else p += 5_cp; // NOLINT
            } else if (target.contains(str)) p += 10_cp; // NOLINT
            else p += 5_cp; // NOLINT
        }
        for (int i = 0; i< v.mWhat.count(); ++i) {
            auto& str = v.mWhat[i];
            if (groups.contains(str)) continue;
            if (first) {
                if (str == found) first = false;
                else if (target.contains(str)) p += 5_cp; // NOLINT
                else p += 3_cp;
            } else if (target.contains(str)) p += 5_cp; // NOLINT
            else p += 3_cp;
        }
        return p + abs(v.mPer) * 3_cp;
    }

    bool isNumber(QString txt) {
        bool ok = true;
        QString check = txt;
        if (txt.isEmpty()) return ok;
        if (txt[0] == '+' || txt[0] == '-') check = txt.right(txt.length() - 1);
        if (!check.isEmpty()) check.toInt(&ok, 10); // NOLINT
        return ok;
    }

    struct vars {
        QStringList mWhat   { };
        int         mPer    = 0;
        int         mDiff   = -1;
        bool        mOnly   = false;
        bool        mSet    = false;
        QString     mEfect = "";
    } v;

    QTreeWidget* what = nullptr;
    QLineEdit*   per = nullptr;
    QComboBox*   diff = nullptr;
    QCheckBox*   only = nullptr;
    QCheckBox*   set = nullptr;
    QLineEdit*   effect = nullptr;

    QString optOut(bool showEND) {
        if (v.mWhat.size() < 1 || (v.mSet && v.mEfect.isEmpty())) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "Images affecting ";
        res += v.mWhat[0];
        auto len = v.mWhat.length();
        for (auto i = 1; i < len; ++i) {
            if (i != len - 1) res += ", ";
            else res += ", and ";
            res += v.mWhat[i];
        }
        if (v.mPer > 0) res += QString("; %2%1 to PER rolls").arg(v.mPer).arg(v.mPer >= 0 ? "+" : "");
        if (v.mDiff == 1) res += "; Half-Phase Action To Change";
        if (v.mDiff == 2) res += "; Full-Phase Action To CHange";
        if (v.mOnly) res += "; Only To Create Light";
        if (v.mSet) res += "; Set Effect (" + v.mEfect + ")";
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

class Invisibility: public AllPowers {
public:
    Invisibility(): AllPowers("Invisibility")              { }
    Invisibility(const Invisibility& s): AllPowers(s)      { }
    Invisibility(Invisibility&& s): AllPowers(s)           { }
    Invisibility(const QJsonObject& json): AllPowers(json) { v.mWhat   = toStringList(json["what"].toArray());
                                                             v.mNo     = json["no"].toBool(false);
                                                             v.mBright = json["bright"].toBool(false);
                                                             v.mCham   = json["cham"].toBool(false);
                                                             v.mNot    = json["not"].toBool(false);
                                                           }
    ~Invisibility() override { }
    Invisibility& operator=(const Invisibility& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    Invisibility& operator=(Invisibility&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* p, QVBoxLayout* l) override           { AllPowers::form(p, l);
                                                                   what   = createTreeWidget(p, l, { { "Hearing",     { "Normal Hearing",
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
                                                                   no     = createCheckBox(p, l, "No Fringe");
                                                                   bright = createCheckBox(p, l, "Bright Fringe");
                                                                   cham   = createCheckBox(p, l, "Chameleon");
                                                                   knot   = createCheckBox(p, l, "Only When Not Attacking");
                                                                 }
    Fraction lim() override                                      { return (v.mBright ? Fraction(1, 4) : Fraction(0)) +
                                                                          (v.mCham   ? Fraction(1, 2) : Fraction(0)) +
                                                                          (v.mNot    ? Fraction(1, 2) : Fraction(0));
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   setTreeWidget(what, s.mWhat);
                                                                   no->setChecked(s.mNo);
                                                                   bright->setChecked(s.mBright);
                                                                   cham->setChecked(s.mCham);
                                                                   knot->setChecked(s.mNot);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mWhat   = treeWidget(what);
                                                                   v.mNo     = no->isChecked();
                                                                   v.mBright = bright->isChecked();
                                                                   v.mCham   = cham->isChecked();
                                                                   v.mNot    = knot->isChecked();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["what"]   = toArray(v.mWhat);
                                                                   obj["no"]     = v.mNo;
                                                                   obj["bright"] = v.mBright;
                                                                   obj["cham"]   = v.mCham;
                                                                   obj["not"]    = v.mNot;
                                                                   return obj;
                                                                 }

private:
    Points points(bool noStore = false) override {
        if (!noStore) store();
        Points p(0);
        QStringList groups = { "Hearing", "Mental", "Radio", "Sight", "Smell/Taste", "Touch" };
        QStringList target = { "Sight", "Normal Sight", "Nightvision", "Infrared Perception", "Ultravioloet Perception" };
        bool targeting = false;
        bool group = false;
        QString found = "";
        for (int i = 0; i< v.mWhat.count(); ++i) {
            auto& str = v.mWhat[i];
            if (target.contains(str)) {
                targeting = true;
                if (found.isEmpty()) found = str;
                if (groups.contains(str)) {
                    group = true;
                    found = str;
                }
            }
        }
        if (targeting) p = 20_cp; // NOLINT
        else p = 10_cp; // NOLINT
        bool first = true;
        for (int i = 0; i< v.mWhat.count(); ++i) {
            auto& str = v.mWhat[i];
            if (!groups.contains(str)) continue;
            if (first) {
                if (str == found && group) first = false;
                else if (target.contains(str)) p += 10_cp; // NOLINT
                else p += 5_cp; // NOLINT
            } else if (target.contains(str)) p += 10_cp; // NOLINT
            else p += 5_cp; // NOLINT
        }
        for (int i = 0; i< v.mWhat.count(); ++i) {
            auto& str = v.mWhat[i];
            if (groups.contains(str)) continue;
            if (first) {
                if (str == found) first = false;
                else if (target.contains(str)) p += 5_cp; // NOLINT
                else p += 3_cp;
            } else if (target.contains(str)) p += 5_cp; // NOLINT
            else p += 3_cp;
        }
        return p + (v.mNo ? 10_cp : 0_cp); // NOLINT
    }

    struct vars {
        QStringList mWhat   { };
        bool        mNo     = false;
        bool        mBright = false;
        bool        mCham   = false;
        bool        mNot    = false;
    } v;

    QTreeWidget* what = nullptr;
    QCheckBox*   no = nullptr;
    QCheckBox*   bright = nullptr;
    QCheckBox*   cham = nullptr;
    QCheckBox*   knot = nullptr;

    QString optOut(bool showEND) {
        if (v.mWhat.size() < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "Invisibility to ";
        res += v.mWhat[0];
        auto len = v.mWhat.length();
        for (auto i = 1; i < len; ++i) {
            if (i != len - 1) res += ", ";
            else res += ", and ";
            res += v.mWhat[i];
        }
        if (v.mNo) res += "; No Fringe";
        if (v.mBright) res += "; Bright Fringe";
        if (v.mCham) res += "; Chameleon";
        if (v.mNot) res += "; Only When Not Attacking";
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
