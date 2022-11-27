#ifndef OPTION_H
#define OPTION_H

#include <QObject>

class Option
{
public:
    Option();

    int     complications()      { return _complications; }
    Option& complications(int x) { _complications = x; return *this; }
    int     totalPoints()        { return _totalPoints; }
    Option& totalPoints(int x)   { _totalPoints = x; return *this; }

private:
    int _totalPoints;
    int _complications;
};

#endif // OPTION_H
