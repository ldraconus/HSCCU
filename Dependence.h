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

    ~Dependence() override { }

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
        if (v.mAddiction && v.mDamage < 1 && !v.mCompetence && !v.mWeakness) return "<incomplete>";
        if (v.mRarity < 0 || (v.mDamage < 1 && v.mRoll < 1 && !v.mCompetence && !v.mWeakness) || (!v.mAddiction && v.mTimeStep < 1)) return "<incomplete>";
        QString result;
        if (v.mAddiction) result = QString("Addiction: %1 (%2").arg(v.mWhat, rarity[v.mRarity]);
        else result = QString("Dependence: %1 (%2").arg(v.mWhat, rarity[v.mRarity]);
        if (v.mDamage >= 1) result += "; " + damage[v.mDamage] + " damage";
        if (v.mTimeStep >= 1 && !v.mAddiction) result += QString("%1 every ").arg(v.mDamage >= 1 ? "" : ";") + time[v.mTimeStep];
        if (v.mRoll >= 1) result += "; " + roll[v.mRoll];
        if (v.mCompetence) result += QString("; ") + "-1 to Skill Rolls and related rolls per time increment";
        if (v.mWeakness) result += QString("; ") + "-3 to all Characteristics per time increment";
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
        return v.mRarity * 5_cp + v.mDamage * 5_cp + v.mRoll * 5_cp + (v.mCompetence ? 5_cp : 0_cp) + (v.mWeakness ? 5_cp : 0_cp) + // NOLINT
               (v.mAddiction ? 5_cp : (30_cp - 5_cp * v.mTimeStep)); // NOLINT
    }
    void restore() override {
        vars s = v;
        addiction->setChecked(s.mAddiction);
        competence->setChecked(s.mCompetence);
        damage->setCurrentIndex(s.mDamage);
        rarity->setCurrentIndex(s.mRarity);
        roll->setCurrentIndex(s.mRoll);
        timeStep->setCurrentIndex(s.mTimeStep);
        weakness->setChecked(s.mWeakness);
        what->setText(s.mWhat);
        v = s;
    }
    void store() override {
        v.mAddiction  = addiction->isChecked();
        v.mCompetence = competence->isChecked();
        v.mDamage     = damage->currentIndex();
        v.mRarity     = rarity->currentIndex();
        v.mRoll       = roll->currentIndex();
        v.mTimeStep   = timeStep->currentIndex();
        v.mWeakness   = weakness->isChecked();
        v.mWhat       = what->text();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]       = "Dependence";
        obj["addiction"]  = v.mAddiction;
        obj["competence"] = v.mCompetence;
        obj["damage"]     = v.mDamage;
        obj["rarity"]     = v.mRarity;
        obj["roll"]       = v.mRoll;
        obj["time step"]  = v.mTimeStep;
        obj["weakness"]   = v.mWeakness;
        obj["what"]       = v.mWhat;
        return obj;
    }

private:
    struct vars {
        bool    mAddiction = true;
        bool    mCompetence = 0;
        int     mDamage = 0;
        int     mRarity = -1;
        int     mRoll = 0;
        int     mTimeStep = -1;
        bool    mWeakness = false;
        QString mWhat = "";
    } v;

    QCheckBox* addiction = nullptr;
    QCheckBox* competence = nullptr;
    QComboBox* damage = nullptr;
    QComboBox* rarity = nullptr;
    QComboBox* roll = nullptr;
    QComboBox* timeStep = nullptr;
    QCheckBox* weakness = nullptr;
    QLineEdit* what = nullptr;

    void checked(bool on) override { timeStep->setEnabled(!on); }
};

#endif // DEPENDENCE_H
