#ifndef PSYCHOLOGICALCOMPLICATION_H
#define PSYCHOLOGICALCOMPLICATION_H

#include "complication.h"

class PsychologicalComplication: public Complication
{
public:
    PsychologicalComplication(): Complication() { }
    PsychologicalComplication(const PsychologicalComplication& ac)
        : Complication()
        , v(ac.v) { }
    PsychologicalComplication(PsychologicalComplication&& ac)
        : Complication()
        , v(ac.v) { }
    PsychologicalComplication(const QJsonObject& json)
        : Complication()
        , v { json["frequency"].toInt(0), json["intensity"].toInt(0), json["what"].toString("") } { }

    PsychologicalComplication& operator=(const PsychologicalComplication& ac) {
        if (this != &ac)v = ac.v;
        return *this;
    }
    PsychologicalComplication& operator=(PsychologicalComplication&& ac) {
        v = ac.v;
        return *this;
    }

    QString description() override {
        static QList<QString> ints { "Moderate", "Strong", "Total" };
        static QList<QString> freq { "Uncommon", "Common", "Very Common" };
        if (v.mFrequency < 0 || v.mIntensity < 0 || v.mWhat.isEmpty()) return "<incomplete>";
        return QString("Pychological Complication: %1 (%2; %3)").arg(v.mWhat, freq[v.mFrequency], ints[v.mIntensity]);
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        what         = createLineEdit(parent, layout, "What is the complication?");
        frequency    = createComboBox(parent, layout, "How often does it come up?", { "Uncommon", "Common", "Very Common" });
        intensity    = createComboBox(parent, layout, "How commited are you?", { "Moderate", "Strong", "Total" });
    }
    Points points(bool noStore = false) override {
        if (!noStore) store();
        return (v.mFrequency + 1) * 5_cp + (v.mIntensity < 0 ? 0 : v.mIntensity) * 5_cp;
    }
    void restore() override {
        vars s = v;
        what->setText(s.mWhat);
        frequency->setCurrentIndex(s.mFrequency);
        intensity->setCurrentIndex(s.mIntensity);
        v = s;
    }
    void store() override {
        v.mWhat      = what->text();
        v.mFrequency = frequency->currentIndex();
        v.mIntensity = intensity->currentIndex();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]      = "Psychological Complication";
        obj["frequency"] = v.mFrequency;
        obj["intensity"] = v.mIntensity;
        obj["what"]      = v.mWhat;
        return obj;
    }

private:
    struct vars {
        int     mFrequency = -1;
        int     mIntensity = -1;
        QString mWhat      = "";
    } v;

    QComboBox* frequency;
    QComboBox* intensity;
    QLineEdit* what;
};

#endif // PSYCHOLOGICALCOMPLICATION_H
