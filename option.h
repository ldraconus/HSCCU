#ifndef OPTION_H
#define OPTION_H

#include "shared.h"

#include <QObject>
#include <QJsonObject>

class Option
{
public:
    Option();
    Option(const Option&);
    Option(Option&&);
    Option(const QJsonObject& obj);

    Option& operator=(const Option&);
    Option& operator=(Option&&);

    Points<> complications()           { return _complications; }
    Option&  complications(Points<> x) { _complications = x; return *this; }
    Points<> totalPoints()             { return _totalPoints; }
    Option&  totalPoints(Points<> x)   { _totalPoints = x; return *this; }

    void load();
    void store();
    void toJson(QJsonObject&);

private:
    Points<> _totalPoints;
    Points<> _complications;
};

#endif // OPTION_H
