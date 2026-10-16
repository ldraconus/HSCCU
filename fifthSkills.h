#pragma once

#include <QUuid>

#ifdef QT_DEBUG
#include "debugdialog.h"
#endif

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
        : v(fs.v) { init(fs.v.mName); }
    FifthSkills(const FifthSkills& fs, QJsonObject& json)
        : v(fs.v) { init(Name, json); }
    FifthSkills(QUrl& filename);

    void load(QJsonObject& json) { init(json[Name].toString(""), json); }
    bool valid()                 { return v.mValid; }

    QString name() override                              { return v.mName; }
    Points  points(bool noStore = !NoStore) override     { if (!noStore) store(); auto& u = Sheet::ref().vm().user(); u.push(fifth::exe(this)); u.pop(); return PntCost(TheCost); }
    void    restore() override                           { Sheet::ref().vm().user().push(fifth::exe(this)); return Void(Restore); }
    QString roll() override                              { Sheet::ref().vm().user().push(fifth::exe(this)); return String(Roll); }
    void    store() override                             { Sheet::ref().vm().user().push(fifth::exe(this)); return Void(Store);  }
    bool    isSkill() override                           { Sheet::ref().vm().user().push(fifth::exe(this)); return Bool(IsSkill); }
    bool    isPerk() override                            { Sheet::ref().vm().user().push(fifth::exe(this)); return Bool(IsPerk); }
    bool    isTalent() override                          { Sheet::ref().vm().user().push(fifth::exe(this));; return Bool(IsTalent); }
    int     rED() override                               { Sheet::ref().vm().user().push(fifth::exe(this)); return Int(RED); }
    int     rPD() override                               { Sheet::ref().vm().user().push(fifth::exe(this)); return Int(RPD); }

    QString strDebug(const QString& sym, int num = -1) {
#ifdef QT_DEBUG
        auto& u = Sheet::ref().vm().user();
        u.push(fifth::exe(this));
        auto& vm = Sheet::ref().vm();
        if (v.mBag.contains(sym)) {
            if (num >= 0) u.push(num >= 0);
            auto s = v.mBag[sym];
            auto c = s.asCallable();
            dbg = new DebugDialog(&vm, c, v.mBag, &Sheet::ref());
            dbg->exec();
            delete dbg;
        } else {
            if (num >= 0) u.pop();
            u.pop();
            return "";
        }
        QString val = u.pop().asString().str();
        if (num >= 0) u.pop();
        u.pop();
        return val;
#else
        return "";
#endif
    }

    QString abbreviation(bool roll = !ShowRoll) override {
#ifdef QT_DEBUG
        auto str = strDebug(Abbreviation, roll);
        if (str.isEmpty()) return description(roll);
        else return str;
#else
        auto& u = Sheet::ref().vm().user();
        u.push(fifth::exe(this));
        QString s = String(Abbreviation, roll);
        return s.isEmpty() ? description(roll) : s;
#endif
    }

    QString description(bool roll = !ShowRoll) override  {
#ifdef QT_DEBUG
        return strDebug(Description, roll);
#else
        auto& u = Sheet::ref().vm().user();
        u.push(fifth::exe(this));
        u.push(roll);
        return String(Description);
#endif
    }

    bool form(QWidget* w, QVBoxLayout* l) override {
        v.mLayout = l;
        v.mWidget = w;
        auto& u = Sheet::ref().vm().user();
        u.push(fifth::exe(this));

        Void(Form);
        return true;
    }

    QJsonObject toJson() override;

    struct vars {
        QMap<QString, fifth::value> mBag;
        QVBoxLayout*                mLayout = nullptr;
        QString                     mName;
        bool                        mValid = false;
        QWidget*                    mWidget = nullptr;
    } v;

#ifdef QT_DEBUG
    DebugDialog* dbg { nullptr };
#endif

    void         set(const QString& fld, fifth::value val) { v.mBag[fld] = val; }
    fifth::value get(const QString& fld)                   { return v.mBag[fld]; }

private:
    auto call(const QString& symbol) {
        auto& vm = Sheet::ref().vm();
        auto s = v.mBag[symbol];
        auto c = s.asCallable();
        c->eval(&vm);
    }

    Points  PntCost(const QString& symbol) {
        auto& vm = Sheet::ref().vm();
        auto& u = vm.user();
        call(symbol);
        auto p = Points(u.pop().asNumber());
        u.pop();
        return p;
    }

    void Void(const QString& symbol) {
        call(symbol);
        Sheet::ref().vm().user().pop();
    }

    int Int(const QString& symbol) {
        auto& vm = Sheet::ref().vm();
        auto& u = vm.user();
        call(symbol);
        auto i = u.pop().asNumber();
        u.pop();
        return i;
    }

    QString String(const QString& symbol, int num = -1) {
        auto& vm = Sheet::ref().vm();
        auto& u = vm.user();
        if (v.mBag.contains(symbol)) {
            if (num >= 0) u.push(num);
            call(symbol);
        } else { u.pop(); u.pop(); return ""; }
        QString val = u.pop().asString().str();
        if (num >= 0) u.pop();
        u.pop();
        return val;
    }

    bool Bool(const QString& symbol) {
        auto& vm = Sheet::ref().vm();
        auto& u = vm.user();
        if (v.mBag.contains(symbol)) call(symbol);
        else {
            u.pop();
            return false;
        }
        bool f = Sheet::ref().vm().user().pop().asNumber();
        u.pop();
        return f;
    }

    void init(QString name, QJsonObject json = { }) {
        auto& vm = Sheet::ref().vm();
        v.mName = name;
        auto keys = json.keys();
        for (const auto& key: std::as_const(keys)) {
            if (json[key].isString()) set(key, fifth::str(json[key].toString()));
            else set(key, json[key].toInt());
        }
    }

    void fromJson(QJsonDocument& doc);
    bool initializeVM();
    bool compile(QJsonObject& obj, const QString& namne, const QString& symbol);
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

    shared_ptr<SkillTalentOrPerk> create() override                  { auto x = std::make_shared<FifthSkills>(*mSkill);
                                                                       return x; }
    shared_ptr<SkillTalentOrPerk> create(QJsonObject& json) override { auto x = std::make_shared<FifthSkills>(*mSkill, json);
                                                                       x->id(json);
                                                                       return x; }

    QString name()  { return mSkill->name(); }
    bool    valid() { return mSkill->valid(); }

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

