#ifndef PSYCHOLOGICALCOMPLICATION_H
#define PSYCHOLOGICALCOMPLICATION_H

#include "complication.h"

class PsychologicalComplication: public Complication
{
public:
    PsychologicalComplication(): Complication() { }
    PsychologicalComplication(const PsychologicalComplication& ac)
        : Complication()
        , _frequency(ac._frequency)
        , _intensity(ac._intensity)
        , _what(ac._what) { }
    PsychologicalComplication(PsychologicalComplication&& ac)
        : Complication()
        , _frequency(ac._frequency)
        , _intensity(ac._intensity)
        , _what(ac._what) { }
    PsychologicalComplication(const QJsonObject& json)
        : Complication()
        , _frequency(json["frequency"].toInt(0))
        , _intensity(json["intesity"].toInt(0))
        , _what(json["what"].toString("")) { }

    PsychologicalComplication& operator=(const PsychologicalComplication& ac) {
        if (this != &ac) {
            _frequency = ac._frequency;
            _intensity = ac._intensity;
            _what      = ac._what;
        }
        return *this;
    }
    PsychologicalComplication& operator=(PsychologicalComplication&& ac) {
        _frequency = ac._frequency;
        _intensity = ac._intensity;
        _what      = ac._what;
        return *this;
    }

    QString description() override {
        static QList<QString> ints { "Moderate", "Strong", "Total" };
        static QList<QString> freq { "Uncommon", "Common", "Very Common" };
        if (_frequency == -1 || _intensity == -1 || _what.isEmpty()) return "<incomplete>";
        return QString("Pychological Complication: %1 (%2; %3)").arg(_what, freq[_frequency], ints[_intensity]);
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        what         = createLineEdit(parent, layout, "What is the complication?");
        frequency    = createComboBox(parent, layout, "How often does it come up?", { "Uncommon", "Common", "Very Common" });
        intensity    = createComboBox(parent, layout, "How commited are you?", { "Moderate", "Strong", "Total" });
    }
    int points(bool noStore = false) override {
        if (!noStore) store();
        return (_frequency + 1) * 5 + _intensity * 5;
    }
    void restore() override {
        what->setText(_what);
        frequency->setCurrentIndex(_frequency);
        intensity->setCurrentIndex(_intensity);
    }
    void store() override {
        _what      = what->text();
        _frequency = frequency->currentIndex();
        _intensity = intensity->currentIndex();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]         = "Psychological Complication";
        obj["frequency"]    = _frequency;
        obj["intensity"]    = _intensity;
        obj["what"]         = _what;
        return obj;
    }

private:
    int     _frequency = -1;
    int     _intensity = -1;
    QString _what = "";

    QComboBox* frequency;
    QComboBox* intensity;
    QLineEdit* what;
};

#endif // PSYCHOLOGICALCOMPLICATION_H
