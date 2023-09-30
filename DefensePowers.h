#ifndef DEFENSE_POWERS_H
#define DEFENSE_POWERS_H

#include "powers.h"
#ifndef ISHSC
#include "sheet.h"
#endif

constexpr Points BaseCost5  = 5_cp;
constexpr Points BaseCost10 = 10_cp;
constexpr Points BaseCost15 = 15_cp;

class Barrier: public AllPowers {
public:
    Barrier(): AllPowers("Barrier")                   { }
    Barrier(const Barrier& s): AllPowers(s)           { }
    Barrier(Barrier&& s): AllPowers(s)                { }
    Barrier(const QJsonObject& json): AllPowers(json) { v._length   = json["length"].toInt(0);
                                                        v._height   = json["height"].toInt(0);
                                                        v._thick    = json["thick"].toInt(0);
                                                        v._body     = json["body"].toInt(0);
                                                        v._pd       = json["pd"].toInt(0);
                                                        v._ed       = json["ed"].toInt(0);
                                                        v._put      = json["put"].toBool(false);
                                                        v._config   = json["config"].toBool(false);
                                                        v._anchor   = json["anchor"].toBool(false);
                                                        v._trans    = json["trans"].toInt(0);
                                                        v._to       = json["to"].toString();
                                                        v._englobe  = json["englobe"].toBool(false);
                                                        v._feedback = json["feedback"].toBool(false);
                                                        v._restr    = json["restr"].toBool(false);
                                                        v._what     = json["what"].toString();
                                                      }
    ~Barrier() override { }
    Barrier& operator=(const Barrier& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    Barrier& operator=(Barrier&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return (v._config      ? Fraction(1, 4) : Fraction(0)) +
                                                                          ((v._trans > 0) ? v._trans * Fraction(1, 2) : Fraction(0)); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   length   = createLineEdit(parent, layout, "Meters long?", std::mem_fn(&Power::numeric));
                                                                   height   = createLineEdit(parent, layout, "Meters high?", std::mem_fn(&Power::numeric));
                                                                   thick    = createLineEdit(parent, layout, Fraction(1, 2).toString() + " Meters Thickness",
                                                                                             std::mem_fn(&Power::numeric));
                                                                   body     = createLineEdit(parent, layout, "Body?", std::mem_fn(&Power::numeric));
                                                                   pd       = createLineEdit(parent, layout, "Resistant PD?", std::mem_fn(&Power::numeric));
                                                                   ed       = createLineEdit(parent, layout, "Resistant ED?", std::mem_fn(&Power::numeric));
                                                                   put      = createCheckBox(parent, layout, "Add to secondary");
                                                                   config   = createCheckBox(parent, layout, "Configurable");
                                                                   anchor   = createCheckBox(parent, layout, "Non-Anchored");
                                                                   trans    = createComboBox(parent, layout, "▲One-way Transparent To?", { "",
                                                                                                                                           "A Special Effect",
                                                                                                                                           "Everything" });
                                                                   to       = createLineEdit(parent, layout, "To?");
                                                                   englobe  = createCheckBox(parent, layout, "Cannot Englobe Target");
                                                                   feedback = createCheckBox(parent, layout, "Cannot Englobe Target");
                                                                   restr    = createCheckBox(parent, layout, "Restricted Shape");
                                                                   what     = createLineEdit(parent, layout, "To What Shape?");
                                                                 }
    Fraction lim() override                                      { return (v._englobe  ? Fraction(1, 4) : Fraction(0)) +
                                                                          (v._feedback ? Fraction(1)    : Fraction(0)) +
                                                                          (v._restr    ? Fraction(1, 4) : Fraction(0)); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   Points defCost = 0_cp;
#ifndef ISHSC
                                                                   if (Sheet::ref().character().hasTakesNoSTUN()) defCost = 3_cp * (3_cp * ((v._pd + v._ed) / 2));
                                                                   else
#endif
                                                                      defCost = 3_cp * ((v._pd + v._ed) / 2);
                                                                   return 3_cp + (v._length - 1) + (v._height - 1) + (v._thick - 1) + v._body +
                                                                           defCost + (v._anchor ? BaseCost10 : 0_cp); }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   length->setText(QString("%1").arg(s._length));
                                                                   height->setText(QString("%1").arg(s._height));
                                                                   thick->setText(QString("%1").arg(s._thick));
                                                                   body->setText(QString("%1").arg(s._body));
                                                                   pd->setText(QString("%1").arg(s._pd));
                                                                   ed->setText(QString("%1").arg(s._ed));
                                                                   put->setChecked(s._put);
                                                                   config->setChecked(s._config);
                                                                   anchor->setChecked(s._anchor);
                                                                   trans->setCurrentIndex(s._trans);
                                                                   to->setText(v._to);
                                                                   englobe->setChecked(s._englobe);
                                                                   feedback->setChecked(s._feedback);
                                                                   restr->setChecked(s._restr);
                                                                   what->setText(s._what);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._length   = length->text().toInt();
                                                                   v._height   = height->text().toInt();
                                                                   v._thick    = thick->text().toInt();
                                                                   v._body     = body->text().toInt();
                                                                   v._pd       = pd->text().toInt();
                                                                   v._ed       = ed->text().toInt();
                                                                   v._put      = put->isChecked();
                                                                   v._config   = config->isChecked();
                                                                   v._anchor   = anchor->isChecked();
                                                                   v._trans    = trans->currentIndex();
                                                                   v._to       = to->text();
                                                                   v._englobe  = englobe->isChecked();
                                                                   v._feedback = feedback->isChecked();
                                                                   v._restr    = restr->isChecked();
                                                                   v._what     = what->text();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["length"]   = v._length;
                                                                   obj["height"]   = v._height;
                                                                   obj["thick"]    = v._height;
                                                                   obj["body"]     = v._body;
                                                                   obj["pd"]       = v._pd;
                                                                   obj["ed"]       = v._ed;
                                                                   obj["put"]      = v._put;
                                                                   obj["config"]   = v._config;
                                                                   obj["anchor"]   = v._anchor;
                                                                   obj["trans"]    = v._trans;
                                                                   obj["to"]       = v._to;
                                                                   obj["englobe"]  = v._englobe;
                                                                   obj["feedback"] = v._feedback;
                                                                   obj["rstr"]     = v._restr;
                                                                   obj["what"]     = v._what;
                                                                   return obj;
                                                                 }

    int rPD() override { return hasModifier("Nonresistant Defenses") ? 0 : v._pd; }
    int rED() override { return hasModifier("Nonresistant Defenses") ? 0 : v._ed; }
    int PD() override  { return hasModifier("Nonresistant Defenses") ? v._pd : 0; }
    int ED() override  { return hasModifier("Nonresistant Defenses") ? v._ed : 0; }
    int place() override { return v._put ? 2 : 0; }

private:
    struct vars {
        int     _length   = 0;
        int     _height   = 0;
        int     _thick    = 0;
        int     _body     = 0;
        int     _pd       = 0;
        int     _ed       = 0;
        bool    _put      = false;
        bool    _config   = false;
        bool    _anchor   = false;
        int     _trans    = -1;
        QString _to       = "";
        bool    _englobe  = false;
        bool    _feedback = false;
        bool    _restr    = false;
        QString _what     = "";
    } v;

    QLineEdit* length = nullptr;
    QLineEdit* height = nullptr;
    QLineEdit* thick = nullptr;
    QLineEdit* pd = nullptr;
    QLineEdit* ed = nullptr;
    QCheckBox* put = nullptr;
    QLineEdit* body = nullptr;
    QCheckBox* config = nullptr;
    QCheckBox* anchor = nullptr;
    QComboBox* trans = nullptr;
    QLineEdit* to = nullptr;
    QCheckBox* englobe = nullptr;
    QCheckBox* feedback = nullptr;
    QCheckBox* restr = nullptr;
    QLineEdit* what = nullptr;

    QString optOut(bool showEND) {
        if (v._length < 1 || v._height < 1 || v._thick < 1 || v._pd + v._ed + v._body < 1 ||
            v._trans == -1 || (v._trans > 0 && v._to.isEmpty()) ||
            (v._restr && v._what.isEmpty())) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        bool notResistant = hasModifier("Nonresistant Defenses");
        res += QString("%1m x %2m x %3m").arg(v._length).arg(v._height).arg(Fraction(v._thick, 2).toString()) + " Barrier";
        res += QString("; %1 %3PD/%2 %3ED").arg(v._pd).arg(v._ed).arg(notResistant ? "" : "r");
        res += QString("; %1 BODY").arg(v._body);
        if (v._config) res += "; Configurable";
        if (v._anchor) res += "; Non-Anchored";
        if (v._trans > 0) {
            if (v._trans == 1) res += "; ▲One-Way Transparent To " + v._to;
            else res += ";  ▲One-Way Transparent To Everything";
        }
        if (v._englobe) res += "; Cannot Englobe";
        if (v._feedback) res += "; Feedback";
        if (v._restr) res += "; Restricted Shape (" + v._what + ")";
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

class DamageNegation: public AllPowers {
public:
    DamageNegation(): AllPowers("Damage Negation▲")          { }
    DamageNegation(const DamageNegation& s): AllPowers(s)    { }
    DamageNegation(DamageNegation&& s): AllPowers(s)         { }
    DamageNegation(const QJsonObject& json): AllPowers(json) { v._dc      = json["dc"].toInt(0);
                                                               v._against = json["against"].toInt(0);
                                                               v._what    = json["what"].toString();
                                                               v._resist  = json["rests"].toBool(false);
                                                             }
    ~DamageNegation() override { }

    DamageNegation& operator=(const DamageNegation& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    DamageNegation& operator=(DamageNegation&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   dc      = createLineEdit(parent, layout, "Damage Classes?", std::mem_fn(&Power::numeric));
                                                                   against = createComboBox(parent, layout, "Against?", { "Physical Attacks",
                                                                                                                          "Energy Attacks",
                                                                                                                          "Mental Attacks",
                                                                                                                          "A Special Effect" });
                                                                   what    = createLineEdit(parent, layout, "Special Effect?");
                                                                   resist  = createCheckBox(parent, layout, "Nonresitant");
                                                                 }
    Fraction lim() override                                      { return (v._resist  ? Fraction(1, 4) : Fraction(0)); }
    Points points(bool noStore = false) override                 { if (!noStore) store();
                                                                   return (
#ifndef ISHSC
                                                                              Sheet::ref().character().hasTakesNoSTUN() ? BaseCost15 :
#endif
                                                                                  BaseCost5) * v._dc; }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dc->setText(QString("%1").arg(s._dc));
                                                                   against->setCurrentIndex(s._against);
                                                                   what->setText(v._what);
                                                                   resist->setChecked(s._resist);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._dc      = dc->text().toInt();
                                                                   v._against = against->currentIndex();
                                                                   v._what    = what->text();
                                                                   v._resist  = resist->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dc"]      = v._dc;
                                                                   obj["against"] = v._against;
                                                                   obj["what"]    = v._what;
                                                                   obj["resist"]  = v._resist;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     _dc      = 0;
        int     _against = -1;
        QString _what    = "";
        bool    _resist  = false;
    } v;

    QLineEdit* dc = nullptr;
    QComboBox* against = nullptr;
    QLineEdit* what = nullptr;
    QCheckBox* resist = nullptr;

    QString optOut(bool showEND) {
        if (v._dc < 1 || v._against < 0 ||
            (v._against == 3 && v._what.isEmpty())) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("-%1 DC").arg(v._dc) + " Damage Negation▲ Against";
        switch (v._against) {
        case 0: res += " Physical Attacks"; break;
        case 1: res += " Energy Attacks";   break;
        case 2: res += " Mental Attacks";   break;
        case 3: res += " " + v._what;       break;
        }
        if (v._resist) res += "; Nonresistant";
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

class DamageResistance: public AllPowers {
public:
    DamageResistance(): AllPowers("Damage Resistance▲")        { }
    DamageResistance(const DamageResistance& s): AllPowers(s)  { }
    DamageResistance(DamageResistance&& s): AllPowers(s)       { }
    DamageResistance(const QJsonObject& json): AllPowers(json) { v._perc    = json["perc"].toInt(0);
                                                                 v._against = json["against"].toInt(0);
                                                                 v._what    = json["what"].toString();
                                                                 v._resist  = json["rests"].toBool(false);
                                                               }
    ~DamageResistance() override { }

    DamageResistance& operator=(const DamageResistance& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    DamageResistance& operator=(DamageResistance&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   perc    = createComboBox(parent, layout, "Damage Percentage0?", { "25%", "50%", "75%" });
                                                                   against = createComboBox(parent, layout, "Against?", { "Physical Attacks",
                                                                                                                          "Energy Attacks",
                                                                                                                          "Mental Attacks",
                                                                                                                          "A Special Effect" });
                                                                   what    = createLineEdit(parent, layout, "Special Effect?");
                                                                   resist  = createCheckBox(parent, layout, "Resistant");
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   QList<Points> n { 0_cp, 10_cp, 20_cp, 30_cp }; // NOLINT
                                                                   QList<Points> r { 0_cp, 15_cp, 30_cp, 60_cp }; // NOLINT
                                                                   return (
#ifndef ISHSC
                                                                              Sheet::ref().character().hasTakesNoSTUN() ? 3 :
#endif
                                                                                  1) *
                                                                          ((v._resist || v._against > 1) ? r[v._perc + 1] : n[v._perc + 1]); }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   perc->setCurrentIndex(s._perc);
                                                                   against->setCurrentIndex(s._against);
                                                                   what->setText(v._what);
                                                                   resist->setChecked(s._resist);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._perc    = perc->currentIndex();
                                                                   v._against = against->currentIndex();
                                                                   v._what    = what->text();
                                                                   v._resist  = resist->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["perc"]    = v._perc;
                                                                   obj["against"] = v._against;
                                                                   obj["what"]    = v._what;
                                                                   obj["resist"]  = v._resist;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     _perc    = -1;
        int     _against = -1;
        QString _what    = "";
        bool    _resist  = false;
    } v;

    QComboBox* perc = nullptr;
    QComboBox* against = nullptr;
    QLineEdit* what = nullptr;
    QCheckBox* resist = nullptr;

    QString optOut(bool showEND) {
        if (v._perc < 0 || v._against < 0 ||
            (v._against == 3 && v._what.isEmpty())) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1").arg(v._perc * 25) + "% Damage Resistance▲ Against"; // NOLINT
        switch (v._against) {
        case 0: res += " Physical Attacks"; break;
        case 1: res += " Energy Attacks";   break;
        case 2: res += " Mental Attacks";   break;
        case 3: res += " " + v._what;       break;
        }
        if (v._resist) res += "; Resistant";
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

class Deflection: public AllPowers {
public:
    Deflection(): AllPowers("Deflection▲")        { }
    Deflection(const Deflection& s): AllPowers(s)  { }
    Deflection(Deflection&& s): AllPowers(s)       { }
    Deflection(const QJsonObject& json): AllPowers(json) { }
    ~Deflection() override { }

    Deflection& operator=(const Deflection& s) {
        if (this != &s) {
            AllPowers::operator=(s);
        }
        return *this;
    }
    Deflection& operator=(Deflection&& s) {
        AllPowers::operator=(s);
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override                 { if (!noStore) store();
                                                                   return
#ifndef ISHSC
                                                                       Sheet::ref().character().hasTakesNoSTUN() ? 60_cp : // NOLINT
#endif
                                                                       20_cp; } // NOLINT
    void     restore() override                                  { AllPowers::restore();
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   return obj;
                                                                 }

private:
    QString optOut(bool showEND) {
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "Deflection▲";
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

class FlashDefense: public AllPowers {
public:
    FlashDefense(): AllPowers("Flash Defense")             { }
    FlashDefense(const FlashDefense& s): AllPowers(s)      { }
    FlashDefense(FlashDefense&& s): AllPowers(s)           { }
    FlashDefense(const QJsonObject& json): AllPowers(json) { v._def = json["def"].toInt(0);
                                                           }
    ~FlashDefense() override { }
    FlashDefense& operator=(const FlashDefense& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    FlashDefense& operator=(FlashDefense&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return (showEND ? "" : "") + optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   def = createLineEdit(parent, layout, "Defense?", std::mem_fn(&Power::numeric));
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override                 { if (!noStore) store();
                                                                   return v._def * (
#ifndef ISHSC
                                                                              Sheet::ref().character().hasTakesNoSTUN() ? 3_cp :
#endif
                                                                                  1_cp); }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   def->setText(QString("%1").arg(s._def));
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._def = def->text().toInt();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["def"] = v._def;
                                                                   return obj;
                                                                 }

    int FD() override { return v._def; }

private:
    struct vars {
        int _def = 0;
    } v;

    QLineEdit* def = nullptr;

    QString optOut(bool showEND) {
        if (v._def < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1").arg(v._def) + " Flash Defense";
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

class KnockbackResistance: public AllPowers {
public:
    KnockbackResistance(): AllPowers("Knockback Resistance")        { }
    KnockbackResistance(const KnockbackResistance& s): AllPowers(s) { }
    KnockbackResistance(KnockbackResistance&& s): AllPowers(s)      { }
    KnockbackResistance(const QJsonObject& json): AllPowers(json)   { v._pts = json["pts"].toInt(0);
                                                                    }
    ~KnockbackResistance() override { }
    KnockbackResistance& operator=(const KnockbackResistance& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    KnockbackResistance& operator=(KnockbackResistance&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   pts = createLineEdit(parent, layout, "Points?", std::mem_fn(&Power::numeric));
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v._pts * (
#ifndef ISHSC
                                                                              Sheet::ref().character().hasTakesNoSTUN() ? 3_cp :
#endif
                                                                                  1_cp); }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   pts->setText(QString("%1").arg(s._pts));
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._pts = pts->text().toInt();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["pts"] = v._pts;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int _pts = 0;
    } v;

    QLineEdit* pts = nullptr;

    QString optOut(bool showEND) {
        if (v._pts < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("-%1").arg(v._pts) + "m Knockback Resistance";
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

class MentalDefense: public AllPowers {
public:
    MentalDefense(): AllPowers("Mental Defense")            { }
    MentalDefense(const MentalDefense& s): AllPowers(s)     { }
    MentalDefense(MentalDefense&& s): AllPowers(s)          { }
    MentalDefense(const QJsonObject& json): AllPowers(json) { v._def = json["def"].toInt(0);
                                                              v._put = json["put"].toInt(1);
                                                            }
    ~MentalDefense() override { }
    MentalDefense& operator=(const MentalDefense& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    MentalDefense& operator=(MentalDefense&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   def = createLineEdit(parent, layout, "Defense?", std::mem_fn(&Power::numeric));
                                                                   put = createComboBox(parent, layout, "Add to?", { "Nothing", "Primary", "Secondary" });
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override                 { if (!noStore) store();
                                                                   return v._def * (
#ifndef ISHSC
                                                                              Sheet::ref().character().hasTakesNoSTUN() ? 3_cp :
#endif
                                                                                  1_cp); }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   def->setText(QString("%1").arg(s._def));
                                                                   put->setCurrentIndex(s._put);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._def = def->text().toInt();
                                                                   v._put = put->currentIndex();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["def"] = v._def;
                                                                   obj["put"] = v._put;
                                                                   return obj;
                                                                 }

    int MD() override    { return v._def; }
    int place() override { return v._put; }

private:
    struct vars {
        int _def = 0;
        int _put = 0;
    } v;

    QLineEdit* def = nullptr;
    QComboBox* put = nullptr;

    QString optOut(bool showEND) {
        if (v._def < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1").arg(v._def) + " Mental Defense";
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

class PowerDefense: public AllPowers {
public:
    PowerDefense(): AllPowers("Power Defense")             { }
    PowerDefense(const PowerDefense& s): AllPowers(s)      { }
    PowerDefense(PowerDefense&& s): AllPowers(s)           { }
    PowerDefense(const QJsonObject& json): AllPowers(json) { v._def = json["def"].toInt(0);
                                                               }
    ~PowerDefense() override { }
    PowerDefense& operator=(const PowerDefense& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    PowerDefense& operator=(PowerDefense&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   def = createLineEdit(parent, layout, "Defense?", std::mem_fn(&Power::numeric));
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override                 { if (!noStore) store();
                                                                   return v._def * (
#ifndef ISHSC
                                                                              Sheet::ref().character().hasTakesNoSTUN() ? 3_cp :
#endif
                                                                                  1_cp); }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   def->setText(QString("%1").arg(s._def));
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._def = def->text().toInt();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["def"] = v._def;
                                                                   return obj;
                                                                 }

    int PowD() override { return v._def; }

private:
    struct vars {
        int _def = 0;
    } v;

    QLineEdit* def = nullptr;

    QString optOut(bool showEND) {
        if (v._def < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1").arg(v._def) + " Power Defense";
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

class ResistantDefense: public AllPowers {
public:
    ResistantDefense(): AllPowers("Resistant Defense")         { }
    ResistantDefense(const ResistantDefense& s): AllPowers(s)  { }
    ResistantDefense(ResistantDefense&& s): AllPowers(s)       { }
    ResistantDefense(const QJsonObject& json): AllPowers(json) { v._pd      = json["pd"].toInt(0);
                                                                 v._ed      = json["ed"].toInt(0);
                                                                 v._imperm  = json["imperm"].toBool(false);
                                                                 v._protect = json["protect"].toBool(false);
                                                                 v._put     = json["put"].toInt(0);
                                                               }
    ~ResistantDefense() override { }
    ResistantDefense& operator=(const ResistantDefense& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    ResistantDefense& operator=(ResistantDefense&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   pd      = createLineEdit(parent, layout, "Resistant PD?", std::mem_fn(&Power::numeric));
                                                                   ed      = createLineEdit(parent, layout, "Resistant ED?", std::mem_fn(&Power::numeric));
                                                                   put     = createComboBox(parent, layout, "Add to?", { "Nothing", "Primary", "Secondary" });
                                                                   imperm  = createCheckBox(parent, layout, "Impermeable");
                                                                   protect = createCheckBox(parent, layout, "Protects Carried Items");
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override                 { if (!noStore) store();
                                                                   Points defCost = 0_cp;
#ifndef ISHSC
                                                                   if (Sheet::ref().character().hasTakesNoSTUN()) defCost = (3_cp * (3 * ((v._pd + v._ed) + 1)) / 2);
                                                                   else
#endif
                                                                       defCost = (3_cp * ((v._pd + v._ed + 1) / 2));
                                                                   return defCost + (v._protect ? 10_cp : 0_cp); } // NOLINT
    void     restore() override                                  { vars s = v;                                                                   
                                                                   AllPowers::restore();
                                                                   put->setCurrentIndex(s._put);
                                                                   pd->setText(QString("%1").arg(s._pd));
                                                                   ed->setText(QString("%1").arg(s._ed));
                                                                   imperm->setChecked(s._imperm);
                                                                   protect->setChecked(s._protect);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._put     = put->currentIndex();
                                                                   v._pd      = pd->text().toInt();
                                                                   v._ed      = ed->text().toInt();
                                                                   v._imperm  = imperm->isChecked();
                                                                   v._protect = protect->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["pd"]      = v._pd;
                                                                   obj["put"]     = v._put;
                                                                   obj["ed"]      = v._ed;
                                                                   obj["imperm"]  = v._imperm;
                                                                   obj["protect"] = v._protect;
                                                                   return obj;
                                                                 }

    int rPD() override { return hasModifier("Nonresistant Defenses") ? 0 : v._pd; }
    int rED() override { return hasModifier("Nonresistant Defenses") ? 0 : v._ed; }
    int PD() override  { return hasModifier("Nonresistant Defenses") ? v._pd : 0; }
    int ED() override  { return hasModifier("Nonresistant Defenses") ? v._ed : 0; }
    int place() override { return v._put; }

private:
    struct vars {
        int     _pd      = 0;
        int     _ed      = 0;
        int     _put     = -1;
        bool    _imperm  = false;
        bool    _protect = false;
    } v;

    QLineEdit* pd = nullptr;
    QLineEdit* ed = nullptr;
    QComboBox* put = nullptr;
    QCheckBox* imperm = nullptr;
    QCheckBox* protect = nullptr;

    QString optOut(bool showEND) {
        if (v._pd + v._ed < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1 %3PD/%2 %3ED").arg(v._pd).arg(v._ed).arg(hasModifier("Nonresistant Defense") ? "" : "r") +
               QString(" %1Defense").arg(hasModifier("Nonresistant Defense") ? "" : "Resistant ");
        if (v._imperm) res += "; Impermeable";
        if (v._protect) res += "; Protects Carried Items";
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
