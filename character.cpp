#include "character.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

Character::Character()
    : _STR(Characteristic(10, 1))
    , _DEX(Characteristic(10, 2))
    , _CON(Characteristic(10, 1))
    , _INT(Characteristic(10, 1))
    , _EGO(Characteristic(10, 1))
    , _PRE(Characteristic(10, 1))
    , _OCV(Characteristic(3, 5))
    , _DCV(Characteristic(3, 5))
    , _OMCV(Characteristic(3, 3))
    , _DMCV(Characteristic(3, 3))
    , _SPD(Characteristic(2, 10))
    , _PD(Characteristic(2, 1))
    , _ED(Characteristic(2, 1))
    , _REC(Characteristic(4, 1))
    , _END(Characteristic(20, 1, 5))
    , _BODY(Characteristic(10, 1))
    , _STUN(Characteristic(20, 1, 2)) { }

Character::Character(const Character& c)
    : _STR(c._STR), _DEX(c._DEX), _CON(c._CON)
    , _INT(c._INT), _EGO(c._EGO), _PRE(c._PRE)
    , _OCV(c._OCV), _DCV(c._DCV), _OMCV(c._OMCV), _DMCV(c._DMCV), _SPD(c._SPD)
    , _PD(c._PD), _ED(c._ED), _REC(c._REC), _END(c._END), _BODY(c._BODY), _STUN(c._STUN) {
    _alternateIds  = c._alternateIds;
    _campaignName  = c._campaignName;
    _characterName = c._characterName;
    _eyeColor      = c._eyeColor;
    _gamemaster    = c._gamemaster;
    _genre         = c._genre;
    _hairColor     = c._hairColor;
    _playerName    = c._playerName;
    _xp            = c._xp;
}

Character& Character::operator=(const Character& c) {
    if (&c != this) {
        _alternateIds         = c._alternateIds;
        _campaignName         = c._campaignName;
        _characterName        = c._characterName;
        _complications        = c._complications;
        _eyeColor             = c._eyeColor;
        _gamemaster           = c._gamemaster;
        _genre                = c._genre;
        _hairColor            = c._hairColor;
        _playerName           = c._playerName;
        _skillsTalentsOrPerks = c._skillsTalentsOrPerks;
        _xp                   = c._xp;

        _STR  = c._STR;
        _DEX  = c._DEX;
        _CON  = c._CON;
        _INT  = c._INT;
        _EGO  = c._EGO;
        _PRE  = c._PRE;
        _OCV  = c._OCV;
        _DCV  = c._DCV;
        _OMCV = c._OMCV;
        _DMCV = c._DMCV;
        _SPD  = c._SPD;
        _PD   = c._PD;
        _ED   = c._ED;
        _REC  = c._REC;
        _END  = c._END;
        _BODY = c._BODY;
        _STUN = c._STUN;
    }
    return *this;
}

Character& Character::operator=(Character&& c) {
    _alternateIds         = c._alternateIds;
    _campaignName         = c._campaignName;
    _characterName        = c._characterName;
    _complications        = c._complications;
    _eyeColor             = c._eyeColor;
    _gamemaster           = c._gamemaster;
    _genre                = c._genre;
    _hairColor            = c._hairColor;
    _playerName           = c._playerName;
    _skillsTalentsOrPerks = c._skillsTalentsOrPerks;
    _xp                   = c._xp;

    _STR  = c._STR;
    _DEX  = c._DEX;
    _CON  = c._CON;
    _INT  = c._INT;
    _EGO  = c._EGO;
    _PRE  = c._PRE;
    _OCV  = c._OCV;
    _DCV  = c._DCV;
    _OMCV = c._OMCV;
    _DMCV = c._DMCV;
    _SPD  = c._SPD;
    _PD   = c._PD;
    _ED   = c._ED;
    _REC  = c._REC;
    _END  = c._END;
    _BODY = c._BODY;
    _STUN = c._STUN;

    return *this;
}

void Character::erase() {
    _alternateIds  = "";
    _campaignName  = "";
    _characterName = "";
    _eyeColor      = "";
    _gamemaster    = "";
    _genre         = "";
    _hairColor     = "";
    _playerName    = "";
    _xp            = 0;
    _STR.base(_STR.init());
    _DEX.base(_DEX.init());
    _CON.base(_CON.init());
    _INT.base(_INT.init());
    _EGO.base(_EGO.init());
    _PRE.base(_PRE.init());
    _OCV.base(_OCV.init());
    _OMCV.base(_OMCV.init());
    _DCV.base(_DCV.init());
    _DMCV.base(_DMCV.init());
    _SPD.base(_SPD.init());
    _PD.base(_PD.init());
    _ED.base(_ED.init());
    _REC.base(_REC.init());
    _END.base(_END.init());
    _BODY.base(_BODY.init());
    _STUN.base(_STUN.init());
    for (const auto& x: _complications) delete x;
    _complications.clear();
    for (const auto& x: _skillsTalentsOrPerks) delete x;
    _skillsTalentsOrPerks.clear();
}

bool Character::load(QString filename) {
    QFile file(filename + ".hsccu");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

    QByteArray data(file.readAll());
    QString jsonStr(data);
    file.close();
    QJsonDocument json = QJsonDocument::fromJson(jsonStr.toUtf8());

    erase();

    const QJsonObject& top = json.object();
    _alternateIds  = top["alternateIds"].toString("");
    _campaignName  = top["campaignName"].toString("");
    _characterName = top["characterName"].toString("");
    _eyeColor      = top["eyeColor"].toString("");
    _gamemaster    = top["gamemaster"].toString("");
    _genre         = top["genre"].toString("");
    _hairColor     = top["hairColor"].toString("");
    _playerName    = top["playerName"].toString("");
    _xp            = top["xp"].toInt(0);

    QJsonObject characteristics = top["characteristics"].toObject();
    _STR  = Characteristic(characteristics["STR"].toObject());
    _DEX  = Characteristic(characteristics["DEX"].toObject());
    _CON  = Characteristic(characteristics["CON"].toObject());
    _INT  = Characteristic(characteristics["INT"].toObject());
    _EGO  = Characteristic(characteristics["EGO"].toObject());
    _PRE  = Characteristic(characteristics["PRE"].toObject());
    _OCV  = Characteristic(characteristics["OCV"].toObject());
    _DCV  = Characteristic(characteristics["DCV"].toObject());
    _OMCV = Characteristic(characteristics["OMCV"].toObject());
    _DMCV = Characteristic(characteristics["DMCV"].toObject());
    _SPD  = Characteristic(characteristics["SPD"].toObject());
    _PD   = Characteristic(characteristics["PD"].toObject());
    _ED   = Characteristic(characteristics["ED"].toObject());
    _REC  = Characteristic(characteristics["REC"].toObject());
    _END  = Characteristic(characteristics["END"].toObject());
    _BODY = Characteristic(characteristics["BODY"].toObject());
    _STUN = Characteristic(characteristics["STUN"].toObject());

    QJsonArray complications = top["complications"].toArray();
    for (const auto& complication: complications) {
        QJsonObject obj = complication.toObject();
        _complications.append(Complication::FromJson(obj["name"].toString(), obj));
    }

    QJsonArray powers = top["powers"].toArray();
    for (const auto& power: powers) {
        QJsonObject obj = power.toObject();
        _powers.append(Power::FromJson(obj["name"].toString(), obj));
    }

    QJsonArray skillsTalentsOrPerks = top["skillsTalentsOrPerks"].toArray();
    for (const auto& skillsTalentsOrPerk: skillsTalentsOrPerks) {
        QJsonObject obj = skillsTalentsOrPerk.toObject();
        _skillsTalentsOrPerks.append(SkillTalentOrPerk::FromJson(obj["name"].toString(), obj));
    }

    return true;
}

bool Character::store(QString filename) {
    QJsonObject top;
    top.insert("alternateIds",  _alternateIds);
    top.insert("campaignName",  _campaignName);
    top.insert("characterName", _characterName);
    top.insert("eyeColor",      _eyeColor);
    top.insert("gamemaster",    _gamemaster);
    top.insert("genre",         _genre);
    top.insert("hairColor",     _hairColor);
    top.insert("playerName",    _playerName);
    top.insert("xp", _xp);

    QJsonObject characteristics;
    characteristics.insert("STR",  _STR.toJson());
    characteristics.insert("DEX",  _DEX.toJson());
    characteristics.insert("CON",  _CON.toJson());
    characteristics.insert("INT",  _INT.toJson());
    characteristics.insert("EGO",  _EGO.toJson());
    characteristics.insert("PRE",  _PRE.toJson());
    characteristics.insert("OCV",  _OCV.toJson());
    characteristics.insert("DCV",  _DCV.toJson());
    characteristics.insert("OMCV", _OMCV.toJson());
    characteristics.insert("DMCV", _DMCV.toJson());
    characteristics.insert("SPD",  _SPD.toJson());
    characteristics.insert("PD",   _PD.toJson());
    characteristics.insert("ED",   _ED.toJson());
    characteristics.insert("REC",  _REC.toJson());
    characteristics.insert("END",  _END.toJson());
    characteristics.insert("BODY", _BODY.toJson());
    characteristics.insert("STUN", _STUN.toJson());
    top.insert("characteristics", characteristics);

    QJsonArray complications;
    for (int i = 0; i < _complications.count(); ++i) complications.append(_complications[i]->toJson());
    top.insert("complications", complications);

    QJsonArray powers;
    for (int i = 0; i < _powers.count(); ++i) powers.append(_powers[i]->toJson());
    top.insert("powers", powers);

    QJsonArray skillsTalentsOrPerks;
    for (int i = 0; i < _skillsTalentsOrPerks.count(); ++i) skillsTalentsOrPerks.append(_skillsTalentsOrPerks[i]->toJson());
    top.insert("skillsTalentsOrPerks", skillsTalentsOrPerks);

    QJsonDocument json;
    json.setObject(top);
    QFile file(filename + ".hsccu");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) return false;
    QTextStream out(&file);
    out << json.toJson();
    file.close();
    return true;
}
