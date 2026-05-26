#ifndef TALENT_H
#define TALENT_H

#include "character.h"
#include "sheet.h"
#include "skilltalentorperk.h"

class Talent: public SkillTalentOrPerk {
public:
    Talent(): SkillTalentOrPerk() { }
    Talent(QString name)
        : v { name } { }
    Talent(const Talent& s)
        : SkillTalentOrPerk()
        , v(s.v) { }
    Talent(Talent&& s)
        : SkillTalentOrPerk()
        , v(s.v) { }
    Talent(const QJsonObject& json)
        : SkillTalentOrPerk()
        , v { json["name"].toString("") } { }

    virtual Talent& operator=(const Talent& s) {
        if (this != &s) v = s.v;
        return *this;
    }
    virtual Talent& operator=(Talent&& s) {
        v = s.v;
        return *this;
    }

    bool isTalent() override { return true; }

    QString description(bool showRoll = false) override { return v._name + (showRoll ? "" : ""); }
    bool    form(QWidget*, QVBoxLayout*) override       { return false; }
    QString name() override                             { return v._name; }
    Points points(bool noStore = false) override        { if (!noStore) store(); return 0_cp; }
    void    restore() override                          { }
    QString roll() override                             { return ""; }
    void    store() override                            { }

    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"] = v._name;
        return obj;
    }

private:
    struct vars {
        QString _name;
    } v;
};

#define CLASS(x,y)\
    class x: public Talent {\
    public:\
        x(): Talent(#x) { }\
        x(const x& s): Talent(s) { }\
        x(x&& s): Talent(s) { }\
        x(const QJsonObject& json): Talent(json) { }\
        Points points(bool noStore = false) { if (!noStore) store(); return y; }\
    };
#define CLASS_SPACE(x,y,z)\
    class x: public Talent {\
    public:\
        x(): Talent(y) { }\
        x(const x& s): Talent(s) { }\
        x(x&& s): Talent(s) { }\
        x(const QJsonObject& json): Talent(json) { }\
        Points points(bool noStore = false) { if (!noStore) store(); return z; }\
    };

class Ambidexterity: public Talent {
public:
    Ambidexterity(): Talent("Ambidexterity")             { }
    Ambidexterity(const Ambidexterity& s): Talent(s)     { }
    Ambidexterity(Ambidexterity&& s): Talent(s)          { }
    Ambidexterity(const QJsonObject& json): Talent(json) { v.mOffhand = json["offhand"].toInt(0);
                                                         }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { offhand = createLineEdit(parent, layout, "Offhand penalties offset", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override                { if (!noStore) store();
                                                                  return Points(v.mOffhand); }
    void    restore() override                                  { vars s = v;
                                                                  QString msg = QString("%1").arg(s.mOffhand);
                                                                  offhand->setText(msg);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mOffhand = offhand->text().toInt(0);
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["offhand"] = v.mOffhand;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int mOffhand = 0;
    } v;

    QLineEdit* offhand;

    QString optOut() {
        return "Ambidexterity: " + QString("+%1").arg(v.mOffhand) + " vs. Offhand Penalties";
    }

    void numeric(QString) override {
        QString txt = offhand->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        offhand->undo();
    }
};

class CombatLuck: public Talent {
public:
    CombatLuck(): Talent("Combat Luck")               { }
    CombatLuck(const CombatLuck& s): Talent(s)        { }
    CombatLuck(CombatLuck&& s): Talent(s)             { }
    CombatLuck(const QJsonObject& json): Talent(json) { v.mLevels = json["levels"].toInt(0);
                                                        v.mPut = json["put"].toInt(1);
                                                      }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { levels = createLineEdit(parent, layout, "Levels of Combat Luck?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  put    = createComboBox(parent, layout, "Add to?", { "Nothing", "Primary", "Secondary" });
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override                { if (!noStore) store();
                                                                  return 6_cp * v.mLevels; }
    void    restore() override                                  { vars s = v;
                                                                  QString msg = QString("%1").arg(s.mLevels);
                                                                  levels->setText(msg);
                                                                  put->setCurrentIndex(s.mPut);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mLevels = levels->text().toInt(0);
                                                                  v.mPut    = put->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["levels"] = v.mLevels;
                                                                  obj["put"]    = v.mPut;
                                                                  return obj;
                                                                }

    int rED() override   { return v.mLevels * 3; }
    int rPD() override   { return rED(); }
    int place() override { return v.mPut; }

private:
    struct vars {
        int mLevels = 0;
        int mPut    = -1;
    } v;

    QLineEdit* levels;
    QComboBox* put;

    QString optOut() {
        return "Combat Luck: " + QString("+%1 rPD/+%1 rED").arg(v.mLevels * 3);
    }

    void numeric(QString) override {
        QString txt = levels->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        levels->undo();
    }
};

class CombatSense: public Talent {
public:
    CombatSense(): Talent("Combat Sense")              { }
    CombatSense(const CombatSense& s): Talent(s)       { }
    CombatSense(CombatSense&& s): Talent(s)            { }
    CombatSense(const QJsonObject& json): Talent(json) { v.mPlus  = json["plus"].toInt(0);
                                                         v._sense = json["sense"].toBool(false);
                                                       }

    QString description(bool showRoll = false) override         { return (showRoll ? roll() + " " : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { plus  = createLineEdit(parent, layout, "Plus to toll?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  sense = createCheckBox(parent, layout, "Make a sense");
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override                { if (!noStore) store();
                                                                  return 15_cp + v.mPlus + (v._sense ? 2_cp : 0_cp); }
    void    restore() override                                  { vars s = v;
                                                                  QString msg = QString("%1").arg(v.mPlus);
                                                                  int sns = v._sense;
                                                                  plus->setText(msg);
                                                                  sense->setChecked(sns);
                                                                  v = s;
                                                                }
    QString roll() override                                     {
#ifndef ISHSC
                                                                  return add(Sheet::ref().character().INT().roll(), v.mPlus);
#else
                                                                  return QString("+%1").arg(v._plus);
#endif
                                                                }
    void    store() override                                    { v.mPlus = plus->text().toInt(0);
                                                                  v._sense = sense->isChecked();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["plus"]  = v.mPlus;
                                                                  obj["sense"] = v._sense;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int  mPlus  = 0;
        bool _sense = false;
    } v;

    QLineEdit* plus;
    QCheckBox* sense;

    QString optOut() {
        QString res = "Combat Sense";
        if (v.mPlus > 0) res += ": +" + QString("%1").arg(v.mPlus);
        if (v._sense) res += " (Sense)";
        return res;
    }

    void numeric(QString) override {
        QString txt = plus->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        plus->undo();
    }
};

class DangerSense: public Talent {
public:
    DangerSense(): Talent("Danger Sense")              { }
    DangerSense(const DangerSense& s): Talent(s)       { }
    DangerSense(DangerSense&& s): Talent(s)            { }
    DangerSense(const QJsonObject& json): Talent(json) { mPlus   = json["plus"].toInt(0);
                                                         mSense  = json["sense"].toBool(false);
                                                         mIntuit = json["intuit"].toBool(false);
                                                         mAnlze  = json["anlze"].toBool(false);
                                                         mDescr  = json["descr"].toBool(false);
                                                         mPen    = json["pen"].toBool(false);
                                                         mThrgh  = json["thrgh"].toString("");
                                                         mDtct   = json["dtct"].toInt(0);
                                                         mArea   = json["area"].toInt(0);
                                                       }

    QString description(bool showRoll = false) override         { return (showRoll ? roll() + " " : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { plus   = createLineEdit(parent, layout, "Plus to toll?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  sense  = createCheckBox(parent, layout, "Make a sense");
                                                                  intuit = createCheckBox(parent, layout, "Intutiton", std::mem_fn(&SkillTalentOrPerk::checked));
                                                                  anlze  = createCheckBox(parent, layout, "Analyze");
                                                                  descr  = createCheckBox(parent, layout, "Descriminatory");
                                                                  pen    = createCheckBox(parent, layout, "Penetrative");
                                                                  thrgh  = createLineEdit(parent, layout, "Through?");
                                                                  dtct   = createComboBox(parent, layout, "Where?", { "In Combat, perceivable",
                                                                                                                      "In or out of combat, perceivable",
                                                                                                                      "Any" });
                                                                  area   = createComboBox(parent, layout, "Area?", { "The character",
                                                                                                                     "Anyone in immediate area",
                                                                                                                     "Anyone in general area",
                                                                                                                     "Anyone in any area" });
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override                { if (!noStore) store();
                                                                  return 15_cp +
                                                                         mPlus +
                                                                         (mSense ? 2_cp : 0_cp) -
                                                                         (mIntuit ? 5_cp : 0_cp) +
                                                                         (mAnlze ? 5_cp : 0_cp) +
                                                                         (mDescr ? 5_cp : 0_cp) +
                                                                         (mPen ? 5_cp : 0_cp) +
                                                                         (mDtct - 1) * 5_cp +
                                                                         (mArea - 1) * 5_cp;
                                                                }
    void    restore() override                                  { bool    sns = mSense;
                                                                  bool    ntt = mIntuit;
                                                                  bool    nlz = mAnlze;
                                                                  bool    dsc = mDescr;
                                                                  bool    pn  = mPen;
                                                                  QString thr = mThrgh;
                                                                  int     dtc = mDtct;
                                                                  int     are = mArea;
                                                                  sense->setChecked(sns);
                                                                  intuit->setChecked(ntt);
                                                                  anlze->setChecked(nlz);
                                                                  descr->setChecked(dsc);
                                                                  pen->setChecked(pn);
                                                                  dtct->setCurrentIndex(dtc);
                                                                  area->setCurrentIndex(are);
                                                                  QString msg = QString("%1").arg(mPlus);
                                                                  thrgh->setText(thr);
                                                                  plus->setText(msg);
                                                                  mSense  = sns;
                                                                  mIntuit = ntt;
                                                                  mAnlze  = nlz;
                                                                  mDescr  = dsc;
                                                                  mPen    = pn;
                                                                  mThrgh  = thr;
                                                                  mDtct   = dtc;
                                                                  mArea   = are;
                                                                }
    QString roll() override                                     {
#ifndef ISHSC
                                                                  return add(Sheet::ref().character().INT().roll(), mPlus);
#else
                                                                  return QString("+%1").arg(_plus);
#endif
                                                                }
    void    store() override                                    { mPlus   = plus->text().toInt(0);
                                                                  mSense  = sense->isChecked();
                                                                  mIntuit = intuit->isChecked();
                                                                  mAnlze  = anlze->isChecked();
                                                                  mDescr  = descr->isChecked();
                                                                  mPen    = pen->isChecked();
                                                                  mThrgh  = thrgh->text();
                                                                  mDtct   = dtct->currentIndex();
                                                                  mArea   = area->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["plus"]   = mPlus;
                                                                  obj["semse"]  = mSense;
                                                                  obj["intuit"] = mIntuit;
                                                                  obj["anlze"]  = mAnlze;
                                                                  obj["descr"]  = mDescr;
                                                                  obj["pen"]    = mPen;
                                                                  obj["thrgh"]  = mThrgh;
                                                                  obj["dtct"]   = mDtct;
                                                                  obj["area"]   = mArea;
                                                                  return obj;
                                                                }

private:
    int     mPlus   = 0;
    bool    mSense  = false;
    bool    mIntuit = false;
    bool    mAnlze  = false;
    bool    mDescr  = false;
    bool    mPen    = false;
    QString mThrgh  = "";
    int     mDtct   = -1;
    int     mArea   = -1;
\
    QLineEdit* plus;
    QCheckBox* sense;
    QCheckBox* intuit;
    QCheckBox* anlze;
    QCheckBox* descr;
    QCheckBox* pen;
    QLineEdit* thrgh;
    QComboBox* dtct;
    QComboBox* area;

    QString optOut() {
        if (mDtct < 0 || mArea < 0 || (mPen && mThrgh.isEmpty()) || (mIntuit && (mDtct > 1 || mArea > 1))) return "<incomplete>";
        QString res = "Danger Senseϴ";
        if (mPlus > 0) res += ": +" + QString("%1").arg(mPlus);
        QString sep = " (";
        if (mSense) { res += sep + "Sense"; sep = "; "; }
        if (mIntuit) { res += sep + "Intuition"; sep = "; "; }
        if (mAnlze) { res += sep + "Analyze"; sep = "; "; }
        if (mDescr) { res += sep + "Discriminate"; sep = "; "; }
        if (mPen) { res += sep + "Pentrates " + mThrgh; sep = "; "; }
        res += sep += QStringList{ "In Combat, perceivable", "In or out of combat, perceivable", "Any" }[mDtct];
        res += "; " + QStringList{ "Personal", "Anyone in immediate area", "Anyone in general area", "Anyone in any area" }[mArea];
        return res + ")";
    }

    void numeric(QString) override {
        QString txt = plus->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        plus->undo();
    }

    void rebuild(QComboBox* cb, int max, QStringList items) {
        int idx = cb->currentIndex();
        cb->clear();
        if (idx >= max) idx = -1;
        for (int i = 0; i < max; ++i) cb->addItem(items[i]);
        cb->setCurrentIndex(idx);
    }

    void checked(bool tf) override {
        rebuild(dtct, tf ? 2 : 3, { "", "In Combat, perceivable", "In or out of combat, perceivable", "Any" });
        rebuild(area, tf ? 2 : 4, { "", "The character", "Anyone in immediate area", "Anyone in general area", "Anyone in any area" });
    }
};

class DeadlyBlow: public Talent {
public:
    DeadlyBlow(): Talent("Deadly Blow")               { }
    DeadlyBlow(const DeadlyBlow& s): Talent(s)        { }
    DeadlyBlow(DeadlyBlow&& s): Talent(s)             { }
    DeadlyBlow(const QJsonObject& json): Talent(json) { v.mMult  =  json["mult"].toInt(0);
                                                        v.mCirc  =  json["circ"].toInt(0);
                                                        v.mRanged = json["ranged"].toBool(false);
                                                        v.mVersus = json["versus"].toString("");
                                                      }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { mult   = createLineEdit(parent, layout, "Multiples?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  circ   = createComboBox(parent, layout, "Circumstances?", { "Very limited circumstances or group",
                                                                                                                              "Limited circumstances or group",
                                                                                                                              "Broad circumstances or group" });
                                                                  ranged = createCheckBox(parent, layout, "Ranged");
                                                                  versus = createLineEdit(parent, layout, "Against?");
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override                { if (!noStore) store();
                                                                  QList<Points> circ { 0_cp, 12_cp, 16_cp, 19_cp };
                                                                  return circ[v.mCirc + 1] * v.mMult; }
    void    restore() override                                  { vars s = v;
                                                                  mult->setText(QString("%1").arg(s.mMult));
                                                                  circ->setCurrentIndex(s.mCirc);
                                                                  ranged->setChecked(s.mRanged);
                                                                  versus->setText(s.mVersus);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mMult   = mult->text().toInt(0);
                                                                  v.mCirc   = circ->currentIndex();
                                                                  v.mRanged = ranged->isChecked();
                                                                  v.mVersus = versus->text();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["mult"]   = v.mMult;
                                                                  obj["circ"]   = v.mCirc;
                                                                  obj["ranged"] = v.mRanged;
                                                                  obj["versus"] = v.mVersus;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     mMult   = 0;
        int     mCirc = -1;
        bool    mRanged = false;
        QString mVersus = "";
    } v;

    QLineEdit* mult;
    QComboBox* circ;
    QCheckBox* ranged;
    QLineEdit* versus;

    QString optOut() {
        if (v.mMult < 1 || v.mCirc < 0 || v.mVersus.isEmpty()) return "<incomplete>";
        QString res = "Deadly Blow▲";
        res += ": +" + QString("%1 DCs").arg(v.mMult * 3) + " against " + v.mVersus + " (" +
               QStringList { "Very limited circumstances or group",
                             "Limited circumstances or group",
                             "Broad circumstances or group" }[v.mCirc];
        res += QString("; ") + (v.mRanged ? "Ranged" : "HTH");
        return res + ")";
    }

    void numeric(QString) override {
        QString txt = mult->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        mult->undo();
    }
};

class EideticMemory: public Talent {
public:
    EideticMemory(): Talent("Eidetic Memory")            { }
    EideticMemory(const EideticMemory& s): Talent(s)     { }
    EideticMemory(EideticMemory&& s): Talent(s)          { }
    EideticMemory(const QJsonObject& json): Talent(json) { v.mSize  = json["size"].toInt(-1);
                                                           v.mSense = json["sense"].toString("");
                                                         }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { size  = createComboBox(parent, layout, "How many senses?", { "All", "Two", "One" });
                                                                  sense = createLineEdit(parent, layout, "Sense?");
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override                { if (!noStore) store();
                                                                  QList<Points> size { 0_cp, 5_cp, 3_cp, 2_cp };
                                                                  return size[v.mSize + 1]; }
    void    restore() override                                  { vars s = v;
                                                                  sense->setText(s.mSense);
                                                                  size->setCurrentIndex(s.mSize);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mSize  = size->currentIndex();
                                                                  v.mSense = sense->text();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["size"]   = v.mSize;
                                                                  obj["sense"]  = v.mSense;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     mSize  = -1;
        QString mSense = "";
    } v;

    QLineEdit* sense;
    QComboBox* size;

    QString optOut() {
        if (v.mSize >= 2 && v.mSense.isEmpty()) return "<incomplete>";
        QString res = "Eidetic Memory";
        if (v.mSize > 0) res += " with " + v.mSense;
        return res;
    }
};

class EnvironmentalMovement: public Talent {
public:
    EnvironmentalMovement(): Talent("Environmental Movement")        { }
    EnvironmentalMovement(const EnvironmentalMovement& s): Talent(s) { }
    EnvironmentalMovement(EnvironmentalMovement&& s): Talent(s)      { }
    EnvironmentalMovement(const QJsonObject& json): Talent(json)     { v.mPlus  =  json["plus"].toInt(0);
                                                                       v.mVersus = json["versus"].toString("");
                                                                     }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { plus   = createLineEdit(parent, layout, "Plus?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  versus = createLineEdit(parent, layout, "Environment?");
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override                { if (!noStore) store();
                                                                  return v.mPlus * 2_cp; }
    void    restore() override                                  { vars s = v;
                                                                  plus->setText(QString("%1").arg(s.mPlus));
                                                                  versus->setText(s.mVersus);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mPlus   = plus->text().toInt(0);
                                                                  v.mVersus = versus->text();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["plus"]   = v.mPlus;
                                                                  obj["versus"] = v.mVersus;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     mPlus   = 0;
        QString mVersus = "";
    } v;

    QLineEdit* plus;
    QLineEdit* versus;

    QString optOut() {
        if (v.mPlus < 1 || v.mVersus.isEmpty()) return "<incomplete>";
        QString res = "Environmental Movement: ";
        res += "-" + QString("%1 CV%2/DC%2").arg(v.mPlus).arg(v.mPlus > 1 ? "s" : "")  + " penalties in " + v.mVersus;
        return res;
    }

    void numeric(QString) override {
        QString txt = plus->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        plus->undo();
    }
};

class LightningReflexes: public Talent {
public:
    LightningReflexes(): Talent("Lightning Reflexes")         { }
    LightningReflexes(const LightningReflexes& s): Talent(s)  { }
    LightningReflexes(LightningReflexes&& s): Talent(s)       { }
    LightningReflexes(const QJsonObject& json): Talent(json) { v.mPlus  = json["plus"].toInt(0);
                                                               v.mLevel = json["level"].toInt(0);
                                                               v.mWith  = json["with"].toString("");
                                                            }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { plus  = createLineEdit(parent, layout, "Plus?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  level = createComboBox(parent, layout, "Level?", { "Act first with All Actions",
                                                                                                                     "Act first with All HTH Attacks",
                                                                                                                     "Act first with All Ranged Attacks",
                                                                                                                     "Act first with a Large Group of Actions",
                                                                                                                     "Act first with a Small Group of Actions",
                                                                                                                     "Act first with a Single Action" });
                                                                  with  = createLineEdit(parent, layout, "Environment?");
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override                { if (!noStore) store();
                                                                  return v.mPlus * 2_cp; }
    void    restore() override                                  { vars s = v;
                                                                  plus->setText(QString("%1").arg(s.mPlus));
                                                                  level->setCurrentIndex(s.mLevel);
                                                                  with->setText(s.mWith);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mPlus  = plus->text().toInt(0);
                                                                  v.mLevel = level->currentIndex();
                                                                  v.mWith  = with->text();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["plus"]  = v.mPlus;
                                                                  obj["with"]  = v.mWith;
                                                                  obj["level"] = v.mLevel;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     mPlus  = 0;
        int     mLevel = -1;
        QString mWith  = "";
    } v;

    QLineEdit* plus;
    QComboBox* level;
    QLineEdit* with;

    QString optOut() {
        if (v.mPlus < 1 || v.mLevel < 0 || (v.mLevel > 3 && v.mWith.isEmpty())) return "<incomplete>";
        QString res = "Lightning Reflexes▲: ";
        res += QString("%1 DEX").arg(v.mPlus)  + " to " + QStringList{ "Act first with All Actions",
                "Act first with All HTH Attacks",
                "Act first with All Ranged Attacks",
                "Act first with a Large Group of Actions",
                "Act first with a Small Group of Actions",
                "Act first with a Single Action"}[v.mLevel];
        if (v.mLevel > 3) res += " with " + v.mWith;
        return res;
    }

    void numeric(QString) override {
        QString txt = plus->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        plus->undo();
    }
};

class Resistance: public Talent {
public:
    Resistance(): Talent("Resistance"       )         { }
    Resistance(const Resistance& s): Talent(s)        { }
    Resistance(Resistance&& s): Talent(s)             { }
    Resistance(const QJsonObject& json): Talent(json) { v.mPlus  = json["plus"].toInt(0);
                                                      }

    QString description(bool showRoll = false) override         { return (showRoll ?
#ifndef ISHSC
                                                                          add(Sheet::ref().character().EGO().roll(), v.mPlus)
#else
                                                                          QString("+%1").arg(v._plus)
#endif
                                                                          + " " : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { plus  = createLineEdit(parent, layout, "Plus?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override                { if (!noStore) store();
                                                                  return Points(v.mPlus); }
    void    restore() override                                  { vars s = v; plus->setText(QString("%1").arg(s.mPlus)); v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mPlus = plus->text().toInt(0);
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["plus"] = v.mPlus;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int mPlus  = 0;
    } v;

    QLineEdit* plus;

    QString optOut() {
        if (v.mPlus < 1) return "<incomplete>";
        QString res = "Resistance: ";
        res += QString("%1 to EGO roll for resisting wounding or interrogation").arg(v.mPlus);
        return res;
    }

    void numeric(QString) override {
        QString txt = plus->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        plus->undo();
    }
};

class SimulateDeath: public Talent {
public:
    SimulateDeath(): Talent("Simulate Death")            { }
    SimulateDeath(const SimulateDeath& s): Talent(s)     { }
    SimulateDeath(SimulateDeath&& s): Talent(s)          { }
    SimulateDeath(const QJsonObject& json): Talent(json) { v.mPlus  = json["plus"].toInt(0);
                                                         }

    QString description(bool showRoll = false) override         { return (showRoll ?
#ifndef ISHSC
                                                                  add(Sheet::ref().character().EGO().roll(), v.mPlus) +
#else
                                                                  QString("+%1").arg(v._plus) +
#endif
                                                                  " " : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { plus  = createLineEdit(parent, layout, "Plus?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                 return true;
                                                                }
    Points points(bool noStore = false) override                { if (!noStore) store();
                                                                  return (v.mPlus + 1) * 3_cp; }
    void    restore() override                                  { vars s = v; plus->setText(QString("%1").arg(s.mPlus)); v = s;
                                                                }
    QString roll() override                                     { return
#ifndef ISHSC
                                                                      add(Sheet::ref().character().EGO().roll(), v.mPlus);
#else
                                                                      QString("+%1").arg(v._plus);
#endif
                                                                }
    void    store() override                                    { v.mPlus = plus->text().toInt(0);
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["plus"] = v.mPlus;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int mPlus  = 0;
    } v;

    QLineEdit* plus;

    QString optOut() {
        if (v.mPlus < 0) return "<incomplete>";
        QString res = "Simulate Death";
        if (v.mPlus > 0) res += QString(": +%1 to EGO").arg(v.mPlus);
        return res;
    }

    void numeric(QString) override {
        QString txt = plus->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        plus->undo();
    }
};

class SpeedReading: public Talent {
public:
    SpeedReading(): Talent("Speed Reading")             { }
    SpeedReading(const SpeedReading& s): Talent(s)      { }
    SpeedReading(SpeedReading&& s): Talent(s)           { }
    SpeedReading(const QJsonObject& json): Talent(json) { v.mMult = json["mult"].toInt(1);
                                                        }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { mult = createLineEdit(parent, layout, "x10?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                 return true;
                                                                }
    Points points(bool noStore = false) override                { if (!noStore) store();
                                                                  return 4_cp + (v.mMult - 1) * 2_cp; }
    void    restore() override                                  { vars s = v; mult->setText(QString("%1").arg(s.mMult)); v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mMult = mult->text().toInt(0);
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["mult"] = v.mMult;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int mMult = 0;
    } v;

    QLineEdit* mult;

    QString optOut() {
        if (v.mMult < 1) return "<incomplete>";
        QString res = "Speed Reading: ";
        res += QString("x%1 normal").arg((int) pow(10.0, v.mMult));
        return res;
    }

    void numeric(QString) override {
        QString txt = mult->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        mult->undo();
    }
};

class StrikingAppearence: public Talent {
public:
    StrikingAppearence(): Talent("Striking Appearences")       { }
    StrikingAppearence(const StrikingAppearence& s): Talent(s) { }
    StrikingAppearence(StrikingAppearence&& s): Talent(s)      { }
    StrikingAppearence(const QJsonObject& json): Talent(json)  { v.mLimit = json["limit"].toBool(false);
                                                                 v.mWho   = json["who"].toString("");
                                                               }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { limit = createCheckBox(parent, layout, "Specific group");
                                                                  who  = createLineEdit(parent, layout, "Who is affected?");
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override                { if (!noStore) store();
                                                                  return v.mLimit ? 2_cp : 3_cp; }
    void    restore() override                                  { vars s = v;
                                                                  limit->setChecked(s.mLimit);
                                                                  who->setText(s.mWho);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mLimit = limit->isChecked();
                                                                  v.mWho   = who->text();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["limit"] = v.mLimit;
                                                                  obj["who"]   = v.mWho;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     mLimit = 0;
        QString mWho  = "";
    } v;

    QCheckBox* limit;
    QLineEdit* who;

    QString optOut() {
        if (v.mLimit && v.mWho.isEmpty()) return "<incomplete>";
        QString res = "Striking Appearence";
        if (v.mLimit) res += "versus " + v.mWho;
        return res;
    }
};

class UniversalTranslator: public Talent {
public:
    UniversalTranslator(): Talent("Universal Translatorϴ")       { }
    UniversalTranslator(const UniversalTranslator& s): Talent(s) { }
    UniversalTranslator(UniversalTranslator&& s): Talent(s)      { }
    UniversalTranslator(const QJsonObject& json): Talent(json)   { v.mPlus = json["plus"].toInt(0);
                                                                 }

    QString description(bool showRoll = false) override         { return (showRoll ?
#ifndef ISHSC
                                                                          add(Sheet::ref().character().INT().roll(), v.mPlus) +
#else
                                                                          QString("+%1").arg(v._plus) +
#endif
                                                                          " " : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { plus = createLineEdit(parent, layout, "Plus?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override                { if (!noStore) store();
                                                                  return 20_cp + v.mPlus; }
    void    restore() override                                  { vars s = v; plus->setText(QString("%1").arg(s.mPlus)); v = s;
                                                                }
    QString roll() override                                     { return
#ifndef ISHSC
                                                                      add(Sheet::ref().character().INT().roll(), v.mPlus); }
#else
                                                                      QString("+%1").arg(v._plus); }
#endif
    void    store() override                                    { v.mPlus = plus->text().toInt(0);
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["plus"] = v.mPlus;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int mPlus  = 0;
    } v;

    QLineEdit* plus;

    QString optOut() {
        QString res = "Universal Translatorϴ";
        if (v.mPlus > 0) res += QString(": +%1 to INT roll").arg(v.mPlus);
        return res;
    }

    void numeric(QString) override {
        QString txt = plus->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        plus->undo();
    }
};

class Weaponmaster: public Talent {
public:
    Weaponmaster(): Talent("Weaponmaster")              { }
    Weaponmaster(const Weaponmaster& s): Talent(s)      { }
    Weaponmaster(Weaponmaster&& s): Talent(s)           { }
    Weaponmaster(const QJsonObject& json): Talent(json) { v.mMult    = json["mult"].toInt(0);
                                                          v.mWpns    = json["wpns"].toInt(0);
                                                          v.mRanged  = json["ranged"].toBool(false);
                                                          v.mKilling = json["killing"].toBool(false);
                                                          v.mWith    = json["with"].toString("");
                                                        }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { mult    = createLineEdit(parent, layout, "Multiples?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  wpns    = createComboBox(parent, layout, "Weapons?", { "Very limited group of weapons",
                                                                                                                         "Limited group of weapons",
                                                                                                                         "Broad group of weapons" });
                                                                  ranged  = createCheckBox(parent, layout, "Ranged");
                                                                  killing = createCheckBox(parent, layout, "Kllling");
                                                                  with    = createLineEdit(parent, layout, "With?");
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override              { if (!noStore) store();
                                                                  QList<Points> wpns { 0_cp, 12_cp, 16_cp, 19_cp };
                                                                  return wpns[v.mWpns + 1] * v.mMult; }
    void    restore() override                                  { vars s = v;
                                                                  mult->setText(QString("%1").arg(s.mMult));
                                                                  wpns->setCurrentIndex(s.mWpns);
                                                                  ranged->setChecked(s.mRanged);
                                                                  killing->setChecked(s.mKilling);
                                                                  with->setText(s.mWith);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mMult    = mult->text().toInt(0);
                                                                  v.mWpns    = wpns->currentIndex();
                                                                  v.mRanged  = ranged->isChecked();
                                                                  v.mKilling = killing->isChecked();
                                                                  v.mWith    = with->text();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["mult"]    = v.mMult;
                                                                  obj["wpns"]    = v.mWpns;
                                                                  obj["ranged"]  = v.mRanged;
                                                                  obj["killing"] = v.mKilling;
                                                                  obj["with"]    = v.mWith;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     mMult    = 0;
        int     mWpns    = -1;
        bool    mRanged  = false;
        bool    mKilling = false;
        QString mWith    = "";
    } v;

    QLineEdit* mult;
    QComboBox* wpns;
    QCheckBox* ranged;
    QCheckBox* killing;
    QLineEdit* with;

    QString optOut() {
        if (v.mMult < 1 || v.mWpns < 0 || v.mWith.isEmpty()) return "<incomplete>";
        QString res = "Weaponmaster▲";
        res += ": +" + QString("%1 DCs").arg(v.mMult * 3) + " with " + v.mWith + " (" +
               QStringList { "Very limited group of weapons",
                             "Limited group of weapons",
                             "Broad group of weapons" }[v.mWpns];
        res += QString("; ") + (v.mRanged ? "Ranged" : "HTH");
        res += QString("; ") + (v.mKilling ? "KIlling" : "Normal");
        return res + ")";
    }

    void numeric(QString) override {
        QString txt = mult->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        mult->undo();
    }
};

CLASS_SPACE(AbsoluteRangeSense,  "Absolute Range Sense", 3_cp);
CLASS_SPACE(AbsoluteTimeSense,   "Absolute Time Sense",  3_cp);
CLASS_SPACE(AnimalFriendship,    "Animal Friendship",    20_cp);
CLASS_SPACE(BumpOfDirection,     "Bump Of Direction",    3_cp);
CLASS_SPACE(DoubleJointed,       "Double Jointed",       4_cp);
CLASS_SPACE(LightningCalculator, "Lightning Calculator", 3_cp);
CLASS      (Lightsleep,                                  3_cp);
CLASS_SPACE(OffHandDefense,      "Off-Hand Defense",     2_cp);
CLASS_SPACE(PerfectPitch,        "Perfect Pitch",        3_cp);

#undef CLASS
#undef CLASS_SPACE

#endif // TALENT_H
