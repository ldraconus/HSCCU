#ifndef ACCIDENTALCHANGE_H
#define ACCIDENTALCHANGE_H

#include "complication.h"

class AccidentalChange: public Complication
{
public:
    AccidentalChange(): Complication() { }
    AccidentalChange(const AccidentalChange& ac)
        : Complication()
        , _circumstance(ac._circumstance)
        , _frequency(ac._frequency)
        , _what(ac._what) { }
    AccidentalChange(AccidentalChange&& ac)
        : Complication()
        , _circumstance(ac._circumstance)
        , _frequency(ac._frequency)
        , _what(ac._what) { }
    AccidentalChange(const QJsonObject& json)
        : Complication()
        , _circumstance(json["circumstance"].toInt(0))
        , _frequency(json["frequence"].toInt(0))
        , _what(json["what"].toString("")) { }

    AccidentalChange& operator=(const AccidentalChange& ac) {
        if (this != &ac) {
            _circumstance = ac._circumstance;
            _frequency    = ac._frequency;
            _what         = ac._what;
        }
        return *this;
    }
    AccidentalChange& operator=(AccidentalChange&& ac) {
        _circumstance = ac._circumstance;
        _frequency    = ac._frequency;
        _what         = ac._what;
        return *this;
    }

    QString description() override {
        static QList<QString> circ { "Uncommon", "Common", "Very Common" };
        static QList<QString> freq { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)", "Always" };
        if (_frequency == -1 || _circumstance == -1 || _what.isEmpty()) return "<incomplete>";
        return QString("Accidental Change: %1 (%2; %3)").arg(_what, circ[_circumstance], freq[_frequency]);
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        what         = createLineEdit(parent, layout, "What sets off the change?");
        circumstance = createComboBox(parent, layout, "How common is the change", { "Uncommmon", "Common", "Very Common" });
        frequency    = createComboBox(parent, layout, "How often do you change", { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)", "Always" });
    }
    int points() override {
        store();
        return _circumstance * 5 + 5 + _frequency * 5;
    }
    void restore() override {
        what->setText(_what);
        circumstance->setCurrentIndex(_circumstance);
        frequency->setCurrentIndex(_frequency);
    }
    void store() override {
        _what = what->text();
        _circumstance = circumstance->currentIndex();
        _frequency = frequency->currentIndex();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]         = "Accidental Change";
        obj["circumstance"] = _circumstance;
        obj["frequence"]    = _frequency;
        obj["what"]         = _what;
        return obj;
    }

private:
    int     _circumstance = -1;
    int     _frequency = -1;
    QString _what = "";

    QComboBox* circumstance;
    QComboBox* frequency;
    QLineEdit* what;
};

#endif // ACCIDENTALCHANGE_H
