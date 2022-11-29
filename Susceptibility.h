#ifndef SUSCEPTIBILITY_H
#define SUSCEPTIBILITY_H

#include "complication.h"

class Susceptibility: public Complication
{
public:
    Susceptibility(): Complication() { }
    Susceptibility(const Susceptibility& ac)
        : Complication()
        , _dice(ac._dice)
        , _every(ac._every)
        , _frequency(ac._frequency)
        , _proximity(ac._proximity)
        , _what(ac._what) { }
    Susceptibility(Susceptibility&& ac)
        : Complication()
        , _dice(ac._dice)
        , _every(ac._every)
        , _frequency(ac._frequency)
        , _proximity(ac._proximity)
        , _what(ac._what) { }
    Susceptibility(const QJsonObject& json)
        : Complication()
        , _dice(json["dice"].toInt(0))
        , _every(json["every"].toInt(0))
        , _frequency(json["frequency"].toInt(0))
        , _proximity(json["proximity"].toInt(0))
        , _what(json["what"].toString("")) { }

    Susceptibility& operator=(const Susceptibility& ac) {
        if (this != &ac) {
            _dice      = ac._dice;
            _every     = ac._every;
            _frequency = ac._frequency;
            _proximity = ac._proximity;
            _what      = ac._what;
        }
        return *this;
    }
    Susceptibility& operator=(Susceptibility&& ac) {
        _dice      = ac._dice;
        _every     = ac._every;
        _frequency = ac._frequency;
        _proximity = ac._proximity;
        _what      = ac._what;
        return *this;
    }

    QString description() override {
        static QList<QString> dice { "1d6", "2d6", "3d6" };
        static QList<QString> freq { "Uncommon", "Common", "Very Common" };
        static QList<QString> evry { " Instantly", "/Segment", "/Phase", "/Turn", "/Minute", "/5 Minutes",
                                     "/20 Minutes", "/Hour", "/6 Hours", "/Day" };
        static QList<QString> prxm { "within 8m", "contact", "internal" };
        if (_frequency == -1 || _every == -1 || _dice == -1 || _proximity == -1 || _what.isEmpty()) return "<incomplete>";
        return QString("Susceptibility: %1 (%2; %3%4; %5)").arg(_what, freq[_frequency], dice[_dice], evry[_every], prxm[_proximity]);
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        what      = createLineEdit(parent, layout, "What are you susceptible to?");
        frequency = createComboBox(parent, layout, "How common is it?", { "Uncommon", "Common", "Uncommon" });
        every     = createComboBox(parent, layout, "How often do you take damage?", { "Effect is Instant", "Segment", "Phase", "Turn", "1 Minute", "5 Minutes",
                                                                                      "20 Minutes", "1 Hour", "6 Hours", "1 Day" });
        dice      = createComboBox(parent, layout, "How many dice of damage?", { "1d6", "2d6", "3d6" });
        proximity = createComboBox(parent, layout, "How close do you have to be?", { "Within 8m", "Touching Skin", "Ingested/Injected" });
    }
    int points(bool noStore = false) override {
        if (!noStore) store();
        return (_every == 0 ? 0 : (15 - (_every - 1) * 5)) * 5 + (_frequency + 1) * 5 + _dice * 5 - _proximity * 5;
    }
    void restore() override {
        what->setText(_what);
        dice->setCurrentIndex(_dice);
        every->setCurrentIndex(_every);
        frequency->setCurrentIndex(_frequency);
        proximity->setCurrentIndex(_proximity);
    }
    void store() override {
        _what      = what->text();
        _dice      = dice->currentIndex();
        _every     = every->currentIndex();
        _frequency = frequency->currentIndex();
        _proximity = proximity->currentIndex();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]      = "Susceptibility";
        obj["dice"]      = _dice;
        obj["every"]     = _every;
        obj["frequency"] = _frequency;
        obj["proximity"] = _proximity;
        obj["what"]      = _what;
        return obj;
    }

private:
    int     _dice = -1;
    int     _every = -1;
    int     _frequency = -1;
    int     _proximity = -1;
    QString _what = "";

    QComboBox* dice;
    QComboBox* every;
    QComboBox* frequency;
    QComboBox* proximity;
    QLineEdit* what;
};

#endif // SUSCEPTIBILITY_H
