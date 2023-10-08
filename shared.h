#ifndef SHARED_H
#define SHARED_H

#include <QJsonArray>

#include <map>
#include <memory>
#include <ratio>
#include <vector>
#include <tuple>

#ifdef ANDROID
#define _Mem_fn __mem_fn
#endif

template <typename T,
          typename TIter = decltype(std::begin(std::declval<T>())),
          typename = decltype(std::end(std::declval<T>()))>
constexpr auto enumerate(T && iterable)
{
    struct iterator
    {
        size_t i;
        TIter iter;
        bool operator != (const iterator & other) const { return iter != other.iter; }
        void operator ++ () { ++i; ++iter; }
        auto operator * () const { return std::tie(i, *iter); }
    };
    struct iterable_wrapper
    {
        T iterable; // NOLINT
        auto begin() { return iterator{ 0, std::begin(iterable) }; }
        auto end() { return iterator{ 0, std::end(iterable) }; }
    };
    return iterable_wrapper{ std::forward<T>(iterable) };
}

using std::shared_ptr;
using std::make_shared;

#ifdef __wasm__
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

inline constexpr auto operator"" _cp(unsigned long long p) { return Points{ (long) p }; }

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
    int _x = 0;
    int _y = 0;

public:
    At() { }
    At(int x, int y)
        : _x(x)
        , _y(y) { }
    At(const At& a)
        : _x(a._x)
        , _y(a._y) { }
    At(At&& a)
        : _x(a._x)
        , _y(a._y) { }
    virtual ~At() { }

    At& operator=(const At& a) {
        if (this != &a) {
            _x = a._x;
            _y = a._y;
        }
        return *this;
    }
    At& operator=(At&& a) {
        _x = a._x;
        _y = a._y;
        return *this;
    }

    int x() { return _x; }
    int y() { return _y; }
};

class Size {
private:
    int _l;
    int _h;

public:
    Size(int l = -1, int h = -1)
        : _l(l)
        , _h(h) { }
    Size(const Size& a)
        : _l(a._l)
        , _h(a._h) { }
    Size(Size&& a)
        : _l(a._l)
        , _h(a._h) { }
    virtual ~Size() { }

    Size& operator=(const Size& s) {
        if (this != &s) {
            _l = s._l;
            _h = s._h;
        }
        return *this;
    }
    Size& operator=(Size&& s) {
        _l = s._l;
        _h = s._h;
        return *this;
    }

    int l() { return _l; }
    int h() { return _h; }
};

constexpr int Hundreds = 100;
constexpr double Half = 0.5;

struct Coins {
private:
    long coins = 0;

public:
    explicit constexpr Coins(long x)
        : coins(x) { }
    Coins(QJsonValue& jv):
        coins(jv.toInt(0)) { }

    Coins& operator+=(Coins b) {
        coins += b.coins;
        return *this;
    }
    Coins& operator*=(Coins b) {
        coins *= b.coins;
        return *this;
    }
    Coins& operator/=(Coins b) {
        if (b.coins < 1) return *this;
        coins /= b.coins;
        return *this;
    }
    Coins& operator%=(Coins b) {
        if (b.coins < 1) return *this;
        coins %= b.coins;
        return *this;
    }
    Coins& operator-=(Coins b) {
        coins -= b.coins;
        return *this;
    }
    Coins& operator*=(long b) {
        coins *= b;
        return *this;
    }
    Coins& operator/=(long b) {
        if (b < 1) return *this;
        coins /= b;
        return *this;
    }
    Coins& operator%=(long b) {
        if (b < 1) return *this;
        coins %= b;
        return *this;
    }
    Coins& operator-=(long b) {
        coins -= b;
        return *this;
    }

    bool operator==(const Coins& p) const { return coins == p.coins; }
    bool operator<(const Coins& p) const { return coins < p.coins; }
    bool operator!=(const Coins& p) const { return !(*this == p); }
    bool operator>=(const Coins& p) const { return !(*this < p); }
    bool operator>(const Coins& p) const { return (*this >= p) && (*this != p); }
    bool operator<=(const Coins& p) const { return !(*this > p); }

    Coins Min(const Coins& p) const { return (*this < p) ? *this : p; }
    Coins Max(const Coins& p) const { return p.Min(*this); }

    QString toString() const { return QString("%1").arg(coins / Hundreds); }
    QJsonValue toJson() const {
        QJsonValue val = (int) coins;
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
    std::map<QString, long> _valuesByName;
    std::map<long, QString> _namesByValue;

public:
    Denominations() = delete;
    explicit Denominations(const std::vector<std::pair<QString, long>>& v) {
        for (const auto& ref: v) {
            _valuesByName[ref.first] = ref.second;
            _namesByValue[ref.second] = ref.first;
        }
    }

    long operator[](const QString& x) {
        if (_valuesByName.find(x) != _valuesByName.end()) return _valuesByName[x];
        return 0;
    }
    QString operator[](const long& x) {
        if (_namesByValue.find(x) != _namesByValue.end()) return _namesByValue[x];
        return 0;
    }
    QStringList keys() {
        QStringList lst;
        for (const auto& x: _valuesByName) lst << x.first;
        return lst;
    }

    size_t size() { return _valuesByName.size(); }
};

class Cash {
private:
    std::vector<Coins> _coins;
    Denominations _denominations;

public:
    Cash() = delete;
    Cash(const Denominations& d)
        : _denominations(d) {
        for (size_t i = 0; i < _denominations.size(); ++i) _coins.emplace_back(Coins(0));
    }

    void add(Coins x, const QString& d) {
        QStringList keys = _denominations.keys();
        for (auto [idx, key]: enumerate(keys))
            if (key == d) {
                _coins[idx] += x;
                return;
            }
    }

    void spend(Coins x, const QString& d) {
        QStringList keys = _denominations.keys();
        for (auto [idx, key]: enumerate(keys))
            if (key == d) {
                _coins[idx] -= x;
                return;
            }
    }

    void minimize() {
        Coins p = Coins(0);
        QStringList keys = _denominations.keys();
        for (auto [idx, key]: enumerate(keys)) p += _coins[idx] * _denominations[key];
        for (auto i = keys.size() - 1; i >= 0; --i) {
            _coins[i] = p / _denominations[keys[i]];
            p %= _denominations[keys[i]];
        }
    }
};

#endif // SHARED_H
