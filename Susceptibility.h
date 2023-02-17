#ifndef SUSCEPTIBILITY_H
#define SUSCEPTIBILITY_H

#include "complication.h"

class Susceptibility: public Complication
{
public:
    Susceptibility(): Complication() { }
    Susceptibility(const Susceptibility& ac)
        : Complication()
        , v(ac.v) { }
    Susceptibility(Susceptibility&& ac)
        : Complication()
        , v(ac.v) { }
    Susceptibility(const QJsonObject& json)
        : Complication()
        , v { json["dice"].toInt(0), json["every"].toInt(0), json["frequency"].toInt(0), json["proximity"].toInt(0), json["what"].toString("") } { }

    Susceptibility& operator=(const Susceptibility& ac) {
        if (this != &ac) v = ac.v;
        return *this;
    }
    Susceptibility& operator=(Susceptibility&& ac) {
        v = ac.v;
        return *this;
    }

    QString description() override {
        static QList<QString> dice { "1d6", "2d6", "3d6" };
        static QList<QString> freq { "Uncommon", "Common", "Very Common" };
        static QList<QString> evry { " Instantly", "/Segment", "/Phase", "/Turn", "/Minute", "/5 Minutes",
                                     "/20 Minutes", "/Hour", "/6 Hours", "/Day" };
        static QList<QString> prxm { "within 8m", "contact", "internal" };
        if (v._frequency < 1 || v._every < 1 || v._dice < 1 || v._proximity < 1 || v._what.isEmpty()) return "<incomplete>";
        return QString("Susceptibility: %1 (%2; %3%4; %5)").arg(v._what, freq[v._frequency], dice[v._dice], evry[v._every], prxm[v._proximity]);
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        what      = createLineEdit(parent, layout, "What are you susceptible to?");
        frequency = createComboBox(parent, layout, "How common is it?", { "Uncommon", "Common", "Very Common" });
        every     = createComboBox(parent, layout, "How often do you take damage?", { "Effect is Instant", "Segment", "Phase", "Turn", "1 Minute", "5 Minutes",
                                                                                      "20 Minutes", "1 Hour", "6 Hours", "1 Day" });
        dice      = createComboBox(parent, layout, "How many dice of damage?", { "1d6", "2d6", "3d6" });
        proximity = createComboBox(parent, layout, "How close do you have to be?", { "Within 8m", "Touching Skin", "Ingested/Injected" });
    }
    Points<> points(bool noStore = false) override {
        if (!noStore) store();
        return (v._every == 0 ? 0_cp : (25_cp - (v._every - 2) * 5_cp)) * 5_cp + (v._frequency + 1) * 5_cp + v._dice * 5_cp - v._proximity * 5_cp;
    }
    void restore() override {
        vars s = v;
        what->setText(v._what);
        dice->setCurrentIndex(v._dice);
        every->setCurrentIndex(v._every);
        frequency->setCurrentIndex(v._frequency);
        proximity->setCurrentIndex(v._proximity);
        v = s;
    }
    void store() override {
        v._what      = what->text();
        v._dice      = dice->currentIndex();
        v._every     = every->currentIndex();
        v._frequency = frequency->currentIndex();
        v._proximity = proximity->currentIndex();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]      = "Susceptibility";
        obj["dice"]      = v._dice;
        obj["every"]     = v._every;
        obj["frequency"] = v._frequency;
        obj["proximity"] = v._proximity;
        obj["what"]      = v._what;
        return obj;
    }

private:
    struct vars {
        int     _dice = -1;
        int     _every = -1;
        int     _frequency = -1;
        int     _proximity = -1;
        QString _what = "";
    } v;

    QComboBox* dice;
    QComboBox* every;
    QComboBox* frequency;
    QComboBox* proximity;
    QLineEdit* what;
};

#endif // SUSCEPTIBILITY_H
