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

    Points  complications()      { return _complications; }
    bool    showNotesPage()      { return _showNotesPage; }
    bool    showFrequencyRolls() { return _showFrequencyRolls; }
    bool    normalHumanMaxima()  { return _normalHumanMaxima; }
    Points  totalPoints()        { return _totalPoints; }

    Option& complications(Points x)    { _complications = x;      return *this; }
    Option& showNotesPage(bool f)      { _showNotesPage = f;      return *this; }
    Option& normalHumanMaxima(bool f)  { _normalHumanMaxima = f;  return *this; }
    Option& showFrequencyRolls(bool f) { _showFrequencyRolls = f; return *this; }
    Option& totalPoints(Points x)      { _totalPoints = x;        return *this; }

    void load();
    void store();
    void toJson(QJsonObject&);

private:
    Points _complications;
    bool   _showFrequencyRolls;
    bool   _showNotesPage;
    bool   _normalHumanMaxima;
    Points _totalPoints;
};

#endif // OPTION_H
