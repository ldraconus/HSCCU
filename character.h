#ifndef CHARACTER_H
#define CHARACTER_H

#include "shared.h"

#include "characteristic.h"
#include "complication.h"
#include "option.h"
#include "powers.h"
#include "skilltalentorperk.h"

#include <QObject>
#include <QPixmap>

class Character {
public:
    Character();
    Character(const Character& c);
    Character(Character&& c): Character(c) { }
    ~Character() { }

    Character& operator=(const Character& c);
    Character& operator=(Character&& c);

    QString    alternateIds()           { return mAlternateIds; }
    Character& alternateIds(QString a)  { mAlternateIds = a; return *this; }
    QString    campaignName()           { return mCampaignName; }
    Character& campaignName(QString n)  { mCampaignName = n; return *this; }
    QString    characterName()          { return mCharacterName; }
    Character& characterName(QString n) { mCharacterName = n; return *this; }
    QString    eyeColor()               { return mEyeColor; }
    Character& eyeColor(QString n)      { mEyeColor = n; return *this; }
    QString    gamemaster()             { return mGameMaster; }
    Character& gamemaster(QString n)    { mGameMaster = n; return *this; }
    QString    genre()                  { return mGenre; }
    Character& genre(QString n)         { mGenre = n; return *this; }
    QString    hairColor()              { return mHairColor; }
    Character& hairColor(QString n)     { mHairColor = n; return *this; }
    QString    playerName()             { return mPlayerName; }
    Character& playerName(QString n)    { mPlayerName = n; return *this; }
    Points     xp()                     { return mXP; }
    Character& xp(Points n)             { mXP = n; return *this; }

    Characteristic& STR()  { return mSTR; }
    Characteristic& DEX()  { return mDEX; }
    Characteristic& CON()  { return mCON; }
    Characteristic& INT()  { return mINT; }
    Characteristic& EGO()  { return mEGO; }
    Characteristic& PRE()  { return mPRE; }
    Characteristic& OCV()  { return mOCV; }
    Characteristic& DCV()  { return mDCV; }
    Characteristic& OMCV() { return mOMCV; }
    Characteristic& DMCV() { return mDMCV; }
    Characteristic& SPD()  { return mSPD; }
    Characteristic& PD()   { return mPD; }
    Characteristic& ED()   { return mED; }
    Characteristic& REC()  { return mREC; }
    Characteristic& END()  { return mEND; }
    Characteristic& BODY() { return mBODY; }
    Characteristic& STUN() { return mSTUN; }

    QString    height()          { return mHeight; }
    Character& height(QString h) { mHeight = h; return *this; }
    QString    weight()          { return mWeight; }
    Character& weight(QString w) { mWeight = w; return *this; }

    int& tempPD()   { return mTempPD; }
    int& tempED()   { return mTempED; }
    int& rPD()      { return mRPD; }
    int& rED()      { return mRED; }
    int& temprPD()  { return mTemprPD; }
    int& temprED()  { return mTemprED; }
    int& FD()       { return mFD; }
    int& MD()       { return mMD; }
    int& PowD()     { return mPowD; }
    int& running()  { return mRunning; }
    int& leaping()  { return mLeaping; }
    int& swimming() { return mSwimming; }

    Characteristic& characteristic(int x) {
        switch (x) {
        case 0:  return mSTR;
        case 1:  return mDEX;
        case 2:  return mCON;
        case 3:  return mINT;
        case 4:  return mEGO;
        case 5:  return mPRE; // NOLINT
        case 6:  return mOCV; // NOLINT
        case 7:  return mDCV; // NOLINT
        case 8:  return mOMCV; // NOLINT
        case 9:  return mDMCV; // NOLINT
        case 10: return mSPD; // NOLINT
        case 11: return mPD; // NOLINT
        case 12: return mED; // NOLINT
        case 13: return mREC; // NOLINT
        case 14: return mEND; // NOLINT
        case 15: return mBODY; // NOLINT
        default: return mSTUN; // NOLINT
        }
    }

    QList<shared_ptr<Complication>>&      complications()        { return mComplications; }
    QList<shared_ptr<SkillTalentOrPerk>>& skillsTalentsOrPerks() { return mSkillsTalentsOrPerks; }
    QList<shared_ptr<Power>>&             powersOrEquipment()    { return mPowers; }
    QString&                              image()                { return mImage; }
    qulonglong&                           imageDate()            { return mImageDate; }
    QByteArray&                           imageData()            { return mImageData; }
    QString&                              notes()                { return mNotes; }

    void  clearEnhancers()     { mHasJackOfAllTrades = mHasLinguist = mHasScholar = mHasScientist = mHasTraveler = mHasWellConnected = false; }
    bool& hasJackOfAllTrades() { return mHasJackOfAllTrades; }
    bool& hasLinguist()        { return mHasLinguist; }
    bool& hasScholar()         { return mHasScholar; }
    bool& hasScientist()       { return mHasScientist; }
    bool& hasTakesNoSTUN()     { return mHasTakesNoSTUN; }
    bool& hasTraveler()        { return mHasTraveler; }
    bool& hasWellConnected()   { return mHasWellConnected; }

    QJsonDocument copy(Option&);
    void          erase();
#ifdef __EMSCRIPTEN__
    bool          load(Option&, const QByteArray&);
#else
    bool          load(Option&, QString);
#endif
    void          paste(Option&, QJsonDocument&);
    bool          store(Option&, QString);

private:
    Characteristic mSTR;
    Characteristic mDEX;
    Characteristic mCON;
    Characteristic mINT;
    Characteristic mEGO;
    Characteristic mPRE;
    Characteristic mOCV;
    Characteristic mDCV;
    Characteristic mOMCV;
    Characteristic mDMCV;
    Characteristic mSPD;
    Characteristic mPD;
    Characteristic mED;
    Characteristic mREC;
    Characteristic mEND;
    Characteristic mBODY;
    Characteristic mSTUN;

    QString mHeight;
    QString mWeight;

    int mFD       = 0;
    int mMD       = 0;
    int mPowD     = 0;
    int mRPD      = 0;
    int mRED      = 0;
    int mTemprPD  = 0;
    int mTemprED  = 0;
    int mTempPD   = 0;
    int mTempED   = 0;
    int mRunning  = 12; // NOLINT
    int mLeaping  = 4;
    int mSwimming = 4;

    bool mHasJackOfAllTrades = false;
    bool mHasLinguist        = false;
    bool mHasScholar         = false;
    bool mHasScientist       = false;
    bool mHasTraveler        = false;
    bool mHasWellConnected   = false;
    bool mHasTakesNoSTUN     = false;

    QList<shared_ptr<Complication>>      mComplications;
    QList<shared_ptr<SkillTalentOrPerk>> mSkillsTalentsOrPerks;
    QList<shared_ptr<Power>>             mPowers;

    QList<std::tuple<QString, Characteristic*>> statPairs {
        { "STR", &mSTR }, { "DEX", &mDEX }, { "CON", &mCON }, { "INT", &mINT }, { "EGO", &mEGO },
        { "PRE", &mPRE }, { "OCV", &mOCV }, { "OMCV", &mOMCV }, { "DCV", &mDCV }, { "DMCV", &mDMCV },
        { "SPD", &mSPD }, { "PD", &mPD }, { "ED", &mED }, { "REC", &mREC }, { "END", &mEND },
        { "BODY",&mBODY }, { "STUN", &mSTUN }
    };

    QString mAlternateIds  = "";
    QString mCampaignName  = "";
    QString mCharacterName = "";
    QString mEyeColor      = "";
    QString mGameMaster    = "";
    QString mGenre         = "";
    QString mHairColor     = "";
    QString mNotes         = "";
    QString mPlayerName    = "";
    Points  mXP            = 0_cp;

    QString    mImage     = "";
    qulonglong mImageDate = 0;
    QByteArray mImageData;

    void          fromJson(Option&, QJsonDocument& doc);
    QJsonDocument toJson(Option&);
};

#endif // CHARACTER_H
