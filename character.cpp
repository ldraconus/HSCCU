#include "character.h"

#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

Character::Character()
    : mSTR(Characteristic(10, 1_cp, 20))      // NOLINT
    , mDEX(Characteristic(10, 2_cp, 20))      // NOLINT
    , mCON(Characteristic(10, 1_cp, 20))      // NOLINT
    , mINT(Characteristic(10, 1_cp, 20))      // NOLINT
    , mEGO(Characteristic(10, 1_cp, 20))      // NOLINT
    , mPRE(Characteristic(10, 1_cp, 20))      // NOLINT
    , mOCV(Characteristic(3, 5_cp, 8))        // NOLINT
    , mDCV(Characteristic(3, 5_cp, 8))        // NOLINT
    , mOMCV(Characteristic(3, 3_cp, 8))       // NOLINT
    , mDMCV(Characteristic(3, 3_cp, 8))       // NOLINT
    , mSPD(Characteristic(2, 10_cp, 4))       // NOLINT
    , mPD(Characteristic(2, 1_cp, 8))         // NOLINT
    , mED(Characteristic(2, 1_cp, 8))         // NOLINT
    , mREC(Characteristic(4, 1_cp, 10))       // NOLINT
    , mEND(Characteristic(20, 1_cp, 50, 5))   // NOLINT
    , mBODY(Characteristic(10, 1_cp, 20))     // NOLINT
    , mSTUN(Characteristic(20, 1_cp, 50, 2))  // NOLINT
    , mHeight("2 m")
    , mWeight("100kg")
    , mXP(0) { }

Character::Character(const Character& c)
    : mSTR(c.mSTR), mDEX(c.mDEX), mCON(c.mCON)
    , mINT(c.mINT), mEGO(c.mEGO), mPRE(c.mPRE)
    , mOCV(c.mOCV), mDCV(c.mDCV), mOMCV(c.mOMCV), mDMCV(c.mDMCV), mSPD(c.mSPD)
    , mPD(c.mPD), mED(c.mED), mREC(c.mREC), mEND(c.mEND), mBODY(c.mBODY), mSTUN(c.mSTUN)
    , mHeight(c.mHeight), mWeight(c.mWeight), mRPD(c.mRPD), mRED(c.mRED)
    , mComplications(c.mComplications), mSkillsTalentsOrPerks(c.mSkillsTalentsOrPerks), mPowers(c.mPowers)
    , mAlternateIds(c.mAlternateIds), mCampaignName(c.mCampaignName), mCharacterName(c.mCharacterName)
    , mEyeColor(c.mEyeColor), mGameMaster(c.mGameMaster), mGenre(c.mGenre), mHairColor(c.mHairColor)
    , mPlayerName(c.mPlayerName), mXP(c.mXP) {
}

Character& Character::operator=(const Character& c) {
    if (&c != this) {
        mHeight               = c.mHeight;
        mWeight               = c.mWeight;
        mAlternateIds         = c.mAlternateIds;
        mCampaignName         = c.mCampaignName;
        mCharacterName        = c.mCharacterName;
        mComplications        = c.mComplications;
        mEyeColor             = c.mEyeColor;
        mGameMaster           = c.mGameMaster;
        mGenre                = c.mGenre;
        mHairColor            = c.mHairColor;
        mPlayerName           = c.mPlayerName;
        mSkillsTalentsOrPerks = c.mSkillsTalentsOrPerks;
        mXP                   = c.mXP;
        mPowers               = c.mPowers;
        mSkillsTalentsOrPerks = c.mSkillsTalentsOrPerks;
        mComplications        = c.mComplications;

        mSTR  = c.mSTR;
        mDEX  = c.mDEX;
        mCON  = c.mCON;
        mINT  = c.mINT;
        mEGO  = c.mEGO;
        mPRE  = c.mPRE;
        mOCV  = c.mOCV;
        mDCV  = c.mDCV;
        mOMCV = c.mOMCV;
        mDMCV = c.mDMCV;
        mSPD  = c.mSPD;
        mPD   = c.mPD;
        mED   = c.mED;
        mREC  = c.mREC;
        mEND  = c.mEND;
        mBODY = c.mBODY;
        mSTUN = c.mSTUN;
    }
    return *this;
}

Character& Character::operator=(Character&& c) {
    mHeight               = c.mHeight;
    mWeight               = c.mWeight;
    mAlternateIds         = c.mAlternateIds;
    mCampaignName         = c.mCampaignName;
    mCharacterName        = c.mCharacterName;
    mComplications        = c.mComplications;
    mEyeColor             = c.mEyeColor;
    mGameMaster           = c.mGameMaster;
    mGenre                = c.mGenre;
    mHairColor            = c.mHairColor;
    mPlayerName           = c.mPlayerName;
    mSkillsTalentsOrPerks = c.mSkillsTalentsOrPerks;
    mXP                   = c.mXP;
    mPowers               = c.mPowers;
    mSkillsTalentsOrPerks = c.mSkillsTalentsOrPerks;
    mComplications        = c.mComplications;

    mSTR  = c.mSTR;
    mDEX  = c.mDEX;
    mCON  = c.mCON;
    mINT  = c.mINT;
    mEGO  = c.mEGO;
    mPRE  = c.mPRE;
    mOCV  = c.mOCV;
    mDCV  = c.mDCV;
    mOMCV = c.mOMCV;
    mDMCV = c.mDMCV;
    mSPD  = c.mSPD;
    mPD   = c.mPD;
    mED   = c.mED;
    mREC  = c.mREC;
    mEND  = c.mEND;
    mBODY = c.mBODY;
    mSTUN = c.mSTUN;

    return *this;
}

void Character::erase() {
    mAlternateIds  = "";
    mCampaignName  = "";
    mCharacterName = "";
    mEyeColor      = "";
    mGameMaster    = "";
    mGenre         = "";
    mHairColor     = "";
    mPlayerName    = "";
    mXP            = 0_cp;
    mNotes         = "";
    QList<Characteristic*> characteristics {
        &mSTR, &mDEX, &mCON, &mINT, &mEGO, &mPRE, &mOCV, &mOMCV, &mDCV, &mDMCV, &mSPD, &mPD, &mED, &mREC, &mEND, &mBODY, &mSTUN
    };
    for (auto [i, characteristic]: enumerate(std::as_const(characteristics))) { // NOLINT
        characteristic->base(characteristic->init());
        characteristic->primary(0);
        characteristic->secondary(0);
    }
    mHeight = "2m";
    mWeight = "100kg";
    mComplications.clear();
    mSkillsTalentsOrPerks.clear();
    mPowers.clear();
    mImage = "";
    mImageData.clear();
}

QJsonDocument Character::copy(Option& opt) {
    return toJson(opt);
}

static constexpr auto         AlternateIds = "alternateIds";
static constexpr auto         CampaignName = "campaignName";
static constexpr auto        CharacterName = "characterName";
static constexpr auto      Characteristics = "characteristics";
static constexpr auto        Complications = "complications";
static constexpr auto                 Data = "data";
static constexpr auto            DateStamp = "datestamp";
static constexpr auto             EyeColor = "eyeColor";
static constexpr auto             Filename = "filename";
static constexpr auto           GameMaster = "gamemaster";
static constexpr auto                Genre = "genre";
static constexpr auto            HairColor = "hairColor";
static constexpr auto               Height = "height";
static constexpr auto                Image = "image";
static constexpr auto                 Name = "name";
static constexpr auto                Notes = "notes";
static constexpr auto           PlayerName = "playerName";
static constexpr auto               Powers = "powers";
static constexpr auto SkillsTalentsOrPerks = "skillsTalentsOrPerks";
static constexpr auto               Weight = "hairColor";
static constexpr auto                   XP = "xp";

#ifndef ISHSC
void Character::fromJson(Option& opt, QJsonDocument& doc) {
    const QJsonObject& top = doc.object();
    opt = Option(top);
#else
void Character::fromJson(Option&, QJsonDocument& doc) {
    const QJsonObject& top = doc.object();
#endif
    mAlternateIds  = top[AlternateIds].toString("");
    mCampaignName  = top[CampaignName].toString("");
    mCharacterName = top[CharacterName].toString("");
    mEyeColor      = top[EyeColor].toString("");
    mGameMaster    = top[GameMaster].toString("");
    mGenre         = top[Genre].toString("");
    mHairColor     = top[HairColor].toString("");
    mHeight        = top[Height].toString("2m");
    mNotes         = top[Notes].toString("");
    mPlayerName    = top[PlayerName].toString("");
    mWeight        = top[Weight].toString("100kg");
    mXP            = Points(top[XP].toInt(0));

    const QJsonObject& objCharacteristics = top[Characteristics].toObject();
    for (int i = 0; i < statPairs.count(); ++i) {
        auto c = statPairs[i];
        *std::get<Characteristic*>(c) = Characteristic(objCharacteristics[std::get<QString>(c)].toObject());
    }

    QJsonArray complications = top[Complications].toArray();
    for (int i = 0; i < complications.count(); ++i) {
        auto complication = complications[i];
        QJsonObject obj = complication.toObject();
        mComplications.append(Complication::FromJson(obj[Name].toString(), obj));
    }

    QJsonArray powers = top[Powers].toArray();
    for (int i = 0; i < powers.count(); ++i) {
        auto power = powers[i];
        QJsonObject obj = power.toObject();
        mPowers.append(Power::FromJson(obj[Name].toString(), obj));
    }

    QJsonArray skillsTalentsOrPerks = top[SkillsTalentsOrPerks].toArray();
    for (int i = 0; i < skillsTalentsOrPerks.count(); ++i) {
        auto skillsTalentsOrPerk = skillsTalentsOrPerks[i];
        QJsonObject obj = skillsTalentsOrPerk.toObject();
        mSkillsTalentsOrPerks.append(SkillTalentOrPerk::FromJson(obj[Name].toString(), obj));
    }

    QJsonObject image = top[Image].toObject();
    mImage = image[Filename].toString();
    qint64 now = time(0L);
    mImageDate = image[DateStamp].toInteger(now);
    mImageData = QByteArray::fromHex(image[Data].toString().toUtf8());
}

#ifdef __wasm__
bool Character::load(Option& opt, const QByteArray& data) {
    QJsonDocument json = QJsonDocument::fromJson(data);
#else
bool Character::load(Option& opt, QUrl filename) {
    QFile file(filename.isLocalFile() ? filename.toLocalFile() : filename.toString());
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

#ifdef __wasm__
bool Character::store(Option& opt, QString filename) {
    QJsonDocument json = toJson(opt);
    QString data = json.toJson();
    QFileDialog::saveFileContent(data.toUtf8(), filename + ".hsccu");
#else
bool Character::store(Option& opt, QUrl filename) {
    QJsonDocument json = toJson(opt);
    QFile file(filename.isLocalFile() ? filename.toLocalFile() : filename.toString());
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
    top.insert(AlternateIds,  mAlternateIds);
    top.insert(CampaignName,  mCampaignName);
    top.insert(CharacterName, mCharacterName);
    top.insert(EyeColor,      mEyeColor);
    top.insert(GameMaster,    mGameMaster);
    top.insert(Genre,         mGenre);
    top.insert(HairColor,     mHairColor);
    top.insert(Height,        mHeight);
    top.insert(Notes,         mNotes);
    top.insert(PlayerName,    mPlayerName);
    top.insert(Weight,        mWeight);
    top.insert(XP,            qlonglong(mXP.points));

    QJsonObject objCharacteristics;
    for (int i = 0; i < statPairs.count(); ++i) {
        auto c = statPairs[i];
        objCharacteristics.insert(std::get<QString>(c), std::get<Characteristic*>(c)->toJson());
    }
    top.insert(Characteristics, objCharacteristics);

    QJsonArray complications;
    for (int i = 0; i < mComplications.count(); ++i) {
        if (mComplications[i] == nullptr) continue;

        complications.append(mComplications[i]->toJson());
    }
    top.insert(Complications, complications);

    QJsonArray powers;
    for (int i = 0; i < mPowers.count(); ++i) {
        if (mPowers[i] == nullptr) continue;

        powers.append(mPowers[i]->toJson());
    }
    top.insert(Powers, powers);

    QJsonArray skillsTalentsOrPerks;
    for (int i = 0; i < mSkillsTalentsOrPerks.count(); ++i) {
        if (mSkillsTalentsOrPerks[i] == nullptr) continue;

        skillsTalentsOrPerks.append(mSkillsTalentsOrPerks[i]->toJson());
    }
    top.insert(SkillsTalentsOrPerks, skillsTalentsOrPerks);

    QJsonObject image;
    image[Filename] = mImage.isLocalFile() ? mImage.toLocalFile() : mImage.toString();
    image[DateStamp] = (qint64) mImageDate;
    QString x = mImageData.toHex().toStdString().c_str();
    image[Data] = x;
    top.insert(Image, image);

    QJsonDocument json;
    json.setObject(top);

    return json;
}
