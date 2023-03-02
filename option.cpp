#include "option.h"

#include <QSettings>

Option::Option()
    : _complications(0_cp)
    , _showFrequencyRolls(false)
    , _showNotesPage(false)
    , _totalPoints(0_cp)
{ }

Option::Option(const Option& opt)
    : _complications(opt._complications)
    , _showFrequencyRolls(opt._showFrequencyRolls)
    , _showNotesPage(opt._showNotesPage)
    , _totalPoints(opt._totalPoints)
{ }

Option::Option(Option&& opt)
    : _complications(opt._complications)
    , _showFrequencyRolls(opt._showFrequencyRolls)
    , _showNotesPage(opt._showNotesPage)
    , _totalPoints(opt._totalPoints)
{ }

Option::Option(const QJsonObject& obj)
    : _complications(Points(obj["complications"].toInt(75)))
    , _showFrequencyRolls(obj["frequency"].toBool(true))
    , _showNotesPage(obj["notes"].toBool(true))
    , _totalPoints(Points(obj["totalPoints"].toInt(400)))
{ }

Option& Option::operator=(const Option& opt) {
    if (this != &opt) {
        _complications = opt._complications;
        _showFrequencyRolls = opt._showFrequencyRolls;
        _showNotesPage = opt._showNotesPage;
        _totalPoints = opt._totalPoints;
    }
    return *this;
}

Option& Option::operator=(Option&& opt) {
    if (this != &opt) {
        _complications = opt._complications;
        _showFrequencyRolls = opt._showFrequencyRolls;
        _showNotesPage = opt._showNotesPage;
        _totalPoints = opt._totalPoints;
    }
    return *this;
}

void Option::load() {
    QSettings settings("SoftwareOnHand", "HSCCU");
    bool ok;

    _complications = Points(settings.value("complications").toInt(&ok));
    if (!ok) _complications = 75_cp;

    _showFrequencyRolls = settings.value("frequency").toBool();

    _showNotesPage = settings.value("notes").toBool();

    _totalPoints = Points(settings.value("totalPoints").toInt(&ok));
    if (!ok) _totalPoints = 400_cp;
}

void Option::store() {
    QSettings settings("SoftwareOnHand", "HSCCU");
    settings.setValue("complications", (int) _complications.points);
    settings.setValue("frequency", _showFrequencyRolls);
    settings.setValue("notes", _showNotesPage);
    settings.setValue("totalPoints",   (int) _totalPoints.points);
}
void Option::toJson(QJsonObject& obj) {
    obj["complications"] = (int) _complications.points;
    obj["frequency"]     = _showFrequencyRolls;
    obj["notes"]         = _showNotesPage;
    obj["totalPoints"]   = (int) _totalPoints.points;
}
