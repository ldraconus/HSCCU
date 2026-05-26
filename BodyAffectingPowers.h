#ifndef BODYAFFECTINGPOWERS_H
#define BODYAFFECTINGPOWERS_H

#include "powers.h"
#include "powerdialog.h"

class DensityIncrease: public AllPowers {
public:
    DensityIncrease(): AllPowers("Density Increase")          { }
    DensityIncrease(const DensityIncrease& s): AllPowers(s)   { }
    DensityIncrease(DensityIncrease&& s): AllPowers(s)        { }
    DensityIncrease(const QJsonObject& json): AllPowers(json) { v.mLevels = json["levels"].toInt(0);
                                                                v.mNoPdEd = json["nopded"].toBool(false);
                                                                v.mNoStr  = json["nostr"].toBool(false);
                                                              }
    ~DensityIncrease() override { }

    DensityIncrease& operator=(const DensityIncrease& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    DensityIncrease& operator=(DensityIncrease&& s) {
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
    Fraction lim() override                                      { return (v.mNoPdEd ? Fraction(1, 4) : Fraction(0)) +
                                                                          (v.mNoStr  ? Fraction(1)    : Fraction(0)); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v.mLevels * 4_cp; }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   levels->setText(QString("%1").arg(s.mLevels));
                                                                   nopded->setChecked(s.mNoPdEd);
                                                                   nostr->setChecked(s.mNoStr);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mLevels = levels->text().toInt();
                                                                   v.mNoPdEd = nopded->isChecked();
                                                                   v.mNoStr  = nostr->isChecked();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["levels"] = v.mLevels;
                                                                   obj["nopded"] = v.mNoPdEd;
                                                                   obj["nostr"]  = v.mNoStr;
                                                                   return obj;
                                                                 }

    int str() override { return v.mNoStr  ? 0 : 5 * v.mLevels; } // NOLINT
    int rPD() override { return v.mNoPdEd ? 0 : v.mLevels; }
    int rED() override { return v.mNoPdEd ? 0 : v.mLevels; }

private:
    struct vars {
        int  mLevels = 0;
        bool mNoPdEd = false;
        bool mNoStr = false;
    } v;

    QLineEdit* levels = nullptr;
    QCheckBox* nopded = nullptr;
    QCheckBox* nostr = nullptr;

    QString optOut(bool showEND) {
        if (v.mLevels < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("+%1").arg(v.mLevels) + " Levels Density Increase (";
        res += QString("Mass: %L1 kg").arg((int) pow(2, v.mLevels) * 100); // NOLINT
        if (!v.mNoStr) res += QString("; +%1 STR").arg(5 * v.mLevels);     // NOLINT
        res += QString("; -%1 KB").arg(2 * v.mLevels);
        if (!v.mNoPdEd) res += QString("; +%1 PD/+%1 ED").arg(v.mLevels);
        res += ")";
        if (v.mNoPdEd) res += "; No Increased PD/ED";
        if (v.mNoStr) res += "; No Increased STR";
        return res;
    }

    void numeric(int) override {
        QString txt = levels->text();
#ifndef ISHSC
        PowerDialog::ref().updateForm();
#endif
        if (txt.isEmpty() || isNumber(txt)) return;
        levels->undo();
    }
};

class Desolidification: public AllPowers {
public:
    Desolidification(): AllPowers("Desolidificationϴ")         { }
    Desolidification(const Desolidification& s): AllPowers(s)  { }
    Desolidification(Desolidification&& s): AllPowers(s)       { }
    Desolidification(const QJsonObject& json): AllPowers(json) { v.mSolid   = json["solid"].toBool(false);
                                                                 v.mProtect = json["protect"].toBool(false);
                                                                 v.mAffect  = json["affect"].toString();
                                                               }
    ~Desolidification() override { }

    Desolidification& operator=(const Desolidification& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    Desolidification& operator=(Desolidification&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   solid   = createCheckBox(parent, layout, "Cannot Pass Through Solid Objects");
                                                                   protect = createCheckBox(parent, layout, "Doesn't Protect Against Damage");
                                                                   affect  = createLineEdit(parent, layout, "Affected By?");
                                                                 }
    Fraction lim() override                                      { return (v.mSolid ? Fraction(1, 2) : Fraction(0)) +
                                                                          (v.mProtect  ? Fraction(1)    : Fraction(0)); }
    Points   points(bool noStore = false) override               { if (!noStore) store();
                                                                   return 40_cp; } // NOLINT
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   solid->setChecked(s.mSolid);
                                                                   protect->setChecked(s.mProtect);
                                                                   affect->setText(s.mAffect);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mSolid   = solid->isChecked();
                                                                   v.mProtect = protect->isChecked();
                                                                   v.mAffect  = affect->text();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["solid"]   = v.mSolid;
                                                                   obj["protect"] = v.mProtect;
                                                                   obj["affect"]  = v.mAffect;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        bool    mSolid = false;
        bool    mProtect = false;
        QString mAffect = "";
    } v;

    QCheckBox* solid = nullptr;
    QCheckBox* protect = nullptr;
    QLineEdit* affect = nullptr;

    QString optOut(bool showEND) {
        QString res;
        if (v.mAffect.isEmpty()) return "<incomplete>";
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "Desolidificationϴ";
        if (v.mSolid) res += "; Cannot Pass Thought Solid Objects";
        if (v.mProtect) res += "; Doesn't Protect Against Damage";
        res += "; Affected By " + v.mAffect;
        return res;
    }
};

class Duplication: public AllPowers {
public:
    Duplication(): AllPowers("Duplication▲")              { }
    Duplication(const Duplication& s): AllPowers(s)       { }
    Duplication(Duplication&& s): AllPowers(s)            { }
    Duplication(const QJsonObject& json): AllPowers(json) { v.mLevels   = json["levels"].toInt(0);
                                                            v.mAltered  = json["altered"].toInt(0);
                                                            v.mEasy     = json["easy"].toInt(0);
                                                            v.mRanged   = json["ranged"].toBool(false);
                                                            v.mRapid    = json["rapid"].toInt(0);
                                                            v.mRecom    = json["recom"].toBool(false);
                                                            v.mFeedback = json["feedback"].toInt(0);
                                                            v.mAverage  = json["average"].toBool(false);
                                                          }
    ~Duplication() override { }

    Duplication& operator=(const Duplication& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    Duplication& operator=(Duplication&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return ((v.mAltered == 1) ? Fraction(1, 4) : Fraction(0)) +
                                                                          ((v.mAltered == 2) ? Fraction(1, 2) : Fraction(0)) +
                                                                          ((v.mAltered == 3) ? Fraction(1)    : Fraction(0)) +
                                                                          (v.mRanged         ? Fraction(1, 2) : Fraction(0)) +
                                                                          ((v.mRapid - 1) * Fraction(1, 4)); }
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
    Fraction lim() override                                      { return ((v.mFeedback == 1) ? Fraction(1, 4) : Fraction(0)) +
                                                                          ((v.mFeedback == 2) ? Fraction(1, 2) : Fraction(0)) +
                                                                          ((v.mFeedback == 3) ? Fraction(1)    : Fraction(0)); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v.mLevels + v.mEasy * 5_cp; } // NOLINT
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   levels->setText(QString("%1").arg(s.mLevels));
                                                                   altered->setCurrentIndex(s.mAltered);
                                                                   easy->setCurrentIndex(s.mEasy);
                                                                   ranged->setChecked(s.mRanged);
                                                                   rapid->setText(QString("%1").arg(s.mRapid));
                                                                   feedback->setCurrentIndex(s.mFeedback);
                                                                   recom->setChecked(s.mRecom);
                                                                   average->setChecked(s.mAverage);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mLevels   = levels->text().toInt();
                                                                   v.mAltered  = altered->currentIndex();
                                                                   v.mEasy     = easy->currentIndex();
                                                                   v.mRanged   = ranged->isChecked();
                                                                   v.mRapid    = rapid->text().toInt();
                                                                   v.mFeedback = feedback->currentIndex();
                                                                   v.mRecom    = recom->isChecked();
                                                                   v.mAverage  = average->isChecked();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["levels"]   = v.mLevels;
                                                                   obj["altered"]  = v.mAltered;
                                                                   obj["easy"]     = v.mEasy;
                                                                   obj["ranged"]   = v.mRanged;
                                                                   obj["rapid"]    = v.mRapid;
                                                                   obj["feedback"] = v.mFeedback;
                                                                   obj["recom"]    = v.mRecom;
                                                                   obj["average"]  = v.mAverage;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int  mLevels   = 0;
        int  mAltered  = -1;
        int  mEasy     = -1;
        bool mRanged   = false;
        int  mRapid    = 0;
        bool mRecom    = false;
        int  mFeedback = -1;
        bool mAverage  = false;
    } v;

    QLineEdit* levels = nullptr;
    QComboBox* altered = nullptr;
    QComboBox* easy = nullptr;
    QCheckBox* ranged = nullptr;
    QLineEdit* rapid = nullptr;
    QCheckBox* recom = nullptr;
    QComboBox* feedback = nullptr;
    QCheckBox* average = nullptr;

    QString optOut(bool showEND) {
        if (v.mLevels < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("+%1").arg(v.mLevels) + " cp in Duplication▲ (" + QString("%1").arg(v.mLevels * 5) + " cp duplicates)"; // NOLINT
        QStringList altered { "",
                              Fraction(1, 4).toString() + " points different",
                              Fraction(1, 2).toString() + " points different",
                              "All points can be different" };
        if (v.mAltered > 0) res += "; Altered Duplicates (" + altered[v.mAltered] + ")";
        if (v.mEasy > 0) res += QString("; Easy Recombination ") + ((v.mEasy == 1) ? "(Half Phase)" : "(Zero Phase)");
        if (v.mRanged) res += "; Ranged Recombination";
        if (v.mRapid != 0) res += "; Rapid Duplication (" + QString("%1 per Phase)").arg((int) pow(2, v.mRapid));
        if (v.mRecom) res += "; Cannot Recombine";
        QStringList feedback { "",
                               "Damage to Base feeds back to duplicates",
                               "STUN to any affects all",
                               "BODY and STUN to any affects all" };
        if (v.mFeedback > 0) res += "; Feedback (" + feedback[v.mFeedback] + ")";
        if (v.mAverage) res += "; No Averaging";
        return res;
    }

    void numeric(int) override {
        QString txt = levels->text();
#ifndef ISHSC
        PowerDialog::ref().updateForm();
#endif
        if (txt.isEmpty() || isNumber(txt)) return;
        levels->undo();
    }
};

class ExtraLimbs: public AllPowers {
public:
    ExtraLimbs(): AllPowers("Extra Limbs")               { }
    ExtraLimbs(const ExtraLimbs& s): AllPowers(s)        { }
    ExtraLimbs(ExtraLimbs&& s): AllPowers(s)             { }
    ExtraLimbs(const QJsonObject& json): AllPowers(json) { v.mLimbs   = json["limbs"].toInt(0);
                                                           v.mLimited = json["limited"].toBool(false);
                                                         }
    ~ExtraLimbs() override { }
    ExtraLimbs& operator=(const ExtraLimbs& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    ExtraLimbs& operator=(ExtraLimbs&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   limbs   = createLineEdit(parent, layout, "Number of Extra Limbs?", std::mem_fn(&Power::numeric));
                                                                   limited = createCheckBox(parent, layout, "No Increased PD/ED");
                                                                 }
    Fraction lim() override                                      { return (v.mLimited ? Fraction(1, 4) : Fraction(0)); }
    Points points(bool noStore = false) override                 { if (!noStore) store();
                                                                   return 5_cp; } // NOLINT
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   limbs->setText(QString("%1").arg(s.mLimbs));
                                                                   limited->setChecked(s.mLimited);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mLimbs   = limbs->text().toInt();
                                                                   v.mLimited = limited->isChecked();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["limbs"]   = v.mLimbs;
                                                                   obj["limited"] = v.mLimited;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int  mLimbs   = 0;
        bool mLimited = false;
    } v;

    QLineEdit* limbs = nullptr;
    QCheckBox* limited = nullptr;

    QString optOut(bool showEND) {
        if (v.mLimbs < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("+%1").arg(v.mLimbs) + " Extra Limbs";
        if (v.mLimited) res += QString("; Limited Manipulation");
        return res;
    }

    void numeric(int) override {
        QString txt = limbs->text();
#ifndef ISHSC
        PowerDialog::ref().updateForm();
#endif
        if (txt.isEmpty() || isNumber(txt)) return;
        limbs->undo();
    }
};

class Growth: public AllPowers {
public:
    Growth(): AllPowers("Growth")                    { }
    Growth(const Growth& s): AllPowers(s)            { }
    Growth(Growth&& s): AllPowers(s)                 { }
    Growth(const QJsonObject& json): AllPowers(json) { v.mSize = json["size"].toInt(0);
                                                     }
    ~Growth() override { }
    Growth& operator=(const Growth& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    Growth& operator=(Growth&& s) {
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
                                                                   QList<Points> points { 0_cp, 25_cp, 50_cp, 90_cp, 120_cp, 150_cp, 215_cp }; // NOLINT
                                                                   return points[v.mSize + 1]; }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   size->setCurrentIndex(s.mSize);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mSize = size->currentIndex();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["size"] = v.mSize;
                                                                   return obj;
                                                                 }

    sizeMods& growthStats() override { return SizeMods[v.mSize]; }

private:
    struct vars {
        int mSize = -1;
    } v;

    QComboBox* size = nullptr;

    QList<sizeMods> SizeMods {
        { 15,  5,  5,  3,  3,  3,  6,  1, 12,  -6, 0, "Phys. Comp.: Large (Infrequently, Slightly Impairing)" },        // NOLINT
        { 30, 10, 10,  6,  6,  6, 12,  3, 24, -12, 0, "Phys. Comp.: Enormous (Frequently, Slightly Impairing)" },       // NOLINT
        { 45, 15, 15,  9,  9,  9, 18,  7, 36, -18, 1, "Phys. Comp.: Huge (Frequently, Greatly Impairing)" },            // NOLINT
        { 60, 20, 20, 12, 12, 12, 24, 15, 48, -24, 2, "Phys. Comp.: Gigantic (Frequently, Greatly Impairing)" },        // NOLINT
        { 75, 25, 25, 15, 15, 14, 30, 31, 60, -30, 4, "Phys. Comp.: Gargantuan (Very Frequently, Greatly Impairing)" }, // NOLINT
        { 90, 30, 30, 18, 18, 18, 36, 63, 72, -36, 8, "Phys. Comp.: Colossal (All The Time, Greatly Impairing)" }       // NOLINT
    };

    QString optOut(bool showEND) {
        if (v.mSize < 0) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        QStringList size { "Large", "Enormous", "Huge", "Gigantic",
                           "Gargantuan", "Colossal" };
        res += size[v.mSize] + " Growth (";
        res += QString("+%1 STR").arg(SizeMods[v.mSize].mSTR);
        res += QString("; +%1 CON").arg(SizeMods[v.mSize].mCON);
        res += QString("; +%1 PRE").arg(SizeMods[v.mSize].mPRE);
        res += QString("; +%1 PD").arg(SizeMods[v.mSize].mPD);
        res += QString("; +%1 ED").arg(SizeMods[v.mSize].mED);
        res += QString("; +%1 BODY").arg(SizeMods[v.mSize].mBODY);
        res += QString("; +%1 STUN").arg(SizeMods[v.mSize].mSTUN);
        res += QString("; +%1m Reach").arg(SizeMods[v.mSize].mReach);
        res += QString("; +%1m Running").arg(SizeMods[v.mSize].mRunning);
        res += QString("; %1 KB Res").arg(SizeMods[v.mSize].mKBRes);
        if (SizeMods[v.mSize].mAOERad != 0) res += QString("; +%1m AOE Radius HA").arg(SizeMods[v.mSize].mAOERad);
        res += "; " + SizeMods[v.mSize].mComp;
        return res + ")";
    }
};

class Multiform: public AllPowers {
public:
    Multiform(): AllPowers("Multiform")                 { }
    Multiform(const Multiform& s): AllPowers(s)         { }
    Multiform(Multiform&& s): AllPowers(s)              { }
    Multiform(const QJsonObject& json): AllPowers(json) { v.mPoints  = json["points"].toInt(0);
                                                          v.mForm    = json["form"].toString();
                                                          v.mMult    = json["mult"].toInt(0);
                                                          v.mInstant = json["instant"].toBool(false);
                                                          v.mLoss    = json["loss"].toInt(0);
                                                          v.mRevert  = json["revert"].toInt(0);
                                                        }
    ~Multiform() override { }
    Multiform& operator=(const Multiform& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    Multiform& operator=(Multiform&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return ((v.mRevert == 5) ? Fraction(1)                        : Fraction(0)) +   // NOLINT
                                                                          ((v.mRevert == 4) ? Fraction(1, 2)                     : Fraction(0)) +
                                                                          ((v.mLoss >= 1) ? (2 - (v.mLoss - 1) * Fraction(1, 4)) : Fraction(0)); }
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
    Fraction lim() override                                      { return ((v.mRevert == 1) ? Fraction(1)    : Fraction(0)) +
                                                                          ((v.mRevert == 2) ? Fraction(1, 2) : Fraction(0)); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return (v.mPoints + 2_cp) / 5 + (v.mInstant ? 5_cp : 0_cp) + v.mMult * 5_cp; } // NOLINT
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   point->setText(QString("%1").arg(s.mPoints));
                                                                   frm->setText(s.mForm);
                                                                   mult->setText(QString("%1").arg(s.mMult));
                                                                   loss->setCurrentIndex(s.mLoss);
                                                                   instant->setChecked(s.mInstant);
                                                                   revert->setCurrentIndex(s.mRevert);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mPoints  = point->text().toInt();
                                                                   v.mForm    = frm->text();
                                                                   v.mMult    = mult->text().toInt();
                                                                   v.mLoss    = loss->currentIndex();
                                                                   v.mInstant = instant->isChecked();
                                                                   v.mRevert  = revert->currentIndex();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["points"]  = v.mPoints;
                                                                   obj["form"]    = v.mForm;
                                                                   obj["mult"]    = v.mMult;
                                                                   obj["loss"]    = v.mLoss;
                                                                   obj["instant"] = v.mInstant;
                                                                   obj["revert"]  = v.mRevert;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     mPoints  = 0;
        QString mForm    = "";
        int     mMult    = 0;
        int     mLoss    = -1;
        bool    mInstant = false;
        int     mRevert  = -1;
    } v;

    QLineEdit* point = nullptr;
    QLineEdit* frm = nullptr;
    QLineEdit* mult = nullptr;
    QComboBox* loss = nullptr;
    QCheckBox* instant = nullptr;
    QComboBox* revert = nullptr;

    QString optOut(bool showEND) {
        if (v.mPoints < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("+%1").arg(v.mPoints) + " CP Multiform Into " + v.mForm;
        if (v.mMult > 0) res += QString("; x%1 Forms").arg((int) pow(2, v.mMult));
        QStringList loss {"",  "1 Turn", "1 Minute", "5 Minutes", "20 Minutes", "1 Hour", "6 Hours", "1 Day", "1 Week" };
        if (v.mLoss >= 1) res += "; Personality Loss Rolls Start Ater " + loss[v.mLoss];
        if (v.mInstant) res += "; Instant Change";
        if (v.mRevert >= 1) res += "; Revert To True Form If Stunned Or KO";
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

class ShapeShift: public AllPowers {
public:
    ShapeShift(): AllPowers("Shape Shift")               { }
    ShapeShift(const ShapeShift& s): AllPowers(s)        { }
    ShapeShift(ShapeShift&& s): AllPowers(s)             { }
    ShapeShift(const QJsonObject& json): AllPowers(json) { v.mChange   = json["change"].toInt(0);
                                                           v.mForms    = json["forms"].toString();
                                                           v.mSight    = json["sight"].toBool(false);
                                                           v.mHearing  = json["hearing"].toBool(false);
                                                           v.mTouch    = json["touch"].toBool(false);
                                                           v.mMental   = json["mental"].toBool(false);
                                                           v.mRadio    = json["radio"].toBool(false);
                                                           v.mSmell    = json["smell"].toBool(false);
                                                           v.mClair    = json["clair"].toBool(false);
                                                           v.mSpatial  = json["spatial"].toBool(false);
                                                           v.mCellular = json["cellular"].toBool(false);
                                                           v.mImitate  = json["imitate"].toBool(false);
                                                           v.mInstant  = json["instant"].toBool(false);
                                                           v.mMakeover = json["makeover"].toBool(false);
                                                           v.mBody     = json["body"].toBool(false);}
    ~ShapeShift() override { }
    ShapeShift& operator=(const ShapeShift& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    ShapeShift& operator=(ShapeShift&& s) {
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
    Fraction lim() override                                      { return v.mBody ? Fraction(1, 4) : Fraction(0);
                                                                 }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   QList<Points> change { 0_cp, 0_cp, 3_cp, 5_cp, 10_cp }; // NOLINT
                                                                   Points pts = change[v.mChange + 1];
                                                                   pts += v.mSight    ?  8_cp : 0_cp; // NOLINT
                                                                   pts += v.mHearing  ?  5_cp : 0_cp; // NOLINT
                                                                   pts += v.mTouch    ?  5_cp : 0_cp; // NOLINT
                                                                   pts += v.mMental   ?  2_cp : 0_cp;
                                                                   pts += v.mRadio    ?  2_cp : 0_cp;
                                                                   pts += v.mSmell    ?  2_cp : 0_cp;
                                                                   pts += v.mClair    ?  3_cp : 0_cp;
                                                                   pts += v.mSpatial  ?  3_cp : 0_cp;
                                                                   pts += v.mCellular ? 10_cp : 0_cp; // NOLINT
                                                                   pts += v.mInstant  ?  5_cp : 0_cp; // NOLINT
                                                                   pts += v.mMakeover ?  5_cp : 0_cp; // NOLINT
                                                                   return pts;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   change->setCurrentIndex(s.mChange);
                                                                   forms->setText(s.mForms);
                                                                   sight->setChecked(s.mSight);
                                                                   hearing->setChecked(s.mHearing);
                                                                   touch->setChecked(s.mTouch);
                                                                   mental->setChecked(s.mMental);
                                                                   radio->setChecked(s.mRadio);
                                                                   smell->setChecked(s.mSmell);
                                                                   clair->setChecked(s.mClair);
                                                                   spatial->setChecked(s.mSpatial);
                                                                   cellular->setChecked(s.mCellular);
                                                                   instant->setChecked(s.mInstant);
                                                                   makeover->setChecked(s.mMakeover);
                                                                   body->setChecked(s.mBody);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mChange   = change->currentIndex();
                                                                   v.mForms    = forms->text();
                                                                   v.mSight    = sight->isChecked();
                                                                   v.mHearing  = hearing->isChecked();
                                                                   v.mTouch    = touch->isChecked();
                                                                   v.mMental   = mental->isChecked();
                                                                   v.mRadio    = radio->isChecked();
                                                                   v.mSmell    = smell->isChecked();
                                                                   v.mClair    = clair->isChecked();
                                                                   v.mSpatial  = spatial->isChecked();
                                                                   v.mCellular = cellular->isChecked();
                                                                   v.mInstant  = instant->isChecked();
                                                                   v.mMakeover = makeover->isChecked();
                                                                   v.mBody     = body->isChecked();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["change"]   = v.mChange;
                                                                   obj["forms"]    = v.mForms;
                                                                   obj["sight"]    = v.mSight;
                                                                   obj["hearing"]  = v.mHearing;
                                                                   obj["touch"]    = v.mTouch;
                                                                   obj["mental"]   = v.mMental;
                                                                   obj["radio"]    = v.mRadio;
                                                                   obj["smell"]    = v.mSmell;
                                                                   obj["clair"]    = v.mClair;
                                                                   obj["spatial"]  = v.mSpatial;
                                                                   obj["cellular"] = v.mCellular;
                                                                   obj["instant"]  = v.mInstant;
                                                                   obj["makeover"] = v.mMakeover;
                                                                   obj["body"]     = v.mBody;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     mChange   = -1;
        QString mForms    = "";
        bool    mSight    = false;
        bool    mHearing  = false;
        bool    mTouch    = false;
        bool    mMental   = false;
        bool    mRadio    = false;
        bool    mSmell    = false;
        bool    mSonar    = false;
        bool    mClair    = false;
        bool    mSpatial  = false;
        bool    mCellular = false;
        bool    mImitate  = false;
        bool    mInstant  = false;
        bool    mMakeover = false;
        bool    mBody     = false;
    } v;

    QComboBox* change = nullptr;
    QLineEdit* forms = nullptr;
    QCheckBox* sight = nullptr;
    QCheckBox* hearing = nullptr;
    QCheckBox* touch = nullptr;
    QCheckBox* mental = nullptr;
    QCheckBox* radio = nullptr;
    QCheckBox* smell = nullptr;
    QCheckBox* clair = nullptr;
    QCheckBox* spatial = nullptr;
    QCheckBox* cellular = nullptr;
    QCheckBox* imitate = nullptr;
    QCheckBox* instant = nullptr;
    QCheckBox* makeover = nullptr;
    QCheckBox* body = nullptr;

    QString optOut(bool showEND) {
        if (v.mChange == -1 || (v.mChange < 3 && v.mForms.isEmpty())) return "<incomplete>";
        if (!(v.mSight || v.mHearing || v.mTouch || v.mMental || v.mRadio || v.mSmell || v.mClair || v.mSpatial)) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "Shape Shift Into ";
        QStringList forms { "A Single Shape", "Four shapes", "A Limited Group", "Any Shape" };
        if (v.mChange < 3) res += forms[v.mChange] + " (" + v.mForms + ")";
        else res += "Any Shape";
        res += " Affecting ";
        QStringList senses;
        if (v.mSight) senses << "Sight";
        if (v.mHearing) senses << "Hearing";
        if (v.mTouch) senses << "Touch";
        if (v.mMental) senses << "Mental";
        if (v.mRadio) senses << "Radio";
        if (v.mSmell) senses << "Smell";
        if (v.mClair) senses << "Clairsentience";
        if (v.mSpatial) senses << "Spatial Awareness";
        QString list = "";
        for (int i = 0; i < senses.count() - 1; ++i) {
            if (i != 0) list += ", ";
            list += senses[i];
        }
        if (list.isEmpty()) list = senses[0];
        else list += ", and " + senses.back();
        res += list;
        if (v.mCellular) res += "; Cellular";
        if (v.mImitate) res += "; Imitate";
        if (v.mInstant) res += "; Instant Change";
        if (v.mMakeover) res += "; Makeover";
        if (v.mBody) res += "; Affects Body Only";
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

class Shrinking: public AllPowers {
public:
    Shrinking(): AllPowers("Shrinking")                 { }
    Shrinking(const Shrinking& s): AllPowers(s)         { }
    Shrinking(Shrinking&& s): AllPowers(s)              { }
    Shrinking(const QJsonObject& json): AllPowers(json) { v.mLevels = json["levels"].toInt(0);
                                                          v.mMass   = json["mass"].toBool(false);
                                                          v.mPerc   = json["perc"].toBool(false);
                                                        }
    ~Shrinking() override { }
    Shrinking& operator=(const Shrinking& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    Shrinking& operator=(Shrinking&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(1, 2) * (v.mMass + 1);
                                                                 }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   levels = createLineEdit(parent, layout, "Levels of Shrinking?", std::mem_fn(&Power::numeric));
                                                                   mass   = createComboBox(parent, layout, "No Mass Decrease?", { "No Mass Decrease",
                                                                                                                                  "Choice Mass Decrease" });
                                                                   perc   = createCheckBox(parent, layout, "Easily Percieved");
                                                                 }
    Fraction lim() override                                      { return (v.mPerc ? Fraction(1, 4) : Fraction(0));
                                                                 }
    Points points(bool noStore = false) override                 { if (!noStore) store();
                                                                   return v.mLevels * 6_cp; } // NOLINT
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   levels->setText(QString("%1").arg(s.mLevels));
                                                                   mass->setCurrentIndex(s.mMass);
                                                                   perc->setChecked(s.mPerc);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mLevels = levels->text().toInt();
                                                                   v.mMass   = mass->currentIndex();
                                                                   v.mPerc   = perc->isChecked();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["levels"] = v.mLevels;
                                                                   obj["mass"]   = v.mMass;
                                                                   obj["perc"]   = v.mPerc;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int  mLevels = 0;
        int  mMass   = -1;
        bool mPerc   = false;
    } v;

    QLineEdit* levels = nullptr;
    QComboBox* mass = nullptr;
    QCheckBox* perc = nullptr;

    QString optOut(bool showEND) {
        if (v.mLevels < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("+%1").arg(v.mLevels) + " Levels Shrinking (";
        res += QString("Height: %2m").arg(2.0 / pow(2.0, v.mLevels)); // NOLINT
        if (v.mMass == -1) {
            res += QString("; Mass: %1 kg").arg(1.0 / (8.0 * pow(2, v.mLevels))); // NOLINT
            res += QString("; +%1 KB").arg(6 * v.mLevels); // NOLINT
        }
        if (v.mMass == 1) {
            res += QString("; Optionally %1 kg").arg(1.0 / (8.0 * pow(2, v.mLevels))); // NOLINT
            res += QString("; Optionally +%1 KB").arg(6 * v.mLevels); // NOLINT
        }
        if (!v.mPerc) res += QString("; -%1 PER Rolls Against").arg(2 * v.mLevels);
        res += ")";
        if (v.mMass == 0) res += "; No Decreased Mass";
        else if (v.mMass == 1) res += "; Choice To Decrease Mass";
        if (v.mPerc) res += "; Easily Perceived";
        return res;
    }

    void numeric(int) override {
        QString txt = levels->text();
#ifndef ISHSC
        PowerDialog::ref().updateForm();
#endif
        if (txt.isEmpty() || isNumber(txt)) return;
        levels->undo();
    }
};

class Stretching: public AllPowers {
public:
    Stretching(): AllPowers("Stretching")                { }
    Stretching(const Stretching& s): AllPowers(s)        { }
    Stretching(Stretching&& s): AllPowers(s)             { }
    Stretching(const QJsonObject& json): AllPowers(json) { v.mMeters  = json["meters"].toInt(0);
                                                           v.mDistort = json["distort"].toInt(0);
                                                           v.mSpace   = json["space"].toBool(false);
                                                           v.mNonCom  = json["noncom"].toInt(0);
                                                           v.mDirect  = json["direct"].toBool(false);
                                                           v.mDamage  = json["damage"].toBool(false);
                                                           v.mLimit   = json["limit"].toBool(false);
                                                           v.mParts   = json["parts"].toString();
                                                           v.mNonOn   = json["nonon"].toBool(false);
                                                           v.mCause   = json["cause"].toBool(false);
                                                           v.mRange   = json["range"].toBool(false);
                                                         }
    ~Stretching() override { }
    Stretching& operator=(const Stretching& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    Stretching& operator=(Stretching&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return (v.mSpace ? Fraction(1, 4) : Fraction(0)); }
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
    Fraction lim() override                                      { return (v.mDirect ? Fraction(1, 4) : Fraction(0)) +
                                                                          (v.mDamage ? Fraction(1, 2) : Fraction(0)) +
                                                                          (v.mLimit  ? Fraction(1, 4) : Fraction(0)) +
                                                                          (v.mNonOn  ? Fraction(1, 4) : Fraction(0)) +
                                                                          (v.mCause  ? Fraction(1, 2) : Fraction(0)) +
                                                                          (v.mRange  ? Fraction(1, 4) : Fraction(0)); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v.mMeters + 5_cp * v.mDistort + 5_cp * v.mNonCom; } // NOLINT
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   meters->setText(QString("%1").arg(s.mMeters));
                                                                   distort->setText(QString("%1").arg(s.mDistort));
                                                                   space->setChecked(s.mSpace);
                                                                   noncom->setText(QString("%1").arg(s.mNonCom));
                                                                   direct->setChecked(s.mDirect);
                                                                   damage->setChecked(s.mDamage);
                                                                   limit->setChecked(s.mLimit);
                                                                   parts->setText(s.mParts);
                                                                   nonon->setChecked(s.mNonOn);
                                                                   cause->setChecked(s.mCause);
                                                                   range->setChecked(s.mRange);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mMeters  = meters->text().toInt();
                                                                   v.mDistort = distort->text().toInt();
                                                                   v.mSpace   = space->isChecked();
                                                                   v.mNonCom  = noncom->text().toInt();
                                                                   v.mDirect  = direct->isChecked();
                                                                   v.mDamage  = damage->isChecked();
                                                                   v.mLimit   = limit->isChecked();
                                                                   v.mParts   = parts->text();
                                                                   v.mNonOn   = nonon->isChecked();
                                                                   v.mCause   = cause->isChecked();
                                                                   v.mRange   = range->isChecked();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["meters"]  = v.mMeters;
                                                                   obj["distort"] = v.mDistort;
                                                                   obj["space"]   = v.mSpace;
                                                                   obj["noncom"]  = v.mNonCom;
                                                                   obj["direct"]  = v.mDirect;
                                                                   obj["damage"]  = v.mDamage;
                                                                   obj["limit"]   = v.mLimit;
                                                                   obj["parts"]   = v.mParts;
                                                                   obj["nonon"]   = v.mNonOn;
                                                                   obj["cause"]   = v.mCause;
                                                                   obj["range"]   = v.mRange;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     mMeters  = 0;
        int     mDistort = 0;
        bool    mSpace   = false;
        int     mNonCom  = 0;
        bool    mDirect  = false;
        bool    mDamage  = false;
        bool    mLimit   = false;
        QString mParts   = "";
        bool    mNonOn   = false;
        bool    mCause   = false;
        bool    mRange   = false;
    } v;

    QLineEdit* meters = nullptr;
    QLineEdit* distort = nullptr;
    QCheckBox* space = nullptr;
    QLineEdit* noncom = nullptr;
    QCheckBox* direct = nullptr;
    QCheckBox* damage = nullptr;
    QCheckBox* limit = nullptr;
    QLineEdit* parts = nullptr;
    QCheckBox* nonon = nullptr;
    QCheckBox* cause = nullptr;
    QCheckBox* range = nullptr;

    QString optOut(bool showEND) {
        if (v.mMeters < 1 || (v.mLimit && v.mParts.isEmpty()) || (v.mCause && v.mDamage)) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("+%1").arg(v.mMeters) + "m Stretching";
        res += QString("; Distort %1x & 1/%1x").arg((int) pow(2, v.mDistort));
        if (v.mSpace) res += QString("; Doesn't Cross Intervening Space");
        if (!v.mNonOn) res += QString("; %1m Noncombat").arg((int) pow(2, v.mNonCom + 1));
        if (v.mDirect) res += QString("; Always Direct");
        if (v.mDamage) res += QString("; Cannot Do Damage");
        if (v.mLimit) res += "; Limited Body Parts (" + v.mParts + ")";
        if (v.mNonOn) res += "; No Noncombat Stretching";
        if (v.mCause) res += "; Can Only Cause Damage";
        if (v.mRange) res += "; Range Modifiers Apply";
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

#endif // BODYAFFECTINGPOWERS_H
