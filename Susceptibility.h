#ifndef SUSCEPTIBILITY_H
#define SUSCEPTIBILITY_H

#include "complication.h"

class Susceptibility: public Complication {
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
        if (v.mFrequency < 0 || v.mEvery < 0 || v.mDice < 0 || v.mProximity < 0 || v.mWhat.isEmpty()) return "<incomplete>";
        return QString("Susceptibility: %1 (%2; %3%4; %5)").arg(v.mWhat, freq[v.mFrequency], dice[v.mDice], evry[v.mEvery], prxm[v.mProximity]);
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        what      = createLineEdit(parent, layout, "What are you susceptible to?");
        frequency = createComboBox(parent, layout, "How common is it?", { "Uncommon", "Common", "Very Common" });
        every     = createComboBox(parent, layout, "How often do you take damage?", { "Effect is Instant", "Segment", "Phase", "Turn", "1 Minute", "5 Minutes",
                                                                                      "20 Minutes", "1 Hour", "6 Hours", "1 Day" });
        dice      = createComboBox(parent, layout, "How many dice of damage?", { "1d6", "2d6", "3d6" });
        proximity = createComboBox(parent, layout, "How close do you have to be?", { "Within 8m", "Touching Skin", "Ingested/Injected" });
    }
    Points points(bool noStore = false) override {
        if (!noStore) store();
        return v.mEvery == 0 ? 0_cp : (20_cp - v.mEvery * 5) + (v.mFrequency + 1) * 5_cp + (v.mDice - 1) * 5_cp - v.mProximity * 5_cp;
    }
    void restore() override {
        vars s = v;
        what->setText(v.mWhat);
        dice->setCurrentIndex(v.mDice);
        every->setCurrentIndex(v.mEvery);
        frequency->setCurrentIndex(v.mFrequency);
        proximity->setCurrentIndex(v.mProximity);
        v = s;
    }
    void store() override {
        v.mWhat      = what->text();
        v.mDice      = dice->currentIndex();
        v.mEvery     = every->currentIndex();
        v.mFrequency = frequency->currentIndex();
        v.mProximity = proximity->currentIndex();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]      = "Susceptibility";
        obj["dice"]      = v.mDice;
        obj["every"]     = v.mEvery;
        obj["frequency"] = v.mFrequency;
        obj["proximity"] = v.mProximity;
        obj["what"]      = v.mWhat;
        return obj;
    }

private:
    struct vars {
        int     mDice = -1;
        int     mEvery = -1;
        int     mFrequency = -1;
        int     mProximity = -1;
        QString mWhat = "";
    } v;

    QComboBox* dice;
    QComboBox* every;
    QComboBox* frequency;
    QComboBox* proximity;
    QLineEdit* what;
};

#endif // SUSCEPTIBILITY_H
