#include "fraction.h"

#include <cmath>

static long gcd(long a, long b) {
    if (b == 0) return a;
    return gcd(b, a % b);
}

void Fraction::reduce() {
    long g = gcd(::abs(mNumerator) % ::abs(mDenominator), std::abs(mDenominator));
    mNumerator /= g;
    mDenominator /= g;
}

static QString toSuper(const QString& n) {
    static QString super = "⁰¹²³⁴⁵⁶⁷⁸⁹";
    QString res;
    for (const auto x: n) res += super[x.toLatin1() - '0'];
    return res;
}

static QString toSub(const QString& n) {
    static QString sub = "₀₁₂₃₄₅₆₇₈₉";
    QString res;
    for (const auto x: n) res += sub[x.toLatin1() - '0'];
    return res;
}

Fraction Fraction::abs() {
    Fraction n(::abs(mNumerator), mDenominator);
    return n;
}

QString Fraction::toString() {
    QString w = QString("%1").arg(::abs(mNumerator / mDenominator));
    QString n = QString("%1").arg(::abs(mNumerator % mDenominator));
    QString d = QString("%1").arg(::abs(mDenominator));
    return ((mNumerator < 0) ? "-" : "") + ((w != "0") ? w : "") + ((n != "0") ? toSuper(n) + "⁄" + toSub(d) : ((w == "0") ? "0" : ""));
}

long&    operator+=(long& x, const Fraction f) { x += f.toInt(); return x; }
long&    operator-=(long& x, const Fraction f) { x -= f.toInt(); return x; }
long&    operator*=(long& x, const Fraction f) { x += f.toInt(); return x; }
Fraction operator+(long x,   const Fraction f) { return Fraction(x) + f; }
Fraction operator-(long x,   const Fraction f) { return Fraction(x) - f; }
Fraction operator*(long x,   const Fraction f) { return Fraction(x) * f; }
Fraction operator/(long x,   const Fraction f) { return Fraction(x) / f; }
bool     operator<(long x,   const Fraction f) { return Fraction(x) < f; }
bool     operator<=(long x,  const Fraction f) { return Fraction(x) <= f; }
bool     operator==(long x,  const Fraction f) { return Fraction(x) == f; }
bool     operator!=(long x,  const Fraction f) { return Fraction(x) != f; }
bool     operator>=(long x,  const Fraction f) { return Fraction(x) >= f; }
bool     operator>(long x,   const Fraction f) { return Fraction(x) > f; }
