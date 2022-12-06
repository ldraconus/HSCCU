#ifndef DEPENDENT_H
#define DEPENDENT_H

#include <cmath>

#include "complication.h"

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

    Dependent& operator=(const Dependent& ac) {
        if (this != &ac) v = ac.v;
        return *this;
    }
    Dependent& operator=(Dependent&& ac) {
        v = ac.v;
        return *this;
    }

    QString description() override {
        static QList<QString> comp { "Incompetent", "Normal", "Slightly Less Powerful Then The PC", "As Powerful As The PC" };
        static QList<QString> freq { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)" };
        if (v._frequency == -1 || v._competence == -1 || v._who.isEmpty()) return "<incomplete>";
        QString result = "Dependent NPC: " + v._who + " (" + freq[v._frequency];
        if (v._competence != -1) result += "; " + comp[v._competence];
        if (v._unaware) result += "; DNPC is unaware of character's adventuring";
        if (v._useful) result += "; DNPC has useful noncombat position or skills";
        if (v._multiples > 0) result += QString("; %1x people").arg(std::pow(2, v._multiples));
        return result + ")";
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        who        = createLineEdit(parent, layout, "Who is it?");
        competence = createComboBox(parent, layout, "How compentent is the DNPC?", { "Incompetent", "Normal", "Slightly Less Powerful Then The PC",
                                                                                     "As Powerful As The PC" });
        useful     = createCheckBox(parent, layout, "DNPC has useful noncombat position or skills");
        frequency  = createComboBox(parent, layout, "How often do they Appear?", { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)" });
        unaware    = createCheckBox(parent, layout, "DNPC is unaware of character's adventuring");
        multiples  = createLineEdit(parent, layout, "How many multiples of 2?", std::mem_fn(&Complication::numeric));
    }
    int points(bool noStore = false) override {
        if (!noStore) store();
        return 10 - v._competence * 5 + (v._useful ? 5 : 0) + 5 * (v._frequency + 1) + (v._unaware ? 5 : 0) + v._multiples * 5;
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

    QComboBox* competence;
    QComboBox* frequency;
    QLineEdit* multiples;
    QCheckBox* unaware;
    QCheckBox* useful;
    QLineEdit* who;

    void numeric(QString) override {
        QString txt = multiples->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        multiples->undo();
    }
};

#endif // DEPENDENT_H
