#include "characteristic.h"
#ifndef ISHSC
#include "sheet.h"
#endif

Characteristic::Characteristic(const QJsonObject& c)
    : mCost(this->cost())
{
    if (c.find("base") == c.end()) throw("");

    mBase = c["base"].toInt(0);
    mInit = this->init();
    mPer = this->per();
}

Characteristic& Characteristic::operator=(const Characteristic& c) {
    if (this != &c) mBase = c.mBase;
    return *this;
}

Characteristic& Characteristic::operator=(Characteristic&& c) {
    mBase = c.mBase;
    return *this;
}

Points Characteristic::points() {
#ifndef ISHSC
    Option& opt = Sheet::ref().option();
#endif

    int half = mPer / 2;
    if (mPer % 2 == 0) half--;
#ifndef ISHSC
    int max = opt.normalHumanMaxima() ? mMaxima : INT_MAX;
#else
    int max = INT_MAX;
#endif
    int val = mBase - mInit;
    int min = (val + mInit > max) ? max - mInit : val;
    int dbl = val - min;

    return Points(((min + dbl) * mCost.points + dbl * mCost.points + half) / mPer);
}


QString Characteristic::roll() {
    constexpr int baseRoll = 9;
    constexpr int step = 5;
    constexpr int halfStep = step / 2;

    QString r = QString("%1-").arg(baseRoll + (mBase + mPrimary + halfStep) / step);
    if (mSecondary != 0) r += QString("/%1-").arg(baseRoll + (mBase + mPrimary + mSecondary + halfStep) / step);
    return r;
}

QJsonObject Characteristic::toJson() {
    QJsonObject obj;
    obj["base"] = mBase;
    return obj;
}

QString Characteristic::value() {
    QString v = QString("%1").arg(mBase + mPrimary);
    if (mSecondary != 0) v += QString("/%1").arg(mBase + mPrimary + mSecondary);
    return v;
}

