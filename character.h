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

class Character
{
public:
    Character();
    Character(const Character& c);
    Character(Character&& c): Character(c) { }

    Character& operator=(const Character& c);
    Character& operator=(Character&& c);

    QString    alternateIds()           { return _alternateIds; }
    Character& alternateIds(QString a)  { _alternateIds = a; return *this; }
    QString    campaignName()           { return _campaignName; }
    Character& campaignName(QString n)  { _campaignName = n; return *this; }
    QString    characterName()          { return _characterName; }
    Character& characterName(QString n) { _characterName = n; return *this; }
    QString    eyeColor()               { return _eyeColor; }
    Character& eyeColor(QString n)      { _eyeColor = n; return *this; }
    QString    gamemaster()             { return _gamemaster; }
    Character& gamemaster(QString n)    { _gamemaster = n; return *this; }
    QString    genre()                  { return _genre; }
    Character& genre(QString n)         { _genre = n; return *this; }
    QString    hairColor()              { return _hairColor; }
    Character& hairColor(QString n)     { _hairColor = n; return *this; }
    QString    playerName()             { return _playerName; }
    Character& playerName(QString n)    { _playerName = n; return *this; }
    Points     xp()                     { return _xp; }
    Character& xp(Points n)             { _xp = n; return *this; }

    Characteristic& STR()  { return _STR; }
    Characteristic& DEX()  { return _DEX; }
    Characteristic& CON()  { return _CON; }
    Characteristic& INT()  { return _INT; }
    Characteristic& EGO()  { return _EGO; }
    Characteristic& PRE()  { return _PRE; }
    Characteristic& OCV()  { return _OCV; }
    Characteristic& DCV()  { return _DCV; }
    Characteristic& OMCV() { return _OMCV; }
    Characteristic& DMCV() { return _DMCV; }
    Characteristic& SPD()  { return _SPD; }
    Characteristic& PD()   { return _PD; }
    Characteristic& ED()   { return _ED; }
    Characteristic& REC()  { return _REC; }
    Characteristic& END()  { return _END; }
    Characteristic& BODY() { return _BODY; }
    Characteristic& STUN() { return _STUN; }

    QString    height()          { return _height; }
    Character& height(QString h) { _height = h; return *this; }
    QString    weight()          { return _weight; }
    Character& weight(QString w) { _weight = w; return *this; }

    int& tempPD()   { return _tempPD; }
    int& tempED()   { return _tempED; }
    int& rPD()      { return _rPD; }
    int& rED()      { return _rED; }
    int& temprPD()  { return _temprPD; }
    int& temprED()  { return _temprED; }
    int& FD()       { return _FD; }
    int& MD()       { return _MD; }
    int& PowD()     { return _PowD; }
    int& running()  { return _running; }
    int& leaping()  { return _leaping; }
    int& swimming() { return _swimming; }

    Characteristic& characteristic(int x) {
        switch (x) {
        case 0:  return _STR;
        case 1:  return _DEX;
        case 2:  return _CON;
        case 3:  return _INT;
        case 4:  return _EGO;
        case 5:  return _PRE;
        case 6:  return _OCV;
        case 7:  return _DCV;
        case 8:  return _OMCV;
        case 9:  return _DMCV;
        case 10: return _SPD;
        case 11: return _PD;
        case 12: return _ED;
        case 13: return _REC;
        case 14: return _END;
        case 15: return _BODY;
        default: return _STUN;
        }
    }

    QList<shared_ptr<Complication>>&      complications()        { return _complications; }
    QList<shared_ptr<SkillTalentOrPerk>>& skillsTalentsOrPerks() { return _skillsTalentsOrPerks; }
    QList<shared_ptr<Power>>&             powersOrEquipment()    { return _powers; }
    QString&                              image()                { return _image; }
    qulonglong&                           imageDate()            { return _imageDate; }
    QByteArray&                           imageData()            { return _imageData; }
    QString&                              notes()                { return _notes; }

    void  clearEnhancers()     { _hasJackOfAllTrades = _hasLinguist = _hasScholar = _hasScientist = _hasTraveler = _hasWellConnected = false; }
    bool& hasJackOfAllTrades() { return _hasJackOfAllTrades; }
    bool& hasLinguist()        { return _hasLinguist; }
    bool& hasScholar()         { return _hasScholar; }
    bool& hasScientist()       { return _hasScientist; }
    bool& hasTakesNoSTUN()     { return _hasTakesNoSTUN; }
    bool& hasTraveler()        { return _hasTraveler; }
    bool& hasWellConnected()   { return _hasWellConnected; }

    QJsonDocument copy(Option&);
    void          erase();
    bool          load(Option&, QString);
    void          paste(Option&, QJsonDocument&);
    bool          store(Option&, QString);

private:
    Characteristic _STR;
    Characteristic _DEX;
    Characteristic _CON;
    Characteristic _INT;
    Characteristic _EGO;
    Characteristic _PRE;
    Characteristic _OCV;
    Characteristic _DCV;
    Characteristic _OMCV;
    Characteristic _DMCV;
    Characteristic _SPD;
    Characteristic _PD;
    Characteristic _ED;
    Characteristic _REC;
    Characteristic _END;
    Characteristic _BODY;
    Characteristic _STUN;

    QString _height;
    QString _weight;

    int _FD       = 0;
    int _MD       = 0;
    int _PowD     = 0;
    int _rPD      = 0;
    int _rED      = 0;
    int _temprPD  = 0;
    int _temprED  = 0;
    int _tempPD   = 0;
    int _tempED   = 0;
    int _running  = 12;
    int _leaping  = 4;
    int _swimming = 4;

    bool _hasJackOfAllTrades = false;
    bool _hasLinguist        = false;
    bool _hasScholar         = false;
    bool _hasScientist       = false;
    bool _hasTraveler        = false;
    bool _hasWellConnected   = false;
    bool _hasTakesNoSTUN     = false;

    QList<shared_ptr<Complication>>      _complications;
    QList<shared_ptr<SkillTalentOrPerk>> _skillsTalentsOrPerks;
    QList<shared_ptr<Power>>             _powers;

    QString _alternateIds  = "";
    QString _campaignName  = "";
    QString _characterName = "";
    QString _eyeColor      = "";
    QString _gamemaster    = "";
    QString _genre         = "";
    QString _hairColor     = "";
    QString _notes         = "";
    QString _playerName    = "";
    Points  _xp            = 0_cp;

    QString    _image     = "";
    qulonglong _imageDate = 0;
    QByteArray _imageData;

    void          fromJson(Option&, QJsonDocument& doc);
    QJsonDocument toJson(Option&);
};

#endif // CHARACTER_H
