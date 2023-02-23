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
        , v { json["frequency"].toInt(0), json["intesity"].toInt(0), json["what"].toString("") } { }

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
        if (v._frequency < 0 || v._intensity < 0 || v._what.isEmpty()) return "<incomplete>";
        return QString("Pychological Complication: %1 (%2; %3)").arg(v._what, freq[v._frequency], ints[v._intensity]);
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        what         = createLineEdit(parent, layout, "What is the complication?");
        frequency    = createComboBox(parent, layout, "How often does it come up?", { "Uncommon", "Common", "Very Common" });
        intensity    = createComboBox(parent, layout, "How commited are you?", { "Moderate", "Strong", "Total" });
    }
    Points points(bool noStore = false) override {
        if (!noStore) store();
        return (v._frequency + 1) * 5_cp + (v._intensity < 0 ? 0 : v._intensity) * 5_cp;
    }
    void restore() override {
        vars s = v;
        what->setText(s._what);
        frequency->setCurrentIndex(s._frequency);
        intensity->setCurrentIndex(s._intensity);
        v = s;
    }
    void store() override {
        v._what      = what->text();
        v._frequency = frequency->currentIndex();
        v._intensity = intensity->currentIndex();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]      = "Psychological Complication";
        obj["frequency"] = v._frequency;
        obj["intensity"] = v._intensity;
        obj["what"]      = v._what;
        return obj;
    }

private:
    struct vars {
        int     _frequency = -1;
        int     _intensity = -1;
        QString _what      = "";
    } v;

    QComboBox* frequency;
    QComboBox* intensity;
    QLineEdit* what;
};

#endif // PSYCHOLOGICALCOMPLICATION_H
