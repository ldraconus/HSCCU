#ifndef ACCIDENTALCHANGE_H
#define ACCIDENTALCHANGE_H

#include "complication.h"
#ifndef ISHSC
#include "sheet.h"
#endif

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
    ~AccidentalChange() override { }

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
        if (v.mFrequency < 0 || v.mCircumstance  < 0 || v.mWhat.isEmpty()) return "<incomplete>";
#ifndef ISHSC
        if (Sheet::ref().getOption().showFrequencyRolls())
#endif
            return QString("Accidental Change: %1 (%2; %3)").arg(v.mWhat, circ[v.mCircumstance], freq[v.mFrequency]);
#ifndef ISHSC
        else return QString("Accidental Change: %1 (%2; %3)").arg(v.mWhat, circ[v.mCircumstance], freqSans[v.mFrequency]);
#endif
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        what          = createLineEdit(parent, layout, "What sets off the change?");
        circumstance  = createComboBox(parent, layout, "How common is the change", { "Uncommmon", "Common", "Very Common" });
#ifndef ISHSC
        if (Sheet::ref().getOption().showFrequencyRolls())
#endif
            frequency = createComboBox(parent, layout, "How often do you change", { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)", "Always" });
#ifndef ISHSC
        else
            frequency = createComboBox(parent, layout, "How often do you change", { "Infrequently", "Frequently", "Very Frequently", "Always" });
#endif
    }
    Points points(bool noStore = false) override {
        if (!noStore) store();
        return (v.mCircumstance > -1 && v.mFrequency > -1) ? (v.mCircumstance - 1) * 5_cp + v.mFrequency * 5_cp : 0_cp; // NOLINT
    }
    void restore() override {
        vars s = v;
        what->setText(s.mWhat);
        circumstance->setCurrentIndex(s.mCircumstance);
        frequency->setCurrentIndex(s.mFrequency);
        v = s;
    }
    void store() override {
        v.mWhat         = what->text();
        v.mCircumstance = circumstance->currentIndex();
        v.mFrequency    = frequency->currentIndex();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]         = "Accidental Change";
        obj["circumstance"] = v.mCircumstance;
        obj["frequency"]    = v.mFrequency;
        obj["what"]         = v.mWhat;
        return obj;
    }

private:
    struct vars {
        int     mCircumstance = -1;
        int     mFrequency = -1;
        QString mWhat = "";
    } v;

    gsl::owner<QComboBox*> circumstance = nullptr;
    gsl::owner<QComboBox*> frequency = nullptr;
    gsl::owner<QLineEdit*> what = nullptr;
};

#endif // ACCIDENTALCHANGE_H
