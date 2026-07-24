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

    QString banner() const             { return mBanner; }
    Points  complications() const      { return mComplications; }
    bool    equipmentFree() const      { return mEquipmentFree; }
    bool    showNotesPage() const      { return mShowNotesPage; }
    bool    showFrequencyRolls() const { return mShowFrequencyRolls; }
    bool    normalHumanMaxima() const  { return mNormalHumanMaxima; }
    bool    abbreviations() const      { return mAbbreviations; }
    bool    greenfields() const        { return mGreenfields; }
    Points  activePerEND() const       { return mActivePerEND; }
    Points  totalPoints() const        { return mTotalPoints; }

    Option& abbreviations(bool f)      { mAbbreviations = f;      return *this; }
    Option& banner(const QString& nm)  { mBanner = nm;            return *this; }
    Option& complications(Points x)    { mComplications = x;      return *this; }
    Option& equipmentFree(bool f)      { mEquipmentFree = f;      return *this; }
    Option& greenfields(bool f)        { mGreenfields = f;        return *this; }
    Option& showNotesPage(bool f)      { mShowNotesPage = f;      return *this; }
    Option& normalHumanMaxima(bool f)  { mNormalHumanMaxima = f;  return *this; }
    Option& showFrequencyRolls(bool f) { mShowFrequencyRolls = f; return *this; }
    Option& activePerEND(Points x)     { mActivePerEND = x;       return *this; }
    Option& totalPoints(Points x)      { mTotalPoints = x;        return *this; }

    void load();
    void store();
    void toJson(QJsonObject&);

private:
    bool    mAbbreviations = false;
    Points  mActivePerEND = 0_cp;
    QString mBanner = ":/gfx/HeroSystem-Banner.png";
    Points  mComplications = 0_cp;
    bool    mEquipmentFree = false;
    bool    mGreenfields = false;
    bool    mNormalHumanMaxima = false;
    bool    mShowFrequencyRolls = false;
    bool    mShowNotesPage = false;
    Points  mTotalPoints = 0_cp;
};

#endif // OPTION_H
