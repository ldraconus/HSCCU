#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include "powers.h"
#include "shared.h"
#ifndef ISHSC
#include "powerdialog.h"
#include "sheet.h"
#endif


class Equipment: public AllPowers {
public:
    Equipment()
        : AllPowers("Equipment") { }
    Equipment(const Equipment& s)
        : AllPowers(s) { }
    Equipment(Equipment&& s)
        : AllPowers(s) { }
    Equipment(const QJsonObject& json);
    ~Equipment() override { }

    Equipment& operator=(const Equipment& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    Equipment& operator=(Equipment&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    void load(const QJsonObject& json, const QString& name) {
        AllPowers::load(json, name);
        v.mCost = json["cost"].toInt(0);
        v.mWeight = json["weight"].toInt(0);
        v.mActing = json["acting"].toInt(0);
        v.mExtraNotes = json["extraNotes"].toString("");
    }

    bool isEquipment() override { return true; }
    Fraction adv() override { return Fraction(0); }
    QString description(bool showEND = false) override { return optOut(showEND); }
    void form(QWidget* parent, QVBoxLayout* layout) override {
#ifndef ISHSC
        PowerDialog::ref().setEquipment();
#endif
        AllPowers::form(parent, layout);
        extraNotes = createLineEdit(parent, layout, "Notes");
    }
    Fraction lim() override { return Fraction(0); }
    Points points(bool) override { return 0_cp; }
    void restore() override {
        vars s = v;
        AllPowers::restore();
        extraNotes->setText(s.mExtraNotes);
        v = s;
    }
    void store() override {
        AllPowers::store();
        v.mExtraNotes = extraNotes->text();
    }
    QJsonObject toJson() const override {
        QJsonObject obj = AllPowers::toJson();
        obj["cost"] = v.mCost;
        obj["weight"] = v.mWeight;
        obj["acting"] = v.mActing;
        obj["extraNotes"] = v.mExtraNotes;
        return obj;
    }

    QString end() override { return "-"; }

    int cost() { return v.mCost; }
    int weight() { return v.mWeight; }
    int acting() { return v.mActing; }

private:
    struct vars {
        int mCost = 0;
        int mWeight = 0;
        int mActing = 0;
        QString mExtraNotes = "";
    } v;

    QLineEdit* extraNotes = nullptr;

protected:
    QString optOut(bool, double mod = 1.0, double modCost = 1.0) {
        QString res;
        if (v.mCost != 0 || v.mWeight != 0) res += "(";
        if (v.mCost != 0) res += QString("%1 sc").arg((int) (v.mCost * modCost + 0.5)); // NOLINT
        int wgt = (int) (v.mWeight * mod + 0.5); // NOLINT
        if (v.mWeight != 0) res += QString("%1%2.%3 kg").arg(v.mCost != 0 ? "/" : "").arg(wgt / 10).arg(wgt % 10); // NOLINT
        if (!res.isEmpty()) res += ")";
        if (!v.mExtraNotes.isEmpty()) res += v.mExtraNotes;
        return res;
    }
};

class Weapon: public Equipment {
public:
    Weapon()
        : Equipment() { }
    Weapon(const Weapon& s)
        : Equipment(s)
        , v(s.v) { }
    Weapon(Weapon&& s)
        : Equipment(s)
        , v(s.v) { }
    Weapon(const QJsonObject& json) {
        load(json);
    }
    ~Weapon() override { }

    Weapon& operator=(const Weapon& s) {
        if (this != &s) {
            Equipment::operator=(s);
            v = s.v;
        }
        return *this;
    }
    Weapon& operator=(Weapon&& s) {
        Equipment::operator=(s);
        v = s.v;
        return *this;
    }

    void load(const QJsonObject& json) {
        Equipment::load(json, "Weapon");
        v.mTwoHanded = json["twoHanded"].toBool(false);
        v.mWhich = json["which"].toInt(-1);
        v.mWithSTR = json["withSTR"].toBool(true);
        v.mWithoutMin = json["withoutMin"].toBool(false);
        v.mKilling = json["killing"].toBool(true);
        v.mRP = json["RP"].toBool(false);
        v.mOCV = json["OCV"].toInt(0);
        v.mDmgClasses = json["dmgClasses"].toInt(0);
        v.mBonusDC = json["bonusDC"].toInt(0);
        v.mBonusOCV = json["bonusOCV"].toInt(0);
        v.mSTRMin = json["STRMin"].toInt(0);
        v.mNormalSTR = json["normalSTR"].toBool(false);
        v.mNotes = json["notes"].toString("");
    }

    void index(int idx) override {
        auto keys = mWeapons.keys();
        load(mWeapons[keys[idx]]);
    }

    QString nickname() override {
        auto keys = mWeapons.keys();
        return keys[v.mWhich];
    }

    QString end() override {
        int STR = 0;
#ifndef ISHSC
        STR = Sheet::ref().character().STR().base() +
            Sheet::ref().character().STR().primary() +
            Sheet::ref().character().STR().secondary();
#endif
        return QString("%1").arg(STR / 5); // NOLINT
    }

    QString description(bool showEND = false) override { return optOut(showEND); }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        Equipment::form(parent, layout);
        which = createComboBox(parent, layout, "Pick a weapon", mWeapons.keys(), std::mem_fn(&Power::index));
        bonusDC = createLineEdit(parent, layout, "Bonus Damage Classes", std::mem_fn(&Power::numeric));
        bonusOCV = createLineEdit(parent, layout, "Bonus to Hit", std::mem_fn(&Power::numeric));
    }
    void restore() override {
        vars s = v;
        Equipment::restore();
        which->setCurrentIndex(s.mWhich);
        bonusDC->setText(QString("%1").arg(s.mBonusDC));
        bonusOCV->setText(QString("%1").arg(s.mBonusOCV));
        v = s;
    }
    void store() override {
        Equipment::store();
        v.mWhich = which->currentIndex();
        v.mBonusDC = bonusDC->text().toInt();
        v.mBonusOCV = bonusOCV->text().toInt();
    }
    QJsonObject toJson() const override {
        QJsonObject obj = Equipment::toJson();
        obj["twoHanded"] = v.mTwoHanded;
        obj["which"] = v.mWhich;
        obj["withSTR"] = v.mWithSTR;
        obj["withoutMin"] = v.mWithoutMin;
        obj["killing"] = v.mKilling;
        obj["RP"] = v.mRP;
        obj["OCV"] = v.mOCV;
        obj["dmgClasses"] = v.mDmgClasses;
        obj["bonusDC"] = v.mBonusDC;
        obj["bonusOCV"] = v.mBonusOCV;
        obj["STRMin"] = v.mSTRMin;
        obj["normalSTR"] = v.mNormalSTR;
        obj["notes"] = v.mNotes;
        return obj;
    }

    static QMap<QString, QJsonObject>& catalog() { return mWeapons; }

private:
    struct vars {
        bool mTwoHanded = false;
        bool mWithSTR = true;
        bool mWithoutMin = false;
        bool mKilling = true;
        bool mRP = false;
        int mWhich = -1;
        int mOCV = 0;
        int mDmgClasses = 0;
        int mBonusDC = 0;
        int mBonusOCV = 0;
        int mSTRMin = 0;
        bool mNormalSTR = false;
        QString mNotes = "";
    } v;

    static QMap<QString, QJsonObject> mWeapons; // NOLINT

    QComboBox* which = nullptr;
    QLineEdit* bonusDC = nullptr;
    QLineEdit* bonusOCV = nullptr;

    QString optOut(bool b) {
        if (v.mWhich < 0) return "<incomplete>";

        QString res = Equipment::optOut(b);
        QString sep = ", ";
        if (v.mTwoHanded) res += sep + "2 Handed";
        int STR = 0;
        QString dmg = "";
        if (v.mWithSTR) {
#ifndef ISHSC
            STR = Sheet::ref().character().STR().base() +
                Sheet::ref().character().STR().primary() +
                Sheet::ref().character().STR().secondary();
#endif
            if (STR < v.mSTRMin) STR = 0;
            else if (v.mWithoutMin) STR -= v.mSTRMin;

            if (STR < 0) STR = 0;
        }
        if (dmg.isEmpty()) {
            int classes = (v.mDmgClasses + v.mBonusDC);
            if (v.mNormalSTR) classes += Fraction(STR, 5).toInt(); // NOLINT
            else {
                Fraction cost(v.mDmgClasses, acting());
                classes += (STR * cost).toInt(true);
            }
            if (v.mKilling) {
                dmg += QString("%1d6").arg(classes / 3);
                switch (classes % 3) {
                case 0:
                    break;
                case 1:
                    dmg += "+1";
                    break;
                default:
                    dmg += "+" + Fraction(1, 2).toString();
                    break;
                }
            } else dmg += QString("%1d6 N").arg(classes);
        }
        res += sep + dmg;
        res += sep + QString("%1 STR min").arg(v.mSTRMin);
        if (v.mRP) res += sep + "Reduced Penetration";
        int ocv = v.mOCV + v.mBonusOCV;
        if (ocv >= 0) res += sep + QString("+%1").arg(ocv);
        else res += sep + QString("%1").arg(ocv);
        res += " OCV";
        res += sep + v.mNotes;
        return res;
    }
};

class Armor: public Equipment {
public:
    Armor()
        : Equipment() { }
    Armor(const Armor& s)
        : Equipment(s)
        , v(s.v) { }
    Armor(Armor&& s)
        : Equipment(s)
        , v(s.v) { }
    Armor(const QJsonObject& json) { load(json); }
    ~Armor() override { }
    Armor& operator=(const Armor& s) {
        if (this != &s) {
            Equipment::operator=(s);
            v = s.v;
        }
        return *this;
    }
    Armor& operator=(Armor&& s) {
        Equipment::operator=(s);
        v = s.v;
        return *this;
    }

    QString nickname() override {
        auto keys = mArmors.keys();
        return keys[v.mWhich];
    }

    QString hitLocations() {
        return v.mHitLocations;
    }
    int DEF() {
        QList<int> value {1, 2, 2, 3, 1, 2, 3, 1, 3, 3, 3, 2, 4, 4, 4, 3, 5, 5, 5, 4, 5, 6, 6, 7, 7, 7, 7, 7, 8}; // NOLINT
        int def = v.mBonus + value[v.mType];
        if (def < 0) def = 0;
        return def;
    }

    void index(int idx) override {
        auto keys = mArmors.keys();
        load(mArmors[keys[idx]]);
    }
    QList<double> fromArray(const QJsonArray& arr) {
        QList<double> src;
        for (const auto x: arr) src.push_back(x.toDouble());
        return src;
    }
    void load(const QJsonObject& json) {
        Equipment::load(json, "Armor");
        v.mHitLocations = json["hitLocations"].toString("3-18");
        v.mWhich = json["which"].toInt(-1);
        v.mType = json["type"].toInt(-1);
        v.mBonus = json["bonus"].toInt(0);
        v.mWeights = fromArray(json["weights"].toArray());
    }
    Fraction adv() override { return Fraction(0); }
    QString description(bool showEND = false) override { return optOut(showEND); }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        Equipment::form(parent, layout);
        which = createComboBox(parent, layout, "Pick an armor", mArmors.keys(), std::mem_fn(&Power::index));
        type = createComboBox(parent, layout, "Type of armor?", {"Heavy Cloth", "Padded Cloth", "Woven Cord", "Heavy Animal Hides", "Soft Leather", "Heavy Leather", "Cuir-Bouilli", "Studded Soft Leather", "Ring Armor (Soft Leather)", "Bezainted Soft Leather", "Jazeraint Soft Leather", "Studded Heavy Leather", "Ring Armor (Heavy Leather)", "Bezainted Heavy Leather", "Jazeraint Heavy Leather", "Studded Cuir-Bouilli", "Ring Armor (Cuir-Bouilli)", "Bezainted Cuir-Bouilli", "Jazeraint Cuir-Bouilli", "Brigandine", "Lamellar (Splint Armor)", "Banded Mail", "Chainmail", "Double Mail/Bar Mail", "Reinforced Chainmail", "Plate And Chain", "Plate Armor", "Field Plate Armor", "Full Plate Armor"});
        bonus = createLineEdit(parent, layout, "Bonus DEF?", std::mem_fn(&Power::numeric));
    }
    Fraction lim() override { return Fraction(0); }
    Points points(bool) override { return 0_cp; }
    void restore() override {
        vars s = v;
        Equipment::restore();
        which->setCurrentIndex(s.mWhich);
        type->setCurrentIndex(s.mType);
        bonus->setText(QString("%1").arg(s.mBonus));
        v = s;
    }
    void store() override {
        Equipment::store();
        v.mWhich = which->currentIndex();
        v.mType = type->currentIndex();
        v.mBonus = bonus->text().toInt();
    }
    QJsonArray toArray(const QList<double>& src) const {
        QJsonArray arr;
        for (const auto& x: src) arr.push_back(x);
        return arr;
    }
    QJsonObject toJson() const override {
        QJsonObject obj = Equipment::toJson();
        obj["hitLocations"] = v.mHitLocations;
        obj["which"] = v.mWhich;
        obj["type"] = v.mType;
        obj["bonus"] = v.mBonus;
        obj["weights"] = toArray(v.mWeights);
        return obj;
    }

    static QMap<QString, QJsonObject>& catalog() { return mArmors; }

private:
    struct vars {
        QString mHitLocations = "3-18";
        int mWhich = -1;
        int mType = 0;
        int mBonus = 0;
        QList<double> mWeights;
    } v;

    static QMap<QString, QJsonObject> mArmors; // NOLINT

    QComboBox* which = nullptr;
    QComboBox* type = nullptr;
    QLineEdit* bonus = nullptr;

    QString optOut(bool b) {
        if (v.mType < 0 || v.mWhich < 0) return "<incomplete>";
        QStringList typeOf {"Heavy Cloth", "Padded Cloth", "Woven Cord", "Heavy Animal Hides", "Soft Leather", "Heavy Leather", "Cuir-Bouilli", "Studded Soft Leather", "Ring Armor (Soft Leather)", "Bezainted Soft Leather", "Jazeraint Soft Leather", "Studded Heavy Leather", "Ring Armor (Heavy Leather)", "Bezainted Heavy Leather", "Jazeraint Heavy Leather", "Studded Cuir-Bouilli", "Ring Armor (Cuir-Bouilli)", "Bezainted Cuir-Bouilli", "Jazeraint Cuir-Bouilli", "Brigandine", "Lamellar (Splint Armor)", "Banded Mail", "Chainmail", "Double Mail/Bar Mail", "Reinforced Chainmail", "Plate And Chain", "Plate Armor", "Field Plate Armor", "Full Plate Armor"};
        QList<int> price   {20,            40,             40,           50,                   30,             50,              75,             36,                     75,                          80,                       90,                       90,                      90,                           100,                       110,                       110,                    100,                         110,                      110,                      100,          125,                       150,           150,         200,                    200,                    275,               350,           500,                 800}; // NOLINT
        QList<double> mass {3.5,           5.0,            5.0,          7.0,                  3.5,            5.0,             7.0,            3.5,                    7.0,                         7.0,                      7.0,                      5.0,                     10.0,                         10.0,                      10.0,                      7.0,                    14.0,                        14.0,                     14.0,                     10.0,         14.0,                      20.0,          20.0,        28.0,                   28.0,                   28.0,              28.0,          28.0,                40.0}; // NOLINT
        QList<int> value {1, 2, 2, 3, 1, 2, 3, 1, 3, 3, 3, 2, 4, 4, 4, 3, 5, 5, 5, 4, 5, 6, 6, 7, 7, 7, 7, 7, 8}; // NOLINT
        QString res = Equipment::optOut(b, 10.0 * v.mWeights[value[v.mType] - 1] * value[v.mType] / 8.0, price[v.mType] * v.mWeights[value[v.mType] - 1] / mass[v.mType]); // NOLINT
        QString sep = ", ";
        res += sep + typeOf[v.mType];
        int def = v.mBonus + value[v.mType];
        if (def < 0) def = 0;
        res += sep + QString("%1 rDEF").arg(def);
        res += sep + v.mHitLocations + " Locations";
        return res;
    }
};

class Equip: public Equipment {
public:
    Equip()
        : Equipment() { }
    Equip(const Equip& s)
        : Equipment(s)
        , v(s.v) { }
    Equip(Equip&& s)
        : Equipment(s)
        , v(s.v) { }
    Equip(const QJsonObject& json) { load(json); }
    ~Equip() override { }
    Equip& operator=(const Equip& s) {
        if (this != &s) {
            Equipment::operator=(s);
            v = s.v;
        }
        return *this;
    }
    Equip& operator=(Equip&& s) {
        Equipment::operator=(s);
        v = s.v;
        return *this;
    }

    QString nickname() override {
        auto keys = mEquip.keys();
        return keys[v.mWhich];
    }

    void index(int idx) override {
        auto keys = mEquip.keys();
        load(mEquip[keys[idx]]);
    }
    void load(const QJsonObject& json) {
        Equipment::load(json, "Equipment");
        v.mWhich = json["which"].toInt(-1);
        v.mNotes = json["notes"].toString();
        v.mSTRMin = json["strMin"].toInt(0);
    }
    Fraction adv() override { return Fraction(0); }
    QString description(bool showEND = false) override { return optOut(showEND); }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        Equipment::form(parent, layout);
        which = createComboBox(parent, layout, "Pick some equipment", mEquip.keys(), std::mem_fn(&Power::index));
    }
    Fraction lim() override { return Fraction(0); }
    Points points(bool) override { return 0_cp; }
    void restore() override {
        vars s = v;
        Equipment::restore();
        which->setCurrentIndex(s.mWhich);
        v = s;
    }
    void store() override {
        Equipment::store();
        v.mWhich = which->currentIndex();
    }
    QJsonObject toJson() const override {
        QJsonObject obj = Equipment::toJson();
        obj["which"] = v.mWhich;
        obj["strMin"] = v.mSTRMin;
        obj["notes"] = v.mNotes;
        return obj;
    }

    static QMap<QString, QJsonObject>& catalog() { return mEquip; }

private:
    struct vars {
        int mWhich = -1;
        int mSTRMin = 0;
        QString mNotes = "";
    } v;

    static QMap<QString, QJsonObject> mEquip; // NOLINT

    QComboBox* which = nullptr;

    QString optOut(bool b) {
        if (v.mWhich < 0) return "<incomplete>";
        QString res = Equipment::optOut(b);
        QString sep = " ";
        if (v.mSTRMin != 0) {
            res += sep + QString("%1 STR min").arg(v.mSTRMin);
            sep = ", ";
        }
        if (!v.mNotes.isEmpty()) res += sep + v.mNotes;
        return res;
    }
};

#endif
