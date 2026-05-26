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
    ~CombatSkills() override { }

    CombatSkills& operator=(const CombatSkills& s) {
        if (this != &s) v = s.v;
        return *this;
    }
    CombatSkills& operator=(CombatSkills&& s) {
        v = s.v;
        return *this;
    }

    bool isSkill() override { return true; }

    QString description(bool showRoll = false) override { return showRoll ? v.mName : v.mName; }
    bool form(QWidget*, QVBoxLayout*) override          { return false; }
    QString name() override                             { return v.mName; }
    Points points(bool noStore = false) override        { if (!noStore) store(); return 10_cp; } // NOLINT
    void restore() override                             { }
    QString roll() override                             { return ""; }
    void    store() override                            { }

    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"] = v.mName;
        return obj;
    }

private:
    struct vars {
        QString mName;
    } v;
};

// NOLINTNEXTLINE
#define CLASS(x)                                        \
    class x: public CombatSkills {                      \
    public:                                             \
        x()                                             \
            : CombatSkills(#x) { }                      \
        x(const x& s)                                   \
            : CombatSkills(s) { }                       \
        x(x&& s)                                        \
            : CombatSkills(s) { }                       \
        x(const QJsonObject& json)                      \
            : CombatSkills(json) { }                    \
        ~x() override { }                               \
        x& operator=(const x& s) {                      \
            if (this != &s) CombatSkills::operator=(s); \
            return *this;                               \
        }                                               \
        x& operator=(x&& s) {                           \
            CombatSkills::operator=(s);                 \
            return *this;                               \
        }                                               \
    };
// NOLINTNEXTLINE
#define CLASS_SPACE(x, y)                               \
    class x: public CombatSkills {                      \
    public:                                             \
        x()                                             \
            : CombatSkills(y) { }                       \
        x(const x& s)                                   \
            : CombatSkills(s) { }                       \
        x(x&& s)                                        \
            : CombatSkills(s) { }                       \
        x(const QJsonObject& json)                      \
            : CombatSkills(json) { }                    \
        ~x() override { }                               \
        x& operator=(const x& s) {                      \
            if (this != &s) CombatSkills::operator=(s); \
            return *this;                               \
        }                                               \
        x& operator=(x&& s) {                           \
            CombatSkills::operator=(s);                 \
            return *this;                               \
        }                                               \
    };

class AutofireSkills: public CombatSkills {
public:
    AutofireSkills(): CombatSkills("Autofire Skills")           { }
    AutofireSkills(const AutofireSkills& s): CombatSkills(s)    { }
    AutofireSkills(AutofireSkills&& s): CombatSkills(s)         { }
    AutofireSkills(const QJsonObject& json): CombatSkills(json) { v.mAccurateSprayfire     = json["accurate sprayfire"].toBool(false);
                                                                  v.mConcentratedSprayfire = json["concetrade sprayfire"].toBool(false);
                                                                  v.mRapidAutofire         = json["rapid autofire"].toBool(false);
                                                                  v.mSkipoverSprayfire = json["skipover sprayfire"].toBool(false);
    }
    ~AutofireSkills() override { }
    AutofireSkills& operator=(const AutofireSkills& s) {
        if (this != &s) {
            CombatSkills::operator=(s);
            v = s.v;
        }
        return *this;
    }
    AutofireSkills& operator=(AutofireSkills&& s) {
        CombatSkills::operator=(s);
        v = s.v;
        return *this;
    }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + CombatSkills::description() + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { accurateSprayfire     = createCheckBox(parent, layout, "Accurate Sprayfire");
                                                                  concentratedSprayfire = createCheckBox(parent, layout, "Concetrated Sprayfire");
                                                                  rapidAutofire         = createCheckBox(parent, layout, "Rapid Autofire");
                                                                  skipoverSprayfire     = createCheckBox(parent, layout, "Skipover Sprayfire");
                                                                  return true;
                                                                }
    Points   points(bool noStore = false) override              { if (!noStore) store();
                                                                  return (v.mAccurateSprayfire ? 5_cp : 0_cp) + // NOLINT
                                                                         (v.mConcentratedSprayfire ? 5_cp : 0_cp) + // NOLINT
                                                                         (v.mRapidAutofire ? 5_cp : 0_cp) + // NOLINT
                                                                         (v.mSkipoverSprayfire ? 5_cp : 0_cp); } // NOLINT
    void    restore() override                                  { vars s = v;
                                                                  accurateSprayfire->setChecked(s.mAccurateSprayfire);
                                                                  concentratedSprayfire->setChecked(s.mConcentratedSprayfire);
                                                                  rapidAutofire->setChecked(s.mRapidAutofire);
                                                                  skipoverSprayfire->setChecked(s.mSkipoverSprayfire);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mAccurateSprayfire     = accurateSprayfire->isChecked();
                                                                  v.mConcentratedSprayfire = concentratedSprayfire->isChecked();
                                                                  v.mRapidAutofire         = rapidAutofire->isChecked();
                                                                  v.mSkipoverSprayfire     = skipoverSprayfire->isChecked(); }
    QJsonObject toJson() override                               { QJsonObject obj = CombatSkills::toJson();
                                                                  obj["accurate sprayfire"]     = v.mAccurateSprayfire;
                                                                  obj["concentrated sprayfire"] = v.mConcentratedSprayfire;
                                                                  obj["rapid autofire"]         = v.mRapidAutofire;
                                                                  obj["skipover sprayfire"]     = v.mSkipoverSprayfire;
                                                                  return obj;
                                                                }

private:
    struct vars {
        bool mAccurateSprayfire = false;
        bool mConcentratedSprayfire = false;
        bool mRapidAutofire = false;
        bool mSkipoverSprayfire = false;
    } v;

    QCheckBox* accurateSprayfire = nullptr;
    QCheckBox* concentratedSprayfire = nullptr;
    QCheckBox* rapidAutofire = nullptr;
    QCheckBox* skipoverSprayfire = nullptr;

    QString optOut() {
        QString res;
        QString sep = ": ";
        if (v.mAccurateSprayfire)     { res += sep + "Accurate Sprayfire";    sep = ", "; }
        if (v.mConcentratedSprayfire) { res += sep + "Concetrated Sprayfire"; sep = ", "; }
        if (v.mRapidAutofire)         { res += sep + "Rapid Autofire";        sep = ", "; }
        if (v.mSkipoverSprayfire)     { res += sep + "Skipover Sprayfire";    sep = ", "; }
        return res;
    }
};

class CSL: public CombatSkills {
public:
    CSL(): CombatSkills("Combat Skill Levels")       { }
    CSL(const CSL& s): CombatSkills(s)               { }
    CSL(CSL&& s): CombatSkills(s)                    { }
    CSL(const QJsonObject& json): CombatSkills(json) { v.mPlus = json["plus"].toInt(1);
                                                       v.mFor  = json["for"].toString("");
                                                       v.mSize = json["size"].toInt(0);
                                                     }
    ~CSL() override { }
    CSL& operator=(const CSL& s) {
        if (this != &s) {
            CombatSkills::operator=(s);
            v = s.v;
        }
        return *this;
    }
    CSL& operator=(CSL&& s) {
        CombatSkills::operator=(s);
        v = s.v;
        return *this;
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
    Points points(bool noStore = false) override                { if (!noStore) store();
                                                                  QList<Points> size{ 0_cp, 2_cp, 3_cp, 5_cp, 8_cp, 8_cp, 10_cp }; // NOLINT
                                                                  return v.mPlus * size[v.mSize + 1]; }
    void    restore() override                                  { vars s = v;
                                                                  plus->setText(QString("%1").arg(s.mPlus));
                                                                  size->setCurrentIndex(s.mSize);
                                                                  forwhat->setText(s.mFor);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mPlus = plus->text().toInt(0);
                                                                  v.mFor  = forwhat->text();
                                                                  v.mSize = size->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = CombatSkills::toJson();
                                                                  obj["plus"] = v.mPlus;
                                                                  obj["for"]  = v.mFor;
                                                                  obj["size"] = v.mSize;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     mPlus = 0;
        QString mFor = "";
        int     mSize = -1;
    } v;

    QLineEdit* plus = nullptr;
    QLineEdit* forwhat = nullptr;
    QComboBox* size = nullptr;

    QString optOut() {
        if (v.mPlus < 1) return "<incomplete>";
        if (v.mSize < 3 && v.mFor.isEmpty()) return "<incomplete>";
        QString res = "Combat Skill Levels: ";
        switch (v.mSize) {
        case 0: res += QString("+%1 with %2").arg(v.mPlus).arg(v.mFor);               break;
        case 1: res += QString("+%1 with small group (%2)").arg(v.mPlus).arg(v.mFor); break;
        case 2: res += QString("+%1 with large group (%2)").arg(v.mPlus).arg(v.mFor); break;
        case 3: res += QString("+%1 with small all HTH combat").arg(v.mPlus);         break;
        case 4: res += QString("+%1 with small all ranged combat").arg(v.mPlus);      break;
        case 5: res += QString("+%1 with all combat").arg(v.mPlus);                   break; // NOLINT
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
    DefenseManeuver(const QJsonObject& json): CombatSkills(json) { v.mWhich = json["which"].toInt(0); }
    ~DefenseManeuver() override { }
    DefenseManeuver& operator=(const DefenseManeuver& s) {
        if (this != &s) {
            CombatSkills::operator=(s);
            v = s.v;
        }
        return *this;
    }
    DefenseManeuver& operator=(DefenseManeuver&& s) {
        CombatSkills::operator=(s);
        v = s.v;
        return *this;
    }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { which = createComboBox(parent, layout, "Which defense maneuver?", { "Defense Maneuver I",
                                                                                                                                      "Defense Maneuver II",
                                                                                                                                      "Defense Maneuver III",
                                                                                                                                      "Defense Maneuver IV" });
                                                                  return true;
                                                                }
    Points   points(bool noStore = false) override              { if (!noStore) store();
                                                                  QList<Points> which{ 0_cp, 3_cp, 5_cp, 8_cp, 10_cp }; // NOLINT
                                                                  return which[v.mWhich + 1]; }
    void    restore() override                                  { vars s = v;
                                                                  which->setCurrentIndex(s.mWhich);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mWhich = which->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = CombatSkills::toJson();
                                                                  obj["which"] = v.mWhich;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int mWhich = 1;
    } v;

    QComboBox* which = nullptr;

    QString optOut() {
        if (v.mWhich < 0) return "<incomplete>";
        QString res = "Defense Maneuver ";
        switch (v.mWhich) {
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
    MartialArts(const QJsonObject& json): CombatSkills(json) { v.mChokeHold = json["chokehold"].toBool(false);
                                                               v.mDefensiveStrike  = json["defensive strike"].toBool(false);
                                                               v.mKillingStrike    = json["killing strike"].toBool(false);
                                                               v.mLegSweep         = json["legsweep"].toBool(false);
                                                               v.mMartialBlock     = json["martial block"].toBool(false);
                                                               v.mMartialDisarm    = json["martial disarm"].toBool(false);
                                                               v.mMartialDodge     = json["martial dodge"].toBool(false);
                                                               v.mMartialEscape    = json["martial escape"].toBool(false);
                                                               v.mMartialGrab      = json["martial grab"].toBool(false);
                                                               v.mMartialStrike    = json["martial strike"].toBool(false);
                                                               v.mMartialThrow     = json["martial throw"].toBool(false);
                                                               v.mNerveStrike      = json["nerve strike"].toBool(false);
                                                               v.mOffensiveStrike  = json["offensive strike"].toBool(false);
                                                               v.mPassingStrike    = json["passing strike"].toBool(false);
                                                               v.mSacrifceThrow    = json["sacrifce throw"].toBool(false);
                                                               v.mExtraDamageClass = json["extra damage classes"].toInt(0);
                                                               v.mWeaponElements   = json["weapon elements"].toInt(0);
                                                               v.mWeapons          = json["weapons"].toString("");
                                                               }
    ~MartialArts() override { }
    MartialArts& operator=(const MartialArts& s) {
        if (this != &s) {
            CombatSkills::operator=(s);
            v = s.v;
        }
        return *this;
    }
    MartialArts& operator=(MartialArts&& s) {
        CombatSkills::operator=(s);
        v = s.v;
        return *this;
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
                                                                  return (v.mChokeHold ? 4_cp : 0_cp) +
                                                                         (v.mDefensiveStrike ? 5_cp : 0_cp) + // NOLINT
                                                                         (v.mKillingStrike ? 4_cp : 0_cp) +
                                                                         (v.mLegSweep ? 3_cp : 0_cp) +
                                                                         (v.mMartialBlock ? 4_cp : 0_cp) +
                                                                         (v.mMartialDisarm ? 4_cp : 0_cp) +
                                                                         (v.mMartialDodge ? 4_cp : 0_cp) +
                                                                         (v.mMartialEscape ? 4_cp : 0_cp) +
                                                                         (v.mMartialGrab ? 3_cp : 0_cp) +
                                                                         (v.mMartialStrike ? 4_cp : 0_cp) +
                                                                         (v.mMartialThrow ? 3_cp : 0_cp) +
                                                                         (v.mNerveStrike ? 4_cp : 0_cp) +
                                                                         (v.mOffensiveStrike ? 5_cp : 0_cp) + // NOLINT
                                                                         (v.mPassingStrike ? 5_cp : 0_cp) + // NOLINT
                                                                         (v.mSacrifceThrow ? 3_cp : 0_cp) +
                                                                         v.mExtraDamageClass * 4_cp +
                                                                         v.mWeaponElements;
                                                                }
    void    restore() override                                  { vars s = v;
                                                                  chokehold->setChecked(s.mChokeHold);
                                                                  defensivestrike->setChecked(s.mDefensiveStrike);
                                                                  killingstrike->setChecked(s.mKillingStrike);
                                                                  legsweep->setChecked(s.mLegSweep);
                                                                  martialblock->setChecked(s.mMartialBlock);
                                                                  martialdisarm->setChecked(s.mMartialDisarm);
                                                                  martialdodge->setChecked(s.mMartialDodge);
                                                                  martialescape->setChecked(s.mMartialEscape);
                                                                  martialgrab->setChecked(s.mMartialGrab);
                                                                  martialstrike->setChecked(s.mMartialStrike);
                                                                  martialthrow->setChecked(s.mMartialThrow);
                                                                  nervestrike->setChecked(s.mNerveStrike);
                                                                  offensivestrike->setChecked(s.mOffensiveStrike);
                                                                  passingstrike->setChecked(s.mPassingStrike);
                                                                  sacrifcethrow->setChecked(s.mSacrifceThrow);
                                                                  extradamageclass->setText(QString("%1").arg(s.mExtraDamageClass));
                                                                  weaponelements->setText(QString("%1").arg(s.mWeaponElements));
                                                                  weapons->setText(s.mWeapons);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mChokeHold = chokehold->isChecked();
                                                                  v.mDefensiveStrike = defensivestrike->isChecked();
                                                                  v.mKillingStrike = killingstrike->isChecked();
                                                                  v.mLegSweep = legsweep->isChecked();
                                                                  v.mMartialBlock = martialblock->isChecked();
                                                                  v.mMartialDisarm = martialdisarm->isChecked();
                                                                  v.mMartialDodge = martialdodge->isChecked();
                                                                  v.mMartialEscape = martialescape->isChecked();
                                                                  v.mMartialGrab = martialgrab->isChecked();
                                                                  v.mMartialStrike = martialstrike->isChecked();
                                                                  v.mMartialThrow = martialthrow->isChecked();
                                                                  v.mNerveStrike = nervestrike->isChecked();
                                                                  v.mOffensiveStrike = offensivestrike->isChecked();
                                                                  v.mPassingStrike = passingstrike->isChecked();
                                                                  v.mSacrifceThrow = sacrifcethrow->isChecked();
                                                                  v.mExtraDamageClass = extradamageclass->text().toInt(0);
                                                                  v.mWeaponElements = weaponelements->text().toInt(0);
                                                                  v.mWeapons = weapons->text();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = CombatSkills::toJson();
                                                                  obj["choke hold"]           = v.mChokeHold;
                                                                  obj["defensive strike"]     = v.mDefensiveStrike;
                                                                  obj["killing strike"]       = v.mKillingStrike;
                                                                  obj["legsweep"]             = v.mLegSweep;
                                                                  obj["martial block"]        = v.mMartialBlock;
                                                                  obj["martial disarm"]       = v.mMartialDisarm;
                                                                  obj["martial dodge"]        = v.mMartialDodge;
                                                                  obj["martial escape"]       = v.mMartialEscape;
                                                                  obj["martial grab"]         = v.mMartialGrab;
                                                                  obj["martial strike"]       = v.mMartialStrike;
                                                                  obj["martial throw"]        = v.mMartialThrow;
                                                                  obj["nerve strike"]         = v.mNerveStrike;
                                                                  obj["offensive strike"]     = v.mOffensiveStrike;
                                                                  obj["passing strike"]       = v.mPassingStrike;
                                                                  obj["sacrifce throw"]       = v.mSacrifceThrow;
                                                                  obj["extra damage classes"] = v.mExtraDamageClass;
                                                                  obj["weapon elements"]      = v.mWeaponElements;
                                                                  obj["weapons"]              = v.mWeapons;
                                                                  return obj;
                                                                }

private:
    struct vars {
        bool mChokeHold        = false;
        bool mDefensiveStrike  = false;
        bool mKillingStrike    = false;
        bool mLegSweep         = false;
        bool mMartialBlock     = false;
        bool mMartialDisarm    = false;
        bool mMartialDodge     = false;
        bool mMartialEscape    = false;
        bool mMartialGrab      = false;
        bool mMartialStrike    = false;
        bool mMartialThrow     = false;
        bool mNerveStrike      = false;
        bool mOffensiveStrike  = false;
        bool mPassingStrike    = false;
        bool mSacrifceThrow    = false;
        int  mExtraDamageClass = 0;
        int  mWeaponElements   = 0;
        QString mWeapons       = "";
    } v;

    QCheckBox* chokehold = nullptr;
    QCheckBox* defensivestrike = nullptr;
    QCheckBox* killingstrike = nullptr;
    QCheckBox* legsweep = nullptr;
    QCheckBox* martialblock = nullptr;
    QCheckBox* martialdisarm = nullptr;
    QCheckBox* martialdodge = nullptr;
    QCheckBox* martialescape = nullptr;
    QCheckBox* martialgrab = nullptr;
    QCheckBox* martialstrike = nullptr;
    QCheckBox* martialthrow = nullptr;
    QCheckBox* nervestrike = nullptr;
    QCheckBox* offensivestrike = nullptr;
    QCheckBox* passingstrike = nullptr;
    QCheckBox* sacrifcethrow = nullptr;
    QLineEdit* extradamageclass = nullptr;
    QLineEdit* weaponelements = nullptr;
    QLineEdit* weapons = nullptr;

    QString optOut() {
        if (points(SkillTalentOrPerk::NoStore) < 10) return "<incomplete>"; // NOLINT
        if (v.mWeaponElements > 0 && v.mWeapons.isEmpty()) return "<incomplete>";
        QString res = CombatSkills::description();
        QString sep = ": ";

        if (v.mChokeHold) { res += sep + "Choke Hold"; sep = ", "; }
        if (v.mDefensiveStrike) { res += sep + "Defensive Strike"; sep = ", "; }
        if (v.mKillingStrike) { res += sep + "Killing Strike"; sep = ", "; }
        if (v.mLegSweep) { res += sep + "Legsweep"; sep = ", "; }
        if (v.mMartialBlock) { res += sep + "Martial Block"; sep = ", "; }
        if (v.mMartialDisarm) { res += sep + "Martial Disarm"; sep = ", "; }
        if (v.mMartialDodge) { res += sep + "Martial Dodge"; sep = ", "; }
        if (v.mMartialEscape) { res += sep + "Martial Escape"; sep = ", "; }
        if (v.mMartialGrab) { res += sep + "Martial Grab"; sep = ", "; }
        if (v.mMartialStrike) { res += sep + "Martial Strike"; sep = ", "; }
        if (v.mMartialThrow) { res += sep + "Martial Throw"; sep = ", "; }
        if (v.mNerveStrike) { res += sep + "Nerve Strike"; sep = ", "; }
        if (v.mPassingStrike) { res += sep + "Passing Strike"; sep = ", "; }
        if (v.mSacrifceThrow) { res += sep + "Sacrifice Throw"; sep = ", "; }
        if (v.mExtraDamageClass > 0) { res += sep + QString("%1 Extra Damage Class%2").arg(v.mExtraDamageClass).arg((v.mExtraDamageClass > 1) ? "es": ""); sep = ", "; }
        if (v.mWeaponElements > 0) { res += sep + "Weapon Elements(" + v.mWeapons + ")"; sep = ", "; }

        return res;
    }

    void numeric(QString) override {
        QLineEdit* fld = dynamic_cast<QLineEdit*>(SkillTalentOrPerk::sender());
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
    MCSL(const QJsonObject& json): CombatSkills(json)  { v.mPlus = json["plus"].toInt(1);
                                                         v.mFor  = json["for"].toString("");
                                                         v.mSize = json["size"].toInt(0);
                                                       }
    ~MCSL() override { }
    MCSL& operator=(const MCSL& s) {
        if (this != &s) {
            CombatSkills::operator=(s);
            v = s.v;
        }
        return *this;
    }
    MCSL& operator=(MCSL&& s) {
        CombatSkills::operator=(s);
        v = s.v;
        return *this;
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
                                                                  QList<Points> size{ 0_cp, 1_cp, 3_cp, 6_cp }; // NOLINT
                                                                  return v.mPlus * size[v.mSize + 1]; }
    void    restore() override                                  { vars s = v;
                                                                  plus->setText(QString("%1").arg(v.mPlus));
                                                                  size->setCurrentIndex(v.mSize);
                                                                  forwhat->setText(v.mFor);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mPlus = plus->text().toInt(0);
                                                                  v.mFor  = forwhat->text();
                                                                  v.mSize = size->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = CombatSkills::toJson();
                                                                  obj["plus"] = v.mPlus;
                                                                  obj["for"]  = v.mFor;
                                                                  obj["size"] = v.mSize;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     mPlus = 0;
        QString mFor = "";
        int     mSize = -1;
    } v;

    QLineEdit* plus = nullptr;
    QLineEdit* forwhat = nullptr;
    QComboBox* size = nullptr;

    QString optOut() {
        if (v.mPlus < 1) return "<incomplete>";
        QString res = "Mental Combat Skill Levels: ";
        switch (v.mSize) {
        case 0: res += QString("+%1 with %2").arg(v.mPlus).arg(v.mFor);               break;
        case 1: res += QString("+%1 with small group (%2)").arg(v.mPlus).arg(v.mFor); break;
        case 2: res += QString("+%1 with all mental combat").arg(v.mPlus);            break;
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
    PenaltySkillLevels(const QJsonObject& json): CombatSkills(json) { v.mPlus = json["plus"].toInt(0);
                                                                      v.mWhat = json["what"].toInt(0);
                                                                      v.mWith = json["with"].toString("");
                                                                    }
    ~PenaltySkillLevels() override { }
    PenaltySkillLevels& operator=(const PenaltySkillLevels& s) {
        if (this != &s) {
            CombatSkills::operator=(s);
            v = s.v;
        }
        return *this;
    }
    PenaltySkillLevels& operator=(PenaltySkillLevels&& s) {
        CombatSkills::operator=(s);
        v = s.v;
        return *this;
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
                                                                  return v.mPlus * what[v.mWhat + 1]; }
    void    restore() override                                  { vars s = v;
                                                                  plus->setText(QString("%1").arg(s.mPlus));
                                                                  what->setCurrentIndex(s.mWhat);
                                                                  with->setText(s.mWith);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mPlus = plus->text().toInt(0);
                                                                  v.mWhat = what->currentIndex();
                                                                  v.mWith = with->text();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = CombatSkills::toJson();
                                                                  obj["plus"] = v.mPlus;
                                                                  obj["what"] = v.mWhat;
                                                                  obj["with"] = v.mWith;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     mPlus = 0;
        int     mWhat = -1;
        QString mWith = "";
    } v;

    QLineEdit* plus = nullptr;
    QComboBox* what = nullptr;
    QLineEdit* with = nullptr;

    QString optOut() {
        if (v.mPlus < 1 || v.mWith.isEmpty()) return "<incomplete>";
        QString res = "Penalty Skill Levels: ";
        res += QString("+%1 with ").arg(v.mPlus);
        switch (v.mWhat) {
        case 0: res += QString("(Single Attack (OCV); %1)").arg(v.mWith);          break;
        case 1: res += QString("(Tight Group of Attacks (OCV); %1)").arg(v.mWith); break;
        case 2: res += QString("(All attacks (OCV))").arg(v.mPlus);                break;
        case 3: res += QString("(A single condition (DCV); %1)").arg(v.mWith);     break;
        case 4: res += QString("(A Group of Conditions (DCV); %1)").arg(v.mWith);  break;
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
    WeaponFamiliarity(const QJsonObject& json): CombatSkills(json) { v.mWhat = json["what"].toInt(0);
                                                                     v.mWith = json["with"].toString("");
                                                                   }
    ~WeaponFamiliarity() override { }
    WeaponFamiliarity& operator=(const WeaponFamiliarity& s) {
        if (this != &s) {
            CombatSkills::operator=(s);
            v = s.v;
        }
        return *this;
    }
    WeaponFamiliarity& operator=(WeaponFamiliarity&& s) {
        CombatSkills::operator=(s);
        v = s.v;
        return *this;
    }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { what = createComboBox(parent, layout, "Familar with?", { "One class of weapons",
                                                                                                                           "Broad category of weapons"});
                                                                  with = createLineEdit(parent, layout, "Applies to what?");
                                                                  return true;
                                                                }
    Points   points(bool noStore = false) override              { if (!noStore) store();
                                                                  QList<Points> what{ 0_cp, 1_cp, 2_cp };
                                                                  return what[v.mWhat + 1]; }
    void    restore() override                                  { vars s = v;
                                                                  what->setCurrentIndex(s.mWhat);
                                                                  with->setText(s.mWith);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mWhat = what->currentIndex();
                                                                  v.mWith = with->text();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = CombatSkills::toJson();
                                                                  obj["what"] = v.mWhat;
                                                                  obj["with"] = v.mWith;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     mWhat = -1;
        QString mWith = "";
    } v;

    QComboBox* what = nullptr;
    QLineEdit* with = nullptr;

    QString optOut() {
        if (v.mWith.isEmpty()) return "<incomplete>";
        QString res = "WF: ";
        switch (v.mWhat) {
        case 0: res += QString("(One Class of Weapons; %1)").arg(v.mWith); break;
        case 1: res += QString("(A Group of Weapons; %1)").arg(v.mWith);   break;
        default: return "<incomplete>";
        }

        return res;
    }
};

#undef CLASS
#undef CLASS_SPACE

#endif // COMBATSKILLS_H
