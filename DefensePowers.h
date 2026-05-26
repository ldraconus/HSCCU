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
    Barrier(const QJsonObject& json): AllPowers(json) { v.mLength   = json["length"].toInt(0);
                                                        v.mHeight   = json["height"].toInt(0);
                                                        v.mThick    = json["thick"].toInt(0);
                                                        v.mBody     = json["body"].toInt(0);
                                                        v.mPD       = json["pd"].toInt(0);
                                                        v.mED       = json["ed"].toInt(0);
                                                        v.mPut      = json["put"].toBool(false);
                                                        v.mConfig   = json["config"].toBool(false);
                                                        v.mAnchor   = json["anchor"].toBool(false);
                                                        v.mTrans    = json["trans"].toInt(0);
                                                        v.mTo       = json["to"].toString();
                                                        v.mEnglobe  = json["englobe"].toBool(false);
                                                        v.mFeedback = json["feedback"].toBool(false);
                                                        v.mRestr    = json["restr"].toBool(false);
                                                        v.mWhat     = json["what"].toString();
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

    Fraction adv() override                                      { return (v.mConfig      ? Fraction(1, 4) : Fraction(0)) +
                                                                          ((v.mTrans > 0) ? v.mTrans * Fraction(1, 2) : Fraction(0)); }
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
    Fraction lim() override                                      { return (v.mEnglobe  ? Fraction(1, 4) : Fraction(0)) +
                                                                          (v.mFeedback ? Fraction(1)    : Fraction(0)) +
                                                                          (v.mRestr    ? Fraction(1, 4) : Fraction(0)); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   Points defCost = 0_cp;
#ifndef ISHSC
                                                                   if (Sheet::ref().character().hasTakesNoSTUN()) defCost = 3_cp * (3_cp * ((v.mPD + v.mED) / 2));
                                                                   else
#endif
                                                                      defCost = 3_cp * ((v.mPD + v.mED) / 2);
                                                                   return 3_cp + (v.mLength - 1) + (v.mHeight - 1) + (v.mThick - 1) + v.mBody +
                                                                           defCost + (v.mAnchor ? BaseCost10 : 0_cp); }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   length->setText(QString("%1").arg(s.mLength));
                                                                   height->setText(QString("%1").arg(s.mHeight));
                                                                   thick->setText(QString("%1").arg(s.mThick));
                                                                   body->setText(QString("%1").arg(s.mBody));
                                                                   pd->setText(QString("%1").arg(s.mPD));
                                                                   ed->setText(QString("%1").arg(s.mED));
                                                                   put->setChecked(s.mPut);
                                                                   config->setChecked(s.mConfig);
                                                                   anchor->setChecked(s.mAnchor);
                                                                   trans->setCurrentIndex(s.mTrans);
                                                                   to->setText(v.mTo);
                                                                   englobe->setChecked(s.mEnglobe);
                                                                   feedback->setChecked(s.mFeedback);
                                                                   restr->setChecked(s.mRestr);
                                                                   what->setText(s.mWhat);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mLength   = length->text().toInt();
                                                                   v.mHeight   = height->text().toInt();
                                                                   v.mThick    = thick->text().toInt();
                                                                   v.mBody     = body->text().toInt();
                                                                   v.mPD       = pd->text().toInt();
                                                                   v.mED       = ed->text().toInt();
                                                                   v.mPut      = put->isChecked();
                                                                   v.mConfig   = config->isChecked();
                                                                   v.mAnchor   = anchor->isChecked();
                                                                   v.mTrans    = trans->currentIndex();
                                                                   v.mTo       = to->text();
                                                                   v.mEnglobe  = englobe->isChecked();
                                                                   v.mFeedback = feedback->isChecked();
                                                                   v.mRestr    = restr->isChecked();
                                                                   v.mWhat     = what->text();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["length"]   = v.mLength;
                                                                   obj["height"]   = v.mHeight;
                                                                   obj["thick"]    = v.mHeight;
                                                                   obj["body"]     = v.mBody;
                                                                   obj["pd"]       = v.mPD;
                                                                   obj["ed"]       = v.mED;
                                                                   obj["put"]      = v.mPut;
                                                                   obj["config"]   = v.mConfig;
                                                                   obj["anchor"]   = v.mAnchor;
                                                                   obj["trans"]    = v.mTrans;
                                                                   obj["to"]       = v.mTo;
                                                                   obj["englobe"]  = v.mEnglobe;
                                                                   obj["feedback"] = v.mFeedback;
                                                                   obj["rstr"]     = v.mRestr;
                                                                   obj["what"]     = v.mWhat;
                                                                   return obj;
                                                                 }

    int rPD() override { return hasModifier("Nonresistant Defenses") ? 0 : v.mPD; }
    int rED() override { return hasModifier("Nonresistant Defenses") ? 0 : v.mED; }
    int PD() override  { return hasModifier("Nonresistant Defenses") ? v.mPD : 0; }
    int ED() override  { return hasModifier("Nonresistant Defenses") ? v.mED : 0; }
    int place() override { return v.mPut ? 2 : 0; }

private:
    struct vars {
        int     mLength   = 0;
        int     mHeight   = 0;
        int     mThick    = 0;
        int     mBody     = 0;
        int     mPD       = 0;
        int     mED       = 0;
        bool    mPut      = false;
        bool    mConfig   = false;
        bool    mAnchor   = false;
        int     mTrans    = -1;
        QString mTo       = "";
        bool    mEnglobe  = false;
        bool    mFeedback = false;
        bool    mRestr    = false;
        QString mWhat     = "";
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
        if (v.mLength < 1 || v.mHeight < 1 || v.mThick < 1 || v.mPD + v.mED + v.mBody < 1 ||
            v.mTrans == -1 || (v.mTrans > 0 && v.mTo.isEmpty()) ||
            (v.mRestr && v.mWhat.isEmpty())) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        bool notResistant = hasModifier("Nonresistant Defenses");
        res += QString("%1m x %2m x %3m").arg(v.mLength).arg(v.mHeight).arg(Fraction(v.mThick, 2).toString()) + " Barrier";
        res += QString("; %1 %3PD/%2 %3ED").arg(v.mPD).arg(v.mED).arg(notResistant ? "" : "r");
        res += QString("; %1 BODY").arg(v.mBody);
        if (v.mConfig) res += "; Configurable";
        if (v.mAnchor) res += "; Non-Anchored";
        if (v.mTrans > 0) {
            if (v.mTrans == 1) res += "; ▲One-Way Transparent To " + v.mTo;
            else res += ";  ▲One-Way Transparent To Everything";
        }
        if (v.mEnglobe) res += "; Cannot Englobe";
        if (v.mFeedback) res += "; Feedback";
        if (v.mRestr) res += "; Restricted Shape (" + v.mWhat + ")";
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
    DamageNegation(const QJsonObject& json): AllPowers(json) { v.mDC      = json["dc"].toInt(0);
                                                               v.mAgainst = json["against"].toInt(0);
                                                               v.mWhat    = json["what"].toString();
                                                               v.mResist  = json["rests"].toBool(false);
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
    Fraction lim() override                                      { return (v.mResist  ? Fraction(1, 4) : Fraction(0)); }
    Points points(bool noStore = false) override                 { if (!noStore) store();
                                                                   return (
#ifndef ISHSC
                                                                              Sheet::ref().character().hasTakesNoSTUN() ? BaseCost15 :
#endif
                                                                                  BaseCost5) * v.mDC; }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dc->setText(QString("%1").arg(s.mDC));
                                                                   against->setCurrentIndex(s.mAgainst);
                                                                   what->setText(v.mWhat);
                                                                   resist->setChecked(s.mResist);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mDC      = dc->text().toInt();
                                                                   v.mAgainst = against->currentIndex();
                                                                   v.mWhat    = what->text();
                                                                   v.mResist  = resist->isChecked();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dc"]      = v.mDC;
                                                                   obj["against"] = v.mAgainst;
                                                                   obj["what"]    = v.mWhat;
                                                                   obj["resist"]  = v.mResist;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     mDC      = 0;
        int     mAgainst = -1;
        QString mWhat    = "";
        bool    mResist  = false;
    } v;

    QLineEdit* dc = nullptr;
    QComboBox* against = nullptr;
    QLineEdit* what = nullptr;
    QCheckBox* resist = nullptr;

    QString optOut(bool showEND) {
        if (v.mDC < 1 || v.mAgainst < 0 ||
            (v.mAgainst == 3 && v.mWhat.isEmpty())) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("-%1 DC").arg(v.mDC) + " Damage Negation▲ Against";
        switch (v.mAgainst) {
        case 0: res += " Physical Attacks"; break;
        case 1: res += " Energy Attacks";   break;
        case 2: res += " Mental Attacks";   break;
        case 3: res += " " + v.mWhat;       break;
        }
        if (v.mResist) res += "; Nonresistant";
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
    DamageResistance(const QJsonObject& json): AllPowers(json) { v.mPerc    = json["perc"].toInt(0);
                                                                 v.mAgainst = json["against"].toInt(0);
                                                                 v.mWhat    = json["what"].toString();
                                                                 v.mResist  = json["rests"].toBool(false);
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
                                                                          ((v.mResist || v.mAgainst > 1) ? r[v.mPerc + 1] : n[v.mPerc + 1]); }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   perc->setCurrentIndex(s.mPerc);
                                                                   against->setCurrentIndex(s.mAgainst);
                                                                   what->setText(v.mWhat);
                                                                   resist->setChecked(s.mResist);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mPerc    = perc->currentIndex();
                                                                   v.mAgainst = against->currentIndex();
                                                                   v.mWhat    = what->text();
                                                                   v.mResist  = resist->isChecked();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["perc"]    = v.mPerc;
                                                                   obj["against"] = v.mAgainst;
                                                                   obj["what"]    = v.mWhat;
                                                                   obj["resist"]  = v.mResist;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     mPerc    = -1;
        int     mAgainst = -1;
        QString mWhat    = "";
        bool    mResist  = false;
    } v;

    QComboBox* perc = nullptr;
    QComboBox* against = nullptr;
    QLineEdit* what = nullptr;
    QCheckBox* resist = nullptr;

    QString optOut(bool showEND) {
        if (v.mPerc < 0 || v.mAgainst < 0 ||
            (v.mAgainst == 3 && v.mWhat.isEmpty())) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1").arg(v.mPerc * 25) + "% Damage Resistance▲ Against"; // NOLINT
        switch (v.mAgainst) {
        case 0: res += " Physical Attacks"; break;
        case 1: res += " Energy Attacks";   break;
        case 2: res += " Mental Attacks";   break;
        case 3: res += " " + v.mWhat;       break;
        }
        if (v.mResist) res += "; Resistant";
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
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
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
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
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
    KnockbackResistance(const QJsonObject& json): AllPowers(json)   { v.mPts = json["pts"].toInt(0);
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
                                                                   return v.mPts * (
#ifndef ISHSC
                                                                              Sheet::ref().character().hasTakesNoSTUN() ? 3_cp :
#endif
                                                                                  1_cp); }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   pts->setText(QString("%1").arg(s.mPts));
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mPts = pts->text().toInt();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["pts"] = v.mPts;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int mPts = 0;
    } v;

    QLineEdit* pts = nullptr;

    QString optOut(bool showEND) {
        if (v.mPts < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("-%1").arg(v.mPts) + "m Knockback Resistance";
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
    MentalDefense(const QJsonObject& json): AllPowers(json) { v.mDef = json["def"].toInt(0);
                                                              v.mPut = json["put"].toInt(1);
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
                                                                   return v.mDef * (
#ifndef ISHSC
                                                                              Sheet::ref().character().hasTakesNoSTUN() ? 3_cp :
#endif
                                                                                  1_cp); }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   def->setText(QString("%1").arg(s.mDef));
                                                                   put->setCurrentIndex(s.mPut);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mDef = def->text().toInt();
                                                                   v.mPut = put->currentIndex();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["def"] = v.mDef;
                                                                   obj["put"] = v.mPut;
                                                                   return obj;
                                                                 }

    int MD() override    { return v.mDef; }
    int place() override { return v.mPut; }

private:
    struct vars {
        int mDef = 0;
        int mPut = 0;
    } v;

    QLineEdit* def = nullptr;
    QComboBox* put = nullptr;

    QString optOut(bool showEND) {
        if (v.mDef < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1").arg(v.mDef) + " Mental Defense";
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
    PowerDefense(const QJsonObject& json): AllPowers(json) { v.mDef = json["def"].toInt(0);
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
                                                                   return v.mDef * (
#ifndef ISHSC
                                                                              Sheet::ref().character().hasTakesNoSTUN() ? 3_cp :
#endif
                                                                                  1_cp); }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   def->setText(QString("%1").arg(s.mDef));
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mDef = def->text().toInt();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["def"] = v.mDef;
                                                                   return obj;
                                                                 }

    int PowD() override { return v.mDef; }

private:
    struct vars {
        int mDef = 0;
    } v;

    QLineEdit* def = nullptr;

    QString optOut(bool showEND) {
        if (v.mDef < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1").arg(v.mDef) + " Power Defense";
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
    ResistantDefense(const QJsonObject& json): AllPowers(json) { v.mPD      = json["pd"].toInt(0);
                                                                 v.mED      = json["ed"].toInt(0);
                                                                 v.mImperm  = json["imperm"].toBool(false);
                                                                 v.mProtect = json["protect"].toBool(false);
                                                                 v.mPut     = json["put"].toInt(0);
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
                                                                   if (Sheet::ref().character().hasTakesNoSTUN()) defCost = (3_cp * (3 * ((v.mPD + v.mED) + 1)) / 2);
                                                                   else
#endif
                                                                       defCost = (3_cp * ((v.mPD + v.mED + 1) / 2));
                                                                   return defCost + (v.mProtect ? 10_cp : 0_cp); } // NOLINT
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   put->setCurrentIndex(s.mPut);
                                                                   pd->setText(QString("%1").arg(s.mPD));
                                                                   ed->setText(QString("%1").arg(s.mED));
                                                                   imperm->setChecked(s.mImperm);
                                                                   protect->setChecked(s.mProtect);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mPut     = put->currentIndex();
                                                                   v.mPD      = pd->text().toInt();
                                                                   v.mED      = ed->text().toInt();
                                                                   v.mImperm  = imperm->isChecked();
                                                                   v.mProtect = protect->isChecked();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["pd"]      = v.mPD;
                                                                   obj["put"]     = v.mPut;
                                                                   obj["ed"]      = v.mED;
                                                                   obj["imperm"]  = v.mImperm;
                                                                   obj["protect"] = v.mProtect;
                                                                   return obj;
                                                                 }

    int rPD() override { return hasModifier("Nonresistant Defenses") ? 0 : v.mPD; }
    int rED() override { return hasModifier("Nonresistant Defenses") ? 0 : v.mED; }
    int PD() override  { return hasModifier("Nonresistant Defenses") ? v.mPD : 0; }
    int ED() override  { return hasModifier("Nonresistant Defenses") ? v.mED : 0; }
    int place() override { return v.mPut; }

private:
    struct vars {
        int     mPD      = 0;
        int     mED      = 0;
        int     mPut     = -1;
        bool    mImperm  = false;
        bool    mProtect = false;
    } v;

    QLineEdit* pd = nullptr;
    QLineEdit* ed = nullptr;
    QComboBox* put = nullptr;
    QCheckBox* imperm = nullptr;
    QCheckBox* protect = nullptr;

    QString optOut(bool showEND) {
        if (v.mPD + v.mED < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1 %3PD/%2 %3ED").arg(v.mPD).arg(v.mED).arg(hasModifier("Nonresistant Defense") ? "" : "r") +
               QString(" %1Defense").arg(hasModifier("Nonresistant Defense") ? "" : "Resistant ");
        if (v.mImperm) res += "; Impermeable";
        if (v.mProtect) res += "; Protects Carried Items";
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
