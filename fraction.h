#ifndef FRACTION_H
#define FRACTION_H

#include <QString>
#include <cmath>

class Fraction {
public:
    Fraction()
        : mNumerator(0)
        , mDenominator(1) { }
    Fraction(long n, long d)
        : mNumerator(n)
        , mDenominator(std::abs(d)) { reduce(); }
    Fraction(long w, long n, long d)
        : mNumerator(n + w * d)
        , mDenominator(std::abs(d)) { reduce(); }
    Fraction(long w, const Fraction& f)
        : mNumerator(f.mNumerator + w * f.mDenominator)
        , mDenominator(f.mDenominator) { reduce(); }
    explicit Fraction(long x)
        : mNumerator(x)
        , mDenominator(1) { }
    Fraction(const Fraction& f)
        : mNumerator(f.mNumerator)
        , mDenominator(f.mDenominator) { }
    Fraction(Fraction&& f)
        : mNumerator(f.mNumerator)
        , mDenominator(f.mDenominator) { }
    virtual ~Fraction() { }

    long numerator()   { return mNumerator; }
    long denominator() { return mDenominator; }

    Fraction& operator=(const Fraction& f) {
        if (&f != this) {
            mNumerator = f.mNumerator;
            mDenominator = f.mDenominator;
        }
        return *this;
    }
    Fraction& operator=(Fraction&& f) {
        mNumerator = f.mNumerator;
        mDenominator = f.mDenominator;
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
        long long n1 = mNumerator;
        long long d1 = mDenominator;
        long long n2 = f.mNumerator;
        long long d2 = f.mDenominator;
        Fraction fr(long(n1 * d2 + n2 * d1), long(d1 * d2));
        fr.reduce();
        return fr;
    }
    Fraction operator+(const long n) { return *this + Fraction(n); }
    Fraction operator-(const Fraction f) {
        long long n1 = mNumerator;
        long long d1 = mDenominator;
        long long n2 = f.mNumerator;
        long long d2 = f.mDenominator;
        Fraction fr(long(n1 * d2 - n2 * d1), long(d1 * d2));
        fr.reduce();
        return fr;
    }
    Fraction operator-(const long n) { return *this - Fraction(n); }
    Fraction operator*(const Fraction f) {
        long long n1 = mNumerator;
        long long d1 = mDenominator;
        long long n2 = f.mNumerator;
        long long d2 = f.mDenominator;
        Fraction fr(long(n1 * n2), long(d1 * d2));
        fr.reduce();
        return fr;
    }
    Fraction operator*(const long n) { return *this * Fraction(n); }
    Fraction operator/(const Fraction f) {
        long long n1 = mNumerator;
        long long d1 = mDenominator;
        long long n2 = f.mNumerator;
        long long d2 = f.mDenominator;
        Fraction fr(long(n1 * d2), ::abs(long(d1 * n2)));
        fr.reduce();
        return fr;
    }
    Fraction operator/(const long n) { return *this / Fraction(n); }
    bool     operator<(const Fraction f) {
        Fraction n = *this - f;
        return n.mNumerator < 0;
    }
    bool     operator<(const long n) { return *this < Fraction(n); }
    bool     operator<=(const Fraction f) {
        Fraction n = *this - f;
        return n.mNumerator <= 0;
    }
    bool     operator<=(const long n) { return *this <= Fraction(n); }
    bool     operator==(const Fraction f) {
        Fraction n = *this - f;
        return n.mNumerator == 0;
    }
    bool     operator==(const long n) { return *this == Fraction(n); }
    bool     operator!=(const Fraction f) {
        Fraction n = *this - f;
        return n.mNumerator != 0;
    }
    bool     operator!=(const long n) { return *this == Fraction(n); }
    bool     operator>=(const Fraction f) {
        Fraction n = *this - f;
        return n.mNumerator >= 0;
    }
    bool     operator>=(const long n) { return *this >= Fraction(n); }
    bool     operator>(const Fraction f) {
        Fraction n = *this - f;
        return n.mNumerator > 0;
    }
    bool     operator>(const long n) { return *this > Fraction(n); }

    int toInt(bool down = false) const {
        if (down && mDenominator % 2 == 0) return (mNumerator + (mDenominator / 2) - 1) / mDenominator;
        return (mNumerator + (mDenominator / 2)) / mDenominator;
    }

    Fraction abs();
    QString  toString();

private:
    long mNumerator;
    long mDenominator;

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
extern bool      operator!=(long x, const Fraction f);
extern bool      operator>=(long x, const Fraction f);
extern bool      operator>(long x,  const Fraction f);
extern Fraction  operator*(long x,  const Fraction f);
extern Fraction  operator/(long x,  const Fraction f);

#endif // FRACTION_H
