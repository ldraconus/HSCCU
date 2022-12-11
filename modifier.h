#ifndef MODIFIER_H
#define MODIFIER_H

#include "fraction.h"

#include <QObject>

#include <variant>
#include <functional>

using std::function;
using std::variant;

typedef variant<Fraction, int> value;

class Modifier {
public:
    static const bool isLimitation = 1;
    static const bool isAdvantage  = 0;
    static const int  isBoth       = 2;
    static const bool isAdder      = true;
    static const bool isModifier   = false;

    Modifier(QString nm, int lim, bool adder)
        : _name(nm)
        , _limitation(lim)
        , _adder(adder) { }
    Modifier(Modifier&& m)
        : _name(m._name)
        , _limitation(m._limitation)
        , _adder(m._adder) { }
    Modifier(const Modifier& m)
        : _name(m._name)
        , _limitation(m._limitation)
        , _adder(m._adder) { }

    Modifier& operator=(const Modifier& m) {
        if (this != &m) {
            _name       = m._name;
            _limitation = m._limitation;
            _adder      = m._adder;
        }
        return *this;
    }
    Modifier& operator=(Modifier&& m) {
        _name       = m._name;
        _limitation = m._limitation;
        _adder      = m._adder;
        return *this;
    }

    QString name() { return _name; }

private:
    QString _name;
    int     _limitation;
    bool    _adder;
};

class Modifiers {
private:
    QList<Modifier> _modifiers;

public:
    Modifiers();

    QList<Modifier> operator()(class Power*);
};


#endif // MODIFIER_H
