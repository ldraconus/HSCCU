#include "option.h"

#include <QSettings>

Option::Option()
    : mAbbreviations(false)
    , mBanner(":/gfx/HeroSystem-Banner.png")
    , mComplications(0_cp)
    , mEquipmentFree(false)
    , mGreenFields(false)
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
    : mAbbreviations(obj["Abbreviations"].toBool(false))
    , mBanner(obj["banner"].toString())
    , mComplications(Points(obj["complications"].toInt(75)))
    , mEquipmentFree(obj["equipmentFree"].toBool(false))
    , mGreenfields(obj["greenfields"].toBool(false))
    , mShowFrequencyRolls(obj["frequency"].toBool(true))
    , mShowNotesPage(obj["notes"].toBool(true))
    , mNormalHumanMaxima(obj["humanMaxima"].toBool(false))
    , mActivePerEND(Points(obj["activePerEND"].toInt(10)))
    , mTotalPoints(Points(obj["totalPoints"].toInt(400))) { }

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

    mBanner = settings.value("banner").toString();
    if (mBanner.isEmpty()) mBanner = ":/gfx/HeroSystem-Banner.png";

    mComplications = Points(settings.value("complications").toInt(&ok));
    if (!ok) mComplications = 75_cp;

         mAbbreviations = settings.value("abbrevitions").toBool();
         mEquipmentFree = settings.value("equipmentFree").toBool();
    mShowFrequencyRolls = settings.value("frequency").toBool();
           mGreenfields = settings.value("greenfields").toBool();
         mShowNotesPage = settings.value("notes").toBool();
     mNormalHumanMaxima = settings.value("humanMaxima").toBool();

    mActivePerEND = Points(settings.value("activePerEND").toInt(&ok));
    if (!ok) mActivePerEND = 10_cp;

    mTotalPoints = Points(settings.value("totalPoints").toInt(&ok));
    if (!ok) mTotalPoints = 400_cp;
}

void Option::store() {
    QSettings settings("SoftwareOnHand", "HSCCU");
    settings.setValue("abbreviations", mAbbreviations);
    settings.setValue("banner", mBanner);
    settings.setValue("complications", (int) mComplications.points);
    settings.setValue("equipmentFree", mEquipmentFree);
    settings.setValue("frequency", mShowFrequencyRolls);
    settings.setValue("greefields", mGreenfields);
    settings.setValue("notes", mShowNotesPage);
    settings.setValue("humanMaxima", mNormalHumanMaxima);
    settings.setValue("activePerEND", (int) mActivePerEND.points);
    settings.setValue("totalPoints",   (int) mTotalPoints.points);
}

void Option::toJson(QJsonObject& obj) {
     obj["abbrevitions"] = mAbbreviations;
           obj["banner"] = mBanner;
    obj["complications"] = (int) mComplications.points;
     obj["equimentFree"] = mEquipmentFree;
        obj["frequency"] = mShowFrequencyRolls;
      obj["greenfields"] = mGreenfields;
            obj["notes"] = mShowNotesPage;
      obj["humanMaxima"] = mNormalHumanMaxima;
     obj["activePerEND"] = (int) mActivePerEND.points;
      obj["totalPoints"] = (int) mTotalPoints.points;
}
