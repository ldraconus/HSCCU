#include "Equipment.h"

QMap<QString, QJsonObject> Armor::mArmors;   // NOLINT
QMap<QString, QJsonObject> Weapon::mWeapons; // NOLINT
QMap<QString, QJsonObject> Equip::mEquip;    // NOLINT

Equipment::Equipment(const QJsonObject& json) {
    QString type;
    QString name = json["powerName"].toString("");
    if (Equip::catalog().find(name) != Equip::catalog().end()) type = "Equip";
    else if (Weapon::catalog().find(name) != Weapon::catalog().end()) type = "Weapon";
    else if (Armor::catalog().find(name) != Armor::catalog().end()) type = "Armor";
    else return;
    load(json, type);
}
