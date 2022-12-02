#ifndef COMBATSKILLS_H
#define COMBATSKILLS_H

#include "skilltalentorperk.h"

class CombatSkills: public SkillTalentOrPerk {
public:
    CombatSkills(): SkillTalentOrPerk() { }
    CombatSkills(QString name)
        : _name(name) { }
    CombatSkills(const CombatSkills& s)
        : SkillTalentOrPerk()
        , _name(s._name) { }
    CombatSkills(CombatSkills&& s)
        : SkillTalentOrPerk()
        , _name(s._name) { }
    CombatSkills(const QJsonObject& json)
        : SkillTalentOrPerk()
        , _name(json["name"].toString("")) { }

    virtual CombatSkills& operator=(const CombatSkills& s) {
        if (this != &s) {
            _name = s._name;
        }
        return *this;
    }
    virtual CombatSkills& operator=(CombatSkills&& s) {
        _name = s._name;
        return *this;
    }

    QString description(bool showRoll = false) override { return showRoll ? _name : _name; }
    void form(QWidget*, QVBoxLayout*) override          { throw "No options, immediately accept"; }
    int points(bool noStore = false) override           { if (!noStore) store(); return 10; }
    void restore() override                             { }
    QString roll() override                             { return ""; }
    void    store() override                            { }

    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"] = _name;
        return obj;
    }

private:
    QString _name;
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
    AutofireSkills(const QJsonObject& json): CombatSkills(json) { _accurateSprayfire     = json["accurate sprayfire"].toBool(false);
                                                                  _concentratedSprayfire = json["concetrade sprayfire"].toBool(false);
                                                                  _rapidAutofire         = json["rapid autofire"].toBool(false);
                                                                  _skipoverSprayfire     = json["skipover sprayfire"].toBool(false);
                                                                }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + CombatSkills::description() + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { accurateSprayfire = createCheckBox(parent, layout, "Accurate Sprayfire");
                                                                  concentratedSprayfire = createCheckBox(parent, layout, "Concetrated Sprayfire");
                                                                  rapidAutofire = createCheckBox(parent, layout, "Rapid Autofire");
                                                                  skipoverSprayfire = createCheckBox(parent, layout, "Skipover Sprayfire");
                                                                }
    int     points(bool noStore = false) override               { if (!noStore) store();
                                                                  return (_accurateSprayfire ? 5 : 0) +
                                                                         (_concentratedSprayfire ? 5 : 0) +
                                                                         (_rapidAutofire ? 5 : 0) +
                                                                         (_skipoverSprayfire ? 5 : 0); }
    void    restore() override                                  { accurateSprayfire->setChecked(_accurateSprayfire);
                                                                  concentratedSprayfire->setChecked(_concentratedSprayfire);
                                                                  rapidAutofire->setChecked(_rapidAutofire);
                                                                  skipoverSprayfire->setChecked(_skipoverSprayfire);
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { _accurateSprayfire = accurateSprayfire->isChecked();
                                                                  _concentratedSprayfire = concentratedSprayfire->isChecked();
                                                                  _rapidAutofire = rapidAutofire->isChecked();
                                                                  _skipoverSprayfire = skipoverSprayfire->isChecked(); }
    QJsonObject toJson() override                               { QJsonObject obj = CombatSkills::toJson();
                                                                  obj["accurate sprayfire"]     = _accurateSprayfire;
                                                                  obj["concentrated sprayfire"] = _concentratedSprayfire;
                                                                  obj["rapid autofire"]         = _rapidAutofire;
                                                                  obj["skipover sprayfire"]     = _skipoverSprayfire;
                                                                  return obj;
                                                                }

private:
    bool _accurateSprayfire = false;
    bool _concentratedSprayfire = false;
    bool _rapidAutofire = false;
    bool _skipoverSprayfire = false;

    QCheckBox* accurateSprayfire;
    QCheckBox* concentratedSprayfire;
    QCheckBox* rapidAutofire;
    QCheckBox* skipoverSprayfire;

    QString optOut() {
        QString res;
        QString sep = ": ";
        if (_accurateSprayfire)     { res += sep + "Accurate Sprayfire";    sep = ", "; }
        if (_concentratedSprayfire) { res += sep + "Concetrated Sprayfire"; sep = ", "; }
        if (_rapidAutofire)         { res += sep + "Rapid Autofire";        sep = ", "; }
        if (_skipoverSprayfire)     { res += sep + "Skipover Sprayfire";    sep = ", "; }
        return res;
    }
};

class CSL: public CombatSkills {
public:
    CSL(): CombatSkills("Combat Skill Levels")           { }
    CSL(const CSL& s): CombatSkills(s)    { }
    CSL(CSL&& s): CombatSkills(s)         { }
    CSL(const QJsonObject& json): CombatSkills(json) { _plus = json["plus"].toInt(1);
                                                       _for  = json["for"].toString("");
                                                       _size = json["size"].toInt(0);
                                                     }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { plus = createLineEdit(parent, layout, "How many pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  forwhat = createLineEdit(parent, layout, "Applies to what?");
                                                                  size = createComboBox(parent, layout, "Size of affected group", { "Specific",
                                                                                                                                    "Small Group",
                                                                                                                                    "Large Group",
                                                                                                                                    "All HTH Combat",
                                                                                                                                    "All Ranged Combat",
                                                                                                                                    "All Combat" });
                                                                }
    int     points(bool noStore = false) override               { if (!noStore) store();
                                                                  return _plus * QList<int>{ 0, 2, 3, 5, 8, 8, 10 }[_size + 1]; }
    void    restore() override                                  { plus->setText(QString("%1").arg(_plus));
                                                                  size->setCurrentIndex(_size);
                                                                  forwhat->setText(_for);
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { _plus = plus->text().toInt(0);
                                                                  _for  = forwhat->text();
                                                                  _size = size->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = CombatSkills::toJson();
                                                                  obj["plus"] = _plus;
                                                                  obj["for"]  = _for;
                                                                  obj["size"] = _size;
                                                                  return obj;
                                                                }

private:
    int     _plus = 0;
    QString _for = "";
    int     _size = -1;

    QLineEdit* plus;
    QLineEdit* forwhat;
    QComboBox* size;

    QString optOut() {
        if (_plus < 0) return "<incomplete>";
        QString res = "Combat Skill Levels: ";
        switch (_size) {
        case 0: res += QString("+%1 with %2").arg(_plus).arg(_for);               break;
        case 1: res += QString("+%1 with small group (%2)").arg(_plus).arg(_for); break;
        case 2: res += QString("+%1 with large group (%2)").arg(_plus).arg(_for); break;
        case 3: res += QString("+%1 with small all HTH combat").arg(_plus);       break;
        case 4: res += QString("+%1 with small all ranged combat").arg(_plus);    break;
        case 5: res += QString("+%1 with all combat").arg(_plus);                 break;
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
    DefenseManeuver(const QJsonObject& json): CombatSkills(json) { _which = json["which"].toInt(0); }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { which = createComboBox(parent, layout, "Which defense maneuver?", { "Defense Maneuver I",
                                                                                                                                      "Defense Maneuver II",
                                                                                                                                      "Defense Maneuver III",
                                                                                                                                      "Defense Maneuver IV" });
                                                                }
    int     points(bool noStore = false) override               { if (!noStore) store();
                                                                  return _which * QList<int>{ 0, 3, 5, 8, 10 }[_which + 1]; }
    void    restore() override                                  { which->setCurrentIndex(_which);
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { _which = which->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = CombatSkills::toJson();
                                                                  obj["which"] = _which;
                                                                  return obj;
                                                                }

private:
    int     _which = 1;

    QComboBox* which;

    QString optOut() {
        if (_which < 0) return "<incomplete>";
        QString res = "Defense Maneuver ";
        switch (_which) {
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
    MartialArts(const QJsonObject& json): CombatSkills(json) { _chokehold = json["chokehold"].toBool(false);
                                                               _defensivestrike = json["defensive strike"].toBool(false);
                                                               _killingstrike = json["killing strike"].toBool(false);
                                                               _legsweep = json["legsweep"].toBool(false);
                                                               _martialblock = json["martial block"].toBool(false);
                                                               _martialdisarm = json["martial disarm"].toBool(false);
                                                               _martialdodge = json["martial dodge"].toBool(false);
                                                               _martialescape = json["martial escape"].toBool(false);
                                                               _martialgrab = json["martial grab"].toBool(false);
                                                               _martialstrike = json["martial strike"].toBool(false);
                                                               _martialthrow = json["martial throw"].toBool(false);
                                                               _nervestrike = json["nerve strike"].toBool(false);
                                                               _offensivestrike = json["offensive strike"].toBool(false);
                                                               _passingstrike = json["passing strike"].toBool(false);
                                                               _sacrifcethrow = json["sacrifce throw"].toBool(false);
                                                               _extradamageclass = json["extra damage clsases"].toInt(0);
                                                               _weaponelements = json["weapon elements"].toInt(0);
                                                               _weapons = json["weapons"].toString("");
                                                               }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { chokehold = createCheckBox(parent, layout, "Choke Hold");
                                                                  defensivestrike = createCheckBox(parent, layout, "Defensive Strike");
                                                                  killingstrike = createCheckBox(parent, layout, "Killing Strike");
                                                                  legsweep = createCheckBox(parent, layout, "Legsweep");
                                                                  martialblock = createCheckBox(parent, layout, "Martial Block");
                                                                  martialdisarm = createCheckBox(parent, layout, "Martial Disarm");
                                                                  martialdodge = createCheckBox(parent, layout, "Martial Dodge");
                                                                  martialescape = createCheckBox(parent, layout, "Martial Escape");
                                                                  martialgrab = createCheckBox(parent, layout, "Martial Grab");
                                                                  martialstrike = createCheckBox(parent, layout, "Martial Strike");
                                                                  martialthrow = createCheckBox(parent, layout, "Martial Throw");
                                                                  nervestrike = createCheckBox(parent, layout, "Nerve Strike");
                                                                  offensivestrike = createCheckBox(parent, layout, "Offensive Strike");
                                                                  passingstrike = createCheckBox(parent, layout, "Passing Strike");
                                                                  sacrifcethrow = createCheckBox(parent, layout, "Sacrifce Throw");
                                                                  extradamageclass = createLineEdit(parent, layout, "Extra Damage Classes", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  weaponelements = createLineEdit(parent, layout, "Weapon Elements", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  weapons  = createLineEdit(parent, layout, "Weapons?");
                                                                }
    int     points(bool noStore = false) override               { if (!noStore) store();
                                                                  return (_chokehold ? 4 : 0) +
                                                                         (_defensivestrike ? 5 : 0) +
                                                                         (_killingstrike ? 4 : 0) +
                                                                         (_legsweep ? 3 : 0) +
                                                                         (_martialblock ? 4 : 0) +
                                                                         (_martialdisarm ? 4 : 0) +
                                                                         (_martialdodge ? 4 : 0) +
                                                                         (_martialescape ? 4 : 0) +
                                                                         (_martialgrab ? 3 : 0) +
                                                                         (_martialstrike ? 4 : 0) +
                                                                         (_martialthrow ? 3 : 0) +
                                                                         (_nervestrike ? 4 : 0) +
                                                                         (_offensivestrike ? 5 : 0) +
                                                                         (_passingstrike ? 5 : 0) +
                                                                         (_sacrifcethrow ? 3 : 0) +
                                                                         _extradamageclass * 4 +
                                                                         _weaponelements;
                                                                }
    void    restore() override                                  { chokehold->setChecked(_chokehold);
                                                                  defensivestrike->setChecked(_defensivestrike);
                                                                  killingstrike->setChecked(_killingstrike);
                                                                  legsweep->setChecked(_legsweep);
                                                                  martialblock->setChecked(_martialblock);
                                                                  martialdisarm->setChecked(_martialdisarm);
                                                                  martialdodge->setChecked(_martialdodge);
                                                                  martialescape->setChecked(_martialescape);
                                                                  martialgrab->setChecked(_martialgrab);
                                                                  martialstrike->setChecked(_martialstrike);
                                                                  martialthrow->setChecked(_martialthrow);
                                                                  nervestrike->setChecked(_nervestrike);
                                                                  offensivestrike->setChecked(_offensivestrike);
                                                                  passingstrike->setChecked(_passingstrike);
                                                                  sacrifcethrow->setChecked(_sacrifcethrow);
                                                                  extradamageclass->setText(QString("%1").arg(_extradamageclass));
                                                                  weaponelements->setText(QString("%1").arg(_weaponelements));
                                                                  weapons->setText(_weapons);
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { _chokehold = chokehold->isChecked();
                                                                  _defensivestrike = defensivestrike->isChecked();
                                                                  _killingstrike = killingstrike->isChecked();
                                                                  _legsweep = legsweep->isChecked();
                                                                  _martialblock = martialblock->isChecked();
                                                                  _martialdisarm = martialdisarm->isChecked();
                                                                  _martialdodge = martialdodge->isChecked();
                                                                  _martialescape = martialescape->isChecked();
                                                                  _martialgrab = martialgrab->isChecked();
                                                                  _martialstrike = martialstrike->isChecked();
                                                                  _martialthrow = martialthrow->isChecked();
                                                                  _nervestrike = nervestrike->isChecked();
                                                                  _offensivestrike = offensivestrike->isChecked();
                                                                  _passingstrike = passingstrike->isChecked();
                                                                  _sacrifcethrow = sacrifcethrow->isChecked();
                                                                  _extradamageclass = extradamageclass->text().toInt(0);
                                                                  _weaponelements = weaponelements->text().toInt(0);
                                                                  _weapons = weapons->text();
 }
    QJsonObject toJson() override                               { QJsonObject obj = CombatSkills::toJson();
                                                                  obj["choke hold"] = _chokehold;
                                                                  obj["defensive strike"] = _defensivestrike;
                                                                  obj["killing strike"] = _killingstrike;
                                                                  obj["legsweep"] = _legsweep;
                                                                  obj["martial block"] = _martialblock;
                                                                  obj["martial disarm"] = _martialdisarm;
                                                                  obj["martial dodge"] = _martialdodge;
                                                                  obj["martial escape"] = _martialescape;
                                                                  obj["martial grab"] = _martialgrab;
                                                                  obj["martial strike"] = _martialstrike;
                                                                  obj["martial throw"] = _martialthrow;
                                                                  obj["nerve strike"] = _nervestrike;
                                                                  obj["offensive strike"] = _offensivestrike;
                                                                  obj["passing strike"] = _passingstrike;
                                                                  obj["sacrifce throw"] = _sacrifcethrow;
                                                                  obj["extra damage classes"] = _extradamageclass;
                                                                  obj["weapon elements"] = _weaponelements;
                                                                  obj["weapons"] = _weapons;
                                                                  return obj;
                                                                }

private:
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
        if (_weaponelements > 0 && _weapons.isEmpty()) return "<incomplete>";
        QString res = CombatSkills::description();
        QString sep = ": ";

        if (_chokehold) { res += sep + "Choke Hold"; sep = ", "; }
        if (_defensivestrike) { res += sep + "Defensive Strike"; sep = ", "; }
        if (_killingstrike) { res += sep + "Killing Strike"; sep = ", "; }
        if (_legsweep) { res += sep + "Legsweep"; sep = ", "; }
        if (_martialblock) { res += sep + "Martial Block"; sep = ", "; }
        if (_martialdisarm) { res += sep + "Martial Disarm"; sep = ", "; }
        if (_martialdodge) { res += sep + "Martial Dodge"; sep = ", "; }
        if (_martialescape) { res += sep + "Martial Escape"; sep = ", "; }
        if (_martialgrab) { res += sep + "Martial Grab"; sep = ", "; }
        if (_martialstrike) { res += sep + "Martial Strike"; sep = ", "; }
        if (_martialthrow) { res += sep + "Martial Throw"; sep = ", "; }
        if (_nervestrike) { res += sep + "Nerve Strike"; sep = ", "; }
        if (_passingstrike) { res += sep + "Passing Strike"; sep = ", "; }
        if (_sacrifcethrow) { res += sep + "Sacrifice Throw"; sep = ", "; }
        if (_extradamageclass > 0) { res += sep + QString("%1 Extra Damage Class%2").arg(_extradamageclass).arg((_extradamageclass > 1) ? "es": ""); sep = ", "; }
        if (_weaponelements > 0) { res += sep + "Weapon Elements(" + _weapons + ")"; sep = ", "; }

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
    MCSL(): CombatSkills("Mental Combat Skill Levels")           { }
    MCSL(const MCSL& s): CombatSkills(s)    { }
    MCSL(MCSL&& s): CombatSkills(s)         { }
    MCSL(const QJsonObject& json): CombatSkills(json) { _plus = json["plus"].toInt(1);
                                                        _for  = json["for"].toString("");
                                                        _size = json["size"].toInt(0);
                                                      }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { plus = createLineEdit(parent, layout, "How many pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  forwhat = createLineEdit(parent, layout, "Applies to what?");
                                                                  size = createComboBox(parent, layout, "Size of affected group", { "Specific",
                                                                                                                                    "Small Group",
                                                                                                                                    "All MentalCombat" });
                                                                }
    int     points(bool noStore = false) override               { if (!noStore) store();
                                                                  return _plus * QList<int>{ 0, 1, 3, 6 }[_size + 1]; }
    void    restore() override                                  { plus->setText(QString("%1").arg(_plus));
                                                                  size->setCurrentIndex(_size);
                                                                  forwhat->setText(_for);
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { _plus = plus->text().toInt(0);
                                                                  _for  = forwhat->text();
                                                                  _size = size->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = CombatSkills::toJson();
                                                                  obj["plus"] = _plus;
                                                                  obj["for"]  = _for;
                                                                  obj["size"] = _size;
                                                                  return obj;
                                                                }

private:
    int     _plus = 0;
    QString _for = "";
    int     _size = -1;

    QLineEdit* plus;
    QLineEdit* forwhat;
    QComboBox* size;

    QString optOut() {
        if (_plus < 0) return "<incomplete>";
        QString res = "Mental Combat Skill Levels: ";
        switch (_size) {
        case 0: res += QString("+%1 with %2").arg(_plus).arg(_for);               break;
        case 1: res += QString("+%1 with small group (%2)").arg(_plus).arg(_for); break;
        case 2: res += QString("+%1 with all mental combat").arg(_plus);                 break;
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
    PenaltySkillLevels(const QJsonObject& json): CombatSkills(json) { _plus = json["plus"].toInt(0);
                                                                      _what = json["what"].toInt(0);
                                                                      _with = json["with"].toString("");
                                                      }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { plus = createLineEdit(parent, layout, "How many?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  what = createComboBox(parent, layout, "Penalty versus?", { "Single attack (OCV)",
                                                                                                                             "Tight group of attacks (OCV)",
                                                                                                                             "All attacks (OCV)",
                                                                                                                             "Single condition (DCV)",
                                                                                                                             "Group of conditions (DCV)"});
                                                                  with = createLineEdit(parent, layout, "Applies to what?");
                                                                }
    int     points(bool noStore = false) override               { if (!noStore) store();
                                                                  return _plus * QList<int>{ 0, 1, 2, 3, 2, 3 }[_what + 1]; }
    void    restore() override                                  { plus->setText(QString("%1").arg(_plus));
                                                                  what->setCurrentIndex(_what);
                                                                  with->setText(_with);
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { _plus = plus->text().toInt(0);
                                                                  _what = what->currentIndex();
                                                                  _with = with->text();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = CombatSkills::toJson();
                                                                  obj["plus"] = _plus;
                                                                  obj["what"] = _what;
                                                                  obj["with"] = _with;
                                                                  return obj;
                                                                }

private:
    int     _plus = 0;
    int     _what = -1;
    QString _with = "";

    QLineEdit* plus;
    QComboBox* what;
    QLineEdit* with;

    QString optOut() {
        if (_plus < 0 || _with.isEmpty()) return "<incomplete>";
        QString res = "Penalty Skill Levels: ";
        res += QString("+%1 with ").arg(_plus);
        switch (_what) {
        case 0: res += QString("(Single Attack (OCV); %1)").arg(_with);          break;
        case 1: res += QString("(Tight Group of Attacks (OCV); %1)").arg(_with); break;
        case 2: res += QString("(All attacks (OCV))").arg(_plus);                break;
        case 3: res += QString("(A single condition (DCV); %1)").arg(_with);     break;
        case 4: res += QString("(A Group of Conditions (DCV); %1)").arg(_with);  break;
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
    WeaponFamiliarity(const QJsonObject& json): CombatSkills(json) { _what = json["what"].toInt(0);
                                                                     _with = json["with"].toString("");
                                                                   }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { what = createComboBox(parent, layout, "Familar with?", { "One class of weapons",
                                                                                                                           "Broad category of weapons"});
                                                                  with = createLineEdit(parent, layout, "Applies to what?");
                                                                }
    int     points(bool noStore = false) override               { if (!noStore) store();
                                                                  return QList<int>{ 0, 1, 2 }[_what + 1]; }
    void    restore() override                                  { what->setCurrentIndex(_what);
                                                                  with->setText(_with);
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { _what = what->currentIndex();
                                                                  _with = with->text();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = CombatSkills::toJson();
                                                                  obj["what"] = _what;
                                                                  obj["with"] = _with;
                                                                  return obj;
                                                                }

private:
    int     _what = -1;
    QString _with = "";

    QComboBox* what;
    QLineEdit* with;

    QString optOut() {
        if (_with.isEmpty()) return "<incomplete>";
        QString res = "WF: ";
        switch (_what) {
        case 0: res += QString("(One Class of Weapons; %1)").arg(_with); break;
        case 1: res += QString("(A Group of Weapons; %1)").arg(_with);   break;
        default: return "<incomplete>";
        }

        return res;
    }
};

#undef CLASS
#undef CLASS_SPACE

#endif // COMBATSKILLS_H
