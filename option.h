#ifndef OPTION_H
#define OPTION_H

#include "shared.h"

#include <QObject>
#include <QJsonObject>

class Option {
public:
    Option();
    Option(const Option&);
    Option(Option&&);
    Option(const QJsonObject& obj);
    virtual ~Option() { }

    Option& operator=(const Option&);
    Option& operator=(Option&&);

    QString banner()             { return mBanner; }
    Points  complications()      { return mComplications; }
    bool    equipmentFree()      { return mEquipmentFree; }
    bool    showNotesPage()      { return mShowNotesPage; }
    bool    showFrequencyRolls() { return mShowFrequencyRolls; }
    bool    normalHumanMaxima()  { return mNormalHumanMaxima; }
    Points  activePerEND()       { return mActivePerEND; }
    Points  totalPoints()        { return mTotalPoints; }

    Option& banner(const QString& nm)  { mBanner = nm;            return *this; }
    Option& complications(Points x)    { mComplications = x;      return *this; }
    Option& equipmentFree(bool f)      { mEquipmentFree = f;      return *this; }
    Option& showNotesPage(bool f)      { mShowNotesPage = f;      return *this; }
    Option& normalHumanMaxima(bool f)  { mNormalHumanMaxima = f;  return *this; }
    Option& showFrequencyRolls(bool f) { mShowFrequencyRolls = f; return *this; }
    Option& activePerEND(Points x)     { mActivePerEND = x;       return *this; }
    Option& totalPoints(Points x)      { mTotalPoints = x;        return *this; }

    void load();
    void store();
    void toJson(QJsonObject&);

private:
    QString mBanner = ":/gfx/HeroSystem-Banner.png";
    Points  mComplications = 0_cp;
    bool    mEquipmentFree = false;
    bool    mShowFrequencyRolls = false;
    bool    mShowNotesPage = false;
    bool    mNormalHumanMaxima = false;
    Points  mActivePerEND = 0_cp;
    Points  mTotalPoints = 0_cp;
};

#endif // OPTION_H
