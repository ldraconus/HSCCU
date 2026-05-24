#ifndef SHARED_H
#define SHARED_H

#include <QJsonArray>

#include <map>
#include <memory>
#include <vector>
#include <tuple>

#ifdef ANDROID
#define _Mem_fn __mem_fn
#endif

template <typename T,
          typename TIter = decltype(std::begin(std::declval<T>())),
          typename = decltype(std::end(std::declval<T>()))>
constexpr auto enumerate(T && iterable) {
    struct iterator {
        size_t i;
        TIter iter;
        bool operator != (const iterator & other) const { return iter != other.iter; }
        void operator ++ () { ++i; ++iter; }
        auto operator * () const { return std::tie(i, *iter); }
    };
    struct iterable_wrapper {
        T iterable; // NOLINT
        auto begin() { return iterator{ 0, std::begin(iterable) }; }
        auto end() { return iterator{ 0, std::end(iterable) }; }
    };
    return iterable_wrapper{ std::forward<T>(iterable) };
}

using std::shared_ptr;
using std::make_shared;

#ifdef __EMSCRIPTEN__
#define _Mem_fn __mem_fn
#endif

struct Points {
    long points;
    explicit constexpr Points(long x)
        : points(x) { }

    Points& operator+=(Points b) { points += b.points; return *this; }
    Points& operator-=(Points b) { points -= b.points; return *this; }
    Points& operator*=(Points b) { points *= b.points; return *this; }
    Points& operator/=(Points b) { points /= b.points; return *this; }
    Points& operator%=(Points b) { points %= b.points; return *this; }

    bool Min(const Points& p) const { return points < p.points ? points : p.points; }
    bool Max(const Points& p) const { return p.Min(*this); }
};

inline constexpr auto operator"" _cp(unsigned long long p) { return Points{ (long) p }; } // NOLINT

inline Points operator*(long a, Points b)   { return Points(a * b.points); }
inline Points operator*(Points a, long b)   { return Points(a.points * b); }
inline Points operator*(Points a, Points b) { return Points(a.points * b.points); }

inline Points operator/(long a, Points b)   { return Points(a / b.points); }
inline Points operator/(Points a, long b)   { return Points(a.points / b); }
inline Points operator/(Points a, Points b) { return Points(a.points / b.points); }

inline Points operator%(long a, Points b)   { return Points(a / b.points); }
inline Points operator%(Points a, long b)   { return Points(a.points / b); }
inline Points operator%(Points a, Points b) { return Points(a.points / b.points); }

inline Points operator+(long a, Points b)   { return Points(a + b.points); }
inline Points operator+(Points a, long b)   { return Points(a.points + b); }
inline Points operator+(Points a, Points b) { return Points(a.points + b.points); }

inline Points operator-(long a, Points b)   { return Points(a - b.points); }
inline Points operator-(Points a, long b)   { return Points(a.points - b); }
inline Points operator-(Points a, Points b) { return Points(a.points - b.points); }

inline bool operator<(long a, Points b)   { return a < b.points; }
inline bool operator<(Points a, long b)   { return a.points < b; }
inline bool operator<(Points a, Points b) { return a.points < b.points; }

inline bool operator<=(long a, Points b)   { return a <= b.points; }
inline bool operator<=(Points a, long b)   { return a.points <= b; }
inline bool operator<=(Points a, Points b) { return a.points <= b.points; }

inline bool operator==(long a, Points b)   { return a == b.points; }
inline bool operator==(Points a, long b)   { return a.points == b; }
inline bool operator==(Points a, Points b) { return a.points == b.points; }

inline bool operator>(long a, Points b)   { return a > b.points; }
inline bool operator>(Points a, long b)   { return a.points > b; }
inline bool operator>(Points a, Points b) { return a.points > b.points; }

inline bool operator>=(long a, Points b)   { return a >= b.points; }
inline bool operator>=(Points a, long b)   { return a.points >= b; }
inline bool operator>=(Points a, Points b) { return a.points >= b.points; }

class At {
private:
    int mX = 0;
    int mY = 0;

public:
    At() { }
    At(int x, int y)
        : mX(x)
        , mY(y) { }
    At(const At& a)
        : mX(a.mX)
        , mY(a.mY) { }
    At(At&& a)
        : mX(a.mX)
        , mY(a.mY) { }
    virtual ~At() { }

    At& operator=(const At& a) {
        if (this != &a) {
            mX = a.mX;
            mY = a.mY;
        }
        return *this;
    }
    At& operator=(At&& a) {
        mX = a.mX;
        mY = a.mY;
        return *this;
    }

    int x() { return mX; }
    int y() { return mY; }
};

class Size {
private:
    int mL;
    int mH;

public:
    Size(int l = -1, int h = -1)
        : mL(l)
        , mH(h) { }
    Size(const Size& a)
        : mL(a.mL)
        , mH(a.mH) { }
    Size(Size&& a)
        : mL(a.mL)
        , mH(a.mH) { }
    virtual ~Size() { }

    Size& operator=(const Size& s) {
        if (this != &s) {
            mL = s.mL;
            mH = s.mH;
        }
        return *this;
    }
    Size& operator=(Size&& s) {
        mL = s.mL;
        mH = s.mH;
        return *this;
    }

    int l() { return mL; }
    int h() { return mH; }
};

constexpr int Hundreds = 100;
constexpr double Half = 0.5;

struct Coins {
private:
    long mCoins = 0;

public:
    explicit constexpr Coins(long x)
        : mCoins(x) { }
    Coins(QJsonValue& jv):
        mCoins(jv.toInt(0)) { }

    Coins& operator+=(Coins b) {
        mCoins += b.mCoins;
        return *this;
    }
    Coins& operator*=(Coins b) {
        mCoins *= b.mCoins;
        return *this;
    }
    Coins& operator/=(Coins b) {
        if (b.mCoins < 1) return *this;
        mCoins /= b.mCoins;
        return *this;
    }
    Coins& operator%=(Coins b) {
        if (b.mCoins < 1) return *this;
        mCoins %= b.mCoins;
        return *this;
    }
    Coins& operator-=(Coins b) {
        mCoins -= b.mCoins;
        return *this;
    }
    Coins& operator*=(long b) {
        mCoins *= b;
        return *this;
    }
    Coins& operator/=(long b) {
        if (b < 1) return *this;
        mCoins /= b;
        return *this;
    }
    Coins& operator%=(long b) {
        if (b < 1) return *this;
        mCoins %= b;
        return *this;
    }
    Coins& operator-=(long b) {
        mCoins -= b;
        return *this;
    }

    bool operator==(const Coins& p) const { return mCoins == p.mCoins; }
    bool operator<(const Coins& p) const { return mCoins < p.mCoins; }
    bool operator!=(const Coins& p) const { return !(*this == p); }
    bool operator>=(const Coins& p) const { return !(*this < p); }
    bool operator>(const Coins& p) const { return (*this >= p) && (*this != p); }
    bool operator<=(const Coins& p) const { return !(*this > p); }

    Coins Min(const Coins& p) const { return (*this < p) ? *this : p; }
    Coins Max(const Coins& p) const { return p.Min(*this); }

    QString toString() const { return QString("%1").arg(mCoins / Hundreds); }
    QJsonValue toJson() const {
        QJsonValue val = (int) mCoins;
        return val;
    }
};

inline Coins operator+(long a, Coins b) {
    Coins c(a);
    return c += b;
}
inline Coins operator+(Coins a, long b) {
    Coins c(b);
    return c += a;
}
inline Coins operator+(Coins a, Coins b) {
    Coins c(a);
    return c += b;
}

inline Coins operator*(long a, Coins b) {
    Coins c(a);
    return c *= b;
}
inline Coins operator*(Coins a, long b) {
    Coins c(b);
    return c *= a;
}
inline Coins operator*(Coins a, Coins b) {
    Coins c(a);
    return c = b;
}

inline Coins operator/(long a, Coins b) {
    Coins c(a);
    return c /= b;
}
inline Coins operator/(Coins a, long b) {
    Coins c(b);
    return c /= a;
}
inline Coins operator/(Coins a, Coins b) {
    Coins c(a);
    return c /= b;
}

inline Coins operator%(long a, Coins b) {
    Coins c(a);
    return c %= b;
}
inline Coins operator%(Coins a, long b) {
    Coins c(b);
    return c %= a;
}
inline Coins operator%(Coins a, Coins b) {
    Coins c(a);
    return c %= b;
}

inline Coins operator-(long a, Coins b) {
    Coins c(a);
    return c -= b;
}
inline Coins operator-(Coins a, long b) {
    Coins c(b);
    return a -= c;
}
inline Coins operator-(Coins a, Coins b) {
    Coins c(a);
    return c -= b;
}

inline bool operator<(long a, Coins b) { return Coins(a) < b; }
inline bool operator<(Coins a, long b) { return a < Coins(b); }

inline bool operator<=(long a, Coins b) { return Coins(a) <= b; }
inline bool operator<=(Coins a, long b) { return a <= Coins(b); }

inline bool operator==(long a, Coins b) { return Coins(a) == b; }
inline bool operator==(Coins a, long b) { return a == Coins(b); }

inline bool operator>(long a, Coins b) { return Coins(a) > b; }
inline bool operator>(Coins a, long b) { return a > Coins(b); }

inline bool operator>=(long a, Coins b) { return Coins(a) >= b; }
inline bool operator>=(Coins a, long b) { return a >= Coins(b); }

class Denominations {
private:
    std::map<QString, long> mValuesByName;
    std::map<long, QString> mNamesByValue;

public:
    Denominations() = delete;
    explicit Denominations(const std::vector<std::pair<QString, long>>& v) {
        for (const auto& ref: v) {
            mValuesByName[ref.first] = ref.second;
            mNamesByValue[ref.second] = ref.first;
        }
    }

    long operator[](const QString& x) {
        if (mValuesByName.find(x) != mValuesByName.end()) return mValuesByName[x];
        return 0;
    }
    QString operator[](const long& x) {
        if (mNamesByValue.find(x) != mNamesByValue.end()) return mNamesByValue[x];
        return 0;
    }
    QStringList keys() {
        QStringList lst;
        for (const auto& x: mValuesByName) lst << x.first;
        return lst;
    }

    size_t size() { return mValuesByName.size(); }
};

class Cash {
private:
    std::vector<Coins> mCoins;
    Denominations mDenominations;

public:
    Cash() = delete;
    Cash(const Denominations& d)
        : mDenominations(d) {
        for (size_t i = 0; i < mDenominations.size(); ++i) mCoins.emplace_back(Coins(0));
    }

    void add(Coins x, const QString& d) {
        QStringList keys = mDenominations.keys();
        for (auto [idx, key]: enumerate(keys))
            if (key == d) {
                mCoins[idx] += x;
                return;
            }
    }

    void spend(Coins x, const QString& d) {
        QStringList keys = mDenominations.keys();
        for (auto [idx, key]: enumerate(keys))
            if (key == d) {
                mCoins[idx] -= x;
                return;
            }
    }

    void minimize() {
        Coins p = Coins(0);
        QStringList keys = mDenominations.keys();
        for (auto [idx, key]: enumerate(keys)) p += mCoins[idx] * mDenominations[key];
        for (auto i = keys.size() - 1; i >= 0; --i) {
            mCoins[i] = p / mDenominations[keys[i]];
            p %= mDenominations[keys[i]];
        }
    }
};

#endif // SHARED_H
