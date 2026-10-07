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
static constexpr auto* TheCost      = "points";
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
    FifthSkills(const FifthSkills& fs)
        : v(fs.v) { init(v.mName); }
    FifthSkills(const FifthSkills& fs, QJsonObject& json)
        : v(fs.v) { init(v.mName, json); }
    FifthSkills(QUrl& filename);

    void load(QJsonObject& json) { init(json[Name].toString(""), json); }
    bool valid()                 { return v.mValid; }

    QString abbreviation(bool roll = !ShowRoll) override { auto& u = Sheet::ref().vm().user(); u.push(roll); QString s = String(Abbreviation); return s.isEmpty() ? description(u.pop().asNumber()) : s; }
    QString description(bool roll = !ShowRoll) override  { Sheet::ref().vm().user().push(roll); return String(Description); }
    QString name() override                              { return v.mName; }
    Points  points(bool noStore = !NoStore) override     { if (!noStore) store(); return PntCost(TheCost); }
    void    restore() override                           { return Void(Restore); }
    QString roll() override                              { return String(Roll); }
    void    store() override                             { return Void(Store); }
    bool    isSkill() override                           { return Bool(IsSkill); }
    bool    isPerk() override                            { return Bool(IsPerk); }
    bool    isTalent() override                          { return Bool(IsTalent); }
    int     rED() override                               { return Int(RED); }
    int     rPD() override                               { return Int(RPD); }

    QString myV() { return mGuid; }

    bool form(QWidget* w, QVBoxLayout* l) override {
        auto& u = Sheet::ref().vm().user();
        u.push(fifth::exe(w));
        u.push(fifth::exe(l)); Void(Form);
        return true;
    }

    QJsonObject toJson() override;

private:
    struct vars {
        QString mGuid;
        QString mName;
        bool    mValid = false;
    } v;

    auto call(const QString& symbol) {
        auto& vm = Sheet::ref().vm();
        auto bag = vm.bag(sGuidMap[v.mGuid]);
        bag[symbol].asCallable()->eval(&vm);
    }

    Points  PntCost(const QString& symbol) { call(symbol); return Points(Sheet::ref().vm().user().pop().asNumber()); }
    void    Void(const QString& symbol)    { call(symbol); }
    int     Int(const QString& symbol)     { call(symbol); return Sheet::ref().vm().user().pop().asNumber(); }

    QString String(const QString& symbol, std::function<QString()> def = [](){ return QString(""); }) {
        auto& vm = Sheet::ref().vm();
        auto bag = vm.bag(sGuidMap[v.mGuid]);
        if (bag.contains(symbol)) call(symbol);
        else return def();
        return Sheet::ref().vm().user().pop().asString().str();
    }
    bool Bool(const QString& symbol, std::function<bool()> def = [](){ return false; }) {
        auto& vm = Sheet::ref().vm();
        auto bag = vm.bag(sGuidMap[v.mGuid]);
        if (bag.contains(symbol)) call(symbol);
        else return def();
        return Sheet::ref().vm().user().pop().asNumber();
    }

    void init(QString name, QJsonObject json = { }) {
        auto& vm = Sheet::ref().vm();
        if (!sNameMap.contains(name)) {
            auto guid = QUuid::createUuid().toString();
            v.mGuid = guid;
            sGuidMap[guid] = name;
            sNameMap[name] = guid;
            vm.createBag(guid);
            auto keys = json.keys();
            vm.set(guid, Name, fifth::str(name));
            for (const auto& key: std::as_const(keys)) {
                if (json[key].isString()) vm.set(guid, key, fifth::str(json[key].toString()));
                else vm.set(guid, key, json[key].toInt());
            }
        }
    }

    void fromJson(QJsonDocument& doc);
    bool initializeVM();
    bool compile(QJsonObject& obj, const QString& namne, const QString& symbol);

    static QMap<QString, QString> sGuidMap;
    static QMap<QString, QString> sNameMap;
};

class fifthBase: public SkillTalentOrPerk::skillBase {
public:
    fifthBase() = default;

    shared_ptr<SkillTalentOrPerk> create() override                  { return nullptr; }
    shared_ptr<SkillTalentOrPerk> create(QJsonObject& json) override { return nullptr; }
};

class fifthSkill: public fifthBase {
public:
    fifthSkill() = default;
    fifthSkill(QUrl& url)
        : fifthBase()
        , mSkill(std::make_shared<FifthSkills>(url)) { }

    shared_ptr<SkillTalentOrPerk> create() override                  { auto x = std::make_shared<FifthSkills>(*mSkill);                    return x; }
    shared_ptr<SkillTalentOrPerk> create(QJsonObject& json) override { auto x = std::make_shared<FifthSkills>(*mSkill, json); x->id(json); return x; }

    QString name()  { return mSkill->name(); }
    bool    valid() {
        return mSkill->valid();
    }

private:
    std::shared_ptr<FifthSkills> mSkill;
};

class fifthTBase: public SkillTalentOrPerk::talentBase {
public:
    fifthTBase() = default;

    shared_ptr<SkillTalentOrPerk> create() override                  { return nullptr; }
    shared_ptr<SkillTalentOrPerk> create(QJsonObject& json) override { return nullptr; }
};

class fifthTalent: public fifthTBase {
public:
    fifthTalent() = default;
    fifthTalent(QUrl& url)
        : fifthTBase()
        , mSkill(std::make_shared<FifthSkills>(url)) { }

    shared_ptr<SkillTalentOrPerk> create() override                  { auto x = std::make_shared<FifthSkills>(*mSkill);                    return x; }
    shared_ptr<SkillTalentOrPerk> create(QJsonObject& json) override { auto x = std::make_shared<FifthSkills>(*mSkill, json); x->id(json); return x; }

    QString name()  { return mSkill->name(); }
    bool    valid() { return mSkill->valid(); }

private:
    std::shared_ptr<FifthSkills> mSkill;
};

class fifthPBase: public SkillTalentOrPerk::perkBase {
public:
    fifthPBase() = default;

    shared_ptr<SkillTalentOrPerk> create() override                  { return nullptr; }
    shared_ptr<SkillTalentOrPerk> create(QJsonObject& json) override { return nullptr; }
};

class fifthPerk: public fifthPBase {
public:
    fifthPerk() = default;
    fifthPerk(QUrl& url)
        : fifthPBase()
        , mSkill(std::make_shared<FifthSkills>(url)) { }

    shared_ptr<SkillTalentOrPerk> create() override                  { auto x = std::make_shared<FifthSkills>(*mSkill);                   return x; }
    shared_ptr<SkillTalentOrPerk> create(QJsonObject& json) override { auto x = std::make_shared<FifthSkills>(*mSkill, json); x->id(json); return x; }

    QString name()  { return mSkill->name(); }
    bool    valid() { return mSkill->valid(); }

private:
    std::shared_ptr<FifthSkills> mSkill;
};

