#ifndef ACCIDENTALCHANGE_H
#define ACCIDENTALCHANGE_H

#include "complication.h"
#ifndef ISHSC
#include "sheet.h"
#endif

class AccidentalChange: public Complication {
public:
    AccidentalChange() = default;
    AccidentalChange(const QJsonObject& json)
        : Complication(json)
        , v { json["circumstance"].toInt(0), json["frequency"].toInt(0), json["what"].toString("") } { }

    QString abbreviation() override {
        static QList<QString> circ     { "Unc", "Com", "V. Com" };
        static QList<QString> freq     { "8-", "11-", "14-", "Always" };
        static QList<QString> freqSans { "Inf.", "Freq.", "Very Freq.", "Always" };
        if (v.mFrequency < 0 || v.mCircumstance  < 0 || v.mWhat.isEmpty()) return "<incomplete>";
#ifndef ISHSC
        if (Sheet::ref().getOption().showFrequencyRolls()) return QString("Acc. Chng: %1 (%2; %3)").arg(v.mWhat, circ[v.mCircumstance], freq[v.mFrequency]);
        else return QString("Acc. Chng: %1 (%2; %3)").arg(v.mWhat, circ[v.mCircumstance], freqSans[v.mFrequency]);
#else
        return QString("Acc. Chng: %1 (%2; %3)").arg(v.mWhat, circ[v.mCircumstance], freq[v.mFrequency]);
#endif
    }
    QString description() override {
        static QList<QString> circ     { "Uncommon", "Common", "Very Common" };
        static QList<QString> freq     { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)", "Always" };
        static QList<QString> freqSans { "Infrequently", "Frequently", "Very Frequently", "Always" };
        if (v.mFrequency < 0 || v.mCircumstance  < 0 || v.mWhat.isEmpty()) return "<incomplete>";
#ifndef ISHSC
        if (Sheet::ref().getOption().showFrequencyRolls()) return QString("Accidental Change: %1 (%2; %3)").arg(v.mWhat, circ[v.mCircumstance], freq[v.mFrequency]);
        else return QString("Accidental Change: %1 (%2; %3)").arg(v.mWhat, circ[v.mCircumstance], freqSans[v.mFrequency]);
#else
        return QString("Accidental Change: %1 (%2; %3)").arg(v.mWhat, circ[v.mCircumstance], freq[v.mFrequency]);
#endif
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        what          = createLineEdit(parent, layout, "What sets off the change?");
        circumstance  = createComboBox(parent, layout, "How common is the change", { "Uncommmon", "Common", "Very Common" });
#ifndef ISHSC
        if (Sheet::ref().getOption().showFrequencyRolls()) frequency = createComboBox(parent, layout, "How often do you change", { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)", "Always" });
        else frequency = createComboBox(parent, layout, "How often do you change", { "Infrequently", "Frequently", "Very Frequently", "Always" });
#else
        frequency = createComboBox(parent, layout, "How often do you change", { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)", "Always" });
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
        QJsonObject obj     = Complication::toJson();;
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

    QComboBox* circumstance = nullptr;
    QComboBox* frequency = nullptr;
    QLineEdit* what = nullptr;
};

#endif // ACCIDENTALCHANGE_H
