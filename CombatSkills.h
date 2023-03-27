#ifndef COMBATSKILLS_H
#define COMBATSKILLS_H

#include "skilltalentorperk.h"

class CombatSkills: public SkillTalentOrPerk {
public:
    CombatSkills(): SkillTalentOrPerk() { }
    CombatSkills(QString name)
        : v { name } { }
    CombatSkills(const CombatSkills& s)
        : SkillTalentOrPerk()
        , v(s.v) { }
    CombatSkills(CombatSkills&& s)
        : SkillTalentOrPerk()
        , v(s.v) { }
    CombatSkills(const QJsonObject& json)
        : SkillTalentOrPerk()
        , v { json["name"].toString("") } { }

    virtual CombatSkills& operator=(const CombatSkills& s) {
        if (this != &s) v = s.v;
        return *this;
    }
    virtual CombatSkills& operator=(CombatSkills&& s) {
        v = s.v;
        return *this;
    }

    bool isSkill() override { return true; }

    QString description(bool showRoll = false) override { return showRoll ? v._name : v._name; }
    bool form(QWidget*, QVBoxLayout*) override          { return false; }
    QString name() override                             { return v._name; }
    Points points(bool noStore = false) override      { if (!noStore) store(); return 10_cp; }
    void restore() override                             { }
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

#define CLASS(x)\
    class x: public CombatSkills {\
    public:\
        x(): CombatSkills(#x) { }\
        x(const x& s): CombatSkills(s) { }\
        x(x&& s): CombatSkills(s) { }\
        x(const QJsonObject& json): CombatSkills(json) { }\
    };
#define CLASS_SPACE(x,y)\
    class x: public CombatSkills {\
    public:\
        x(): CombatSkills(y) { }\
        x(const x& s): CombatSkills(s) { }\
        x(x&& s): CombatSkills(s) { }\
        x(const QJsonObject& json): CombatSkills(json) { }\
    };

class AutofireSkills: public CombatSkills {
public:
    AutofireSkills(): CombatSkills("Autofire Skills")           { }
    AutofireSkills(const AutofireSkills& s): CombatSkills(s)    { }
    AutofireSkills(AutofireSkills&& s): CombatSkills(s)         { }
    AutofireSkills(const QJsonObject& json): CombatSkills(json) { v._accurateSprayfire     = json["accurate sprayfire"].toBool(false);
                                                                  v._concentratedSprayfire = json["concetrade sprayfire"].toBool(false);
                                                                  v._rapidAutofire         = json["rapid autofire"].toBool(false);
                                                                  v._skipoverSprayfire     = json["skipover sprayfire"].toBool(false);
                                                                }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + CombatSkills::description() + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { accurateSprayfire     = createCheckBox(parent, layout, "Accurate Sprayfire");
                                                                  concentratedSprayfire = createCheckBox(parent, layout, "Concetrated Sprayfire");
                                                                  rapidAutofire         = createCheckBox(parent, layout, "Rapid Autofire");
                                                                  skipoverSprayfire     = createCheckBox(parent, layout, "Skipover Sprayfire");
                                                                  return true;
                                                                }
    Points   points(bool noStore = false) override              { if (!noStore) store();
                                                                  return (v._accurateSprayfire ? 5_cp : 0_cp) +
                                                                         (v._concentratedSprayfire ? 5_cp : 0_cp) +
                                                                         (v._rapidAutofire ? 5_cp : 0_cp) +
                                                                         (v._skipoverSprayfire ? 5_cp : 0_cp); }
    void    restore() override                                  { vars s = v;
                                                                  accurateSprayfire->setChecked(s._accurateSprayfire);
                                                                  concentratedSprayfire->setChecked(s._concentratedSprayfire);
                                                                  rapidAutofire->setChecked(s._rapidAutofire);
                                                                  skipoverSprayfire->setChecked(s._skipoverSprayfire);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._accurateSprayfire     = accurateSprayfire->isChecked();
                                                                  v._concentratedSprayfire = concentratedSprayfire->isChecked();
                                                                  v._rapidAutofire         = rapidAutofire->isChecked();
                                                                  v._skipoverSprayfire     = skipoverSprayfire->isChecked(); }
    QJsonObject toJson() override                               { QJsonObject obj = CombatSkills::toJson();
                                                                  obj["accurate sprayfire"]     = v._accurateSprayfire;
                                                                  obj["concentrated sprayfire"] = v._concentratedSprayfire;
                                                                  obj["rapid autofire"]         = v._rapidAutofire;
                                                                  obj["skipover sprayfire"]     = v._skipoverSprayfire;
                                                                  return obj;
                                                                }

private:
    struct vars {
        bool _accurateSprayfire = false;
        bool _concentratedSprayfire = false;
        bool _rapidAutofire = false;
        bool _skipoverSprayfire = false;
    } v;

    QCheckBox* accurateSprayfire;
    QCheckBox* concentratedSprayfire;
    QCheckBox* rapidAutofire;
    QCheckBox* skipoverSprayfire;

    QString optOut() {
        QString res;
        QString sep = ": ";
        if (v._accurateSprayfire)     { res += sep + "Accurate Sprayfire";    sep = ", "; }
        if (v._concentratedSprayfire) { res += sep + "Concetrated Sprayfire"; sep = ", "; }
        if (v._rapidAutofire)         { res += sep + "Rapid Autofire";        sep = ", "; }
        if (v._skipoverSprayfire)     { res += sep + "Skipover Sprayfire";    sep = ", "; }
        return res;
    }
};

class CSL: public CombatSkills {
public:
    CSL(): CombatSkills("Combat Skill Levels")       { }
    CSL(const CSL& s): CombatSkills(s)               { }
    CSL(CSL&& s): CombatSkills(s)                    { }
    CSL(const QJsonObject& json): CombatSkills(json) { v._plus = json["plus"].toInt(1);
                                                       v._for  = json["for"].toString("");
                                                       v._size = json["size"].toInt(0);
                                                     }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { plus = createLineEdit(parent, layout, "How many pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  forwhat = createLineEdit(parent, layout, "Applies to what?");
                                                                  size = createComboBox(parent, layout, "Size of affected group", { "Single Attack",
                                                                                                                                    "Small Group",
                                                                                                                                    "Large Group",
                                                                                                                                    "All HTH Combat",
                                                                                                                                    "All Ranged Combat",
                                                                                                                                    "All Combat" });
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override              { if (!noStore) store();
                                                                  QList<Points> size{ 0_cp, 2_cp, 3_cp, 5_cp, 8_cp, 8_cp, 10_cp };
                                                                  return v._plus * size[v._size + 1]; }
    void    restore() override                                  { vars s = v;
                                                                  plus->setText(QString("%1").arg(s._plus));
                                                                  size->setCurrentIndex(s._size);
                                                                  forwhat->setText(s._for);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._plus = plus->text().toInt(0);
                                                                  v._for  = forwhat->text();
                                                                  v._size = size->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = CombatSkills::toJson();
                                                                  obj["plus"] = v._plus;
                                                                  obj["for"]  = v._for;
                                                                  obj["size"] = v._size;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     _plus = 0;
        QString _for = "";
        int     _size = -1;
    } v;

    QLineEdit* plus;
    QLineEdit* forwhat;
    QComboBox* size;

    QString optOut() {
        if (v._plus < 1) return "<incomplete>";
        if (v._size < 3 && v._for.isEmpty()) return "<incomplete>";
        QString res = "Combat Skill Levels: ";
        switch (v._size) {
        case 0: res += QString("+%1 with %2").arg(v._plus).arg(v._for);               break;
        case 1: res += QString("+%1 with small group (%2)").arg(v._plus).arg(v._for); break;
        case 2: res += QString("+%1 with large group (%2)").arg(v._plus).arg(v._for); break;
        case 3: res += QString("+%1 with small all HTH combat").arg(v._plus);         break;
        case 4: res += QString("+%1 with small all ranged combat").arg(v._plus);      break;
        case 5: res += QString("+%1 with all combat").arg(v._plus);                   break;
        default: return "<incomplete>";
        }

        return res;
    }

    void numeric(QString) override {
        QString txt = plus->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        plus->undo();
    }
};

class DefenseManeuver: public CombatSkills {
public:
    DefenseManeuver(): CombatSkills("Defense Maneuver")       { }
    DefenseManeuver(const DefenseManeuver& s): CombatSkills(s) { }
    DefenseManeuver(DefenseManeuver&& s): CombatSkills(s)      { }
    DefenseManeuver(const QJsonObject& json): CombatSkills(json) { v._which = json["which"].toInt(0); }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { which = createComboBox(parent, layout, "Which defense maneuver?", { "Defense Maneuver I",
                                                                                                                                      "Defense Maneuver II",
                                                                                                                                      "Defense Maneuver III",
                                                                                                                                      "Defense Maneuver IV" });
                                                                  return true;
                                                                }
    Points   points(bool noStore = false) override              { if (!noStore) store();
                                                                  QList<Points> which{ 0_cp, 3_cp, 5_cp, 8_cp, 10_cp };
                                                                  return which[v._which + 1]; }
    void    restore() override                                  { vars s = v;
                                                                  which->setCurrentIndex(s._which);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._which = which->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = CombatSkills::toJson();
                                                                  obj["which"] = v._which;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int _which = 1;
    } v;

    QComboBox* which;

    QString optOut() {
        if (v._which < 0) return "<incomplete>";
        QString res = "Defense Maneuver ";
        switch (v._which) {
        case 0: res += "I";   break;
        case 1: res += "II";  break;
        case 2: res += "III"; break;
        case 3: res += "IV";  break;
        default: return "<incomplete>";
        }

        return res;
    }
};

class MartialArts: public CombatSkills {
public:
    MartialArts(): CombatSkills("Martial Arts")              { }
    MartialArts(const MartialArts& s): CombatSkills(s)       { }
    MartialArts(MartialArts&& s): CombatSkills(s)            { }
    MartialArts(const QJsonObject& json): CombatSkills(json) { v._chokehold = json["chokehold"].toBool(false);
                                                               v._defensivestrike  = json["defensive strike"].toBool(false);
                                                               v._killingstrike    = json["killing strike"].toBool(false);
                                                               v._legsweep         = json["legsweep"].toBool(false);
                                                               v._martialblock     = json["martial block"].toBool(false);
                                                               v._martialdisarm    = json["martial disarm"].toBool(false);
                                                               v._martialdodge     = json["martial dodge"].toBool(false);
                                                               v._martialescape    = json["martial escape"].toBool(false);
                                                               v._martialgrab      = json["martial grab"].toBool(false);
                                                               v._martialstrike    = json["martial strike"].toBool(false);
                                                               v._martialthrow     = json["martial throw"].toBool(false);
                                                               v._nervestrike      = json["nerve strike"].toBool(false);
                                                               v._offensivestrike  = json["offensive strike"].toBool(false);
                                                               v._passingstrike    = json["passing strike"].toBool(false);
                                                               v._sacrifcethrow    = json["sacrifce throw"].toBool(false);
                                                               v._extradamageclass = json["extra damage clsases"].toInt(0);
                                                               v._weaponelements   = json["weapon elements"].toInt(0);
                                                               v._weapons          = json["weapons"].toString("");
                                                               }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { chokehold        = createCheckBox(parent, layout, "Choke Hold");
                                                                  defensivestrike  = createCheckBox(parent, layout, "Defensive Strike");
                                                                  killingstrike    = createCheckBox(parent, layout, "Killing Strike");
                                                                  legsweep         = createCheckBox(parent, layout, "Legsweep");
                                                                  martialblock     = createCheckBox(parent, layout, "Martial Block");
                                                                  martialdisarm    = createCheckBox(parent, layout, "Martial Disarm");
                                                                  martialdodge     = createCheckBox(parent, layout, "Martial Dodge");
                                                                  martialescape    = createCheckBox(parent, layout, "Martial Escape");
                                                                  martialgrab      = createCheckBox(parent, layout, "Martial Grab");
                                                                  martialstrike    = createCheckBox(parent, layout, "Martial Strike");
                                                                  martialthrow     = createCheckBox(parent, layout, "Martial Throw");
                                                                  nervestrike      = createCheckBox(parent, layout, "Nerve Strike");
                                                                  offensivestrike  = createCheckBox(parent, layout, "Offensive Strike");
                                                                  passingstrike    = createCheckBox(parent, layout, "Passing Strike");
                                                                  sacrifcethrow    = createCheckBox(parent, layout, "Sacrifce Throw");
                                                                  extradamageclass = createLineEdit(parent, layout, "Extra Damage Classes", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  weaponelements   = createLineEdit(parent, layout, "Weapon Elements", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  weapons          = createLineEdit(parent, layout, "Weapons?");
                                                                  return true;
                                                                }
    Points   points(bool noStore = false) override              { if (!noStore) store();
                                                                  return (v._chokehold ? 4_cp : 0_cp) +
                                                                         (v._defensivestrike ? 5_cp : 0_cp) +
                                                                         (v._killingstrike ? 4_cp : 0_cp) +
                                                                         (v._legsweep ? 3_cp : 0_cp) +
                                                                         (v._martialblock ? 4_cp : 0_cp) +
                                                                         (v._martialdisarm ? 4_cp : 0_cp) +
                                                                         (v._martialdodge ? 4_cp : 0_cp) +
                                                                         (v._martialescape ? 4_cp : 0_cp) +
                                                                         (v._martialgrab ? 3_cp : 0_cp) +
                                                                         (v._martialstrike ? 4_cp : 0_cp) +
                                                                         (v._martialthrow ? 3_cp : 0_cp) +
                                                                         (v._nervestrike ? 4_cp : 0_cp) +
                                                                         (v._offensivestrike ? 5_cp : 0_cp) +
                                                                         (v._passingstrike ? 5_cp : 0_cp) +
                                                                         (v._sacrifcethrow ? 3_cp : 0_cp) +
                                                                         v._extradamageclass * 4_cp +
                                                                         v._weaponelements;
                                                                }
    void    restore() override                                  { vars s = v;
                                                                  chokehold->setChecked(s._chokehold);
                                                                  defensivestrike->setChecked(s._defensivestrike);
                                                                  killingstrike->setChecked(s._killingstrike);
                                                                  legsweep->setChecked(s._legsweep);
                                                                  martialblock->setChecked(s._martialblock);
                                                                  martialdisarm->setChecked(s._martialdisarm);
                                                                  martialdodge->setChecked(s._martialdodge);
                                                                  martialescape->setChecked(s._martialescape);
                                                                  martialgrab->setChecked(s._martialgrab);
                                                                  martialstrike->setChecked(s._martialstrike);
                                                                  martialthrow->setChecked(s._martialthrow);
                                                                  nervestrike->setChecked(s._nervestrike);
                                                                  offensivestrike->setChecked(s._offensivestrike);
                                                                  passingstrike->setChecked(s._passingstrike);
                                                                  sacrifcethrow->setChecked(s._sacrifcethrow);
                                                                  extradamageclass->setText(QString("%1").arg(s._extradamageclass));
                                                                  weaponelements->setText(QString("%1").arg(s._weaponelements));
                                                                  weapons->setText(s._weapons);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._chokehold = chokehold->isChecked();
                                                                  v._defensivestrike = defensivestrike->isChecked();
                                                                  v._killingstrike = killingstrike->isChecked();
                                                                  v._legsweep = legsweep->isChecked();
                                                                  v._martialblock = martialblock->isChecked();
                                                                  v._martialdisarm = martialdisarm->isChecked();
                                                                  v._martialdodge = martialdodge->isChecked();
                                                                  v._martialescape = martialescape->isChecked();
                                                                  v._martialgrab = martialgrab->isChecked();
                                                                  v._martialstrike = martialstrike->isChecked();
                                                                  v._martialthrow = martialthrow->isChecked();
                                                                  v._nervestrike = nervestrike->isChecked();
                                                                  v._offensivestrike = offensivestrike->isChecked();
                                                                  v._passingstrike = passingstrike->isChecked();
                                                                  v._sacrifcethrow = sacrifcethrow->isChecked();
                                                                  v._extradamageclass = extradamageclass->text().toInt(0);
                                                                  v._weaponelements = weaponelements->text().toInt(0);
                                                                  v._weapons = weapons->text();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = CombatSkills::toJson();
                                                                  obj["choke hold"]           = v._chokehold;
                                                                  obj["defensive strike"]     = v._defensivestrike;
                                                                  obj["killing strike"]       = v._killingstrike;
                                                                  obj["legsweep"]             = v._legsweep;
                                                                  obj["martial block"]        = v._martialblock;
                                                                  obj["martial disarm"]       = v._martialdisarm;
                                                                  obj["martial dodge"]        = v._martialdodge;
                                                                  obj["martial escape"]       = v._martialescape;
                                                                  obj["martial grab"]         = v._martialgrab;
                                                                  obj["martial strike"]       = v._martialstrike;
                                                                  obj["martial throw"]        = v._martialthrow;
                                                                  obj["nerve strike"]         = v._nervestrike;
                                                                  obj["offensive strike"]     = v._offensivestrike;
                                                                  obj["passing strike"]       = v._passingstrike;
                                                                  obj["sacrifce throw"]       = v._sacrifcethrow;
                                                                  obj["extra damage classes"] = v._extradamageclass;
                                                                  obj["weapon elements"]      = v._weaponelements;
                                                                  obj["weapons"]              = v._weapons;
                                                                  return obj;
                                                                }

private:
    struct vars {
        bool _chokehold        = false;
        bool _defensivestrike  = false;
        bool _killingstrike    = false;
        bool _legsweep         = false;
        bool _martialblock     = false;
        bool _martialdisarm    = false;
        bool _martialdodge     = false;
        bool _martialescape    = false;
        bool _martialgrab      = false;
        bool _martialstrike    = false;
        bool _martialthrow     = false;
        bool _nervestrike      = false;
        bool _offensivestrike  = false;
        bool _passingstrike    = false;
        bool _sacrifcethrow    = false;
        int  _extradamageclass = 0;
        int  _weaponelements   = 0;
        QString _weapons       = "";
    } v;

    QCheckBox* chokehold;
    QCheckBox* defensivestrike;
    QCheckBox* killingstrike;
    QCheckBox* legsweep;
    QCheckBox* martialblock;
    QCheckBox* martialdisarm;
    QCheckBox* martialdodge;
    QCheckBox* martialescape;
    QCheckBox* martialgrab;
    QCheckBox* martialstrike;
    QCheckBox* martialthrow;
    QCheckBox* nervestrike;
    QCheckBox* offensivestrike;
    QCheckBox* passingstrike;
    QCheckBox* sacrifcethrow;
    QLineEdit* extradamageclass;
    QLineEdit* weaponelements;
    QLineEdit* weapons;

    QString optOut() {
        if (points(SkillTalentOrPerk::NoStore) < 10) return "<incomplete>";
        if (v._weaponelements > 0 && v._weapons.isEmpty()) return "<incomplete>";
        QString res = CombatSkills::description();
        QString sep = ": ";

        if (v._chokehold) { res += sep + "Choke Hold"; sep = ", "; }
        if (v._defensivestrike) { res += sep + "Defensive Strike"; sep = ", "; }
        if (v._killingstrike) { res += sep + "Killing Strike"; sep = ", "; }
        if (v._legsweep) { res += sep + "Legsweep"; sep = ", "; }
        if (v._martialblock) { res += sep + "Martial Block"; sep = ", "; }
        if (v._martialdisarm) { res += sep + "Martial Disarm"; sep = ", "; }
        if (v._martialdodge) { res += sep + "Martial Dodge"; sep = ", "; }
        if (v._martialescape) { res += sep + "Martial Escape"; sep = ", "; }
        if (v._martialgrab) { res += sep + "Martial Grab"; sep = ", "; }
        if (v._martialstrike) { res += sep + "Martial Strike"; sep = ", "; }
        if (v._martialthrow) { res += sep + "Martial Throw"; sep = ", "; }
        if (v._nervestrike) { res += sep + "Nerve Strike"; sep = ", "; }
        if (v._passingstrike) { res += sep + "Passing Strike"; sep = ", "; }
        if (v._sacrifcethrow) { res += sep + "Sacrifice Throw"; sep = ", "; }
        if (v._extradamageclass > 0) { res += sep + QString("%1 Extra Damage Class%2").arg(v._extradamageclass).arg((v._extradamageclass > 1) ? "es": ""); sep = ", "; }
        if (v._weaponelements > 0) { res += sep + "Weapon Elements(" + v._weapons + ")"; sep = ", "; }

        return res;
    }

    void numeric(QString) override {
        QLineEdit* fld = static_cast<QLineEdit*>(SkillTalentOrPerk::sender());
        QString txt = fld->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        fld->undo();
    }
};

class MCSL: public CombatSkills {
public:
    MCSL(): CombatSkills("Mental Combat Skill Levels") { }
    MCSL(const MCSL& s): CombatSkills(s)               { }
    MCSL(MCSL&& s): CombatSkills(s)                    { }
    MCSL(const QJsonObject& json): CombatSkills(json)  { v._plus = json["plus"].toInt(1);
                                                         v._for  = json["for"].toString("");
                                                         v._size = json["size"].toInt(0);
                                                       }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { plus = createLineEdit(parent, layout, "How many pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  forwhat = createLineEdit(parent, layout, "Applies to what?");
                                                                  size = createComboBox(parent, layout, "Size of affected group", { "Specific",
                                                                                                                                    "Small Group",
                                                                                                                                    "All MentalCombat" });
                                                                  return true;
                                                                }
    Points   points(bool noStore = false) override              { if (!noStore) store();
                                                                  QList<Points> size{ 0_cp, 1_cp, 3_cp, 6_cp };
                                                                  return v._plus * size[v._size + 1]; }
    void    restore() override                                  { vars s = v;
                                                                  plus->setText(QString("%1").arg(v._plus));
                                                                  size->setCurrentIndex(v._size);
                                                                  forwhat->setText(v._for);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._plus = plus->text().toInt(0);
                                                                  v._for  = forwhat->text();
                                                                  v._size = size->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = CombatSkills::toJson();
                                                                  obj["plus"] = v._plus;
                                                                  obj["for"]  = v._for;
                                                                  obj["size"] = v._size;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     _plus = 0;
        QString _for = "";
        int     _size = -1;
    } v;

    QLineEdit* plus;
    QLineEdit* forwhat;
    QComboBox* size;

    QString optOut() {
        if (v._plus < 1) return "<incomplete>";
        QString res = "Mental Combat Skill Levels: ";
        switch (v._size) {
        case 0: res += QString("+%1 with %2").arg(v._plus).arg(v._for);               break;
        case 1: res += QString("+%1 with small group (%2)").arg(v._plus).arg(v._for); break;
        case 2: res += QString("+%1 with all mental combat").arg(v._plus);            break;
        default: return "<incomplete>";
        }

        return res;
    }

    void numeric(QString) override {
        QString txt = plus->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        plus->undo();
    }
};

class PenaltySkillLevels: public CombatSkills {
public:
    PenaltySkillLevels(): CombatSkills("Penalty Skill Levels")          { }
    PenaltySkillLevels(const PenaltySkillLevels& s): CombatSkills(s)    { }
    PenaltySkillLevels(PenaltySkillLevels&& s): CombatSkills(s)         { }
    PenaltySkillLevels(const QJsonObject& json): CombatSkills(json) { v._plus = json["plus"].toInt(0);
                                                                      v._what = json["what"].toInt(0);
                                                                      v._with = json["with"].toString("");
                                                      }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { plus = createLineEdit(parent, layout, "How many?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  what = createComboBox(parent, layout, "Penalty versus?", { "Single attack (OCV)",
                                                                                                                             "Tight group of attacks (OCV)",
                                                                                                                             "All attacks (OCV)",
                                                                                                                             "Single condition (DCV)",
                                                                                                                             "Group of conditions (DCV)"});
                                                                  with = createLineEdit(parent, layout, "Applies to what?");
                                                                  return true;
                                                                }
    Points   points(bool noStore = false) override              { if (!noStore) store();
                                                                  QList<Points> what{ 0_cp, 1_cp, 2_cp, 3_cp, 2_cp, 3_cp };
                                                                  return v._plus * what[v._what + 1]; }
    void    restore() override                                  { vars s = v;
                                                                  plus->setText(QString("%1").arg(s._plus));
                                                                  what->setCurrentIndex(s._what);
                                                                  with->setText(s._with);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._plus = plus->text().toInt(0);
                                                                  v._what = what->currentIndex();
                                                                  v._with = with->text();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = CombatSkills::toJson();
                                                                  obj["plus"] = v._plus;
                                                                  obj["what"] = v._what;
                                                                  obj["with"] = v._with;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     _plus = 0;
        int     _what = -1;
        QString _with = "";
    } v;

    QLineEdit* plus;
    QComboBox* what;
    QLineEdit* with;

    QString optOut() {
        if (v._plus < 1 || v._with.isEmpty()) return "<incomplete>";
        QString res = "Penalty Skill Levels: ";
        res += QString("+%1 with ").arg(v._plus);
        switch (v._what) {
        case 0: res += QString("(Single Attack (OCV); %1)").arg(v._with);          break;
        case 1: res += QString("(Tight Group of Attacks (OCV); %1)").arg(v._with); break;
        case 2: res += QString("(All attacks (OCV))").arg(v._plus);                break;
        case 3: res += QString("(A single condition (DCV); %1)").arg(v._with);     break;
        case 4: res += QString("(A Group of Conditions (DCV); %1)").arg(v._with);  break;
        default: return "<incomplete>";
        }

        return res;
    }

    void numeric(QString) override {
        QString txt = plus->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        plus->undo();
    }
};

CLASS_SPACE(RapidAttack,       "Rapid Attack");
CLASS_SPACE(TwoWeaponFighting, "Two-Weapon Fighting");

class WeaponFamiliarity: public CombatSkills {
public:
    WeaponFamiliarity(): CombatSkills("Weapon Familiarity")        { }
    WeaponFamiliarity(const WeaponFamiliarity& s): CombatSkills(s) { }
    WeaponFamiliarity(WeaponFamiliarity&& s): CombatSkills(s)      { }
    WeaponFamiliarity(const QJsonObject& json): CombatSkills(json) { v._what = json["what"].toInt(0);
                                                                     v._with = json["with"].toString("");
                                                                   }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { what = createComboBox(parent, layout, "Familar with?", { "One class of weapons",
                                                                                                                           "Broad category of weapons"});
                                                                  with = createLineEdit(parent, layout, "Applies to what?");
                                                                  return true;
                                                                }
    Points   points(bool noStore = false) override              { if (!noStore) store();
                                                                  QList<Points> what{ 0_cp, 1_cp, 2_cp };
                                                                  return what[v._what + 1]; }
    void    restore() override                                  { vars s = v;
                                                                  what->setCurrentIndex(s._what);
                                                                  with->setText(s._with);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._what = what->currentIndex();
                                                                  v._with = with->text();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = CombatSkills::toJson();
                                                                  obj["what"] = v._what;
                                                                  obj["with"] = v._with;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     _what = -1;
        QString _with = "";
    } v;

    QComboBox* what;
    QLineEdit* with;

    QString optOut() {
        if (v._with.isEmpty()) return "<incomplete>";
        QString res = "WF: ";
        switch (v._what) {
        case 0: res += QString("(One Class of Weapons; %1)").arg(v._with); break;
        case 1: res += QString("(A Group of Weapons; %1)").arg(v._with);   break;
        default: return "<incomplete>";
        }

        return res;
    }
};

#undef CLASS
#undef CLASS_SPACE

#endif // COMBATSKILLS_H
