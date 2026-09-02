#ifndef DEPENDENT_H
#define DEPENDENT_H

#include <cmath>

#include "complication.h"
#ifndef ISHSC
#include "sheet.h"
#endif

class Dependent: public Complication {
public:
    Dependent() = default;
    Dependent(const QJsonObject& json)
        : Complication(json)
        , v { json["competence"].toInt(0)
            , json["frequency"].toInt(0)
            , json["multiples"].toInt(0)
            , json["unaware"].toBool(false)
            , json["useful"].toBool(false)
            , json["who"].toString("") } { }

    QString abbreviation() override { return str(); }
    QString description() override  { return str(); }
    QString str(bool abbr = false) {
        QList<QString> comp     { abbr ? "Incomp." : "Incompetent", "Normal", abbr ? "< Power. Then The PC" : "Slightly Less Powerful Then The PC",
                                  abbr ? "Equal to PC" : "As Powerful As The PC" };
        QList<QString> freq     { abbr ? "8-" : "Infrequently (8-)", abbr ? "11-" : "Frequently (11-)", abbr ? "14-" : "Very Frequently (14-)" };
        QList<QString> freqSans { abbr ? "Infreq." : "Infrequently", abbr ? "Freq." : "Frequently", abbr ? "V. Freq." : "Very Frequently", "Always" };
        if (v.mFrequency < 0 || v.mCompetence < 0 || v.mWho.isEmpty()) return "<incomplete>";
        QString result = "Dependent NPC: " + v.mWho + " (";
#ifndef ISHSC
        if (Sheet::ref().getOption().showFrequencyRolls()) result += freq[v.mFrequency];
        else result += freqSans[v.mFrequency];
#else
        result += freq[v.mFrequency];
#endif
        result += "); " + comp[v.mCompetence];
        if (v.mUnaware) result += abbr ? "; DNPC unaware" : "; DNPC is unaware of character's adventuring";
        if (v.mUseful) result += abbr ? "; DNPC is useful" : "; DNPC has useful noncombat position or skills";
        if (v.mMultiples > 1) result += QString("; %1 people").arg(v.mMultiples);
        return result;
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        who        = createLineEdit(parent, layout, "Who is it?");
        competence = createComboBox(parent, layout, "How compentent is the DNPC?", { "Incompetent", "Normal", "Slightly Less Powerful Then The PC",
                                                                                     "As Powerful As The PC" });
        useful     = createCheckBox(parent, layout, "DNPC has useful noncombat position or skills");
#ifndef ISHSC
        if (Sheet::ref().getOption().showFrequencyRolls()) frequency  = createComboBox(parent, layout, "How often do they Appear?", { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)" });
        else frequency  = createComboBox(parent, layout, "How often do they Appear?", { "Infrequently", "Frequently", "Very Frequently" });
#else
        frequency  = createComboBox(parent, layout, "How often do they Appear?", { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)" });
#endif
        unaware    = createCheckBox(parent, layout, "DNPC is unaware of character's adventuring");
        multiples  = createLineEdit(parent, layout, "How many dependants?", std::mem_fn(&Complication::numeric));
    }
    Points points(bool noStore = false) override {
        if (!noStore) store();
        int mult = (int) (log((double) v.mMultiples) / log(2.0)); // NOLINT
        if (mult < 0) mult = 0;
        return 10_cp - (v.mCompetence < 0 ? 0 : v.mCompetence) * 5_cp + (v.mUseful ? 5_cp : 0_cp) + 5_cp * (v.mFrequency + 1) + (v.mUnaware ? 5_cp : 0_cp) + mult * 5_cp; // NOLINT
    }
    void restore() override {
        vars s = v;
        who->setText(s.mWho);
        competence->setCurrentIndex(s.mCompetence);
        frequency->setCurrentIndex(s.mFrequency);
        multiples->setText(QString("%1").arg(s.mMultiples));
        unaware->setChecked(s.mUnaware);
        useful->setChecked(s.mUseful);
        v = s;
    }
    void store() override {
        v.mWho        = who->text();
        v.mCompetence = competence->currentIndex();
        v.mFrequency  = frequency->currentIndex();
        v.mMultiples  = multiples->text().toInt(0);
        v.mUnaware    = unaware->isChecked();
        v.mUseful     = useful->isChecked();
    }
    QJsonObject toJson() override {
        QJsonObject obj   = Complication::toJson();
        obj["name"]       = "Dependent NPC";
        obj["competence"] = v.mCompetence;
        obj["frequency"]  = v.mFrequency;
        obj["multiples"]  = v.mMultiples;
        obj["unaware"]    = v.mUnaware;
        obj["useful"]     = v.mUseful;
        obj["who"]        = v.mWho;
        return obj;
    }

private:
    struct vars {
        int     mCompetence = -1;
        int     mFrequency = -1;
        int     mMultiples = 0;
        bool    mUnaware = false;
        bool    mUseful = false;
        QString mWho = "";
    } v;

    QComboBox* competence = nullptr;
    QComboBox* frequency = nullptr;
    QLineEdit* multiples = nullptr;
    QCheckBox* unaware = nullptr;
    QCheckBox* useful = nullptr;
    QLineEdit* who = nullptr;

    void numeric(QString) override {
        QString txt = multiples->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        multiples->undo();
    }
};

#endif // DEPENDENT_H
