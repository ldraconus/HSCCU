#ifndef BODYAFFECTINGPOWERS_H
#define BODYAFFECTINGPOWERS_H

#include "powers.h"

class DensityIncrease: public AllPowers {
public:
    DensityIncrease(): AllPowers("Density Increase")          { }
    DensityIncrease(const DensityIncrease& s): AllPowers(s)   { }
    DensityIncrease(DensityIncrease&& s): AllPowers(s)        { }
    DensityIncrease(const QJsonObject& json): AllPowers(json) { v._levels = json["levels"].toInt(0);
                                                                v._nopded = json["nopded"].toBool(false);
                                                                v._nostr  = json["nostr"].toBool(false);
                                                              }
    virtual DensityIncrease& operator=(const DensityIncrease& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual DensityIncrease& operator=(DensityIncrease&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   levels = createLineEdit(parent, layout, "Levels of Density Increase?", std::mem_fn(&Power::numeric));
                                                                   nopded = createCheckBox(parent, layout, "No Increased PD/ED");
                                                                   nostr  = createCheckBox(parent, layout, "No Increased STR");
                                                                 }
    Fraction lim() override                                      { return (v._nopded ? Fraction(1, 4) : Fraction(0)) +
                                                                          (v._nostr  ? Fraction(1)    : Fraction(0)); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v._levels * 4_cp; }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   levels->setText(QString("%1").arg(s._levels));
                                                                   nopded->setChecked(s._nopded);
                                                                   nostr->setChecked(s._nostr);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._levels = levels->text().toInt();
                                                                   v._nopded = nopded->isChecked();
                                                                   v._nostr  = nostr->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["levels"] = v._levels;
                                                                   obj["nopded"] = v._nopded;
                                                                   obj["nostr"]  = v._nostr;
                                                                   return obj;
                                                                 }

    int str() override { return v._nostr  ? 0 : 5 * v._levels; }
    int rPD() override { return v._nopded ? 0 : v._levels; }
    int rED() override { return v._nopded ? 0 : v._levels; }

private:
    struct vars {
        int  _levels = 0;
        bool _nopded = false;
        bool _nostr = false;
    } v;

    QLineEdit* levels;
    QCheckBox* nopded;
    QCheckBox* nostr;

    QString optOut(bool showEND) {
        if (v._levels < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("+%1").arg(v._levels) + " Levels Density Increase (";
        res += QString("Mass: %L1 kg").arg((int) pow(2, v._levels) * 100);
        if (!v._nostr) res += QString("; +%1 STR").arg(5 * v._levels);
        res += QString("; -%1 KB").arg(2 * v._levels);
        if (!v._nopded) res += QString("; +%1 PD/+%1 ED").arg(v._levels);
        res += ")";
        if (v._nopded) res += "; No Increased PD/ED";
        if (v._nostr) res += "; No Increased STR";
        return res;
    }

    void numeric(int) override {
        QString txt = levels->text();
        PowerDialog::ref().updateForm();
        if (txt.isEmpty() || isNumber(txt)) return;
        levels->undo();
    }
};

class Desolidification: public AllPowers {
public:
    Desolidification(): AllPowers("Desolidifcationϴ")          { }
    Desolidification(const Desolidification& s): AllPowers(s)  { }
    Desolidification(Desolidification&& s): AllPowers(s)       { }
    Desolidification(const QJsonObject& json): AllPowers(json) { v._solid   = json["solid"].toBool(false);
                                                                 v._protect = json["protect"].toBool(false);
                                                               }
    virtual Desolidification& operator=(const Desolidification& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Desolidification& operator=(Desolidification&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   solid   = createCheckBox(parent, layout, "Cannot Pass THrough Solid Objects");
                                                                   protect = createCheckBox(parent, layout, "Doesn't Protect Against Damage");
                                                                 }
    Fraction lim() override                                      { return (v._solid ? Fraction(1, 2) : Fraction(0)) +
                                                                          (v._protect  ? Fraction(1)    : Fraction(0)); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return 40_cp; }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   solid->setChecked(s._solid);
                                                                   protect->setChecked(s._protect);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._solid   = solid->isChecked();
                                                                   v._protect = protect->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["solid"]   = v._solid;
                                                                   obj["protect"] = v._protect;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        bool _solid = false;
        bool _protect = false;
    } v;

    QCheckBox* solid;
    QCheckBox* protect;

    QString optOut(bool showEND) {
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "Desolidificationϴ";
        if (v._solid) res += "; Cannot Pass Thought Solid Objects";
        if (v._protect) res += "; Doesn't Protect Against Damage";
        return res;
    }
};

class Duplication: public AllPowers {
public:
    Duplication(): AllPowers("Duplication▲")              { }
    Duplication(const Duplication& s): AllPowers(s)       { }
    Duplication(Duplication&& s): AllPowers(s)            { }
    Duplication(const QJsonObject& json): AllPowers(json) { v._levels   = json["levels"].toInt(0);
                                                            v._altered  = json["altered"].toInt(0);
                                                            v._easy     = json["easy"].toInt(0);
                                                            v._ranged   = json["ranged"].toBool(false);
                                                            v._rapid    = json["rapid"].toInt(0);
                                                            v._recom    = json["recom"].toBool(false);
                                                            v._feedback = json["feedback"].toInt(0);
                                                            v._average  = json["average"].toBool(false);
                                                          }
    virtual Duplication& operator=(const Duplication& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Duplication& operator=(Duplication&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return ((v._altered == 1) ? Fraction(1, 4) : Fraction(0)) +
                                                                          ((v._altered == 2) ? Fraction(1, 2) : Fraction(0)) +
                                                                          ((v._altered == 3) ? Fraction(1)    : Fraction(0)) +
                                                                          (v._ranged         ? Fraction(1, 2) : Fraction(0)) +
                                                                          ((v._rapid - 1) * Fraction(1, 4)); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   levels   = createLineEdit(parent, layout, "Points in duplication?", std::mem_fn(&Power::numeric));
                                                                   altered  = createComboBox(parent, layout, "Altered Duplicates?", { "All Identical",
                                                                                                                                      Fraction(1, 4).toString() + " points different",
                                                                                                                                      Fraction(1, 2).toString() + " points different",
                                                                                                                                      "All points can be different" });
                                                                   easy     = createComboBox(parent, layout, "Easy Recombination?", { "Action",
                                                                                                                                      "Half Phase Action",
                                                                                                                                      "Zero Phase Action" });
                                                                   ranged   = createCheckBox(parent, layout, "Ranged Recombination");
                                                                   rapid    = createLineEdit(parent, layout, "Rapid duplication?", std::mem_fn(&Power::numeric));
                                                                   recom    = createCheckBox(parent, layout, "Cannot Recombine");
                                                                   feedback = createComboBox(parent, layout, "Feedback?", { "No Feedback",
                                                                                                                            "Damage to Base feeds back to duplicates",
                                                                                                                            "STUN to any affects all",
                                                                                                                            "BODY and STUN to any affects all" });
                                                                   average  = createCheckBox(parent, layout, "No Averaging");
                                                                 }
    Fraction lim() override                                      { return ((v._feedback == 1) ? Fraction(1, 4) : Fraction(0)) +
                                                                          ((v._feedback == 2) ? Fraction(1, 2) : Fraction(0)) +
                                                                          ((v._feedback == 3) ? Fraction(1)    : Fraction(0)); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v._levels + v._easy * 5_cp; }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   levels->setText(QString("%1").arg(s._levels));
                                                                   altered->setCurrentIndex(s._altered);
                                                                   easy->setCurrentIndex(s._easy);
                                                                   ranged->setChecked(s._ranged);
                                                                   rapid->setText(QString("%1").arg(s._rapid));
                                                                   feedback->setCurrentIndex(s._feedback);
                                                                   recom->setChecked(s._recom);
                                                                   average->setChecked(s._average);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._levels   = levels->text().toInt();
                                                                   v._altered  = altered->currentIndex();
                                                                   v._easy     = easy->currentIndex();
                                                                   v._ranged   = ranged->isChecked();
                                                                   v._rapid    = rapid->text().toInt();
                                                                   v._feedback = feedback->currentIndex();
                                                                   v._recom    = recom->isChecked();
                                                                   v._average  = average->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["levels"]   = v._levels;
                                                                   obj["altered"]  = v._altered;
                                                                   obj["easy"]     = v._easy;
                                                                   obj["ranged"]   = v._ranged;
                                                                   obj["rapid"]    = v._rapid;
                                                                   obj["feedback"] = v._feedback;
                                                                   obj["recom"]    = v._recom;
                                                                   obj["average"]  = v._average;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int  _levels   = 0;
        int  _altered  = -1;
        int  _easy     = -1;
        bool _ranged   = false;
        int  _rapid    = 0;
        bool _recom    = false;
        int  _feedback = -1;
        bool _average  = false;
    } v;

    QLineEdit* levels;
    QComboBox* altered;
    QComboBox* easy;
    QCheckBox* ranged;
    QLineEdit* rapid;
    QCheckBox* recom;
    QComboBox* feedback;
    QCheckBox* average;

    QString optOut(bool showEND) {
        if (v._levels < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("+%1").arg(v._levels) + " cp in Duplication▲ (" + QString("%1").arg(v._levels * 5) + " cp duplicates)";
        QStringList altered { "",
                              Fraction(1, 4).toString() + " points different",
                              Fraction(1, 2).toString() + " points different",
                              "All points can be different" };
        if (v._altered > 0) res += "; Altered Duplicates (" + altered[v._altered] + ")";
        if (v._easy > 0) res += QString("; Easy Recombination ") + ((v._easy == 1) ? "(Half Phase)" : "(Zero Phase)");
        if (v._ranged) res += "; Ranged Recombination";
        if (v._rapid != 0) res += "; Rapid Duplication (" + QString("%1 per Phase)").arg((int) pow(2, v._rapid));
        if (v._recom) res += "; Cannot Recombine";
        QStringList feedback { "",
                               "Damage to Base feeds back to duplicates",
                               "STUN to any affects all",
                               "BODY and STUN to any affects all" };
        if (v._feedback > 0) res += "; Feedback (" + feedback[v._feedback] + ")";
        if (v._average) res += "; No Averaging";
        return res;
    }

    void numeric(int) override {
        QString txt = levels->text();
        PowerDialog::ref().updateForm();
        if (txt.isEmpty() || isNumber(txt)) return;
        levels->undo();
    }
};

class ExtraLimbs: public AllPowers {
public:
    ExtraLimbs(): AllPowers("Extra Limbs")               { }
    ExtraLimbs(const ExtraLimbs& s): AllPowers(s)        { }
    ExtraLimbs(ExtraLimbs&& s): AllPowers(s)             { }
    ExtraLimbs(const QJsonObject& json): AllPowers(json) { v._limbs   = json["limbs"].toInt(0);
                                                           v._limited = json["limited"].toBool(false);
                                                         }
    virtual ExtraLimbs& operator=(const ExtraLimbs& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual ExtraLimbs& operator=(ExtraLimbs&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   limbs   = createLineEdit(parent, layout, "Nunmber of Extra Limbs?", std::mem_fn(&Power::numeric));
                                                                   limited = createCheckBox(parent, layout, "No Increased PD/ED");
                                                                 }
    Fraction lim() override                                      { return (v._limited ? Fraction(1, 4) : Fraction(0)); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return 5_cp; }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   limbs->setText(QString("%1").arg(s._limbs));
                                                                   limited->setChecked(s._limited);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._limbs   = limbs->text().toInt();
                                                                   v._limited = limited->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["limbs"]   = v._limbs;
                                                                   obj["limited"] = v._limited;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int  _limbs   = 0;
        bool _limited = false;
    } v;

    QLineEdit* limbs;
    QCheckBox* limited;

    QString optOut(bool showEND) {
        if (v._limbs < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("+%1").arg(v._limbs) + " Extra Limbs";
        if (v._limited) res += QString("; Limited Manipulation");
        return res;
    }

    void numeric(int) override {
        QString txt = limbs->text();
        PowerDialog::ref().updateForm();
        if (txt.isEmpty() || isNumber(txt)) return;
        limbs->undo();
    }
};

class Growth: public AllPowers {
public:
    Growth(): AllPowers("Growth")                    { }
    Growth(const Growth& s): AllPowers(s)            { }
    Growth(Growth&& s): AllPowers(s)                 { }
    Growth(const QJsonObject& json): AllPowers(json) { v._size = json["size"].toInt(0);
                                                     }
    virtual Growth& operator=(const Growth& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Growth& operator=(Growth&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   size = createComboBox(parent, layout, "Size?", { "Large", "Enormous", "Huge", "Gigantic",
                                                                                                                    "Gargantuan", "Colossal" });
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   QList<Points> points { 0_cp, 25_cp, 50_cp, 90_cp, 120_cp, 150_cp, 215_cp };
                                                                   return points[v._size + 1]; }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   size->setCurrentIndex(s._size);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._size = size->currentIndex();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["size"] = v._size;
                                                                   return obj;
                                                                 }

    sizeMods& growthStats() override { return SizeMods[v._size]; }

private:
    struct vars {
        int _size = -1;
    } v;

    QComboBox* size;

    QList<sizeMods> SizeMods {
        { 15,  5,  5,  3,  3,  3,  6,  1, 12,  -6, 0, "Phys. Comp.: Large (Infrequently, Slightly Impairing)" },
        { 30, 10, 10,  6,  6,  6, 12,  3, 24, -12, 0, "Phys. Comp.: Enormous (Frequently, Slightly Impairing)" },
        { 45, 15, 15,  9,  9,  9, 18,  7, 36, -18, 1, "Phys. Comp.: Huge (Frequently, Greatly Impairing)" },
        { 60, 20, 20, 12, 12, 12, 24, 15, 48, -24, 2, "Phys. Comp.: Gigantic (Frequently, Greatly Impairing)" },
        { 75, 25, 25, 15, 15, 14, 30, 31, 60, -30, 4, "Phys. Comp.: Gargantuan (Very Frequently, Greatly Impairing)" },
        { 90, 30, 30, 18, 18, 18, 36, 63, 72, -36, 8, "Phys. Comp.: Colossal (All The Time, Greatly Impairing)" }
    };

    QString optOut(bool showEND) {
        if (v._size < 0) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        QStringList size { "Large", "Enormous", "Huge", "Gigantic",
                           "Gargantuan", "Colossal" };
        res += size[v._size] + " Growth (";
        res += QString("+%1 STR").arg(SizeMods[v._size]._STR);
        res += QString("; +%1 CON").arg(SizeMods[v._size]._CON);
        res += QString("; +%1 PRE").arg(SizeMods[v._size]._PRE);
        res += QString("; +%1 PD").arg(SizeMods[v._size]._PD);
        res += QString("; +%1 ED").arg(SizeMods[v._size]._ED);
        res += QString("; +%1 BODY").arg(SizeMods[v._size]._BODY);
        res += QString("; +%1 STUN").arg(SizeMods[v._size]._STUN);
        res += QString("; +%1m Reach").arg(SizeMods[v._size]._reach);
        res += QString("; +%1m Running").arg(SizeMods[v._size]._running);
        res += QString("; %1 KB Res").arg(SizeMods[v._size]._KBRes);
        if (SizeMods[v._size]._aoeRad != 0) res += QString("; +%1m AOE Radius HA").arg(SizeMods[v._size]._aoeRad);
        res += "; " + SizeMods[v._size]._comp;
        return res + ")";
    }
};

class Multiform: public AllPowers {
public:
    Multiform(): AllPowers("Multiform")                 { }
    Multiform(const Multiform& s): AllPowers(s)         { }
    Multiform(Multiform&& s): AllPowers(s)              { }
    Multiform(const QJsonObject& json): AllPowers(json) { v._points  = json["points"].toInt(0);
                                                          v._form    = json["form"].toString();
                                                          v._mult    = json["mult"].toInt(0);
                                                          v._instant = json["instant"].toBool(false);
                                                          v._loss    = json["loss"].toInt(0);
                                                          v._revert  = json["revert"].toInt(0);
                                                        }
    virtual Multiform& operator=(const Multiform& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Multiform& operator=(Multiform&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return ((v._revert == 5) ? Fraction(1)                        : Fraction(0)) +
                                                                          ((v._revert == 4) ? Fraction(1, 2)                     : Fraction(0)) +
                                                                          ((v._loss >= 1) ? (2 - (v._loss - 1) * Fraction(1, 4)) : Fraction(0)); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   point   = createLineEdit(parent, layout, "Points in alternate forms?", std::mem_fn(&Power::numeric));
                                                                   frm     = createLineEdit(parent, layout, "Alternate form?");
                                                                   mult    = createLineEdit(parent, layout, "Doublings of forms", std::mem_fn(&Power::numeric));
                                                                   instant = createCheckBox(parent, layout, "Instant Change");
                                                                   loss    = createComboBox(parent, layout, "Personality Loss Starts After?",
                                                                                            { "Never", "1 Turn", "1 Minute", "5 Minutes", "20 Minutes", "1 Hour", "6 Hours",
                                                                                              "1 Day", "1 Week" });
                                                                   revert  = createComboBox(parent, layout, "Reversion, True Form Is?", { "No Reversion",
                                                                                                                                          "Less Powerful",
                                                                                                                                          "Slightly Less Powerful",
                                                                                                                                          "As Powerful"
                                                                                                                                          "Slightly More Powerful",
                                                                                                                                          "More Powerful" });
                                                                 }
    Fraction lim() override                                      { return ((v._revert == 1) ? Fraction(1)    : Fraction(0)) +
                                                                          ((v._revert == 2) ? Fraction(1, 2) : Fraction(0)); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return (v._points + 2_cp) / 5 + (v._instant ? 5_cp : 0_cp) + v._mult * 5_cp; }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   point->setText(QString("%1").arg(s._points));
                                                                   frm->setText(s._form);
                                                                   mult->setText(QString("%1").arg(s._mult));
                                                                   loss->setCurrentIndex(s._loss);
                                                                   instant->setChecked(s._instant);
                                                                   revert->setCurrentIndex(s._revert);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._points  = point->text().toInt();
                                                                   v._form    = frm->text();
                                                                   v._mult    = mult->text().toInt();
                                                                   v._loss    = loss->currentIndex();
                                                                   v._instant = instant->isChecked();
                                                                   v._revert  = revert->currentIndex();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["points"]  = v._points;
                                                                   obj["form"]    = v._form;
                                                                   obj["mult"]    = v._mult;
                                                                   obj["loss"]    = v._loss;
                                                                   obj["instant"] = v._instant;
                                                                   obj["revert"]  = v._revert;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     _points  = 0;
        QString _form    = "";
        int     _mult    = 0;
        int     _loss    = -1;
        bool    _instant = false;
        int     _revert  = -1;
    } v;

    QLineEdit* point;
    QLineEdit* frm;
    QLineEdit* mult;
    QComboBox* loss;
    QCheckBox* instant;
    QComboBox* revert;

    QString optOut(bool showEND) {
        if (v._points < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("+%1").arg(v._points) + " CP Multiform Into " + v._form;
        if (v._mult > 0) res += QString("; x%1 Forms").arg((int) pow(2, v._mult));
        QStringList loss {"",  "1 Turn", "1 Minute", "5 Minutes", "20 Minutes", "1 Hour", "6 Hours", "1 Day", "1 Week" };
        if (v._loss >= 1) res += "; Personality Loss Rolls Start Ater " + loss[v._loss];
        if (v._instant) res += "; Instant Change";
        if (v._revert >= 1) res += "; Revert To True Form If Stunned Or KO";
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

class ShapeShift: public AllPowers {
public:
    ShapeShift(): AllPowers("Shape Shift")               { }
    ShapeShift(const ShapeShift& s): AllPowers(s)        { }
    ShapeShift(ShapeShift&& s): AllPowers(s)             { }
    ShapeShift(const QJsonObject& json): AllPowers(json) { v._change   = json["change"].toInt(0);
                                                           v._forms    = json["forms"].toString();
                                                           v._sight    = json["sight"].toBool(false);
                                                           v._hearing  = json["hearing"].toBool(false);
                                                           v._touch    = json["touch"].toBool(false);
                                                           v._mental   = json["mental"].toBool(false);
                                                           v._radio    = json["radio"].toBool(false);
                                                           v._smell    = json["smell"].toBool(false);
                                                           v._clair    = json["clair"].toBool(false);
                                                           v._spatial  = json["spatial"].toBool(false);
                                                           v._cellular = json["cellular"].toBool(false);
                                                           v._imitate  = json["imitate"].toBool(false);
                                                           v._instant  = json["instant"].toBool(false);
                                                           v._makeover = json["makeover"].toBool(false);
                                                           v._body     = json["body"].toBool(false);}
    virtual ShapeShift& operator=(const ShapeShift& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual ShapeShift& operator=(ShapeShift&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   change   = createComboBox(parent, layout, "Can Change Into?", { "A single Shape",
                                                                                                                                   "Up to four shapes",
                                                                                                                                   "A Limited Group",
                                                                                                                                   "Any Shape"
                                                                                                                                 });
                                                                   forms    = createLineEdit(parent, layout, "Shape(s)?");
                                                                   sight    = createCheckBox(parent, layout, "Affects Sight");
                                                                   hearing  = createCheckBox(parent, layout, "Affects Hearing");
                                                                   touch    = createCheckBox(parent, layout, "Affects Touch");
                                                                   mental   = createCheckBox(parent, layout, "Affects Mental");
                                                                   radio    = createCheckBox(parent, layout, "Affects Radio");
                                                                   smell    = createCheckBox(parent, layout, "Affects Smell/Taste");
                                                                   clair    = createCheckBox(parent, layout, "Affects Clairsentience");
                                                                   spatial  = createCheckBox(parent, layout, "Affects Spatial Awareness");
                                                                   cellular = createCheckBox(parent, layout, "Cellular");
                                                                   imitate  = createCheckBox(parent, layout, "Imitation");
                                                                   instant  = createCheckBox(parent, layout, "Instant Change");
                                                                   makeover = createCheckBox(parent, layout, "Makeover");
                                                                   body     = createCheckBox(parent, layout, "Affects Body Only");
                                                                 }
    Fraction lim() override                                      { return v._body ? Fraction(1, 4) : Fraction(0);
                                                                 }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   QList<Points> change { 0_cp, 0_cp, 3_cp, 5_cp, 10_cp };
                                                                   Points pts = change[v._change + 1];
                                                                   pts += v._sight    ?  8_cp : 0_cp;
                                                                   pts += v._hearing  ?  5_cp : 0_cp;
                                                                   pts += v._touch    ?  5_cp : 0_cp;
                                                                   pts += v._mental   ?  2_cp : 0_cp;
                                                                   pts += v._radio    ?  2_cp : 0_cp;
                                                                   pts += v._smell    ?  2_cp : 0_cp;
                                                                   pts += v._clair    ?  3_cp : 0_cp;
                                                                   pts += v._spatial  ?  3_cp : 0_cp;
                                                                   pts += v._cellular ? 10_cp : 0_cp;
                                                                   pts += v._instant  ?  5_cp : 0_cp;
                                                                   pts += v._makeover ?  5_cp : 0_cp;
                                                                   return pts;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   change->setCurrentIndex(s._change);
                                                                   forms->setText(s._forms);
                                                                   sight->setChecked(s._sight);
                                                                   hearing->setChecked(s._hearing);
                                                                   touch->setChecked(s._touch);
                                                                   mental->setChecked(s._mental);
                                                                   radio->setChecked(s._radio);
                                                                   smell->setChecked(s._smell);
                                                                   clair->setChecked(s._clair);
                                                                   spatial->setChecked(s._spatial);
                                                                   cellular->setChecked(s._cellular);
                                                                   instant->setChecked(s._instant);
                                                                   makeover->setChecked(s._makeover);
                                                                   body->setChecked(s._body);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._change   = change->currentIndex();
                                                                   v._forms    = forms->text();
                                                                   v._sight    = sight->isChecked();
                                                                   v._hearing  = hearing->isChecked();
                                                                   v._touch    = touch->isChecked();
                                                                   v._mental   = mental->isChecked();
                                                                   v._radio    = radio->isChecked();
                                                                   v._smell    = smell->isChecked();
                                                                   v._clair    = clair->isChecked();
                                                                   v._spatial  = spatial->isChecked();
                                                                   v._cellular = cellular->isChecked();
                                                                   v._instant  = instant->isChecked();
                                                                   v._makeover = makeover->isChecked();
                                                                   v._body     = body->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["change"]   = v._change;
                                                                   obj["forms"]    = v._forms;
                                                                   obj["sight"]    = v._sight;
                                                                   obj["hearing"]  = v._hearing;
                                                                   obj["touch"]    = v._touch;
                                                                   obj["mental"]   = v._mental;
                                                                   obj["radio"]    = v._radio;
                                                                   obj["smell"]    = v._smell;
                                                                   obj["clair"]    = v._clair;
                                                                   obj["spatial"]  = v._spatial;
                                                                   obj["cellular"] = v._cellular;
                                                                   obj["instant"]  = v._instant;
                                                                   obj["makeover"] = v._makeover;
                                                                   obj["body"]     = v._body;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     _change   = -1;
        QString _forms    = "";
        bool    _sight    = false;
        bool    _hearing  = false;
        bool    _touch    = false;
        bool    _mental   = false;
        bool    _radio    = false;
        bool    _smell    = false;
        bool    _sonar    = false;
        bool    _clair    = false;
        bool    _spatial  = false;
        bool    _cellular = false;
        bool    _imitate  = false;
        bool    _instant  = false;
        bool    _makeover = false;
        bool    _body     = false;
    } v;

    QComboBox* change;
    QLineEdit* forms;
    QCheckBox* sight;
    QCheckBox* hearing;
    QCheckBox* touch;
    QCheckBox* mental;
    QCheckBox* radio;
    QCheckBox* smell;
    QCheckBox* clair;
    QCheckBox* spatial;
    QCheckBox* cellular;
    QCheckBox* imitate;
    QCheckBox* instant;
    QCheckBox* makeover;
    QCheckBox* body;

    QString optOut(bool showEND) {
        if (v._change == -1 || (v._change < 3 && v._forms.isEmpty())) return "<incomplete>";
        if (!(v._sight || v._hearing || v._touch || v._mental || v._radio || v._smell || v._clair || v._spatial)) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "Shape Shift Into ";
        QStringList forms { "A Single Shape", "Four shapes", "A Limited Group", "Any Shape" };
        if (v._change < 3) res += forms[v._change] + " (" + v._forms + ")";
        else res += "Any Shape";
        res += " Affecting ";
        QStringList senses;
        if (v._sight) senses << "Sight";
        if (v._hearing) senses << "Hearing";
        if (v._touch) senses << "Touch";
        if (v._mental) senses << "Mental";
        if (v._radio) senses << "Radio";
        if (v._smell) senses << "Smell";
        if (v._clair) senses << "Clairsentience";
        if (v._spatial) senses << "Spatial Awareness";
        QString list = "";
        for (int i = 0; i < senses.count() - 1; ++i) {
            if (i != 0) list += ", ";
            list += senses[i];
        }
        if (list.isEmpty()) list = senses[0];
        else list += ", and " + senses.back();
        res += list;
        if (v._cellular) res += "; Cellular";
        if (v._imitate) res += "; Imitate";
        if (v._instant) res += "; Instant Change";
        if (v._makeover) res += "; Makeover";
        if (v._body) res += "; Affects Body Only";
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

class Shrinking: public AllPowers {
public:
    Shrinking(): AllPowers("Shrinking")                 { }
    Shrinking(const Shrinking& s): AllPowers(s)         { }
    Shrinking(Shrinking&& s): AllPowers(s)              { }
    Shrinking(const QJsonObject& json): AllPowers(json) { v._levels = json["levels"].toInt(0);
                                                          v._mass   = json["mass"].toBool(false);
                                                          v._perc   = json["perc"].toBool(false);
                                                        }
    virtual Shrinking& operator=(const Shrinking& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Shrinking& operator=(Shrinking&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(1, 2) * (v._mass + 1);
                                                                 }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   levels = createLineEdit(parent, layout, "Levels of Shrinking?", std::mem_fn(&Power::numeric));
                                                                   mass   = createComboBox(parent, layout, "No Mass Decrease?", { "No Mass Decrease",
                                                                                                                                  "Choice Mass Decrease" });
                                                                   perc   = createCheckBox(parent, layout, "Easily Percieved");
                                                                 }
    Fraction lim() override                                      { return (v._perc ? Fraction(1, 4) : Fraction(0));
                                                                 }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v._levels * 6_cp; }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   levels->setText(QString("%1").arg(s._levels));
                                                                   mass->setCurrentIndex(s._mass);
                                                                   perc->setChecked(s._perc);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._levels = levels->text().toInt();
                                                                   v._mass   = mass->currentIndex();
                                                                   v._perc   = perc->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["levels"] = v._levels;
                                                                   obj["mass"]   = v._mass;
                                                                   obj["perc"]   = v._perc;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int  _levels = 0;
        int  _mass   = -1;
        bool _perc   = false;
    } v;

    QLineEdit* levels;
    QComboBox* mass;
    QCheckBox* perc;

    QString optOut(bool showEND) {
        if (v._levels < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("+%1").arg(v._levels) + " Levels Shrinking (";
        res += QString("Height: %2m").arg(2.0 / pow(2.0, v._levels));
        if (v._mass == -1) {
            res += QString("; Mass: %1 kg").arg(1.0 / (8.0 * pow(2, v._levels)));
            res += QString("; +%1 KB").arg(6 * v._levels);
        }
        if (v._mass == 1) {
            res += QString("; Optionally %1 kg").arg(1.0 / (8.0 * pow(2, v._levels)));
            res += QString("; Optionally +%1 KB").arg(6 * v._levels);
        }
        if (!v._perc) res += QString("; -%1 PER Rolls Against").arg(2 * v._levels);
        res += ")";
        if (v._mass == 0) res += "; No Decreased Mass";
        else if (v._mass == 1) res += "; Choice To Decrease Mass";
        if (v._perc) res += "; Easily Perceived";
        return res;
    }

    void numeric(int) override {
        QString txt = levels->text();
        PowerDialog::ref().updateForm();
        if (txt.isEmpty() || isNumber(txt)) return;
        levels->undo();
    }
};

class Stretching: public AllPowers {
public:
    Stretching(): AllPowers("Stretching")                { }
    Stretching(const Stretching& s): AllPowers(s)        { }
    Stretching(Stretching&& s): AllPowers(s)             { }
    Stretching(const QJsonObject& json): AllPowers(json) { v._meters  = json["meters"].toInt(0);
                                                           v._distort = json["distort"].toInt(0);
                                                           v._space   = json["space"].toBool(false);
                                                           v._noncom  = json["noncom"].toInt(0);
                                                           v._direct  = json["direct"].toBool(false);
                                                           v._damage  = json["damage"].toBool(false);
                                                           v._limit   = json["limit"].toBool(false);
                                                           v._parts   = json["parts"].toString();
                                                           v._nonon   = json["nonon"].toBool(false);
                                                           v._cause   = json["cause"].toBool(false);
                                                           v._range   = json["range"].toBool(false);
                                                         }
    virtual Stretching& operator=(const Stretching& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Stretching& operator=(Stretching&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return (v._space ? Fraction(1, 4) : Fraction(0)); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   meters  = createLineEdit(parent, layout, "Meters of Stretching?", std::mem_fn(&Power::numeric));
                                                                   distort = createLineEdit(parent, layout, "Doubling One Dim/Halving Another Multiples?",
                                                                                            std::mem_fn(&Power::numeric));
                                                                   space   = createCheckBox(parent, layout, "Doesn't Cross Interveneing Space");
                                                                   noncom  = createLineEdit(parent, layout, "Noncombat Stretching Multiples?",
                                                                                            std::mem_fn(&Power::numeric));
                                                                   direct  = createCheckBox(parent, layout, "Always Direct");
                                                                   damage  = createCheckBox(parent, layout, "Cannot Do Damage");
                                                                   limit   = createCheckBox(parent, layout, "Limited Body Parts");
                                                                   parts   = createLineEdit(parent, layout, "Parts?");
                                                                   nonon   = createCheckBox(parent, layout, "No Noncombat Stretching");
                                                                   cause   = createCheckBox(parent, layout, "Only To Cause Damage");
                                                                   range   = createCheckBox(parent, layout, "Range Modifiers Apply");
                                                                 }
    Fraction lim() override                                      { return (v._direct ? Fraction(1, 4) : Fraction(0)) +
                                                                          (v._damage ? Fraction(1, 2) : Fraction(0)) +
                                                                          (v._limit  ? Fraction(1, 4) : Fraction(0)) +
                                                                          (v._nonon  ? Fraction(1, 4) : Fraction(0)) +
                                                                          (v._cause  ? Fraction(1, 2) : Fraction(0)) +
                                                                          (v._range  ? Fraction(1, 4) : Fraction(0)); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v._meters + 5_cp * v._distort + 5_cp * v._noncom; }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   meters->setText(QString("%1").arg(s._meters));
                                                                   distort->setText(QString("%1").arg(s._distort));
                                                                   space->setChecked(s._space);
                                                                   noncom->setText(QString("%1").arg(s._noncom));
                                                                   direct->setChecked(s._direct);
                                                                   damage->setChecked(s._damage);
                                                                   limit->setChecked(s._limit);
                                                                   parts->setText(s._parts);
                                                                   nonon->setChecked(s._nonon);
                                                                   cause->setChecked(s._cause);
                                                                   range->setChecked(s._range);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._meters  = meters->text().toInt();
                                                                   v._distort = distort->text().toInt();
                                                                   v._space   = space->isChecked();
                                                                   v._noncom  = noncom->text().toInt();
                                                                   v._direct  = direct->isChecked();
                                                                   v._damage  = damage->isChecked();
                                                                   v._limit   = limit->isChecked();
                                                                   v._parts   = parts->text();
                                                                   v._nonon   = nonon->isChecked();
                                                                   v._cause   = cause->isChecked();
                                                                   v._range   = range->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["meters"]  = v._meters;
                                                                   obj["distort"] = v._distort;
                                                                   obj["space"]   = v._space;
                                                                   obj["noncom"]  = v._noncom;
                                                                   obj["direct"]  = v._direct;
                                                                   obj["damage"]  = v._damage;
                                                                   obj["limit"]   = v._limit;
                                                                   obj["parts"]   = v._parts;
                                                                   obj["nonon"]   = v._nonon;
                                                                   obj["cause"]   = v._cause;
                                                                   obj["range"]   = v._range;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     _meters  = 0;
        int     _distort = 0;
        bool    _space   = false;
        int     _noncom  = 0;
        bool    _direct  = false;
        bool    _damage  = false;
        bool    _limit   = false;
        QString _parts   = "";
        bool    _nonon   = false;
        bool    _cause   = false;
        bool    _range   = false;
    } v;

    QLineEdit* meters;
    QLineEdit* distort;
    QCheckBox* space;
    QLineEdit* noncom;
    QCheckBox* direct;
    QCheckBox* damage;
    QCheckBox* limit;
    QLineEdit* parts;
    QCheckBox* nonon;
    QCheckBox* cause;
    QCheckBox* range;

    QString optOut(bool showEND) {
        if (v._meters < 1 || (v._limit && v._parts.isEmpty()) || (v._cause && v._damage)) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("+%1").arg(v._meters) + "m Stretching";
        res += QString("; Distort %1x & 1/%1x").arg((int) pow(2, v._distort));
        if (v._space) res += QString("; Doesn't Cross Intervening Space");
        if (!v._nonon) res += QString("; %1m Noncombat").arg((int) pow(2, v._noncom + 1));
        if (v._direct) res += QString("; Always Direct");
        if (v._damage) res += QString("; Cannot Do Damage");
        if (v._limit) res += "; Limited Body Parts (" + v._parts + ")";
        if (v._nonon) res += "; No Noncombat Stretching";
        if (v._cause) res += "; Can Only Cause Damage";
        if (v._range) res += "; Range Modifiers Apply";
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

#endif // BODYAFFECTINGPOWERS_H
