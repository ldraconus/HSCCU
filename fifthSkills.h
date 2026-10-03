#pragma once

#include <QUuid>

#ifndef ISHSC
#include "sheet.h"
#endif
#include "skilltalentorperk.h"
#include "5th.h"

static constexpr auto* Abbreviation = "abbreviation";
static constexpr auto* Description  = "description";
static constexpr auto* Form         = "form";
static constexpr auto* Cost         = "points";
static constexpr auto* Restore      = "restore";
static constexpr auto* Roll         = "roll";
static constexpr auto* Store        = "store";
static constexpr auto* IsSkill      = "isSkill";
static constexpr auto* IsPerk       = "isPerk";
static constexpr auto* IsTalent     = "isTalent";
static constexpr auto* RED          = "rED";
static constexpr auto* RPD          = "rPD";
static constexpr auto* Checked      = "checked";
static constexpr auto* Numeric      = "numeric";

class FifthSkills: public SkillTalentOrPerk {
public:
    FifthSkills(): SkillTalentOrPerk() { }
    FifthSkills(QString name)
        : SkillTalentOrPerk() { init(name); }
    FifthSkills(QJsonObject& json)
        : SkillTalentOrPerk(json) { init(json[Name].toString(""), json); }
    FifthSkills(QUrl filename);

    QString abbreviation(bool roll = !ShowRoll) override { Sheet::ref().vm().user().push(roll); QString s = String(Abbreviation); return s.isEmpty() ? description() : s; }
    QString description(bool roll = !ShowRoll) override  { Sheet::ref().vm().user().push(roll); return String(Description); }
    bool    form(QWidget* w, QVBoxLayout* l) override    { auto u = Sheet::ref().vm().user(); u.push(fifth::num(w)); u.push(fifth::num(l)); return Bool(Form); }
    QString name() override                              { return sGuidMap[v.mGuid]; }
    Points  points(bool noStore = !NoStore) override     { Sheet::ref().vm().user().push(noStore); return PntCost(Cost); }
    void    restore() override                           { return Void(Restore); }
    QString roll() override                              { return String(Roll); }
    void    store() override                             { return Void(Store); }
    bool    isSkill() override                           { return Bool(IsSkill); }
    bool    isPerk() override                            { return Bool(IsPerk); }
    bool    isTalent() override                          { return Bool(IsTalent); }
    int     rED() override                               { return Int(RED); }
    int     rPD() override                               { return Int(RPD); }
    void    numeric(QString) override                    { return Void(Numeric); }

    QJsonObject toJson() override;

private:
    struct vars {
        QString mGuid;
    } v;

    auto call(const QString& symbol) {
        auto& vm = Sheet::ref().vm();
        auto bag = vm.bag(sGuidMap[v.mGuid]);
        if (bag.contains(symbol)) bag[symbol].asCallable()->eval(&vm);
        else vm.user().push(fifth::exe(nullptr));
    }

    QString String(const QString& symbol)  { call(symbol); return Sheet::ref().vm().user().pop().asString().str(); }
    bool    Bool(const QString& symbol)    { call(symbol); return Sheet::ref().vm().user().pop().asNumber(); }
    Points  PntCost(const QString& symbol) { call(symbol); return Points(Sheet::ref().vm().user().pop().asNumber()); }
    void    Void(const QString& symbol)    { call(symbol); }
    int     Int(const QString& symbol)     { call(symbol); return Sheet::ref().vm().user().pop().asNumber(); }

    void init(QString name, QJsonObject json = { }) {
        static bool vmInitialized = false;
        if (!vmInitialized) vmInitialized = initializeVM();
        auto& vm = Sheet::ref().vm();
        auto guid = QUuid::createUuid().toString();
        v.mGuid = guid;
        sGuidMap[guid] = name;
        vm.createBag(guid);
        auto keys = json.keys();
        vm.set(guid, Name, fifth::str(name));
        for (const auto& key: std::as_const(keys)) {
            if (json[key].isString()) vm.set(guid, key, fifth::str(json[key].toString()));
            else vm.set(guid, key, json[key].toInt());
        }
    }

    void fromJson(QJsonDocument& doc);
    bool initializeVM();
    bool compile(QJsonObject& obj, const QString& namne, const QString& symbol);

    static QMap<QString, QString> sGuidMap;
};
