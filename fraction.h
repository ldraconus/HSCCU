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
    Fraction(long x)
        : _numerator(x)
        , _denominator(1) { }
    Fraction(const Fraction& f)
        : _numerator(f._numerator)
        , _denominator(f._denominator) { }
    Fraction(Fraction&& f)
        : _numerator(f._numerator)
        , _denominator(f._denominator) { }

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
    Fraction operator+(const Fraction& f) {
        double n1 = _numerator;
        double d1 = _denominator;
        double n2 = f._numerator;
        double d2 = f._denominator;
        return Fraction(n1 * d2 + n2 * d1, d1 * d2);
    }
    Fraction operator+(const long n) { return *this + Fraction(n); }
    Fraction operator-(const Fraction& f) {
        double n1 = _numerator;
        double d1 = _denominator;
        double n2 = f._numerator;
        double d2 = f._denominator;
        return Fraction(n1 * d2 - n2 * d1, d1 * d2);
    }
    Fraction operator-(const long n) { return *this - Fraction(n); }
    Fraction operator*(const Fraction& f) {
        double n1 = _numerator;
        double d1 = _denominator;
        double n2 = f._numerator;
        double d2 = f._denominator;
        return Fraction(n1 * n2, d1 * d2);
    }
    Fraction operator*(const long n) { return *this * Fraction(n); }
    Fraction operator/(const Fraction& f) {
        double n1 = _numerator;
        double d1 = _denominator;
        double n2 = f._numerator;
        double d2 = f._denominator;
        return Fraction(n1 * d2, d1 * n2);
    }
    Fraction operator/(const long n) { return *this / Fraction(n); }

    int toInt()      { return (_numerator + (_denominator / 2)) / _denominator; }

    QString toString();

private:
    long _numerator;
    long _denominator;

    void reduce();
};

extern Fraction operator+(long x, const Fraction& f);
extern Fraction operator-(long x, const Fraction& f);
extern Fraction operator*(long x, const Fraction& f);
extern Fraction operator/(long x, const Fraction& f);


#endif // FRACTION_H
