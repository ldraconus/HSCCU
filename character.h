#ifndef CHARACTER_H
#define CHARACTER_H

#include "characteristic.h"
#include "complication.h"

#include <QObject>

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
    int        xp()                     { return _xp; }
    Character& xp(int n)                { _xp = n; return *this; }

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

    QList<Complication*>& complications() { return _complications; }

    void erase();
    bool load(QString);
    bool store(QString);

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

    QList<Complication*> _complications;

    QString _alternateIds  = "";
    QString _campaignName  = "";
    QString _characterName = "";
    QString _eyeColor      = "";
    QString _gamemaster    = "";
    QString _genre         = "";
    QString _hairColor     = "";
    QString _playerName    = "";
    int     _xp = 0;
};

#endif // CHARACTER_H
