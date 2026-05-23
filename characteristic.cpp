#include "characteristic.h"
#ifndef ISHSC
#include "sheet.h"
#endif

Characteristic::Characteristic(const QJsonObject& c)
    : _cost(this->cost())
{
    if (c.find("base") == c.end()) throw("");

    _base = c["base"].toInt(0);
    _init = this->init();
    _per = this->per();
}

Characteristic& Characteristic::operator=(const Characteristic& c) {
    if (this != &c) _base = c._base;
    return *this;
}

Characteristic& Characteristic::operator=(Characteristic&& c) {
    _base = c._base;
    return *this;
}

Points Characteristic::points() {
#ifndef ISHSC
    Option& opt = Sheet::ref().option();
#endif

    int half = _per / 2;
    if (_per % 2 == 0) half--;
#ifndef ISHSC
    int max = opt.normalHumanMaxima() ? _maxima : INT_MAX;
#else
    int max = INT_MAX;
#endif
    int val = _base - _init;
    int min = (val + _init > max) ? max - _init : val;
    int dbl = val - min;

    return Points(((min + dbl) * _cost.points + dbl * _cost.points + half) / _per);
}


QString Characteristic::roll() {
    constexpr int baseRoll = 9;
    constexpr int step = 5;
    constexpr int halfStep = step / 2;

    QString r = QString("%1-").arg(baseRoll + (_base + _primary + halfStep) / step);
    if (_secondary != 0) r += QString("/%1-").arg(baseRoll + (_base + _primary + _secondary + halfStep) / step);
    return r;
}

QJsonObject Characteristic::toJson() {
    QJsonObject obj;
    obj["base"] = _base;
    return obj;
}

QString Characteristic::value() {
    QString v = QString("%1").arg(_base + _primary);
    if (_secondary != 0) v += QString("/%1").arg(_base + _primary + _secondary);
    return v;
}

