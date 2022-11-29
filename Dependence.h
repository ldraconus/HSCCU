#ifndef DEPENDENCE_H
#define DEPENDENCE_H

#include "complication.h"

class Dependence: public Complication {
public:
    Dependence(): Complication() { }
    Dependence(const Dependence& d)
        : Complication()
        , _addiction(d._addiction)
        , _competence(d._competence)
        , _damage(d._damage)
        , _rarity(d._rarity)
        , _roll(d._roll)
        , _timeStep(d._timeStep)
        , _weakness(d._weakness)
        , _what(d._what) { }
    Dependence(Dependence&& d)
        : Complication()
        , _addiction(d._addiction)
        , _competence(d._competence)
        , _damage(d._damage)
        , _rarity(d._rarity)
        , _roll(d._roll)
        , _timeStep(d._timeStep)
        , _weakness(d._weakness)
        , _what(d._what) { }
    Dependence(const QJsonObject& json)
        : Complication()
        , _addiction(json["addiction"].toBool(false))
        , _competence(json["competence"].toBool(false))
        , _damage(json["damage"].toInt(0))
        , _rarity(json["rarity"].toInt(-1))
        , _roll(json["roll"].toInt(-1))
        , _timeStep(json["time step"].toInt(-1))
        , _weakness(json["weakness"].toBool(false))
        , _what(json["what"].toString()) { }

    Dependence& operator=(const Dependence& d) {
        if (this != &d) {
            _addiction  = d._addiction;
            _competence = d._competence;
            _damage     = d._damage;
            _rarity     = d._rarity;
            _roll       = d._roll;
            _timeStep   = d._timeStep;
            _weakness   = d._weakness;
            _what       = d._what;
        }
        return *this;
    }
    Dependence& operator=(Dependence&& d) {
        _addiction  = d._addiction;
        _competence = d._competence;
        _damage     = d._damage;
        _rarity     = d._rarity;
        _roll       = d._roll;
        _timeStep   = d._timeStep;
        _weakness   = d._weakness;
        _what       = d._what;
        return *this;
    }

    QString description() override {
        static QList<QString> rarity { "Very Common/Easy To Obtain",
                                       "Common/Difficult To Obtain",
                                       "Uncommon/Extremely Difficult To Obtain" };
        static QList<QString> damage { "1d6", "2d6", "3d6" };
        static QList<QString> roll { "Powers aquire a 14- Required Roll",
                                     "Powers aquire a 11- Required Roll" };
        static QList<QString> time { "Segment", "Phase", "Turn", "Minute", "5 Minutes", "20 Minutes",
                                     "1 Hour", "6 Hours", "Day", "Week", "Month", "Season",
                                     "Year", "5 Years" };
        if (_addiction && _damage < 0 && !_competence && !_weakness) return "<incomplete>";
        if (_rarity < 0 || (_damage < 0 && _roll < 0 && !_competence && !_weakness) || (!_addiction && _timeStep < 0)) return "<incomplete>";
        QString result;
        if (_addiction) result = QString("Addiction: %1 (%2").arg(_what, rarity[_rarity]);
        else result = QString("Dependence: %1 (%2").arg(_what, rarity[_rarity]);
        if (_damage >= 0) result += "; " + damage[_damage] + " damage";
        if (_timeStep >= 0 && !_addiction) result += QString("%1 every ").arg(_damage >= 0 ? "" : ";") + time[_timeStep];
        if (_roll >= 0) result += "; " + roll[_roll];
        if (_competence) result += QString("; ") + "-1 to Skill Rolls and related rolls per time increment";
        if (_weakness) result += QString("; ") + "-3 to all Characteristics per time increment";
        return result + ") ";
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        what       = createLineEdit(parent, layout, "What is the substance?");
        rarity     = createComboBox(parent, layout, "How rare/dificult to get is it?", { "Very Common/Easy To Obtain",
                                                                                         "Common/Difficult To Obtain",
                                                                                         "Uncommon/Extremely Difficult To Obtain" });
        damage     = createComboBox(parent, layout, "How much damage?", { "1d6", "2d6", "3d6" });
        roll       = createComboBox(parent, layout, "Required Roll?",   { "Powers aquire a 14- Required Roll",
                                                                          "Powers aquire a 11- Required Roll" });
        competence = createCheckBox(parent, layout, "Incomptence: -1 to Skill Rolls and related rolls per time increment", none);
        weakness   = createCheckBox(parent, layout, "Weakness: -3 to all Characteristics per time increment", none);
        timeStep   = createComboBox(parent, layout, "Time between penalties", { "Segment", "Phase", "Turn", "1 Minute", "5 Minutes", "20 Minutes",
                                                                                "1 Hour", "6 Hours", "1 Day", "1 Week", "1 Month", "1 Season",
                                                                                "1 Year", "5 Years" });
        addiction  = createCheckBox(parent, layout, "Addiction", std::mem_fn(&Complication::checked));
    }
    int points(bool noStore = false) override {
        if (!noStore) store();
        return (_rarity + 1) * 5 + (_damage + 1) * 5 + (_roll + 1) * 5 + (_competence ? 5 : 0) + (_weakness ? 5 : 0) +
               (_addiction ? 5 : (25 - 5 * _timeStep));
    }
    void restore() override {
        addiction->setChecked(_addiction);
        competence->setChecked(_competence);
        damage->setCurrentIndex(_damage);
        rarity->setCurrentIndex(_rarity);
        roll->setCurrentIndex(_roll);
        timeStep->setCurrentIndex(_timeStep);
        weakness->setChecked(_weakness);
        what->setText(_what);
    }
    void store() override {
        _addiction = addiction->isChecked();
        _competence = competence->isChecked();
        _damage = damage->currentIndex();
        _rarity = rarity->currentIndex();
        _roll = roll->currentIndex();
        _timeStep = timeStep->currentIndex();
        _weakness = weakness->isChecked();
        _what = what->text();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]       = "Dependence";
        obj["addiction"]  = _addiction;
        obj["competence"] = _competence;
        obj["damage"]     = _damage;
        obj["rarity"]     = _rarity;
        obj["roll"]       = _roll;
        obj["time step"]  = _timeStep;
        obj["weakness"]   = _weakness;
        obj["what"]       = _what;
        return obj;
    }

private:
    bool    _addiction = true;
    bool    _competence = 0;
    int     _damage = 0;
    int     _rarity = -1;
    int     _roll = 0;
    int     _timeStep = -1;
    bool    _weakness = false;
    QString _what = "";

    QCheckBox* addiction;
    QCheckBox* competence;
    QComboBox* damage;
    QComboBox* rarity;
    QComboBox* roll;
    QComboBox* timeStep;
    QCheckBox* weakness;
    QLineEdit* what;

    void checked(bool on) override { timeStep->setEnabled(!on); }
};

#endif // DEPENDENCE_H
