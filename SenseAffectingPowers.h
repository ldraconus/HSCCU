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
        v._rad  = json["speed"].toInt(0);
        v._what = toStringList(json["what"].toArray());
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
                                                                   rad->setText(QString("%1").arg(s._rad));
                                                                   setTreeWidget(what, s._what);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._rad  = rad->text().toInt();
                                                                   v._what = treeWidget(what);
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["rad"]  = v._rad;
                                                                   obj["what"] = toArray(v._what);
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
        for (const auto& str: v._what) if (target.contains(str)) {
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
        for (const auto& str: v._what) {
            if (!groups.contains(str)) continue;
            if (first) {
                if (str == found && group) first = false;
                else if (target.contains(str)) p += 10_cp; // NOLINT
                else p += 5_cp; // NOLINT
            } else if (target.contains(str)) p += 10_cp; // NOLINT
            else p += 5_cp; // NOLINT
        }
        for (const auto& str: v._what) {
            if (groups.contains(str)) continue;
            if (first) {
                if (str == found) first = false;
                else if (target.contains(str)) p += 5_cp; // NOLINT
                else p += 3_cp;
            } else if (target.contains(str)) p += 5_cp; // NOLINT
            else p += 3_cp;
        }
        return p * v._rad;
    }

    struct vars {
        int         _rad  = 0;
        QStringList _what { };
    } v;

    QLineEdit*   rad = nullptr;
    QTreeWidget* what = nullptr;

    QString optOut(bool showEND) {
        if (v._rad < 1 || v._what.size() < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1m Radius Darkness to ").arg(v._rad);
        res += v._what[0];
        auto len = v._what.length();
        for (auto i = 1; i < len; ++i) {
            if (i != len - 1) res += ", ";
            else res += ", and ";
            res += v._what[i];
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
    Images(const QJsonObject& json): AllPowers(json) { v._what   = toStringList(json["what"].toArray());
                                                       v._per    = json["per"].toInt(0);
                                                       v._diff   = json["diff"].toInt(0);
                                                       v._only   = json["only"].toBool(false);
                                                       v._set    = json["set"].toBool(false);
                                                       v._effect = json["effect"].toString();
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
    Fraction lim() override                                      { return (v._diff - 1) * Fraction(1, 4) +
                                                                          (v._only ? Fraction(1) : Fraction(0)) +
                                                                          (v._set  ? Fraction(1) : Fraction(0));
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   setTreeWidget(what, s._what);
                                                                   per->setText(QString("%2%1").arg(s._per).arg(s._per >= 0 ? "+" : ""));
                                                                   diff->setCurrentIndex(s._diff);
                                                                   only->setChecked(s._only);
                                                                   set->setChecked(s._set);
                                                                   effect->setText(s._effect);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._what   = treeWidget(what);
                                                                   v._per    = normalize(per->text()).toInt();
                                                                   v._diff   = diff->currentIndex();
                                                                   v._only   = only->isChecked();
                                                                   v._set    = set->isChecked();
                                                                   v._effect = effect->text();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["what"]   = toArray(v._what);
                                                                   obj["per"]    = v._per;
                                                                   obj["diff"]   = v._diff;
                                                                   obj["only"]   = v._only;
                                                                   obj["set"]    = v._set;
                                                                   obj["effect"] = v._effect;
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
        for (const auto& str: v._what) if (target.contains(str)) {
            targeting = true;
            if (found.isEmpty()) found = str;
            if (groups.contains(str)) {
                group = true;
                found = str;
            }
        }
        if (targeting) p = 10_cp; // NOLINT
        else p = 5_cp; // NOLINT
        bool first = true;
        for (const auto& str: v._what) {
            if (!groups.contains(str)) continue;
            if (first) {
                if (str == found && group) first = false;
                else if (target.contains(str)) p += 10_cp; // NOLINT
                else p += 5_cp; // NOLINT
            } else if (target.contains(str)) p += 10_cp; // NOLINT
            else p += 5_cp; // NOLINT
        }
        for (const auto& str: v._what) {
            if (groups.contains(str)) continue;
            if (first) {
                if (str == found) first = false;
                else if (target.contains(str)) p += 5_cp; // NOLINT
                else p += 3_cp;
            } else if (target.contains(str)) p += 5_cp; // NOLINT
            else p += 3_cp;
        }
        return p + abs(v._per) * 3_cp;
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
        QStringList _what   { };
        int         _per    = 0;
        int         _diff   = -1;
        bool        _only   = false;
        bool        _set    = false;
        QString     _effect = "";
    } v;

    QTreeWidget* what = nullptr;
    QLineEdit*   per = nullptr;
    QComboBox*   diff = nullptr;
    QCheckBox*   only = nullptr;
    QCheckBox*   set = nullptr;
    QLineEdit*   effect = nullptr;

    QString optOut(bool showEND) {
        if (v._what.size() < 1 || (v._set && v._effect.isEmpty())) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "Images affecting ";
        res += v._what[0];
        auto len = v._what.length();
        for (auto i = 1; i < len; ++i) {
            if (i != len - 1) res += ", ";
            else res += ", and ";
            res += v._what[i];
        }
        if (v._per > 0) res += QString("; %2%1 to PER rolls").arg(v._per).arg(v._per >= 0 ? "+" : "");
        if (v._diff == 1) res += "; Half-Phase Action To Change";
        if (v._diff == 2) res += "; Full-Phase Action To CHange";
        if (v._only) res += "; Only To Create Light";
        if (v._set) res += "; Set Effect (" + v._effect + ")";
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
    Invisibility(const QJsonObject& json): AllPowers(json) { v._what   = toStringList(json["what"].toArray());
                                                             v._no     = json["per"].toBool(false);
                                                             v._bright = json["diff"].toBool(false);
                                                             v._cham   = json["only"].toBool(false);
                                                             v._not    = json["set"].toBool(false);
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
    Fraction lim() override                                      { return (v._bright ? Fraction(1, 4) : Fraction(0)) +
                                                                          (v._cham   ? Fraction(1, 2) : Fraction(0)) +
                                                                          (v._not    ? Fraction(1, 2) : Fraction(0));
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   setTreeWidget(what, s._what);
                                                                   no->setChecked(s._no);
                                                                   bright->setChecked(s._bright);
                                                                   cham->setChecked(s._cham);
                                                                   knot->setChecked(s._not);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._what   = treeWidget(what);
                                                                   v._no     = no->isChecked();
                                                                   v._bright = bright->isChecked();
                                                                   v._cham   = cham->isChecked();
                                                                   v._not    = knot->isChecked();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["what"]   = toArray(v._what);
                                                                   obj["no"]     = v._no;
                                                                   obj["bright"] = v._bright;
                                                                   obj["cham"]   = v._cham;
                                                                   obj["not"]    = v._not;
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
        for (const auto& str: v._what) if (target.contains(str)) {
            targeting = true;
            if (found.isEmpty()) found = str;
            if (groups.contains(str)) {
                group = true;
                found = str;
            }
        }
        if (targeting) p = 20_cp; // NOLINT
        else p = 10_cp; // NOLINT
        bool first = true;
        for (const auto& str: v._what) {
            if (!groups.contains(str)) continue;
            if (first) {
                if (str == found && group) first = false;
                else if (target.contains(str)) p += 10_cp; // NOLINT
                else p += 5_cp; // NOLINT
            } else if (target.contains(str)) p += 10_cp; // NOLINT
            else p += 5_cp; // NOLINT
        }
        for (const auto& str: v._what) {
            if (groups.contains(str)) continue;
            if (first) {
                if (str == found) first = false;
                else if (target.contains(str)) p += 5_cp; // NOLINT
                else p += 3_cp;
            } else if (target.contains(str)) p += 5_cp; // NOLINT
            else p += 3_cp;
        }
        return p + (v._no ? 10_cp : 0_cp); // NOLINT
    }

    struct vars {
        QStringList _what   { };
        bool        _no     = false;
        bool        _bright = false;
        bool        _cham   = false;
        bool        _not    = false;
    } v;

    QTreeWidget* what = nullptr;
    QCheckBox*   no = nullptr;
    QCheckBox*   bright = nullptr;
    QCheckBox*   cham = nullptr;
    QCheckBox*   knot = nullptr;

    QString optOut(bool showEND) {
        if (v._what.size() < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "Invisibility to ";
        res += v._what[0];
        auto len = v._what.length();
        for (auto i = 1; i < len; ++i) {
            if (i != len - 1) res += ", ";
            else res += ", and ";
            res += v._what[i];
        }
        if (v._no) res += "; No Fringe";
        if (v._bright) res += "; Bright Fringe";
        if (v._cham) res += "; Chameleon";
        if (v._not) res += "; Only When Not Attacking";
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
