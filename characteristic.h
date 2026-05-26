#ifndef CHARACTERISTIC_H
#define CHARACTERISTIC_H

#include "shared.h"

#include <QJsonObject>
#include <QObject>

class Characteristic {
public:
    Characteristic()
        : mBase(0), mCost(0_cp), mInit(0), mPer(0), mPrimary(0), mSecondary(0), mMaxima(0) { }
    Characteristic(int base, Points cost, int maxima, int per = 1, int init = 0, int primary = 0, int secondary = 0)
        : mBase(base), mCost(cost), mInit(init == 0 ? base : init), mPer(per), mPrimary(primary), mSecondary(secondary), mMaxima(maxima) { }
    Characteristic(const Characteristic& c): Characteristic(c.mBase, c.mCost, c.mMaxima, c.mPer, c.mInit, c.mPrimary, c.mSecondary) { }
    Characteristic(Characteristic&& c): Characteristic(c.mBase, c.mCost, c.mMaxima, c.mPer, c.mInit, c.mPrimary, c.mSecondary) { }
    Characteristic(const QJsonObject& c);
    ~Characteristic() { }

    Characteristic& operator=(const Characteristic& c);
    Characteristic& operator=(Characteristic&& c);

    int             base()           { return mBase; }
    Characteristic& base(int n)      { mBase = n; return *this; }
    Points          cost()           { return mCost; }
    Characteristic& cost(Points n)   { mCost = n; return *this; }
    int             init()           { return mInit; }
    Characteristic& init(int n)      { mInit = n; return *this; }
    int             maxima()         { return mMaxima; }
    Characteristic& maxima(int n)    { mMaxima = n; return *this; }
    int             per()            { return mPer; }
    Characteristic& per(int n)       { mPer = n; return *this; }
    int             primary()        { return mPrimary; }
    Characteristic& primary(int n)   { mPrimary = n; return *this; }
    int             secondary()      { return mSecondary; }
    Characteristic& secondary(int n) { mSecondary = n; return *this; }

    Points      points();
    QString     roll();
    QJsonObject toJson();
    QString     value();

private:
    int    mBase = 0;
    Points mCost = 0_cp;
    int    mInit = 0;
    int    mPer = 0;
    int    mPrimary = 0;
    int    mSecondary = 0;
    int    mMaxima = 0;
};

#endif // CHARACTERISTIC_H
