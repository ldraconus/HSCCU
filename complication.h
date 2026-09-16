#ifndef COMPLICATION_H
#define COMPLICATION_H

#include "shared.h"

#include <functional>

#include <QCheckBox>
#include <QComboBox>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QVBoxLayout>

class Complication {
protected:
    using BoolCallback   = std::function<void (Complication*, bool)>;
    using IntCallback    = std::function<void (Complication*, int)>;
    using StringCallback = std::function<void (Complication*, QString)>;

    QCheckBox* createCheckBox(QWidget*, QVBoxLayout*, QString, BoolCallback);
    QCheckBox* createCheckBox(QWidget*, QVBoxLayout*, QString);
    QComboBox* createComboBox(QWidget*, QVBoxLayout*, QString, QList<QString>, IntCallback);
    QComboBox* createComboBox(QWidget*, QVBoxLayout*, QString, QList<QString>);
    QLabel*    createLabel(QWidget*, QVBoxLayout*, QString);
    QLineEdit* createLineEdit(QWidget*, QVBoxLayout*, QString, StringCallback);
    QLineEdit* createLineEdit(QWidget*, QVBoxLayout*, QString);

    static constexpr auto      Addiction = "addiction";
    static constexpr auto         Amount = "amount";
    static constexpr auto         Attack = "attack";
    static constexpr auto   Capabilities = "capabilities";
    static constexpr auto         Chance = "chance";
    static constexpr auto   Circumstance = "circumstance";
    static constexpr auto Concealability = "concealability";
    static constexpr auto     Competence = "competence";
    static constexpr auto     Detectable = "detectable";
    static constexpr auto         Damage = "damage";
    static constexpr auto           Dice = "dice";
    static constexpr auto           Easy = "easy";
    static constexpr auto        Effects = "effects";
    static constexpr auto          Every = "every";
    static constexpr auto        Extreme = "extreme";
    static constexpr auto      Frequency = "frequency";
    static constexpr auto             Id = "id";
    static constexpr auto        Impairs = "impairs";
    static constexpr auto      Intensity = "intensity";
    static constexpr auto        Limited = "limited";
    static constexpr auto     Motivation = "motivation";
    static constexpr auto      Multiples = "multiples";
    static constexpr auto NotDistinctive = "notDistinctive";
    static constexpr auto NotRestrictive = "notRestrictive";
    static constexpr auto           Name = "name";
    static constexpr auto            NCI = "nci";
    static constexpr auto      Proximity = "proximity";
    static constexpr auto         Rarity = "rarity";
    static constexpr auto       Reaction = "reaction";
    static constexpr auto         Regain = "regain";
    static constexpr auto           Roll = "roll";
    static constexpr auto       TimeStep = "time step";
    static constexpr auto           Type = "type";
    static constexpr auto        Unaware = "unaware";
    static constexpr auto         Useful = "useful";
    static constexpr auto       Weakness = "weakness";
    static constexpr auto           What = "what";
    static constexpr auto          Where = "where";
    static constexpr auto            Who = "who";

private:
    QMap<QCheckBox*, BoolCallback>   mCallbacksCB;
    QMap<QComboBox*, IntCallback>    mCallbacksCBox;
    QMap<QLineEdit*, StringCallback> mCallbacksEdit;

    void empty(bool) { }

public:
    Complication() { id({ }); }
    Complication(const QJsonObject& obj) { id(obj); }

    static const bool NoStore = true;

    virtual QString     abbreviation() { return description(); }
    virtual QString     description()                = 0;
    virtual void        form(QWidget*, QVBoxLayout*) = 0;
    virtual Points      points(bool noStore = false) = 0;
    virtual void        restore()                    = 0;
    virtual void        store()                      = 0;

    virtual void        checked(bool) { }
    virtual void        numeric(QString) { }
    virtual QJsonObject toJson() {
        QJsonObject obj;
        obj[Id] = mGuid;
        return obj;
    }

    QString id() { return mGuid; }

    void callback(QCheckBox*);
    void callback(QLineEdit*);
    void createForm(QWidget*, QVBoxLayout*);

    static QList<QString>           Available();
    static shared_ptr<Complication> ByIndex(int);
    static shared_ptr<Complication> FromJson(QString, const QJsonObject&);

    bool isNumber(QString);

protected:
    QString mGuid;

    void id(const QJsonObject& json) { mGuid = json["id"].toString(); if (mGuid.isEmpty()) mGuid = QUuid::createUuid().toString(QUuid::WithoutBraces); }
};

class BlankComp: public Complication {
public:
    BlankComp() = default;
    BlankComp(const QJsonObject& obj)
        : Complication(obj) { }

    QString description() override               { return "-"; }
    void form(QWidget*, QVBoxLayout*) override   { throw "No Form"; }
    Points points(bool) override                 { return 0_cp; }
    void restore() override                      { }
    void store() override                        { }
    QJsonObject toJson() override {
        QJsonObject obj = Complication::toJson();
        obj[Name] = "Blank Line";
        return obj;
    }


};

#endif // COMPLICATION_H
