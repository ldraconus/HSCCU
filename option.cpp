#include "option.h"

#include <QSettings>

Option::Option()
    : _banner(":/gfx/HeroSystem-Banner.png")
    , _complications(0_cp)
    , _equipmentFree(false)
    , _showFrequencyRolls(false)
    , _showNotesPage(false)
    , _normalHumanMaxima(false)
    , _activePerEND(10_cp)
    , _totalPoints(0_cp)
{ }

Option::Option(const Option& opt)
    : _banner(opt._banner)
    , _complications(opt._complications)
    , _equipmentFree(opt._equipmentFree)
    , _showFrequencyRolls(opt._showFrequencyRolls)
    , _showNotesPage(opt._showNotesPage)
    , _normalHumanMaxima(opt._normalHumanMaxima)
    , _activePerEND(opt._activePerEND)
    , _totalPoints(opt._totalPoints)
{ }

Option::Option(Option&& opt): Option(opt) { }

Option::Option(const QJsonObject& obj)
    : _banner(obj["banner"].toString())
    , _complications(Points(obj["complications"].toInt(75)))
    , _equipmentFree(obj["equipmentFree"].toBool(false))
    , _showFrequencyRolls(obj["frequency"].toBool(true))
    , _showNotesPage(obj["notes"].toBool(true))
    , _normalHumanMaxima(obj["humanMaxima"].toBool(false))
    , _activePerEND(Points(obj["activePerEND"].toInt(10)))
    , _totalPoints(Points(obj["totalPoints"].toInt(400)))
{ }

Option& Option::operator=(const Option& opt) {
    if (this != &opt) {
                    _banner = opt._banner;
             _complications = opt._complications;
             _equipmentFree = opt._equipmentFree;
        _showFrequencyRolls = opt._showFrequencyRolls;
             _showNotesPage = opt._showNotesPage;
         _normalHumanMaxima = opt._normalHumanMaxima;
              _activePerEND = opt._activePerEND;
               _totalPoints = opt._totalPoints;
    }
    return *this;
}

Option& Option::operator=(Option&& opt) { return operator=(opt); }

void Option::load() {
    QSettings settings("SoftwareOnHand", "HSCCU");
    bool ok;

    _banner = settings.value("banner").toString();
    if (_banner.isEmpty()) _banner = ":/gfx/HeroiSystem-Banner.png";

    _complications = Points(settings.value("complications").toInt(&ok));
    if (!ok) _complications = 75_cp;

         _equipmentFree = settings.value("equipmentFree").toBool();
    _showFrequencyRolls = settings.value("frequency").toBool();
         _showNotesPage = settings.value("notes").toBool();
     _normalHumanMaxima = settings.value("humanMaxima").toBool();

    _activePerEND = Points(settings.value("activePerEND").toInt(&ok));
    if (!ok) _activePerEND = 10_cp;

    _totalPoints = Points(settings.value("totalPoints").toInt(&ok));
    if (!ok) _totalPoints = 400_cp;
}

void Option::store() {
    QSettings settings("SoftwareOnHand", "HSCCU");
    settings.setValue("banner", _banner);
    settings.setValue("complications", (int) _complications.points);
    settings.setValue("equipmentFree", _equipmentFree);
    settings.setValue("frequency", _showFrequencyRolls);
    settings.setValue("notes", _showNotesPage);
    settings.setValue("humanMaxima", _normalHumanMaxima);
    settings.setValue("activePerEND", (int) _activePerEND.points);
    settings.setValue("totalPoints",   (int) _totalPoints.points);
}

void Option::toJson(QJsonObject& obj) {
           obj["banner"] = _banner;
    obj["complications"] = (int) _complications.points;
     obj["equimentFree"] = _equipmentFree;
        obj["frequency"] = _showFrequencyRolls;
            obj["notes"] = _showNotesPage;
      obj["humanMaxima"] = _normalHumanMaxima;
     obj["activePerEND"] = (int) _activePerEND.points;
      obj["totalPoints"] = (int) _totalPoints.points;
}
