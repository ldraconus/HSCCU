#ifndef OPTION_H
#define OPTION_H

#include <QObject>

class Option
{
public:
    Option();

    int     totalPoints()      { return _totalPoints; }
    Option& totalPoints(int x) { _totalPoints = x; return *this; }

private:
    int _totalPoints;
};

#endif // OPTION_H
