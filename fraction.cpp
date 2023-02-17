#include "fraction.h"

#include <cmath>

static long gcd(long a, long b) {
    if (b == 0) return a;
    return gcd(b, a % b);
}

void Fraction::reduce() {
    long g = gcd(::abs(_numerator) % ::abs(_denominator), std::abs(_denominator));
    _numerator /= g;
    _denominator /= g;
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
    Fraction n(::abs(_numerator), _denominator);
    return n;
}

QString Fraction::toString() {
    QString w = QString("%1").arg(::abs(_numerator / _denominator));
    QString n = QString("%1").arg(::abs(_numerator % _denominator));
    QString d = QString("%1").arg(::abs(_denominator));
    return ((_numerator < 0) ? "-" : "") + ((w != "0") ? w : "") + ((n != "0") ? toSuper(n) + "⁄" + toSub(d) : ((w == "0") ? "0" : ""));
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
bool     operator>=(long x,  const Fraction f) { return Fraction(x) >= f; }
bool     operator>(long x,   const Fraction f) { return Fraction(x) > f; }
