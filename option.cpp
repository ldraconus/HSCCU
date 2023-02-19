#include "option.h"

#include <QSettings>

Option::Option()
    : _totalPoints(0_cp)
    , _complications(0_cp)
{ }

Option::Option(const Option& opt)
    : _totalPoints(opt._totalPoints)
    , _complications(opt._complications)
{ }

Option::Option(Option&& opt)
    : _totalPoints(opt._totalPoints)
    , _complications(opt._complications)
{ }

Option::Option(const QJsonObject& obj)
    : _totalPoints(Points<>(obj["totalPoints"].toInt(400)))
    , _complications(Points<>(obj["complications"].toInt(75)))
{ }

Option& Option::operator=(const Option& opt) {
    if (this != &opt) {
        _totalPoints = opt._totalPoints;
        _complications = opt._complications;
    }
    return *this;
}

Option& Option::operator=(Option&& opt) {
    if (this != &opt) {
        _totalPoints = opt._totalPoints;
        _complications = opt._complications;
    }
    return *this;
}

void Option::load() {
    QSettings settings("SoftwareOnHand", "HSCCU");
    bool ok;

    _totalPoints = Points<>(settings.value("totalPoints").toInt(&ok));
    if (!ok) _totalPoints = 400_cp;

    _complications = Points<>(settings.value("complications").toInt(&ok));
    if (!ok) _complications = 75_cp;
}

void Option::store() {
    QSettings settings("SoftwareOnHand", "HSCCU");
    settings.setValue("totalPoints",   _totalPoints.points);
    settings.setValue("complications", _complications.points);
}
void Option::toJson(QJsonObject& obj) {
    obj["totalPoints"]   = (int) _totalPoints.points;
    obj["complications"] = (int) _complications.points;
}
