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
    virtual ~Option() { }

    Option& operator=(const Option&);
    Option& operator=(Option&&);

    QString banner()             { return _banner; }
    Points  complications()      { return _complications; }
    bool    equipmentFree()      { return _equipmentFree; }
    bool    showNotesPage()      { return _showNotesPage; }
    bool    showFrequencyRolls() { return _showFrequencyRolls; }
    bool    normalHumanMaxima()  { return _normalHumanMaxima; }
    Points  activePerEND()       { return _activePerEND; }
    Points  totalPoints()        { return _totalPoints; }

    Option& banner(const QString& nm)  { _banner = nm;            return *this; }
    Option& complications(Points x)    { _complications = x;      return *this; }
    Option& equipmentFree(bool f)      { _equipmentFree = f;      return *this; }
    Option& showNotesPage(bool f)      { _showNotesPage = f;      return *this; }
    Option& normalHumanMaxima(bool f)  { _normalHumanMaxima = f;  return *this; }
    Option& showFrequencyRolls(bool f) { _showFrequencyRolls = f; return *this; }
    Option& activePerEND(Points x)     { _activePerEND = x;       return *this; }
    Option& totalPoints(Points x)      { _totalPoints = x;        return *this; }

    void load();
    void store();
    void toJson(QJsonObject&);

private:
    QString _banner = ":/gfx/HeroSystem-Banner.png";
    Points  _complications = 0_cp;
    bool    _equipmentFree = false;
    bool    _showFrequencyRolls = false;
    bool    _showNotesPage = false;
    bool    _normalHumanMaxima = false;
    Points  _activePerEND = 0_cp;
    Points  _totalPoints = 0_cp;
};

#endif // OPTION_H
