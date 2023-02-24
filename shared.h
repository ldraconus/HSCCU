#ifndef SHARED_H
#define SHARED_H

#include <memory>
#include <ratio>

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

    bool Min(const Points p) const { return points < p.points ? points : p.points; }
    bool Max(const Points p) const { return p.Min(*this); }
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
    int _x;
    int _y;

public:
    At(int x, int y)
        : _x(x)
        , _y(y) { }
    At(const At& a)
        : _x(a._x)
        , _y(a._y) { }
    At(At&& a)
        : _x(a._x)
        , _y(a._y) { }

    const int& x = _x;
    const int& y = _y;
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

    const int& l = _l;
    const int& h = _h;
};

#endif // SHARED_H
