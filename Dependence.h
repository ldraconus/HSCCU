#ifndef DEPENDENCE_H
#define DEPENDENCE_H

#include "complication.h"

class Dependence: public Complication {
public:
    Dependence(): Complication() { }
    Dependence(const Dependence& d)
        : Complication()
        , v(d.v) { }
    Dependence(Dependence&& d)
        : Complication()
        , v(d.v) { }
    Dependence(const QJsonObject& json)
        : Complication()
        , v { json["addiction"].toBool(false)
            , json["competence"].toBool(false)
            , json["damage"].toInt(0)
            , json["rarity"].toInt(-1)
            , json["roll"].toInt(-1)
            , json["time step"].toInt(-1)
            , json["weakness"].toBool(false)
            , json["what"].toString() } { }

    Dependence& operator=(const Dependence& d) {
        if (this != &d) v = d.v;
        return *this;
    }
    Dependence& operator=(Dependence&& d) {
        v = d.v;
        return *this;
    }

    QString description() override {
        static QList<QString> rarity { "Very Common/Easy To Obtain",
                                       "Common/Difficult To Obtain",
                                       "Uncommon/Extremely Difficult To Obtain" };
        static QList<QString> damage { "", "1d6", "2d6", "3d6" };
        static QList<QString> roll { "", "Powers aquire a 14- Required Roll",
                                     "Powers aquire a 11- Required Roll" };
        static QList<QString> time { "", "Segment", "Phase", "Turn", "Minute", "5 Minutes", "20 Minutes",
                                     "1 Hour", "6 Hours", "Day", "Week", "Month", "Season",
                                     "Year", "5 Years" };
        if (v._addiction && v._damage < 1 && !v._competence && !v._weakness) return "<incomplete>";
        if (v._rarity < 0 || (v._damage < 1 && v._roll < 1 && !v._competence && !v._weakness) || (!v._addiction && v._timeStep < 1)) return "<incomplete>";
        QString result;
        if (v._addiction) result = QString("Addiction: %1 (%2").arg(v._what, rarity[v._rarity]);
        else result = QString("Dependence: %1 (%2").arg(v._what, rarity[v._rarity]);
        if (v._damage >= 1) result += "; " + damage[v._damage] + " damage";
        if (v._timeStep >= 1 && !v._addiction) result += QString("%1 every ").arg(v._damage >= 1 ? "" : ";") + time[v._timeStep];
        if (v._roll >= 1) result += "; " + roll[v._roll];
        if (v._competence) result += QString("; ") + "-1 to Skill Rolls and related rolls per time increment";
        if (v._weakness) result += QString("; ") + "-3 to all Characteristics per time increment";
        return result + ") ";
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        what       = createLineEdit(parent, layout, "What is the substance?");
        rarity     = createComboBox(parent, layout, "How rare/dificult to get is it?", { "Very Common/Easy To Obtain",
                                                                                         "Common/Difficult To Obtain",
                                                                                         "Uncommon/Extremely Difficult To Obtain" });
        damage     = createComboBox(parent, layout, "How much damage?", { "No Damage", "1d6", "2d6", "3d6" });
        roll       = createComboBox(parent, layout, "Required Roll?",   { "No Roll",
                                                                          "Powers aquire a 14- Required Roll",
                                                                          "Powers aquire a 11- Required Roll" });
        competence = createCheckBox(parent, layout, "Incomptence: -1 to Skill Rolls and related rolls per time increment");
        weakness   = createCheckBox(parent, layout, "Weakness: -3 to all Characteristics per time increment");
        timeStep   = createComboBox(parent, layout, "Time between penalties", { "No Time", "Segment", "Phase", "Turn", "1 Minute", "5 Minutes", "20 Minutes",
                                                                                "1 Hour", "6 Hours", "1 Day", "1 Week", "1 Month", "1 Season",
                                                                                "1 Year", "5 Years" });
        addiction  = createCheckBox(parent, layout, "Addiction", std::mem_fn(&Complication::checked));
    }
    Points points(bool noStore = false) override {
        if (!noStore) store();
        return v._rarity * 5_cp + v._damage * 5_cp + v._roll * 5_cp + (v._competence ? 5_cp : 0_cp) + (v._weakness ? 5_cp : 0_cp) +
               (v._addiction ? 5_cp : (30_cp - 5_cp * v._timeStep));
    }
    void restore() override {
        vars s = v;
        addiction->setChecked(s._addiction);
        competence->setChecked(s._competence);
        damage->setCurrentIndex(s._damage);
        rarity->setCurrentIndex(s._rarity);
        roll->setCurrentIndex(s._roll);
        timeStep->setCurrentIndex(s._timeStep);
        weakness->setChecked(s._weakness);
        what->setText(s._what);
        v = s;
    }
    void store() override {
        v._addiction  = addiction->isChecked();
        v._competence = competence->isChecked();
        v._damage     = damage->currentIndex();
        v._rarity     = rarity->currentIndex();
        v._roll       = roll->currentIndex();
        v._timeStep   = timeStep->currentIndex();
        v._weakness   = weakness->isChecked();
        v._what       = what->text();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]       = "Dependence";
        obj["addiction"]  = v._addiction;
        obj["competence"] = v._competence;
        obj["damage"]     = v._damage;
        obj["rarity"]     = v._rarity;
        obj["roll"]       = v._roll;
        obj["time step"]  = v._timeStep;
        obj["weakness"]   = v._weakness;
        obj["what"]       = v._what;
        return obj;
    }

private:
    struct vars {
        bool    _addiction = true;
        bool    _competence = 0;
        int     _damage = 0;
        int     _rarity = -1;
        int     _roll = 0;
        int     _timeStep = -1;
        bool    _weakness = false;
        QString _what = "";
    } v;

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
