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
    void    form(QWidget*, QVBoxLayout*) override       { throw "No options, immediately accept"; }
    QString name() override                             { return v._name; }
    Points<> points(bool noStore = false) override      { if (!noStore) store(); return 0_cp; }
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
        Points<> points(bool noStore = false) { if (!noStore) store(); return y; }\
    };
#define CLASS_SPACE(x,y,z)\
    class x: public Talent {\
    public:\
        x(): Talent(y) { }\
        x(const x& s): Talent(s) { }\
        x(x&& s): Talent(s) { }\
        x(const QJsonObject& json): Talent(json) { }\
        Points<> points(bool noStore = false) { if (!noStore) store(); return z; }\
    };

class Ambidexterity: public Talent {
public:
    Ambidexterity(): Talent("Ambidexterity")             { }
    Ambidexterity(const Ambidexterity& s): Talent(s)     { }
    Ambidexterity(Ambidexterity&& s): Talent(s)          { }
    Ambidexterity(const QJsonObject& json): Talent(json) { v._offhand = json["offhand"].toInt(0);
                                                         }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { offhand = createLineEdit(parent, layout, "Offhand penalties offset", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                }
    Points<> points(bool noStore = false) override              { if (!noStore) store();
                                                                  return Points<>(v._offhand); }
    void    restore() override                                  { vars s = v;
                                                                  QString msg = QString("%1").arg(s._offhand);
                                                                  offhand->setText(msg);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._offhand = offhand->text().toInt(0);
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["offhand"] = v._offhand;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int _offhand = 0;
    } v;

    QLineEdit* offhand;

    QString optOut() {
        return "Ambidexterity: " + QString("+%1").arg(v._offhand) + " vs. Offhand Penalties";
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
    CombatLuck(const QJsonObject& json): Talent(json) { v._levels = json["levels"].toInt(0);
                                                        v._put = json["put"].toInt(1);
                                                      }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { levels = createLineEdit(parent, layout, "Levels of Combat Luck?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  put    = createComboBox(parent, layout, "Add to?", { "Nothing", "Primary", "Secondary" });
                                                                }
    Points<> points(bool noStore = false) override              { if (!noStore) store();
                                                                  return 6_cp * v._levels; }
    void    restore() override                                  { vars s = v;
                                                                  QString msg = QString("%1").arg(s._levels);
                                                                  levels->setText(msg);
                                                                  put->setCurrentIndex(s._put);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._levels = levels->text().toInt(0);
                                                                  v._put    = put->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["levels"] = v._levels;
                                                                  obj["put"]    = v._put;
                                                                  return obj;
                                                                }

    int rED() override   { return v._levels * 3; }
    int rPD() override   { return rED(); }
    int place() override { return v._put; }

private:
    struct vars {
        int _levels = 0;
        int _put    = -1;
    } v;

    QLineEdit* levels;
    QComboBox* put;

    QString optOut() {
        return "Combat Luck: " + QString("+%1 rPD/+%1 rED").arg(v._levels * 3);
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
    CombatSense(const QJsonObject& json): Talent(json) { v._plus  = json["plus"].toInt(0);
                                                         v._sense = json["sense"].toBool(false);
                                                       }

    QString description(bool showRoll = false) override         { return (showRoll ? roll() + " " : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { plus  = createLineEdit(parent, layout, "Plus to toll?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  sense = createCheckBox(parent, layout, "Make a sense");
                                                                }
    Points<> points(bool noStore = false) override              { if (!noStore) store();
                                                                  return 15_cp + v._plus + (v._sense ? 2_cp : 0_cp); }
    void    restore() override                                  { vars s = v;
                                                                  QString msg = QString("%1").arg(v._plus);
                                                                  int sns = v._sense;
                                                                  plus->setText(msg);
                                                                  sense->setChecked(sns);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return add(Sheet::ref().character().INT().roll(), v._plus); }
    void    store() override                                    { v._plus = plus->text().toInt(0);
                                                                  v._sense = sense->isChecked();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["plus"]  = v._plus;
                                                                  obj["sense"] = v._sense;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int  _plus  = 0;
        bool _sense = false;
    } v;

    QLineEdit* plus;
    QCheckBox* sense;

    QString optOut() {
        QString res = "Combat Sense";
        if (v._plus > 0) res += ": +" + QString("%1").arg(v._plus);
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
    DangerSense(const QJsonObject& json): Talent(json) { _plus   = json["plus"].toInt(0);
                                                         _sense  = json["sense"].toBool(false);
                                                         _intuit = json["intuit"].toBool(false);
                                                         _anlze  = json["anlze"].toBool(false);
                                                         _descr  = json["descr"].toBool(false);
                                                         _pen    = json["pen"].toBool(false);
                                                         _thrgh  = json["thrgh"].toString("");
                                                         _dtct   = json["dtct"].toInt(0);
                                                         _area   = json["area"].toInt(0);
                                                       }

    QString description(bool showRoll = false) override         { return (showRoll ? roll() + " " : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { plus   = createLineEdit(parent, layout, "Plus to toll?", std::mem_fn(&SkillTalentOrPerk::numeric));
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
                                                                }
    Points<> points(bool noStore = false) override              { if (!noStore) store();
                                                                  return 15_cp +
                                                                         _plus +
                                                                         (_sense ? 2_cp : 0_cp) -
                                                                         (_intuit ? 5_cp : 0_cp) +
                                                                         (_anlze ? 5_cp : 0_cp) +
                                                                         (_descr ? 5_cp : 0_cp) +
                                                                         (_pen ? 5_cp : 0_cp) +
                                                                         (_dtct - 1) * 5_cp +
                                                                         (_area - 1) * 5_cp;
                                                                }
    void    restore() override                                  { bool    sns = _sense;
                                                                  bool    ntt = _intuit;
                                                                  bool    nlz = _anlze;
                                                                  bool    dsc = _descr;
                                                                  bool    pn  = _pen;
                                                                  QString thr = _thrgh;
                                                                  int     dtc = _dtct;
                                                                  int     are = _area;
                                                                  sense->setChecked(sns);
                                                                  intuit->setChecked(ntt);
                                                                  anlze->setChecked(nlz);
                                                                  descr->setChecked(dsc);
                                                                  pen->setChecked(pn);
                                                                  dtct->setCurrentIndex(dtc);
                                                                  area->setCurrentIndex(are);
                                                                  QString msg = QString("%1").arg(_plus);
                                                                  thrgh->setText(thr);
                                                                  plus->setText(msg);
                                                                  _sense  = sns;
                                                                  _intuit = ntt;
                                                                  _anlze  = nlz;
                                                                  _descr  = dsc;
                                                                  _pen    = pn;
                                                                  _thrgh  = thr;
                                                                  _dtct   = dtc;
                                                                  _area   = are;
                                                                }
    QString roll() override                                     { return add(Sheet::ref().character().INT().roll(), _plus); }
    void    store() override                                    { _plus   = plus->text().toInt(0);
                                                                  _sense  = sense->isChecked();
                                                                  _intuit = intuit->isChecked();
                                                                  _anlze  = anlze->isChecked();
                                                                  _descr  = descr->isChecked();
                                                                  _pen    = pen->isChecked();
                                                                  _thrgh  = thrgh->text();
                                                                  _dtct   = dtct->currentIndex();
                                                                  _area   = area->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["plus"]   = _plus;
                                                                  obj["semse"]  = _sense;
                                                                  obj["intuit"] = _intuit;
                                                                  obj["anlze"]  = _anlze;
                                                                  obj["descr"]  = _descr;
                                                                  obj["pen"]    = _pen;
                                                                  obj["thrgh"]  = _thrgh;
                                                                  obj["dtct"]   = _dtct;
                                                                  obj["area"]   = _area;
                                                                  return obj;
                                                                }

private:
    int     _plus   = 0;
    bool    _sense  = false;
    bool    _intuit = false;
    bool    _anlze  = false;
    bool    _descr  = false;
    bool    _pen    = false;
    QString _thrgh  = "";
    int     _dtct   = -1;
    int     _area   = -1;
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
        if (_dtct < 0 || _area < 0 || (_pen && _thrgh.isEmpty()) || (_intuit && (_dtct > 1 || _area > 1))) return "<incomplete>";
        QString res = "Danger Senseϴ";
        if (_plus > 0) res += ": +" + QString("%1").arg(_plus);
        QString sep = " (";
        if (_sense) { res += sep + "Sense"; sep = "; "; }
        if (_intuit) { res += sep + "Intuition"; sep = "; "; }
        if (_anlze) { res += sep + "Analyze"; sep = "; "; }
        if (_descr) { res += sep + "Discriminate"; sep = "; "; }
        if (_pen) { res += sep + "Pentrates " + _thrgh; sep = "; "; }
        res += sep += QStringList{ "In Combat, perceivable", "In or out of combat, perceivable", "Any" }[_dtct];
        res += "; " + QStringList{ "Personal", "Anyone in immediate area", "Anyone in general area", "Anyone in any area" }[_area];
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
    DeadlyBlow(const QJsonObject& json): Talent(json) { v._mult  =  json["mult"].toInt(0);
                                                        v._circ  =  json["circ"].toInt(0);
                                                        v._ranged = json["ranged"].toBool(false);
                                                        v._versus = json["versus"].toString("");
                                                      }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { mult   = createLineEdit(parent, layout, "Multiples?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  circ   = createComboBox(parent, layout, "Circumstances?", { "Very limited circumstances or group",
                                                                                                                              "Limited circumstances or group",
                                                                                                                              "Broad circumstances or group" });
                                                                  ranged = createCheckBox(parent, layout, "Ranged");
                                                                  versus = createLineEdit(parent, layout, "Against?");
                                                                }
    Points<> points(bool noStore = false) override              { if (!noStore) store();
                                                                  QList<Points<>> circ { 0_cp, 12_cp, 16_cp, 19_cp };
                                                                  return circ[v._circ + 1] * v._mult; }
    void    restore() override                                  { vars s = v;
                                                                  mult->setText(QString("%1").arg(s._mult));
                                                                  circ->setCurrentIndex(s._circ);
                                                                  ranged->setChecked(s._ranged);
                                                                  versus->setText(s._versus);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._mult   = mult->text().toInt(0);
                                                                  v._circ   = circ->currentIndex();
                                                                  v._ranged = ranged->isChecked();
                                                                  v._versus = versus->text();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["mult"]   = v._mult;
                                                                  obj["circ"]   = v._circ;
                                                                  obj["ranged"] = v._ranged;
                                                                  obj["versus"] = v._versus;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     _mult   = 0;
        int     _circ = -1;
        bool    _ranged = false;
        QString _versus = "";
    } v;

    QLineEdit* mult;
    QComboBox* circ;
    QCheckBox* ranged;
    QLineEdit* versus;

    QString optOut() {
        if (v._mult < 1 || v._circ < 0 || v._versus.isEmpty()) return "<incomplete>";
        QString res = "Deadly Blow▲";
        res += ": +" + QString("%1 DCs").arg(v._mult * 3) + " against " + v._versus + " (" +
               QStringList { "Very limited circumstances or group",
                             "Limited circumstances or group",
                             "Broad circumstances or group" }[v._circ];
        res += QString("; ") + (v._ranged ? "Ranged" : "HTH");
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
    EideticMemory(const QJsonObject& json): Talent(json) { v._size  = json["size"].toInt(-1);
                                                           v._sense = json["sense"].toString("");
                                                         }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { size  = createComboBox(parent, layout, "How many senses?", { "All", "Two", "One" });
                                                                  sense = createLineEdit(parent, layout, "Sense?");
                                                                }
    Points<> points(bool noStore = false) override              { if (!noStore) store();
                                                                  QList<Points<>> size { 0_cp, 5_cp, 3_cp, 2_cp };
                                                                  return size[v._size + 1]; }
    void    restore() override                                  { vars s = v;
                                                                  sense->setText(s._sense);
                                                                  size->setCurrentIndex(s._size);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._size  = size->currentIndex();
                                                                  v._sense = sense->text();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["size"]   = v._size;
                                                                  obj["sense"]  = v._sense;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     _size  = -1;
        QString _sense = "";
    } v;

    QLineEdit* sense;
    QComboBox* size;

    QString optOut() {
        if (v._size >= 2 && v._sense.isEmpty()) return "<incomplete>";
        QString res = "Eidetic Memory";
        if (v._size > 0) res += " with " + v._sense;
        return res;
    }
};

class EnvironmentalMovement: public Talent {
public:
    EnvironmentalMovement(): Talent("Environmental Movement")        { }
    EnvironmentalMovement(const EnvironmentalMovement& s): Talent(s) { }
    EnvironmentalMovement(EnvironmentalMovement&& s): Talent(s)      { }
    EnvironmentalMovement(const QJsonObject& json): Talent(json)     { v._plus  =  json["plus"].toInt(0);
                                                                       v._versus = json["versus"].toString("");
                                                                     }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { plus   = createLineEdit(parent, layout, "Plus?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  versus = createLineEdit(parent, layout, "Environment?");
                                                                }
    Points<> points(bool noStore = false) override              { if (!noStore) store();
                                                                  return v._plus * 2_cp; }
    void    restore() override                                  { vars s = v;
                                                                  plus->setText(QString("%1").arg(s._plus));
                                                                  versus->setText(s._versus);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._plus   = plus->text().toInt(0);
                                                                  v._versus = versus->text();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["plus"]   = v._plus;
                                                                  obj["versus"] = v._versus;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     _plus   = 0;
        QString _versus = "";
    } v;

    QLineEdit* plus;
    QLineEdit* versus;

    QString optOut() {
        if (v._plus < 1 || v._versus.isEmpty()) return "<incomplete>";
        QString res = "Environmental Movement: ";
        res += "-" + QString("%1 CV%2/DC%2").arg(v._plus).arg(v._plus > 1 ? "s" : "")  + " penalties in " + v._versus;
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
    LightningReflexes(const QJsonObject& json): Talent(json) { v._plus  = json["plus"].toInt(0);
                                                               v._level = json["level"].toInt(0);
                                                               v._with  = json["with"].toString("");
                                                            }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { plus  = createLineEdit(parent, layout, "Plus?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  level = createComboBox(parent, layout, "Level?", { "Act first with All Actions",
                                                                                                                     "Act first with All HTH Attacks",
                                                                                                                     "Act first with All Ranged Attacks",
                                                                                                                     "Act first with a Large Group of Actions",
                                                                                                                     "Act first with a Small Group of Actions",
                                                                                                                     "Act first with a Single Action" });
                                                                  with  = createLineEdit(parent, layout, "Environment?");
                                                                }
    Points<> points(bool noStore = false) override              { if (!noStore) store();
                                                                  return v._plus * 2_cp; }
    void    restore() override                                  { vars s = v;
                                                                  plus->setText(QString("%1").arg(s._plus));
                                                                  level->setCurrentIndex(s._level);
                                                                  with->setText(s._with);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._plus  = plus->text().toInt(0);
                                                                  v._level = level->currentIndex();
                                                                  v._with  = with->text();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["plus"]  = v._plus;
                                                                  obj["with"]  = v._with;
                                                                  obj["level"] = v._level;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     _plus  = 0;
        int     _level = -1;
        QString _with  = "";
    } v;

    QLineEdit* plus;
    QComboBox* level;
    QLineEdit* with;

    QString optOut() {
        if (v._plus < 1 || v._level < 0 || (v._level > 3 && v._with.isEmpty())) return "<incomplete>";
        QString res = "Lightning Reflexes▲: ";
        res += QString("%1 DEX").arg(v._plus)  + " to " + QStringList{ "Act first with All Actions",
                "Act first with All HTH Attacks",
                "Act first with All Ranged Attacks",
                "Act first with a Large Group of Actions",
                "Act first with a Small Group of Actions",
                "Act first with a Single Action"}[v._level];
        if (v._level > 3) res += " with " + v._with;
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
    Resistance(const QJsonObject& json): Talent(json) { v._plus  = json["plus"].toInt(0);
                                                      }

    QString description(bool showRoll = false) override         { return (showRoll ? add(Sheet::ref().character().EGO().roll(), v._plus) + " " : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { plus  = createLineEdit(parent, layout, "Plus?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                }
    Points<> points(bool noStore = false) override              { if (!noStore) store();
                                                                  return Points<>(v._plus); }
    void    restore() override                                  { vars s = v; plus->setText(QString("%1").arg(s._plus)); v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._plus = plus->text().toInt(0);
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["plus"] = v._plus;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int _plus  = 0;
    } v;

    QLineEdit* plus;

    QString optOut() {
        if (v._plus < 1) return "<incomplete>";
        QString res = "Resistance: ";
        res += QString("%1 to EGO roll for resisting wounding or interrogation").arg(v._plus);
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
    SimulateDeath(const QJsonObject& json): Talent(json) { v._plus  = json["plus"].toInt(0);
                                                         }

    QString description(bool showRoll = false) override         { return (showRoll ? add(Sheet::ref().character().EGO().roll(), v._plus) + " " : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { plus  = createLineEdit(parent, layout, "Plus?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                }
    Points<> points(bool noStore = false) override              { if (!noStore) store();
                                                                  return (v._plus + 1) * 3_cp; }
    void    restore() override                                  { vars s = v; plus->setText(QString("%1").arg(s._plus)); v = s;
                                                                }
    QString roll() override                                     { return add(Sheet::ref().character().EGO().roll(), v._plus); }
    void    store() override                                    { v._plus = plus->text().toInt(0);
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["plus"] = v._plus;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int _plus  = 0;
    } v;

    QLineEdit* plus;

    QString optOut() {
        if (v._plus < 0) return "<incomplete>";
        QString res = "Simulate Death";
        if (v._plus > 0) res += QString(": +%1 to EGO").arg(v._plus);
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
    SpeedReading(const QJsonObject& json): Talent(json) { v._mult = json["mult"].toInt(1);
                                                        }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { mult = createLineEdit(parent, layout, "x10?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                }
    Points<> points(bool noStore = false) override              { if (!noStore) store();
                                                                  return 4_cp + (v._mult - 1) * 2_cp; }
    void    restore() override                                  { vars s = v; mult->setText(QString("%1").arg(s._mult)); v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._mult = mult->text().toInt(0);
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["mult"] = v._mult;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int _mult = 0;
    } v;

    QLineEdit* mult;

    QString optOut() {
        if (v._mult < 1) return "<incomplete>";
        QString res = "Speed Reading: ";
        res += QString("x%1 normal").arg((int) pow(10.0, v._mult));
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
    StrikingAppearence(const QJsonObject& json): Talent(json)  { v._limit = json["limit"].toBool(false);
                                                                 v._who   = json["who"].toString("");
                                                               }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { limit = createCheckBox(parent, layout, "Specific group");
                                                                  who  = createLineEdit(parent, layout, "Who is affected?");
                                                                }
    Points<> points(bool noStore = false) override              { if (!noStore) store();
                                                                  return v._limit ? 2_cp : 3_cp; }
    void    restore() override                                  { vars s = v;
                                                                  limit->setChecked(s._limit);
                                                                  who->setText(s._who);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._limit = limit->isChecked();
                                                                  v._who   = who->text();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["limit"] = v._limit;
                                                                  obj["who"]   = v._who;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     _limit = 0;
        QString _who  = "";
    } v;

    QCheckBox* limit;
    QLineEdit* who;

    QString optOut() {
        if (v._limit && v._who.isEmpty()) return "<incomplete>";
        QString res = "Striking Appearence";
        if (v._limit) res += "versus " + v._who;
        return res;
    }
};

class UniversalTranslator: public Talent {
public:
    UniversalTranslator(): Talent("Universal Translatorϴ")       { }
    UniversalTranslator(const UniversalTranslator& s): Talent(s) { }
    UniversalTranslator(UniversalTranslator&& s): Talent(s)      { }
    UniversalTranslator(const QJsonObject& json): Talent(json)   { v._plus = json["plus"].toInt(0);
                                                                 }

    QString description(bool showRoll = false) override         { return (showRoll ? add(Sheet::ref().character().INT().roll(), v._plus) + " " : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { plus = createLineEdit(parent, layout, "Plus?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                }
    Points<> points(bool noStore = false) override              { if (!noStore) store();
                                                                  return 20_cp + v._plus; }
    void    restore() override                                  { vars s = v; plus->setText(QString("%1").arg(s._plus)); v = s;
                                                                }
    QString roll() override                                     { return add(Sheet::ref().character().INT().roll(), v._plus); }
    void    store() override                                    { v._plus = plus->text().toInt(0);
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["plus"] = v._plus;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int _plus  = 0;
    } v;

    QLineEdit* plus;

    QString optOut() {
        QString res = "Universal Translatorϴ";
        if (v._plus > 0) res += QString(": +%1 to INT roll").arg(v._plus);
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
    Weaponmaster(const QJsonObject& json): Talent(json) { v._mult    = json["mult"].toInt(0);
                                                          v._wpns    = json["wpns"].toInt(0);
                                                          v._ranged  = json["ranged"].toBool(false);
                                                          v._killing = json["killing"].toBool(false);
                                                          v._with    = json["with"].toString("");
                                                        }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { mult    = createLineEdit(parent, layout, "Multiples?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  wpns    = createComboBox(parent, layout, "Weapons?", { "Very limited group of weapons",
                                                                                                                         "Limited group of weapons",
                                                                                                                         "Broad group of weapons" });
                                                                  ranged  = createCheckBox(parent, layout, "Ranged");
                                                                  killing = createCheckBox(parent, layout, "Kllling");
                                                                  with    = createLineEdit(parent, layout, "With?");
                                                                }
    Points<> points(bool noStore = false) override              { if (!noStore) store();
                                                                  QList<Points<>> wpns { 0_cp, 12_cp, 16_cp, 19_cp };
                                                                  return wpns[v._wpns + 1] * v._mult; }
    void    restore() override                                  { vars s = v;
                                                                  mult->setText(QString("%1").arg(s._mult));
                                                                  wpns->setCurrentIndex(s._wpns);
                                                                  ranged->setChecked(s._ranged);
                                                                  killing->setChecked(s._killing);
                                                                  with->setText(s._with);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._mult    = mult->text().toInt(0);
                                                                  v._wpns    = wpns->currentIndex();
                                                                  v._ranged  = ranged->isChecked();
                                                                  v._killing = killing->isChecked();
                                                                  v._with    = with->text();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Talent::toJson();
                                                                  obj["mult"]    = v._mult;
                                                                  obj["wpns"]    = v._wpns;
                                                                  obj["ranged"]  = v._ranged;
                                                                  obj["killing"] = v._killing;
                                                                  obj["with"]    = v._with;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     _mult    = 0;
        int     _wpns    = -1;
        bool    _ranged  = false;
        bool    _killing = false;
        QString _with    = "";
    } v;

    QLineEdit* mult;
    QComboBox* wpns;
    QCheckBox* ranged;
    QCheckBox* killing;
    QLineEdit* with;

    QString optOut() {
        if (v._mult < 1 || v._wpns < 0 || v._with.isEmpty()) return "<incomplete>";
        QString res = "Weaponmaster▲";
        res += ": +" + QString("%1 DCs").arg(v._mult * 3) + " with " + v._with + " (" +
               QStringList { "Very limited group of weapons",
                             "Limited group of weapons",
                             "Broad group of weapons" }[v._wpns];
        res += QString("; ") + (v._ranged ? "Ranged" : "HTH");
        res += QString("; ") + (v._killing ? "KIlling" : "Normal");
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
