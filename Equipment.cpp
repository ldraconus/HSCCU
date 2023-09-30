#include "Equipment.h"

QMap<QString, QJsonObject> Armor::_armors;
QMap<QString, QJsonObject> Weapon::_weapons;
QMap<QString, QJsonObject> Equip::_equip;

Equipment::Equipment(const QJsonObject& json) {
    QString type;
    QString name = json["powerName"].toString("");
    if (Equip::catalog().find(name) != Equip::catalog().end()) type = "Equip";
    else if (Weapon::catalog().find(name) != Weapon::catalog().end()) type = "Weapon";
    else if (Armor::catalog().find(name) != Armor::catalog().end()) type = "Armor";
    else return;
    load(json, type);
}
