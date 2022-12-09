#include "fraction.h"

#include <cmath>

static long gcd(long a, long b) {
    if (b == 0) return a;
    return gcd(b, a % b);
}

void Fraction::reduce() {
    long g = gcd(_numerator % _denominator, std::abs(_denominator));
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

QString Fraction::toString() {
    QString w = QString("%1").arg(abs(_numerator / _denominator));
    QString n = QString("%1").arg(abs(_numerator % _denominator));
    QString d = QString("%1").arg(_denominator);
    return ((_numerator < 0) ? "-" : "") + ((w != "0") ? w : "") + toSuper(n) + "⁄" + toSub(d);
}

Fraction operator+(long x, const Fraction& f) { return Fraction(x) + f; }
Fraction operator-(long x, const Fraction& f) { return Fraction(x) - f; }
Fraction operator*(long x, const Fraction& f) { return Fraction(x) * f; }
Fraction operator/(long x, const Fraction& f) { return Fraction(x) / f; }
