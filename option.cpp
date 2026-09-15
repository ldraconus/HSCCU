#include "option.h"

#include <QSettings>

static constexpr auto Abbreviations = "abbreviatinos";
static constexpr auto  ActivePerEND = "activePerEND";
static constexpr auto        Banner = "banner";
static constexpr auto Complications = "complications";
static constexpr auto EquipmentFree = "equipmentFree";
static constexpr auto     Frequency = "frequency";
static constexpr auto   Greenfields = "greenFields";
static constexpr auto   HumanMaxima = "humanMaxima";
static constexpr auto         Notes = "notes";
static constexpr auto   TotalPoints = "totalPoints";

Option::Option()
    : mAbbreviations(false)
    , mBanner(":/gfx/HeroSystem-Banner.png")
    , mComplications(0_cp)
    , mEquipmentFree(false)
    , mGreenfields(false)
    , mShowFrequencyRolls(false)
    , mShowNotesPage(false)
    , mNormalHumanMaxima(false)
    , mActivePerEND(10_cp)
    , mTotalPoints(0_cp) { }

Option::Option(const Option& opt)
    : mAbbreviations(opt.mAbbreviations)
    , mBanner(opt.mBanner)
    , mComplications(opt.mComplications)
    , mEquipmentFree(opt.mEquipmentFree)
    , mGreenfields(opt.mGreenfields)
    , mShowFrequencyRolls(opt.mShowFrequencyRolls)
    , mShowNotesPage(opt.mShowNotesPage)
    , mNormalHumanMaxima(opt.mNormalHumanMaxima)
    , mActivePerEND(opt.mActivePerEND)
    , mTotalPoints(opt.mTotalPoints) { }

Option::Option(Option&& opt): Option(opt) { }

Option::Option(const QJsonObject& obj)
    : mAbbreviations(obj[Abbreviations].toBool(false))
    , mBanner(obj[Banner].toString())
    , mComplications(Points(obj[Complications].toInt(75)))
    , mEquipmentFree(obj[EquipmentFree].toBool(false))
    , mGreenfields(obj[Greenfields].toBool(false))
    , mShowFrequencyRolls(obj[Frequency].toBool(true))
    , mShowNotesPage(obj[Notes].toBool(true))
    , mNormalHumanMaxima(obj[HumanMaxima].toBool(false))
    , mActivePerEND(Points(obj[ActivePerEND].toInt(10)))
    , mTotalPoints(Points(obj[TotalPoints].toInt(400))) { }

Option& Option::operator=(const Option& opt) {
    if (this != &opt) {
             mAbbreviations = opt.mAbbreviations;
                    mBanner = opt.mBanner;
             mComplications = opt.mComplications;
             mEquipmentFree = opt.mEquipmentFree;
               mGreenfields = opt.mGreenfields;
        mShowFrequencyRolls = opt.mShowFrequencyRolls;
             mShowNotesPage = opt.mShowNotesPage;
         mNormalHumanMaxima = opt.mNormalHumanMaxima;
              mActivePerEND = opt.mActivePerEND;
               mTotalPoints = opt.mTotalPoints;
    }
    return *this;
}

Option& Option::operator=(Option&& opt) {
    return operator=(opt);
}

void Option::load() {
    QSettings settings("SoftwareOnHand", "HSCCU");
    bool ok;

    mBanner = settings.value(Banner).toString();
    if (mBanner.isEmpty()) mBanner = ":/gfx/HeroSystem-Banner.png";

    mComplications = Points(settings.value(Complications).toInt(&ok));
    if (!ok) mComplications = 75_cp;

         mAbbreviations = settings.value(Abbreviations).toBool();
         mEquipmentFree = settings.value(EquipmentFree).toBool();
    mShowFrequencyRolls = settings.value(Frequency).toBool();
           mGreenfields = settings.value(Greenfields).toBool();
         mShowNotesPage = settings.value(Notes).toBool();
     mNormalHumanMaxima = settings.value(HumanMaxima).toBool();

    mActivePerEND = Points(settings.value(ActivePerEND).toInt(&ok));
    if (!ok) mActivePerEND = 10_cp;

    mTotalPoints = Points(settings.value(TotalPoints).toInt(&ok));
    if (!ok) mTotalPoints = 400_cp;
}

void Option::store() {
    QSettings settings("SoftwareOnHand", "HSCCU");
    settings.setValue(Abbreviations, mAbbreviations);
    settings.setValue(Banner,        mBanner);
    settings.setValue(Complications, (int) mComplications.points);
    settings.setValue(EquipmentFree, mEquipmentFree);
    settings.setValue(Frequency,     mShowFrequencyRolls);
    settings.setValue(Greenfields,   mGreenfields);
    settings.setValue(Notes,         mShowNotesPage);
    settings.setValue(HumanMaxima,   mNormalHumanMaxima);
    settings.setValue(ActivePerEND,  (int) mActivePerEND.points);
    settings.setValue(TotalPoints,   (int) mTotalPoints.points);
    settings.sync();
}

void Option::toJson(QJsonObject& obj) {
    obj[Abbreviations] = mAbbreviations;
           obj[Banner] = mBanner;
    obj[Complications] = (int) mComplications.points;
    obj[EquipmentFree] = mEquipmentFree;
        obj[Frequency] = mShowFrequencyRolls;
      obj[Greenfields] = mGreenfields;
            obj[Notes] = mShowNotesPage;
      obj[HumanMaxima] = mNormalHumanMaxima;
     obj[ActivePerEND] = (int) mActivePerEND.points;
      obj[TotalPoints] = (int) mTotalPoints.points;
}
