#ifndef DEPENDENT_H
#define DEPENDENT_H

#include <cmath>

#include "complication.h"
#ifndef ISHSC
#include "sheet.h"
#endif

class Dependent: public Complication
{
public:
    Dependent(): Complication() { }
    Dependent(const Dependent& ac)
        : Complication()
        , v(ac.v) { }
    Dependent(Dependent&& ac)
        : Complication()
        , v(ac.v) { }
    Dependent(const QJsonObject& json)
        : Complication()
        , v { json["competence"].toInt(0)
            , json["frequency"].toInt(0)
            , json["multiples"].toInt(0)
            , json["unaware"].toBool(false)
            , json["useful"].toBool(false)
            , json["who"].toString("") } { }
    ~Dependent() override { }

    Dependent& operator=(const Dependent& ac) {
        if (this != &ac) v = ac.v;
        return *this;
    }
    Dependent& operator=(Dependent&& ac) {
        v = ac.v;
        return *this;
    }

    QString description() override {
        static QList<QString> comp     { "Incompetent", "Normal", "Slightly Less Powerful Then The PC", "As Powerful As The PC" };
        static QList<QString> freq     { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)" };
        static QList<QString> freqSans { "Infrequently", "Frequently", "Very Frequently", "Always" };
        if (v._frequency < 0 || v._competence < 0 || v._who.isEmpty()) return "<incomplete>";
        QString result = "Dependent NPC: " + v._who + " (";
#ifndef ISHSC
        if (Sheet::ref().getOption().showFrequencyRolls())
#endif
            result += freq[v._frequency];
#ifndef ISHSC
        else result += freqSans[v._frequency];
#endif
        result += "); " + comp[v._competence];
        if (v._unaware) result += "; DNPC is unaware of character's adventuring";
        if (v._useful) result += "; DNPC has useful noncombat position or skills";
        if (v._multiples > 1) result += QString("; %1 people").arg(v._multiples);
        return result + ")";
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        who        = createLineEdit(parent, layout, "Who is it?");
        competence = createComboBox(parent, layout, "How compentent is the DNPC?", { "Incompetent", "Normal", "Slightly Less Powerful Then The PC",
                                                                                     "As Powerful As The PC" });
        useful     = createCheckBox(parent, layout, "DNPC has useful noncombat position or skills");
#ifndef ISHSC
        if (Sheet::ref().getOption().showFrequencyRolls())
#endif
            frequency  = createComboBox(parent, layout, "How often do they Appear?", { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)" });
#ifndef ISHSC
        else
            frequency  = createComboBox(parent, layout, "How often do they Appear?", { "Infrequently", "Frequently", "Very Frequently" });
#endif
        unaware    = createCheckBox(parent, layout, "DNPC is unaware of character's adventuring");
        multiples  = createLineEdit(parent, layout, "How many dependants?", std::mem_fn(&Complication::numeric));
    }
    Points points(bool noStore = false) override {
        if (!noStore) store();
        int mult = (int) (log((double) v._multiples) / log(2.0)); // NOLINT
        if (mult < 0) mult = 0;
        return 10_cp - (v._competence < 0 ? 0 : v._competence) * 5_cp + (v._useful ? 5_cp : 0_cp) + 5_cp * (v._frequency + 1) + (v._unaware ? 5_cp : 0_cp) + mult * 5_cp; // NOLINT
    }
    void restore() override {
        vars s = v;
        who->setText(s._who);
        competence->setCurrentIndex(s._competence);
        frequency->setCurrentIndex(s._frequency);
        multiples->setText(QString("%1").arg(s._multiples));
        unaware->setChecked(s._unaware);
        useful->setChecked(s._useful);
        v = s;
    }
    void store() override {
        v._who        = who->text();
        v._competence = competence->currentIndex();
        v._frequency  = frequency->currentIndex();
        v._multiples  = multiples->text().toInt(0);
        v._unaware    = unaware->isChecked();
        v._useful     = useful->isChecked();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]       = "Dependent NPC";
        obj["competence"] = v._competence;
        obj["frequency"]  = v._frequency;
        obj["multiples"]  = v._multiples;
        obj["unaware"]    = v._unaware;
        obj["useful"]     = v._useful;
        obj["who"]        = v._who;
        return obj;
    }

private:
    struct vars {
        int     _competence = -1;
        int     _frequency = -1;
        int     _multiples = 0;
        bool    _unaware = false;
        bool    _useful = false;
        QString _who = "";
    } v;

    gsl::owner<QComboBox*> competence = nullptr;
    gsl::owner<QComboBox*> frequency = nullptr;
    gsl::owner<QLineEdit*> multiples = nullptr;
    gsl::owner<QCheckBox*> unaware = nullptr;
    gsl::owner<QCheckBox*> useful = nullptr;
    gsl::owner<QLineEdit*> who = nullptr;

    void numeric(QString) override {
        QString txt = multiples->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        multiples->undo();
    }
};

#endif // DEPENDENT_H
