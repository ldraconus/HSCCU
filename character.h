#ifndef CHARACTER_H
#define CHARACTER_H

#include <QObject>

class Character
{
public:
    Character();

    QString    alternateIds()           { return _alternateIds; }
    Character& alternateIds(QString a)  { _alternateIds = a; return *this; }
    QString    characterName()          { return _characterName; }
    Character& characterName(QString n) { _characterName = n; return *this; }
    QString    playerName()             { return _playerName; }
    Character& playerName(QString n)    { _playerName = n; return *this; }
    int        xp()                     { return _xp; }
    Character& xp(int n)                { _xp = n; return *this; }

private:
    QString _alternateIds  = "";
    QString _characterName = "";
    QString _playerName    = "";
    int     _xp;
};

#endif // CHARACTER_H
