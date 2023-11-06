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
        v._cost = json["cost"].toInt(0);
        v._weight = json["weight"].toInt(0);
        v._acting = json["acting"].toInt(0);
        v._extraNotes = json["extraNotes"].toString("");
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
        extraNotes->setText(s._extraNotes);
        v = s;
    }
    void store() override {
        AllPowers::store();
        v._extraNotes = extraNotes->text();
    }
    QJsonObject toJson() override {
        QJsonObject obj = AllPowers::toJson();
        obj["cost"] = v._cost;
        obj["weight"] = v._weight;
        obj["acting"] = v._acting;
        obj["extraNotes"] = v._extraNotes;
        return obj;
    }

    QString end() override { return "-"; }

    int cost() { return v._cost; }
    int weight() { return v._weight; }
    int acting() { return v._acting; }

private:
    struct vars {
        int _cost = 0;
        int _weight = 0;
        int _acting = 0;
        QString _extraNotes = "";
    } v;

    QLineEdit* extraNotes = nullptr;

protected:
    QString optOut(bool, double mod = 1.0, double modCost = 1.0) {
        QString res;
        if (v._cost != 0 || v._weight != 0) res += "(";
        if (v._cost != 0) res += QString("%1 sc").arg((int) (v._cost * modCost + 0.5)); // NOLINT
        int wgt = (int) (v._weight * mod + 0.5); // NOLINT
        if (v._weight != 0) res += QString("%1%2.%3 kg").arg(v._cost != 0 ? "/" : "").arg(wgt / 10).arg(wgt % 10); // NOLINT
        if (!res.isEmpty()) res += ")";
        if (!v._extraNotes.isEmpty()) res += v._extraNotes;
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
        v._twoHanded = json["twoHanded"].toBool(false);
        v._which = json["which"].toInt(-1);
        v._withSTR = json["withSTR"].toBool(true);
        v._withoutMin = json["withoutMin"].toBool(false);
        v._killing = json["killing"].toBool(true);
        v._rp = json["RP"].toBool(false);
        v._ocv = json["OCV"].toInt(0);
        v._dmgClasses = json["dmgClasses"].toInt(0);
        v._bonusDC = json["bonusDC"].toInt(0);
        v._bonusOCV = json["bonusOCV"].toInt(0);
        v._strMin = json["STRMin"].toInt(0);
        v._normalSTR = json["normalSTR"].toBool(false);
        v._notes = json["notes"].toString("");
    }

    void index(int idx) override {
        auto keys = _weapons.keys();
        load(_weapons[keys[idx]]);
    }

    QString nickname() override {
        auto keys = _weapons.keys();
        return keys[v._which];
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
        which = createComboBox(parent, layout, "Pick a weapon", _weapons.keys(), std::mem_fn(&Power::index));
        bonusDC = createLineEdit(parent, layout, "Bonus Damage Classes", std::mem_fn(&Power::numeric));
        bonusOCV = createLineEdit(parent, layout, "Bonus to Hit", std::mem_fn(&Power::numeric));
    }
    void restore() override {
        vars s = v;
        Equipment::restore();
        which->setCurrentIndex(s._which);
        bonusDC->setText(QString("%1").arg(s._bonusDC));
        bonusOCV->setText(QString("%1").arg(s._bonusOCV));
        v = s;
    }
    void store() override {
        Equipment::store();
        v._which = which->currentIndex();
        v._bonusDC = bonusDC->text().toInt();
        v._bonusOCV = bonusOCV->text().toInt();
    }
    QJsonObject toJson() override {
        QJsonObject obj = Equipment::toJson();
        obj["twoHanded"] = v._twoHanded;
        obj["which"] = v._which;
        obj["withSTR"] = v._withSTR;
        obj["withoutMin"] = v._withoutMin;
        obj["killing"] = v._killing;
        obj["RP"] = v._rp;
        obj["OCV"] = v._ocv;
        obj["dmgClasses"] = v._dmgClasses;
        obj["bonusDC"] = v._bonusDC;
        obj["bonusOCV"] = v._bonusOCV;
        obj["STRMin"] = v._strMin;
        obj["normalSTR"] = v._normalSTR;
        obj["notes"] = v._notes;
        return obj;
    }

    static QMap<QString, QJsonObject>& catalog() { return _weapons; }

private:
    struct vars {
        bool _twoHanded = false;
        bool _withSTR = true;
        bool _withoutMin = false;
        bool _killing = true;
        bool _rp = false;
        int _which = -1;
        int _ocv = 0;
        int _dmgClasses = 0;
        int _bonusDC = 0;
        int _bonusOCV = 0;
        int _strMin = 0;
        bool _normalSTR = false;
        QString _notes = "";
    } v;

    static QMap<QString, QJsonObject> _weapons; // NOLINT

    QComboBox* which = nullptr;
    QLineEdit* bonusDC = nullptr;
    QLineEdit* bonusOCV = nullptr;

    QString optOut(bool b) {
        if (v._which < 0) return "<incomplete>";

        QString res = Equipment::optOut(b);
        QString sep = ", ";
        if (v._twoHanded) res += sep + "2 Handed";
        int STR = 0;
        QString dmg = "";
        if (v._withSTR) {
#ifndef ISHSC
            STR = Sheet::ref().character().STR().base() +
                Sheet::ref().character().STR().primary() +
                Sheet::ref().character().STR().secondary();
#endif
            if (STR < v._strMin) STR = 0;
            else if (v._withoutMin) STR -= v._strMin;

            if (STR < 0) STR = 0;
        }
        if (dmg.isEmpty()) {
            int classes = (v._dmgClasses + v._bonusDC);
            if (v._normalSTR) classes += Fraction(STR, 5).toInt(); // NOLINT
            else {
                Fraction cost(v._dmgClasses, acting());
                classes += (STR * cost).toInt(true);
            }
            if (v._killing) {
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
        res += sep + QString("%1 STR min").arg(v._strMin);
        if (v._rp) res += sep + "Reduced Penetration";
        int ocv = v._ocv + v._bonusOCV;
        if (ocv >= 0) res += sep + QString("+%1").arg(ocv);
        else res += sep + QString("%1").arg(ocv);
        res += " OCV";
        res += sep + v._notes;
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
        auto keys = _armors.keys();
        return keys[v._which];
    }

    QString hitLocations() {
        return v._hitLocations;
    }
    int DEF() {
        QList<int> value {1, 2, 2, 3, 1, 2, 3, 1, 3, 3, 3, 2, 4, 4, 4, 3, 5, 5, 5, 4, 5, 6, 6, 7, 7, 7, 7, 7, 8}; // NOLINT
        int def = v._bonus + value[v._type];
        if (def < 0) def = 0;
        return def;
    }

    void index(int idx) override {
        auto keys = _armors.keys();
        load(_armors[keys[idx]]);
    }
    QList<double> fromArray(const QJsonArray& arr) {
        QList<double> src;
        for (const auto x: arr) src.push_back(x.toDouble());
        return src;
    }
    void load(const QJsonObject& json) {
        Equipment::load(json, "Armor");
        v._hitLocations = json["hitLocations"].toString("3-18");
        v._which = json["which"].toInt(-1);
        v._type = json["type"].toInt(-1);
        v._bonus = json["bonus"].toInt(0);
        v._weights = fromArray(json["weights"].toArray());
    }
    Fraction adv() override { return Fraction(0); }
    QString description(bool showEND = false) override { return optOut(showEND); }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        Equipment::form(parent, layout);
        which = createComboBox(parent, layout, "Pick an armor", _armors.keys(), std::mem_fn(&Power::index));
        type = createComboBox(parent, layout, "Type of armor?", {"Heavy Cloth", "Padded Cloth", "Woven Cord", "Heavy Animal Hides", "Soft Leather", "Heavy Leather", "Cuir-Bouilli", "Studded Soft Leather", "Ring Armor (Soft Leather)", "Bezainted Soft Leather", "Jazeraint Soft Leather", "Studded Heavy Leather", "Ring Armor (Heavy Leather)", "Bezainted Heavy Leather", "Jazeraint Heavy Leather", "Studded Cuir-Bouilli", "Ring Armor (Cuir-Bouilli)", "Bezainted Cuir-Bouilli", "Jazeraint Cuir-Bouilli", "Brigandine", "Lamellar (Splint Armor)", "Banded Mail", "Chainmail", "Double Mail/Bar Mail", "Reinforced Chainmail", "Plate And Chain", "Plate Armor", "Field Plate Armor", "Full Plate Armor"});
        bonus = createLineEdit(parent, layout, "Bonus DEF?", std::mem_fn(&Power::numeric));
    }
    Fraction lim() override { return Fraction(0); }
    Points points(bool) override { return 0_cp; }
    void restore() override {
        vars s = v;
        Equipment::restore();
        which->setCurrentIndex(s._which);
        type->setCurrentIndex(s._type);
        bonus->setText(QString("%1").arg(s._bonus));
        v = s;
    }
    void store() override {
        Equipment::store();
        v._which = which->currentIndex();
        v._type = type->currentIndex();
        v._bonus = bonus->text().toInt();
    }
    QJsonArray toArray(QList<double>& src) {
        QJsonArray arr;
        for (const auto& x: src) arr.push_back(x);
        return arr;
    }
    QJsonObject toJson() override {
        QJsonObject obj = Equipment::toJson();
        obj["hitLocations"] = v._hitLocations;
        obj["which"] = v._which;
        obj["type"] = v._type;
        obj["bonus"] = v._bonus;
        obj["weights"] = toArray(v._weights);
        return obj;
    }

    static QMap<QString, QJsonObject>& catalog() { return _armors; }

private:
    struct vars {
        QString _hitLocations = "3-18";
        int _which = -1;
        int _type = 0;
        int _bonus = 0;
        QList<double> _weights;
    } v;

    static QMap<QString, QJsonObject> _armors; // NOLINT

    QComboBox* which = nullptr;
    QComboBox* type = nullptr;
    QLineEdit* bonus = nullptr;

    QString optOut(bool b) {
        if (v._type < 0 || v._which < 0) return "<incomplete>";
        QStringList typeOf {"Heavy Cloth", "Padded Cloth", "Woven Cord", "Heavy Animal Hides", "Soft Leather", "Heavy Leather", "Cuir-Bouilli", "Studded Soft Leather", "Ring Armor (Soft Leather)", "Bezainted Soft Leather", "Jazeraint Soft Leather", "Studded Heavy Leather", "Ring Armor (Heavy Leather)", "Bezainted Heavy Leather", "Jazeraint Heavy Leather", "Studded Cuir-Bouilli", "Ring Armor (Cuir-Bouilli)", "Bezainted Cuir-Bouilli", "Jazeraint Cuir-Bouilli", "Brigandine", "Lamellar (Splint Armor)", "Banded Mail", "Chainmail", "Double Mail/Bar Mail", "Reinforced Chainmail", "Plate And Chain", "Plate Armor", "Field Plate Armor", "Full Plate Armor"};
        QList<int> price   {20,            40,             40,           50,                   30,             50,              75,             36,                     75,                          80,                       90,                       90,                      90,                           100,                       110,                       110,                    100,                         110,                      110,                      100,          125,                       150,           150,         200,                    200,                    275,               350,           500,                 800}; // NOLINT
        QList<double> mass {3.5,           5.0,            5.0,          7.0,                  3.5,            5.0,             7.0,            3.5,                    7.0,                         7.0,                      7.0,                      5.0,                     10.0,                         10.0,                      10.0,                      7.0,                    14.0,                        14.0,                     14.0,                     10.0,         14.0,                      20.0,          20.0,        28.0,                   28.0,                   28.0,              28.0,          28.0,                40.0}; // NOLINT
        QList<int> value {1, 2, 2, 3, 1, 2, 3, 1, 3, 3, 3, 2, 4, 4, 4, 3, 5, 5, 5, 4, 5, 6, 6, 7, 7, 7, 7, 7, 8}; // NOLINT
        QString res = Equipment::optOut(b, 10.0 * v._weights[value[v._type] - 1] * value[v._type] / 8.0, price[v._type] * v._weights[value[v._type] - 1] / mass[v._type]); // NOLINT
        QString sep = ", ";
        res += sep + typeOf[v._type];
        int def = v._bonus + value[v._type];
        if (def < 0) def = 0;
        res += sep + QString("%1 rDEF").arg(def);
        res += sep + v._hitLocations + " Locations";
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
        auto keys = _equip.keys();
        return keys[v._which];
    }

    void index(int idx) override {
        auto keys = _equip.keys();
        load(_equip[keys[idx]]);
    }
    void load(const QJsonObject& json) {
        Equipment::load(json, "Equipment");
        v._which = json["which"].toInt(-1);
        v._notes = json["notes"].toString();
        v._strMin = json["strMin"].toInt(0);
    }
    Fraction adv() override { return Fraction(0); }
    QString description(bool showEND = false) override { return optOut(showEND); }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        Equipment::form(parent, layout);
        which = createComboBox(parent, layout, "Pick some equipment", _equip.keys(), std::mem_fn(&Power::index));
    }
    Fraction lim() override { return Fraction(0); }
    Points points(bool) override { return 0_cp; }
    void restore() override {
        vars s = v;
        Equipment::restore();
        which->setCurrentIndex(s._which);
        v = s;
    }
    void store() override {
        Equipment::store();
        v._which = which->currentIndex();
    }
    QJsonObject toJson() override {
        QJsonObject obj = Equipment::toJson();
        obj["which"] = v._which;
        obj["strMin"] = v._strMin;
        obj["notes"] = v._notes;
        return obj;
    }

    static QMap<QString, QJsonObject>& catalog() { return _equip; }

private:
    struct vars {
        int _which = -1;
        int _strMin = 0;
        QString _notes = "";
    } v;

    static QMap<QString, QJsonObject> _equip; // NOLINT

    QComboBox* which = nullptr;

    QString optOut(bool b) {
        if (v._which < 0) return "<incomplete>";
        QString res = Equipment::optOut(b);
        QString sep = " ";
        if (v._strMin != 0) {
            res += sep + QString("%1 STR min").arg(v._strMin);
            sep = ", ";
        }
        if (!v._notes.isEmpty()) res += sep + v._notes;
        return res;
    }
};

#endif
