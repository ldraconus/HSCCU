#include "character.h"

#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

Character::Character()
    : _STR(Characteristic(10, 1_cp, 20))      // NOLINT
    , _DEX(Characteristic(10, 2_cp, 20))      // NOLINT
    , _CON(Characteristic(10, 1_cp, 20))      // NOLINT
    , _INT(Characteristic(10, 1_cp, 20))      // NOLINT
    , _EGO(Characteristic(10, 1_cp, 20))      // NOLINT
    , _PRE(Characteristic(10, 1_cp, 20))      // NOLINT
    , _OCV(Characteristic(3, 5_cp, 8))        // NOLINT
    , _DCV(Characteristic(3, 5_cp, 8))        // NOLINT
    , _OMCV(Characteristic(3, 3_cp, 8))       // NOLINT
    , _DMCV(Characteristic(3, 3_cp, 8))       // NOLINT
    , _SPD(Characteristic(2, 10_cp, 4))       // NOLINT
    , _PD(Characteristic(2, 1_cp, 8))         // NOLINT
    , _ED(Characteristic(2, 1_cp, 8))         // NOLINT
    , _REC(Characteristic(4, 1_cp, 10))       // NOLINT
    , _END(Characteristic(20, 1_cp, 50, 5))   // NOLINT
    , _BODY(Characteristic(10, 1_cp, 20))     // NOLINT
    , _STUN(Characteristic(20, 1_cp, 50, 2))  // NOLINT
    , _height("2 m")
    , _weight("100kg")
    , _xp(0) { }

Character::Character(const Character& c)
    : _STR(c._STR), _DEX(c._DEX), _CON(c._CON)
    , _INT(c._INT), _EGO(c._EGO), _PRE(c._PRE)
    , _OCV(c._OCV), _DCV(c._DCV), _OMCV(c._OMCV), _DMCV(c._DMCV), _SPD(c._SPD)
    , _PD(c._PD), _ED(c._ED), _REC(c._REC), _END(c._END), _BODY(c._BODY), _STUN(c._STUN)
    , _height(c._height), _weight(c._weight), _rPD(c._rPD), _rED(c._rED)
    , _complications(c._complications), _skillsTalentsOrPerks(c._skillsTalentsOrPerks), _powers(c._powers)
    , _alternateIds(c._alternateIds), _campaignName(c._campaignName), _characterName(c._characterName)
    , _eyeColor(c._eyeColor), _gamemaster(c._gamemaster), _genre(c._genre), _hairColor(c._hairColor)
    , _playerName(c._playerName), _xp(c._xp) {
}

Character& Character::operator=(const Character& c) {
    if (&c != this) {
        _height               = c._height;
        _weight               = c._weight;
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
        _powers               = c._powers;
        _skillsTalentsOrPerks = c._skillsTalentsOrPerks;
        _complications        = c._complications;

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
    _height               = c._height;
    _weight               = c._weight;
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
    _powers               = c._powers;
    _skillsTalentsOrPerks = c._skillsTalentsOrPerks;
    _complications        = c._complications;

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
    _xp            = 0_cp;
    _notes         = "";
    QList<Characteristic*> characteristics {
        &_STR, &_DEX, &_CON, &_INT, &_EGO, &_PRE, &_OCV, &_OMCV, &_DCV, &_DMCV, &_SPD, &_PD, &_ED, &_REC, &_END, &_BODY, &_STUN
    };
    for (auto [i, characteristic]: enumerate(characteristics)) {
        characteristic->base(characteristic->init());
        characteristic->primary(0);
        characteristic->secondary(0);
    }
    _height = "2m";
    _weight = "100kg";
    _complications.clear();
    _skillsTalentsOrPerks.clear();
    _powers.clear();
    _image = "";
    _imageData.clear();
}

QJsonDocument Character::copy(Option& opt) {
    return toJson(opt);
}

#ifndef ISHSC
void Character::fromJson(Option& opt, QJsonDocument& doc) {
    const QJsonObject& top = doc.object();
    opt = Option(top);
#else
void Character::fromJson(Option&, QJsonDocument& doc) {
    const QJsonObject& top = doc.object();
#endif
    _alternateIds  = top["alternateIds"].toString("");
    _campaignName  = top["campaignName"].toString("");
    _characterName = top["characterName"].toString("");
    _eyeColor      = top["eyeColor"].toString("");
    _gamemaster    = top["gamemaster"].toString("");
    _genre         = top["genre"].toString("");
    _hairColor     = top["hairColor"].toString("");
    _notes         = top["notes"].toString("");
    _playerName    = top["playerName"].toString("");
    _xp            = Points(top["xp"].toInt(0));
    _height        = top["height"].toString("2m");
    _weight        = top["weight"].toString("100kg");

    const QJsonObject& objCharacteristics = top["characteristics"].toObject();
    for (const auto& c: statPairs) *std::get<Characteristic*>(c) = Characteristic(objCharacteristics[std::get<QString>(c)].toObject());

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

    QJsonObject image = top["image"].toObject();
    _image = image["filename"].toString();
    qint64 now = time(0L);
    _imageDate = image["datestamp"].toInteger(now);
    _imageData = QByteArray::fromHex(image["data"].toString().toUtf8());
}

#ifdef __wasm__
bool Character::load(Option& opt, const QByteArray& data) {
    QJsonDocument json = QJsonDocument::fromJson(data);
#else
bool Character::load(Option& opt, QString filename) {
#ifndef ISHSC
    QFile file(filename + ".hsccu");
#else
    QFile file(filename);
#endif
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

    QByteArray data(file.readAll());
    file.close();

    QString jsonStr(data);
    QJsonDocument json = QJsonDocument::fromJson(jsonStr.toUtf8());
#endif

    erase();
    fromJson(opt, json);

    return true;
}

void Character::paste(Option& opt, QJsonDocument& doc) {
    erase();

    fromJson(opt, doc);
}

bool Character::store(Option& opt, QString filename) {
    QJsonDocument json = toJson(opt);
#ifdef __wasm__
    QString data = json.toJson();
    QFileDialog::saveFileContent(data.toUtf8(), filename + ".hsccu");
#else
    QFile file(filename + ".hsccu");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) return false;
    QTextStream out(&file);
    out << json.toJson();
    file.close();
#endif
    return true;
}

QJsonDocument Character::toJson(Option& opt) {
    QJsonObject top;
    opt.toJson(top);
    top.insert("alternateIds",  _alternateIds);
    top.insert("campaignName",  _campaignName);
    top.insert("characterName", _characterName);
    top.insert("eyeColor",      _eyeColor);
    top.insert("gamemaster",    _gamemaster);
    top.insert("genre",         _genre);
    top.insert("hairColor",     _hairColor);
    top.insert("playerName",    _playerName);
    top.insert("xp",            qlonglong(_xp.points));
    top.insert("notes",         _notes);
    top.insert("height",        _height);
    top.insert("weight",        _weight);

    QJsonObject objCharacteristics;
    for (const auto& c: statPairs) objCharacteristics.insert(std::get<QString>(c), std::get<Characteristic*>(c)->toJson());
    top.insert("characteristics", objCharacteristics);

    QJsonArray complications;
    for (int i = 0; i < _complications.count(); ++i) {
        if (_complications[i] == nullptr) continue;

        complications.append(_complications[i]->toJson());
    }
    top.insert("complications", complications);

    QJsonArray powers;
    for (int i = 0; i < _powers.count(); ++i) {
        if (_powers[i] == nullptr) continue;

        powers.append(_powers[i]->toJson());
    }
    top.insert("powers", powers);

    QJsonArray skillsTalentsOrPerks;
    for (int i = 0; i < _skillsTalentsOrPerks.count(); ++i) {
        if (_skillsTalentsOrPerks[i] == nullptr) continue;

        skillsTalentsOrPerks.append(_skillsTalentsOrPerks[i]->toJson());
    }
    top.insert("skillsTalentsOrPerks", skillsTalentsOrPerks);

    QJsonObject image;
    image["filename"] = _image;
    image["datestamp"] = (qint64) _imageDate;
    QString x = _imageData.toHex().toStdString().c_str();
    image["data"] = x;
    top.insert("image", image);

    QJsonDocument json;
    json.setObject(top);

    return json;
}
