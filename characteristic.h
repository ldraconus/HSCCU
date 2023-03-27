#ifndef CHARACTERISTIC_H
#define CHARACTERISTIC_H

#include "shared.h"

#include <QJsonObject>
#include <QObject>

class Characteristic
{
public:
    Characteristic()
        : _base(0), _cost(0_cp), _init(0), _per(0), _primary(0), _secondary(0) { }
    Characteristic(int base, Points cost, int per = 1, int init = 0, int primary = 0, int secondary = 0)
        : _base(base), _cost(cost), _init(init == 0 ? base : init), _per(per), _primary(primary), _secondary(secondary) { }
    Characteristic(const Characteristic& c): Characteristic(c._base, c._cost, c._per, c._init, c._primary, c._secondary) { }
    Characteristic(Characteristic&& c): Characteristic(c._base, c._cost, c._per, c._init, c._primary, c._secondary) { }
    Characteristic(const QJsonObject& c);

    Characteristic& operator=(const Characteristic& c);
    Characteristic& operator=(Characteristic&& c);

    int             base()           { return _base; }
    Characteristic& base(int n)      { _base = n; return *this; }
    Points          cost()           { return _cost; }
    Characteristic& cost(Points n)   { _cost = n; return *this; }
    int             init()           { return _init; }
    Characteristic& init(int n)      { _init = n; return *this; }
    int             per()            { return _per; }
    Characteristic& per(int n)       { _per = n; return *this; }
    int             primary()        { return _primary; }
    Characteristic& primary(int n)   { _primary = n; return *this; }
    int             secondary()      { return _secondary; }
    Characteristic& secondary(int n) { _secondary = n; return *this; }

    Points      points();
    QString     roll();
    QJsonObject toJson();
    QString     value();

private:
    int    _base;
    Points _cost;
    int    _init;
    int    _per;
    int    _primary = 0;
    int    _secondary = 0;
};

#endif // CHARACTERISTIC_H
