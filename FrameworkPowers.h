#ifndef FRAMEWORKPOWERS_H
#define FRAMEWORKPOWERS_H

#include "powers.h"
#ifndef ISHSC
#include "sheet.h"
#endif

class FrameworkPowers: public Power {
public:
    FrameworkPowers(): Power() { }
    FrameworkPowers(QString name)
        : Power()
        , v { name, "", { } } { }
    FrameworkPowers(const FrameworkPowers& s)
        : Power(s)
        , v(s.v) { }
    FrameworkPowers(FrameworkPowers&& s)
        : Power(s)
        , v(s.v) { }
    FrameworkPowers(const QJsonObject& json)
        : Power()
        , v { json["name"].toString(), json["powerName"].toString(""), { } } { loadPowers(json["powers"].toArray()); }
    ~FrameworkPowers() override { }

    FrameworkPowers& operator=(const FrameworkPowers& s) {
        if (this != &s) v = s.v;
        return *this;
    }
    FrameworkPowers& operator=(FrameworkPowers&& s) {
        v = s.v;
        return *this;
    }

    QString description(bool roll = false) override { return roll ? "" : ""; }
    Points  points(bool noStore = false) override   { return noStore ? 0_cp : 0_cp; }

    void loadPowers(QJsonArray powers) {
        for (const auto& power: powers) {
            QJsonObject obj = power.toObject();
            v._powers.append(Power::FromJson(obj["name"].toString(), obj));
        }
    }

    bool isFramework() override                      { return true; }
    int  count() override                            { return gsl::narrow<int>(v._powers.count()); }
    void append(shared_ptr<Power> p) override        { v._powers.append(p);    p->parent(this); }
    void insert(int n, shared_ptr<Power> p) override { v._powers.insert(n, p); p->parent(this); }
    void remove(int n) override                      { v._powers[n]->parent(nullptr); v._powers.removeAt(n); }
    void remove(shared_ptr<Power> p) override        { p->parent(nullptr); v._powers.removeOne(p); }
    QList<shared_ptr<Power>>& list() override        { return v._powers; }

    Fraction adv() override         { return Fraction(0); }
    void     checked(bool) override { }
    Fraction lim() override         { return Fraction(0); }
    void     numeric(int) override  { }

    void        form(QWidget* widget, QVBoxLayout* layout) override { powerName = createLineEdit(widget, layout, "Nickname of power");
                                                                    }
    QString     name() override                                     { return v._name;
                                                                    }
    QString     nickname() override                                 { return v._powerName;
                                                                    }
    void        restore() override                                  { vars s = v;
                                                                      powerName->setText(s._powerName);
                                                                      v = s;
                                                                    }
    void        store() override                                    { v._powerName = powerName->text();
                                                                    }
    QJsonObject toJson() const override                             { QJsonObject obj = Power::toJson();
                                                                      obj["name"]      = v._name;
                                                                      obj["powerName"] = v._powerName;
                                                                      QJsonArray powers;
                                                                      for (int i = 0; i < v._powers.count(); ++i) powers.append(v._powers[i]->toJson());
                                                                      obj.insert("powers", powers);
                                                                      return obj;
                                                                    }

    Fraction limitations() {
        Fraction limits(1);
        limits += lim();
        for (const auto& mod: limitationsList()) limits += mod->fraction(Power::NoStore).abs();
        return limits;
    }

    Fraction advantages() {
        Fraction advants(1);
        advants += adv();
        for (const auto& mod: advantagesList())
            if (!mod->isAdder()) advants += mod->fraction(Power::NoStore);
        return advants;
    }

    Points modifiers() {
        Points pts(0);
        for (const auto& mod: advantagesList())
            if (mod->isAdder()) pts += mod->points();
        return pts;
    }

protected:
    struct vars {
        QString _name = "";
        QString _powerName = "";
        QList<shared_ptr<Power>> _powers {};
    };

    vars& super() { return v; }

private:
    vars v;

    QLineEdit* powerName = nullptr;
};

class Group: public FrameworkPowers {
public:
    Group(): FrameworkPowers("Group")                     { }
    Group(const Group& s): FrameworkPowers(s)             { }
    Group(Group&& s): FrameworkPowers(s)                  { }
    Group(const QJsonObject& json): FrameworkPowers(json) { }
    ~Group() override { }

    Group& operator=(const Group&) {
        return *this;
    }
    Group& operator=(Group&&) {
        return *this;
    }


    bool isValid(shared_ptr<Power>) override { return true; }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { FrameworkPowers::form(parent, layout);
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points   points(bool noStore = false) override               { if (!noStore) store();
                                                                   return 0_cp; }
    void     restore() override                                  { FrameworkPowers::restore();
                                                                 }
    void     store() override                                    { FrameworkPowers::store();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = FrameworkPowers::toJson();
                                                                   return obj;
                                                                 }

#ifdef ISHSC
    Points display(int&, QTableWidget*) override {
       Points points = 0_cp;
#else
    Points display(int& row, QTableWidget* tbl) override {
        Points points = 0_cp;
        QFont font = tbl->font();
        QFont italic = font;
        italic.setItalic(true);
        int r = 1;
        auto limit = limitations();
        auto advtg = advantages();
        auto modif = modifiers();
        for (const auto& pe: super()._powers) {
            QString descr = pe->description();
            if (descr == "-") descr = "";
            pe->parent(this);
            for (const auto& mod: pe->advantagesList()) {
                if (mod->isAdder()) descr += "; (+" + QString("%1").arg(mod->points(Modifier::NoStore).points) + " pts) ";
                else descr += "; (+" + mod->fraction(true).toString() + ") ";
                descr += mod->description(false);
            }
            for (const auto& mod: pe->limitationsList()) descr += "; (-" + mod->fraction(Modifier::NoStore).abs().toString() + ") " + mod->description(false);
            Fraction pts(pe->real(advtg, modif, limit).points);
            if (pts.toInt() == 0 && !pe->isEquipment()) pts = Fraction(1);
            Sheet::ref().setCell(tbl, row, 0, descr.isEmpty() ? "" : QString("%1").arg(pts.toInt()), font);
            Sheet::ref().setCell(tbl, row, 1, pe->nickname(), italic);
            Sheet::ref().setCell(tbl, row, 2, descr.isEmpty() ? "" : QString("%1) %2").arg(r).arg(descr), font, Sheet::WordWrap);
            QString end = pe->end();
            if (end == "-") end = "";
            Sheet::ref().setCell(tbl, row, 3, end, font);
            points += Points(pts.toInt());
            pe->row(row);
            ++row;
            if (!descr.isEmpty()) ++r;
        }
#endif
        return points;
    }

    Points display(QString& out) override {
        Points points = 0_cp;
        auto limit = limitations();
        auto advtg = advantages();
        auto modif = modifiers();
        int r = 1;
        for (const auto& pe: super()._powers) {
            QString descr = pe->description();
            if (descr == "-") descr = "";
            if (!descr.isEmpty()) {
                pe->parent(this);
                for (const auto& mod: pe->advantagesList()) {
                    if (mod->isAdder()) descr += "; (+" + QString("%1").arg(mod->points(Modifier::NoStore).points) + " pts) ";
                    else descr += "; (+" + mod->fraction(true).toString() + ") ";
                    descr += mod->description(false);
                }
                for (const auto& mod: pe->limitationsList()) descr += "; (-" + mod->fraction(Modifier::NoStore).abs().toString() + ") " + mod->description(false);
                Fraction pts(pe->real(advtg, modif, limit).points);
                if (pts.toInt() == 0 && !pe->isEquipment()) pts = Fraction(1);
                out += QString("%1\t").arg(pts.toInt(), 3);
                out += QString("%1) %2%3").arg(r).arg(pe->nickname().isEmpty() ? "" : "(" + pe->nickname() + ") ", descr);
                QString end = pe->end();
                if (end == "-") end = "";
                out += QString("%1").arg(end.isEmpty() ? "" : " [" + end +"]");
                points += Points(pts.toInt());
            }
            out += "\n";
            if (!descr.isEmpty()) ++r;
        }
        return points;
    }

private:
    struct vars { };

    QString optOut(bool) {
        QString res = "Group";
        return res;
    }
};

class Multipower: public FrameworkPowers {
public:
    Multipower(): FrameworkPowers("Multipower")                { }
    Multipower(const Multipower& s): FrameworkPowers(s)        { }
    Multipower(Multipower&& s): FrameworkPowers(s)             { }
    Multipower(const QJsonObject& json): FrameworkPowers(json) { v._points = json["points"].toInt(0);
                                                               }
    ~Multipower() override { }

    Multipower& operator=(const Multipower& s) {
        if (this != &s) v = s.v;
        return *this;
    }
    Multipower& operator=(Multipower&& s) {
        v = s.v;
        return *this;
    }

    bool isMultipower() override { return true; }
    bool isValid(shared_ptr<Power> power) override {
        if (power->isEquipment()) return false;
        auto points = power->acting();
        return points <= v._points;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { FrameworkPowers::form(parent, layout);
                                                                   pnts = createLineEdit(parent, layout, "Points in the pool?", std::mem_fn(&Power::numeric));
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override                 { if (!noStore) store();
                                                                   return Points(v._points); }
    void     restore() override                                  { vars s = v;
                                                                   FrameworkPowers::restore();
                                                                   pnts->setText(QString("%1").arg(s._points));
                                                                   v = s;
                                                                 }
    void     store() override                                    { FrameworkPowers::store();
                                                                   v._points = pnts->text().toInt();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = FrameworkPowers::toJson();
                                                                   obj["points"] = v._points;
                                                                   return obj;
                                                                 }

    Points display(int& row, QTableWidget* tbl) override {
        Points points = 0_cp;
        QFont font = tbl->font();
        QFont italic = font;
        italic.setItalic(true);
        int r = 1;
        auto limit = limitations();
        auto advtg = advantages();
        auto modif = modifiers();
        for (const auto& pe: FrameworkPowers::super()._powers) {
            QString descr = pe->description();
            if (descr == "-") descr = "";
            pe->parent(this);
            for (const auto& mod: pe->advantagesList()) {
                if (mod->isAdder()) descr += "; (+" + QString("%1").arg(mod->points(Modifier::NoStore).points) + " pts) ";
                else descr += "; (+" + mod->fraction(true).toString() + ") ";
                descr += mod->description(false);
            }
            for (const auto& mod: pe->limitationsList()) descr += "; (-" + mod->fraction(Modifier::NoStore).abs().toString() + ") " + mod->description(false);
            Fraction cost(pe->real().points);
            Fraction pts(pe->real(advtg, modif, limit).points);
            if (pe->varying()) pts = pts / 5; // NOLINT
            else pts = pts / 10; // NOLINT
            if (pts.toInt(true) == 0) pts = Fraction(1);
#ifndef ISHSC
            Sheet::ref().setCell(tbl, row, 0, descr.isEmpty() ? "" : QString("%1%2").arg(pts.toInt(true)).arg(pe->varying() ? "v" : "f"), font);
            Sheet::ref().setCell(tbl, row, 1, " " + pe->nickname(), italic);
            Sheet::ref().setCell(tbl, row, 2, descr.isEmpty() ? "" : QString("%1) [%3] %2").arg(r).arg(descr).arg(cost.toInt(true)), font, Sheet::WordWrap);
#endif
            QString end = pe->end();
            if (end == "-") end = "";
#ifndef ISHSC
            Sheet::ref().setCell(tbl, row, 3, end, font);
#endif
            points += Points(pts.toInt(true));
            pe->row(row);
            ++row;
            if (!descr.isEmpty()) ++r;
        }
        return points;
    }

    Points display(QString& out) override {
        Points points = 0_cp;
        int r = 1;
        auto limit = limitations();
        auto advtg = advantages();
        auto modif = modifiers();
        for (const auto& pe: FrameworkPowers::super()._powers) {
            QString descr = pe->description();
            if (descr == "-") descr = "";
            pe->parent(this);
            for (const auto& mod: pe->advantagesList()) {
                if (mod->isAdder()) descr += "; (+" + QString("%1").arg(mod->points(Modifier::NoStore).points) + " pts) ";
                else descr += "; (+" + mod->fraction(true).toString() + ") ";
                descr += mod->description(false);
            }
            for (const auto& mod: pe->limitationsList()) descr += "; (-" + mod->fraction(Modifier::NoStore).abs().toString() + ") " + mod->description(false);
            Points pnts(pe->real());
            Fraction pts(pe->real(advtg, modif, limit).points);
            if (pe->varying()) pts = pts / 5;  // NOLINT
            else pts = pts / 10;  // NOLINT
            if (pts.toInt() == 0) pts = Fraction(1);
            if (!descr.isEmpty()) {
                out += QString("%1%2\t").arg(pts.toInt(), 3).arg(pe->varying() ? "v" : "f");
                out += QString("%1) [%4] %2%3").arg(r).arg(pe->nickname().isEmpty() ? "" : "(" + pe->nickname() + ") ", descr).arg(pnts.points);
                QString end = pe->end();
                if (end == "-") end = "";
                out += QString("%1").arg(end.isEmpty() ? "" : " [" + end +"]");
                points += Points(pts.toInt());
            }
            out += "\n";
            if (!descr.isEmpty()) ++r;
        }
        return points;
    }

private:
    struct vars {
        int _points = 0;
    } v;

    QLineEdit* pnts = nullptr;

    QString optOut(bool) {
        QString res = "Multipower Pool " + QString("(%1 cp)").arg(v._points);
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

class VPP: public FrameworkPowers {
public:
    VPP(): FrameworkPowers("Variable Power Pool")       { }
    VPP(const VPP& s): FrameworkPowers(s)               { }
    VPP(VPP&& s): FrameworkPowers(s)                    { }
    VPP(const QJsonObject& json): FrameworkPowers(json) { v._pool    = json["pool"].toInt(0);
                                                          v._control = json["control"].toInt(0);
                                                          v._time    = json["time"].toInt(0);
                                                          v._noSkill = json["noSkill"].toBool(false);
                                                          v._given   = json["given"].toBool(false);
                                                          v._circ    = json["circ"].toString();
                                                          v._how     = json["how"].toBool(false);
                                                          v._when2   = json["when2"].toBool(false);
                                                          v._class   = json["class"].toInt(0);
                                                          v._what    = json["what"].toString();
                                                          v._one     = json["one"].toInt(0);
                                                          v._power   = json["power"].toString();
                                                        }
    ~VPP() override { }

    VPP& operator=(const VPP& s) {
        if (this != &s) v = s.v;
        return *this;
    }
    VPP& operator=(VPP&& s) {
        v = s.v;
        return *this;
    }

    bool isVPP() override { return true; }
    bool isValid(shared_ptr<Power> power) override {
        if (power->isEquipment()) return false;
        return power->acting() <= v._control && power->real() <= v._pool;
    }
    Points pool() override { return Points(v._pool); }

    Fraction adv() override                                      { return ((v._time > 3) ? Fraction(1, 2) * (v._time - 3) : Fraction(0)) +
                                                                          (v._noSkill    ? Fraction(1)                    : Fraction(0)); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { FrameworkPowers::form(parent, layout);
                                                                   Pool = createLineEdit(parent, layout, "Points in the pool?", std::mem_fn(&Power::numeric));
                                                                   ctrl = createLineEdit(parent, layout, "Control cost?",       std::mem_fn(&Power::numeric));
                                                                   time = createComboBox(parent, layout, "Powers Can be Changed?", {
                                                                                             "VPP Can Only Be Changed Between Adventures",
                                                                                             "VPP Can Only Be Changed Between Scenes",
                                                                                             "Full Action",
                                                                                             "Powers Can be Changed As A Half-Phase Action",
                                                                                             "Powers Can be Changed As A Zero Phase Action" });
                                                                   skll = createCheckBox(parent, layout, "No Skill Roll Required");
                                                                   givn = createCheckBox(parent, layout, "VPP Can Only Be Changed In A Given Circumstance");
                                                                   circ = createLineEdit(parent, layout, "Circumtances?");
                                                                   how  = createCheckBox(parent, layout, "Character Has No Choice Regarding How Powers Change");
                                                                   whn2 = createCheckBox(parent, layout, "Character Has No Choice Regarding When Powers Change");
                                                                   clss = createComboBox(parent, layout, "Limited Class Of Powers?", {
                                                                                             "Unlimited",
                                                                                             "Slightly Limited Class Of Powers",
                                                                                             "Limited Class Of Powers",
                                                                                             "Very Limited Class Of Powers" });
                                                                   what = createLineEdit(parent, layout, "What Class?");
                                                                   one  = createComboBox(parent, layout, "One Type Of Power?", {
                                                                                         "All Powers",
                                                                                         "Very Useful Power",
                                                                                         "Useful Power",
                                                                                         "Almost Useless Power" });
                                                                   powr = createLineEdit(parent, layout, "What Power?");
                                                                 }
    Fraction lim() override                                      { return ((v._time >= 0 && v._time <= 1) ? 1 - Fraction(1, 2) * v._time : Fraction(0)) +
                                                                          (v._given                       ? Fraction(1, 2)               : Fraction(0)) +
                                                                          (v._how          ? Fraction(1, 2)              : Fraction(0)) +
                                                                          (v._when2        ? Fraction(1, 2)              : Fraction(0)) +
                                                                          ((v._class == 1) ? Fraction(1, 4)              : Fraction(0)) +
                                                                          ((v._class == 2) ? Fraction(1, 2)              : Fraction(0)) +
                                                                          ((v._class == 3) ? Fraction(1)                 : Fraction(0)) +
                                                                          ((v._one == 1)   ? Fraction(1, 2)              : Fraction(0)) +
                                                                          ((v._one == 2)   ? Fraction(1)                 : Fraction(0)) +
                                                                          ((v._one == 3)   ? Fraction(1, Fraction(1, 2)) : Fraction(0)); }
    Points points(bool noStore = false) override                 { if (!noStore) store();
                                                                   return (v._control + 1_cp) / 2; }
    void     restore() override                                  { vars s = v;
                                                                   FrameworkPowers::restore();
                                                                   Pool->setText(QString("%1").arg(s._pool));
                                                                   ctrl->setText(QString("%1").arg(s._control));
                                                                   time->setCurrentIndex(s._time);
                                                                   skll->setChecked(s._noSkill);
                                                                   givn->setChecked(s._given);
                                                                   circ->setText(s._circ);
                                                                   how->setChecked(s._how);
                                                                   whn2->setChecked(s._when2);
                                                                   clss->setCurrentIndex(v._class);
                                                                   what->setText(v._what);
                                                                   one->setCurrentIndex(v._one);
                                                                   powr->setText(v._power);
                                                                   v = s;
                                                                 }
    void     store() override                                    { FrameworkPowers::store();
                                                                   v._pool    = Pool->text().toInt();
                                                                   v._control = ctrl->text().toInt();
                                                                   v._time    = time->currentIndex();
                                                                   v._noSkill = skll->isChecked();
                                                                   v._given   = givn->isChecked();
                                                                   v._circ    = circ->text();
                                                                   v._how     = how->isChecked();
                                                                   v._when2   = whn2->isChecked();
                                                                   v._class   = clss->currentIndex();
                                                                   v._what    = what->text();
                                                                   v._one     = one->currentIndex();
                                                                   v._power   = powr->text();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = FrameworkPowers::toJson();
                                                                   obj["pool"]    = v._pool;
                                                                   obj["control"] = v._control;
                                                                   obj["time"]    = v._time;
                                                                   obj["noSkill"] = v._noSkill;
                                                                   obj["given"]   = v._given;
                                                                   obj["circ"]    = v._circ;
                                                                   obj["how"]     = v._how;
                                                                   obj["when2"]   = v._when2;
                                                                   obj["class"]   = v._class;
                                                                   obj["what"]    = v._what;
                                                                   obj["one"]     = v._one;
                                                                   obj["power"]   = v._power;
                                                                   return obj;
                                                                 }

    Points display(int& row, QTableWidget* tbl) override {
        Points points = 0_cp;
        QFont font = tbl->font();
        QFont italic = font;
        italic.setItalic(true);
        int r = 1;
        auto limit = limitations() - lim();
        auto advtg = advantages() - adv();
        auto modif = modifiers();
        for (const auto& pe: FrameworkPowers::super()._powers) {
            if (pe == nullptr) continue;
            QString descr = pe->description();
            if (descr == "-") descr = "";
            pe->parent(this);
            for (const auto& mod: pe->advantagesList()) {
                if (mod->isAdder()) descr += "; (+" + QString("%1").arg(mod->points(Modifier::NoStore).points) + " pts) ";
                else descr += "; (+" + mod->fraction(true).toString() + ") ";
                descr += mod->description(false);
            }
            for (const auto& mod: pe->limitationsList()) descr += "; (-" + mod->fraction(Modifier::NoStore).abs().toString() + ") " + mod->description(false);
            Fraction pts(pe->real(advtg, modif, limit).points);
            if (pts.toInt() == 0 && !descr.isEmpty()) pts = Fraction(1);
#ifndef ISHSC
            Sheet::ref().setCell(tbl, row, 0, "", font);
            Sheet::ref().setCell(tbl, row, 1, pe->nickname(), italic);
            if (descr.isEmpty()) Sheet::ref().setCell(tbl, row, 2, "", font, Sheet::WordWrap);
            else Sheet::ref().setCell(tbl, row, 2, QString("%1) [%3] %2").arg(r).arg(descr).arg(pts.toInt()), font, Sheet::WordWrap);
#endif
            QString end = pe->end();
            if (end == "-") end = "";
#ifndef ISHSC
            Sheet::ref().setCell(tbl, row, 3, end, font);
#endif
            pe->row(row);
            ++row;
            if (!descr.isEmpty()) ++r;
        }
        return points;
    }

    Points display(QString& out) override {
        Points points = 0_cp;
        int r = 1;
        auto limit = limitations() - lim();
        auto advtg = advantages() - adv();
        auto modif = modifiers();
        for (const auto& pe: FrameworkPowers::super()._powers) {
            QString descr = pe->description();
            if (descr == "-") descr = "";
            pe->parent(this);
            for (const auto& mod: pe->advantagesList()) {
                if (mod->isAdder()) descr += "; (+" + QString("%1").arg(mod->points(Modifier::NoStore).points) + " pts) ";
                else descr += "; (+" + mod->fraction(true).toString() + ") ";
                descr += mod->description(false);
            }
            for (const auto& mod: pe->limitationsList()) descr += "; (-" + mod->fraction(Modifier::NoStore).abs().toString() + ") " + mod->description(false);
            Fraction pts(0);
            if (!descr.isEmpty()) {
                pts = Fraction(pe->real(advtg, modif, limit).points);
                if (pts.toInt() == 0 && !descr.isEmpty()) pts = Fraction(1);
                out += "\t";
                out += QString("%1) [%3] %4%2").arg(r).arg(descr).arg(pts.toInt()).arg(pe->nickname().isEmpty() ? "" : "(" + pe->nickname() + ") ");
                QString end = pe->end();
                if (end == "-") end = "";
                out += QString("%1").arg(end.isEmpty() ? "" : " [" + end + "]");
            }
            out += "\n";
            if (!descr.isEmpty()) ++r;
        }
        return points;
    }

private:
    struct vars {
        int     _pool    = 0;
        int     _control = 0;
        int     _time    = -1;
        bool    _noSkill = false;
        bool    _given   = false;
        QString _circ    = "";
        bool    _how     = false;
        bool    _when2   = false;
        int     _class   = -1;
        QString _what    = "";
        int     _one     = -1;
        QString _power   = "";
    } v;

    QLineEdit* Pool = nullptr;
    QLineEdit* ctrl = nullptr;
    QComboBox* time = nullptr;
    QCheckBox* skll = nullptr;
    QCheckBox* givn = nullptr;
    QLineEdit* circ = nullptr;
    QCheckBox* how = nullptr;
    QCheckBox* whn2 = nullptr;
    QComboBox* clss = nullptr;
    QLineEdit* what = nullptr;
    QComboBox* one = nullptr;
    QLineEdit* powr = nullptr;

    QString optOut(bool showEND) {
        QString res;
        if (showEND) res = nickname().isEmpty() ? "" : nickname() + " ";
        res += QString("VPP (%1 cp, %2 cp Control)").arg(v._pool).arg(v._control);
        QStringList time {
            "VPP Can Only Be Changed Between Adventures", "VPP Can Only Be Changed Between Scenes", "Full Action", "Powers Can be Changed As A Half-Phase Action",
                "Powers Can be Changed As A Zero Phase Action" };
        if (v._time >= 0 && v._time != 2) res += "; " + time[v._time];
        if (v._noSkill) res += "; No Skill Roll Required";
        if (v._given) res += "; Powers Can Only Be Changed When " + v._circ;
        if (v._how) res += "; Character Has No Control Over How Powers Change";
        if (v._when2) res += "; Character Has No Control Over When Powers Change";
        QStringList clss {
            "", "Slightly Limited Class Of Powers",
                "Limited Class Of Powers",
                "Very Limited Class Of Powers" };
        if (v._class > 0) res += "; " + clss[v._class] + " - " + v._what;
        if (v._one > 0) res += "; One Type Of Power - " + v._power;
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

#endif // FRAMEWORKPOWERS_H
