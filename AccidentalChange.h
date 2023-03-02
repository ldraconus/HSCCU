#ifndef ACCIDENTALCHANGE_H
#define ACCIDENTALCHANGE_H

#include "complication.h"
#include "sheet.h"

class AccidentalChange: public Complication
{
public:
    AccidentalChange(): Complication() { }
    AccidentalChange(const AccidentalChange& ac)
        : Complication()
        , v(ac.v) { }
    AccidentalChange(AccidentalChange&& ac)
        : Complication()
        , v(ac.v) { }
    AccidentalChange(const QJsonObject& json)
        : Complication()
        , v { json["circumstance"].toInt(0), json["frequency"].toInt(0), json["what"].toString("") } { }

    AccidentalChange& operator=(const AccidentalChange& ac) {
        if (this != &ac) v = ac.v;
        return *this;
    }
    AccidentalChange& operator=(AccidentalChange&& ac) {
        v = ac.v;
        return *this;
    }

    QString description() override {
        static QList<QString> circ     { "Uncommon", "Common", "Very Common" };
        static QList<QString> freq     { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)", "Always" };
        static QList<QString> freqSans { "Infrequently", "Frequently", "Very Frequently", "Always" };
        if (v._frequency < 0 || v._circumstance  < 0 || v._what.isEmpty()) return "<incomplete>";
        if (Sheet::ref().getOption().showFrequencyRolls()) return QString("Accidental Change: %1 (%2; %3)").arg(v._what, circ[v._circumstance], freq[v._frequency]);
        else return QString("Accidental Change: %1 (%2; %3)").arg(v._what, circ[v._circumstance], freqSans[v._frequency]);
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        what          = createLineEdit(parent, layout, "What sets off the change?");
        circumstance  = createComboBox(parent, layout, "How common is the change", { "Uncommmon", "Common", "Very Common" });
        if (Sheet::ref().getOption().showFrequencyRolls())
            frequency = createComboBox(parent, layout, "How often do you change", { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)", "Always" });
        else
            frequency = createComboBox(parent, layout, "How often do you change", { "Infrequently", "Frequently", "Very Frequently", "Always" });
    }
    Points points(bool noStore = false) override {
        if (!noStore) store();
        return (v._circumstance > -1 && v._frequency > -1) ? (v._circumstance - 1) * 5_cp + v._frequency * 5_cp : 0_cp;
    }
    void restore() override {
        vars s = v;
        what->setText(s._what);
        circumstance->setCurrentIndex(s._circumstance);
        frequency->setCurrentIndex(s._frequency);
        v = s;
    }
    void store() override {
        v._what         = what->text();
        v._circumstance = circumstance->currentIndex();
        v._frequency    = frequency->currentIndex();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]         = "Accidental Change";
        obj["circumstance"] = v._circumstance;
        obj["frequency"]    = v._frequency;
        obj["what"]         = v._what;
        return obj;
    }

private:
    struct vars {
        int     _circumstance = -1;
        int     _frequency = -1;
        QString _what = "";
    } v;

    QComboBox* circumstance;
    QComboBox* frequency;
    QLineEdit* what;
};

#endif // ACCIDENTALCHANGE_H
