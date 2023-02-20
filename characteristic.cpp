#include "characteristic.h"

Characteristic::Characteristic(const QJsonObject& c)
    : _cost(this->cost())
{
    if (c.find("base")      == c.end() ||
        c.find("primary")   == c.end() ||
        c.find("secondary") == c.end()) throw("");

    _base      = c["base"].toInt(0);
    _primary   = c["primary"].toInt(0);
    _secondary = c["secondary"].toInt(0);
}

Characteristic& Characteristic::operator=(const Characteristic& c) {
    if (this != &c) {
        _base      = c._base;
        _primary   = c._primary;
        _secondary = c._secondary;
    }
    return *this;
}

Characteristic& Characteristic::operator=(Characteristic&& c) {
    _base      = c._base;
    _primary   = c._primary;
    _secondary = c._secondary;
    return *this;
}

Points<> Characteristic::points() {
    int half = _per / 2;
    if (_per % 2 == 0) half--;
    return Points<>(((_base - _init) * _cost.points + half) / _per);
}


QString Characteristic::roll() {
    QString r = QString("%1-").arg(9 + (_base + _primary + 2) / 5);
    if (_secondary != 0) r += QString("/%1-").arg(9 + (_base + _primary + _secondary + 2) / 5);
    return r;
}

QJsonObject Characteristic::toJson() {
    QJsonObject obj;
    obj["base"]      = _base;
    obj["primary"]   = _primary;
    obj["secondary"] = _secondary;
    return obj;
}

QString Characteristic::value() {
    QString v = QString("%1").arg(_base + _primary);
    if (_secondary != 0) v += QString("/%1").arg(_base + _primary + _secondary);
    return v;
}
