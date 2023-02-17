#ifndef OPTION_H
#define OPTION_H

#include "shared.h"

#include <QObject>

class Option
{
public:
    Option();

    Points<> complications()           { return _complications; }
    Option&  complications(Points<> x) { _complications = x; return *this; }
    Points<> totalPoints()             { return _totalPoints; }
    Option&  totalPoints(Points<> x)   { _totalPoints = x; return *this; }

private:
    Points<> _totalPoints;
    Points<> _complications;
};

#endif // OPTION_H
