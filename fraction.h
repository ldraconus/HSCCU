#ifndef FRACTION_H
#define FRACTION_H

#include <QObject>

class Fraction
{
public:
    Fraction()
        : _numerator(0)
        , _denominator(1) { }
    Fraction(long n, long d)
        : _numerator(n)
        , _denominator(std::abs(d)) { reduce(); }
    Fraction(long w, long n, long d)
        : _numerator(n + w * d)
        , _denominator(std::abs(d)) { reduce(); }
    Fraction(long w, const Fraction& f)
        : _numerator(f._numerator + w * f._denominator)
        , _denominator(f._denominator) { reduce(); }
    explicit Fraction(long x)
        : _numerator(x)
        , _denominator(1) { }
    Fraction(const Fraction& f)
        : _numerator(f._numerator)
        , _denominator(f._denominator) { }
    Fraction(Fraction&& f)
        : _numerator(f._numerator)
        , _denominator(f._denominator) { }

    long numerator()   { return _numerator; }
    long denominator() { return _denominator; }

    Fraction& operator=(const Fraction& f) {
        if (&f != this) {
            _numerator = f._numerator;
            _denominator = f._denominator;
        }
        return *this;
    }
    Fraction& operator=(Fraction&& f) {
        _numerator = f._numerator;
        _denominator = f._denominator;
        return *this;
    }
    Fraction& operator=(const long n) {
        Fraction f(n);
        return *this = f;
    }
    Fraction& operator+=(const Fraction f) {
        *this = *this + f;
        return *this;
    }
    Fraction operator+=(const long n) { return *this += Fraction(n); }
    Fraction& operator-=(const Fraction f) {
        *this = *this - f;
        return *this;
    }
    Fraction operator-=(const long n) { return *this += Fraction(n); }
    Fraction& operator*=(const Fraction f) {
        *this = *this * f;
        return *this;
    }
    Fraction operator*=(const long n) { return *this *= Fraction(n); }
    Fraction operator+(const Fraction f) {
        long long n1 = _numerator;
        long long d1 = _denominator;
        long long n2 = f._numerator;
        long long d2 = f._denominator;
        Fraction fr(n1 * d2 + n2 * d1, d1 * d2);
        fr.reduce();
        return fr;
    }
    Fraction operator+(const long n) { return *this + Fraction(n); }
    Fraction operator-(const Fraction f) {
        long long n1 = _numerator;
        long long d1 = _denominator;
        long long n2 = f._numerator;
        long long d2 = f._denominator;
        Fraction fr(n1 * d2 - n2 * d1, d1 * d2);
        fr.reduce();
        return fr;
    }
    Fraction operator-(const long n) { return *this - Fraction(n); }
    Fraction operator*(const Fraction f) {
        long long n1 = _numerator;
        long long d1 = _denominator;
        long long n2 = f._numerator;
        long long d2 = f._denominator;
        Fraction fr(n1 * n2, d1 * d2);
        fr.reduce();
        return fr;
    }
    Fraction operator*(const long n) { return *this * Fraction(n); }
    Fraction operator/(const Fraction f) {
        long long n1 = _numerator;
        long long d1 = _denominator;
        long long n2 = f._numerator;
        long long d2 = f._denominator;
        Fraction fr(n1 * d2, ::abs(d1 * n2));
        fr.reduce();
        return fr;
    }
    Fraction operator/(const long n) { return *this / Fraction(n); }
    bool     operator<(const Fraction f) {
        Fraction n = *this - f;
        return n._numerator < 0;
    }
    bool     operator<(const long n) { return *this < Fraction(n); }
    bool     operator<=(const Fraction f) {
        Fraction n = *this - f;
        return n._numerator <= 0;
    }
    bool     operator<=(const long n) { return *this <= Fraction(n); }
    bool     operator==(const Fraction f) {
        Fraction n = *this - f;
        return n._numerator == 0;
    }
    bool     operator==(const long n) { return *this == Fraction(n); }
    bool     operator>=(const Fraction f) {
        Fraction n = *this - f;
        return n._numerator >= 0;
    }
    bool     operator>=(const long n) { return *this >= Fraction(n); }
    bool     operator>(const Fraction f) {
        Fraction n = *this - f;
        return n._numerator > 0;
    }
    bool     operator>(const long n) { return *this > Fraction(n); }

    int toInt(bool down = false) const {
        if (down && _denominator % 2 == 0) return (_numerator + (_denominator / 2) - 1) / _denominator;
        return (_numerator + (_denominator / 2)) / _denominator;
    }

    Fraction abs();
    QString  toString();

private:
    long _numerator;
    long _denominator;

    void reduce();
};

extern long&     operator+=(long& x, const Fraction f);
extern long&     operator-=(long& x, const Fraction f);
extern long&     operator*=(long& x, const Fraction f);
extern Fraction  operator+(long x,  const Fraction f);
extern Fraction  operator-(long x,  const Fraction f);
extern bool      operator<(long x,  const Fraction f);
extern bool      operator<=(long x, const Fraction f);
extern bool      operator==(long x, const Fraction f);
extern bool      operator>=(long x, const Fraction f);
extern bool      operator>(long x,  const Fraction f);
extern Fraction  operator*(long x,  const Fraction f);
extern Fraction  operator/(long x,  const Fraction f);

#endif // FRACTION_H
