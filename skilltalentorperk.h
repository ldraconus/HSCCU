#ifndef SKILLTALENTORPERK_H
#define SKILLTALENTORPERK_H

#include "shared.h"

#include <functional>

#include <QCheckBox>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QUuid>
#include <QVBoxLayout>

#include "combobox.h"

class SkillTalentOrPerk {
private:
    QWidget* mSender {};

protected:
    using BoolCallback   = std::function<void (SkillTalentOrPerk*, bool)>;
    using IntCallback    = std::function<void (SkillTalentOrPerk*, int)>;
    using StringCallback = std::function<void (SkillTalentOrPerk*, QString)>;

    QCheckBox* createCheckBox(QWidget*, QVBoxLayout*, QString, BoolCallback);
    QCheckBox* createCheckBox(QWidget*, QVBoxLayout*, QString);
    ComboBox*  createComboBox(QWidget*, QVBoxLayout*, QString, QList<QString>, IntCallback);
    ComboBox*  createComboBox(QWidget*, QVBoxLayout*, QString, QList<QString>);
    QLabel*    createLabel(QWidget*, QVBoxLayout*, QString);
    QLineEdit* createLineEdit(QWidget*, QVBoxLayout*, QString, StringCallback);
    QLineEdit* createLineEdit(QWidget*, QVBoxLayout*, QString);

    QMap<QCheckBox*, BoolCallback>   mCallbacksCB;
    QMap<ComboBox*,  IntCallback>    mCallbacksCBox;
    QMap<QLineEdit*, StringCallback> mCallbacksEdit;

    void empty(bool)      { }

    QString add(QString n, int p) {
        QList<QString> vals = n.split("/");
        QString res;
        bool first = true;
        for (int i = 0; i < vals.count(); ++i) {
            auto& val = vals[i];
            if (first) first = false;
            else res += "/";
            QList<QString> num = val.split("-");
            res += QString("%1-").arg(num[0].toInt(0) + p);
        }
        return res;
    }

    static constexpr auto                Access = "access";
    static constexpr auto     AccurateSprayfire = "accurate sprayfire";
    static constexpr auto                Amount = "amount";
    static constexpr auto                 Anlze = "anlze";
    static constexpr auto                  Area = "area";
    static constexpr auto                 AsInt = "int";
    static constexpr auto                    As = "as";
    static constexpr auto                  Base = "base";
    static constexpr auto             Chokehold = "chokehold";
    static constexpr auto                  Circ = "circ";
    static constexpr auto ConcentratedSprayfire = "concentrated sprayfire";
    static constexpr auto              Contacts = "contacts";
    static constexpr auto                  Cost = "cost";
    static constexpr auto                Custom = "custom";
    static constexpr auto       DefensiveStrike = "defensive strike";
    static constexpr auto                 Descr = "descr";
    static constexpr auto                  Dtct = "dtct";
    static constexpr auto    ExtraDamageClasses = "extra damage classes";
    static constexpr auto                 Extra = "extra";
    static constexpr auto                   For = "for";
    static constexpr auto                  Guid = "id";
    static constexpr auto                  Hide = "hide";
    static constexpr auto               IntRoll = "introll";
    static constexpr auto                Intuit = "intuit";
    static constexpr auto               Killing = "killing";
    static constexpr auto         KillingStrike = "killing strike";
    static constexpr auto                 Known = "known";
    static constexpr auto              Legsweep = "legsweep";
    static constexpr auto                 Level = "level";
    static constexpr auto                Levels = "levels";
    static constexpr auto                 Limit = "limit";
    static constexpr auto               Limited = "limited";
    static constexpr auto              Literate = "literate";
    static constexpr auto          MartialBlock = "martial block";
    static constexpr auto         MartialDisarm = "martial disarm";
    static constexpr auto          MartialDodge = "martial dodge";
    static constexpr auto         MartialEscape = "martial escape";
    static constexpr auto           MartialGrab = "martial grab";
    static constexpr auto         MartialStrike = "martial strike";
    static constexpr auto          MartialThrow = "martial throw";
    static constexpr auto                  Mult = "mult";
    static constexpr auto                  Name = "name";
    static constexpr auto           NerveStrike = "nerve strike";
    static constexpr auto       OffensiveStrike = "offensive strike";
    static constexpr auto               Offhand = "offhand";
    static constexpr auto                   Org = "org";
    static constexpr auto         PassingStrike = "passing strike";
    static constexpr auto                   Pen = "pen";
    static constexpr auto                  Plus = "plus";
    static constexpr auto                  Pnts = "pnts";
    static constexpr auto                   Put = "put";
    static constexpr auto                Ranged = "ranged";
    static constexpr auto         RapidAutofire = "rapid autofire";
    static constexpr auto                Relate = "relate";
    static constexpr auto         SacrifceThrow = "sacrifce throw";
    static constexpr auto                 Sense = "sense";
    static constexpr auto                  Size = "size";
    static constexpr auto     SkipoverSprayfire = "skipover sprayfire";
    static constexpr auto                  Stat = "stat";
    static constexpr auto                 Topic = "topic";
    static constexpr auto                 Thrgh = "thrgh";
    static constexpr auto                  Type = "type";
    static constexpr auto                Useful = "useful";
    static constexpr auto                 Value = "value";
    static constexpr auto                Versus = "versus";
    static constexpr auto        WeaponElements = "weapon elements";
    static constexpr auto               Weapons = "weapons";
    static constexpr auto                  What = "what";
    static constexpr auto                 Which = "which";
    static constexpr auto                   Who = "who";
    static constexpr auto                  With = "with";
    static constexpr auto                  Wpns = "wpns";

public:
    class skillBase {
    public:
        skillBase() = default;

        virtual shared_ptr<SkillTalentOrPerk> create()                  = 0;
        virtual shared_ptr<SkillTalentOrPerk> create(QJsonObject& json) = 0;
    };

    template <typename T>
    class skill: public skillBase {
    public:
        skill() = default;
\
        shared_ptr<SkillTalentOrPerk> create() override                  { auto x = make_shared<T>();     x->id();     return x; }
        shared_ptr<SkillTalentOrPerk> create(QJsonObject& json) override { auto x = make_shared<T>(json); x->id(json); return x; }
    };

    class perkBase {
    public:
        perkBase() = default;
        virtual shared_ptr<SkillTalentOrPerk> create()                  = 0;
        virtual shared_ptr<SkillTalentOrPerk> create(QJsonObject& json) = 0;
    };

    template <typename T>
    class perk: public perkBase {
    public:
        perk() = default;

        shared_ptr<SkillTalentOrPerk> create() override                  { auto x = make_shared<T>();     x->id();     return x; }
        shared_ptr<SkillTalentOrPerk> create(QJsonObject& json) override { auto x = make_shared<T>(json); x->id(json); return x; }
    };

    class talentBase {
    public:
        talentBase() = default;

        virtual shared_ptr<SkillTalentOrPerk> create()                  = 0;
        virtual shared_ptr<SkillTalentOrPerk> create(QJsonObject& json) = 0;
    };

    template <typename T>
    class talent: public talentBase {
    public:
        talent() = default;

        shared_ptr<SkillTalentOrPerk> create() override                  { auto x = make_shared<T>();     x->id();     return x; }
        shared_ptr<SkillTalentOrPerk> create(QJsonObject& json) override { auto x = make_shared<T>(json); x->id(json); return x; }
    };

    class enhancerBase {
    public:
        enhancerBase() = default;

        virtual shared_ptr<SkillTalentOrPerk> create()                  = 0;
        virtual shared_ptr<SkillTalentOrPerk> create(QJsonObject& json) = 0;
    };

    template <typename T>
    class enhancer: public enhancerBase {
    public:
        enhancer() = default;

        shared_ptr<SkillTalentOrPerk> create() override                  { auto x = make_shared<T>();     x->id();     return x; }
        shared_ptr<SkillTalentOrPerk> create(QJsonObject& json) override { auto x = make_shared<T>(json); x->id(json); return x; }
    };

    SkillTalentOrPerk()                  { id(); }
    SkillTalentOrPerk(QJsonObject& json) { id(json); }

    static const bool NoStore = true;
    static const bool ShowRoll = true;

    virtual QString     abbreviation(bool roll = false) { return description(roll); }
    virtual QString     description(bool roll = false) = 0;
    virtual bool        form(QWidget*, QVBoxLayout*)   = 0;
    virtual QString     name()                         = 0;
    virtual Points      points(bool noStore = false)   = 0;
    virtual void        restore()                      = 0;
    virtual QString     roll()                         = 0;
    virtual void        store()                        = 0;
    virtual bool        isSkill()                      { return false; }
    virtual bool        isPerk()                       { return false; }
    virtual bool        isTalent()                     { return false; }
    virtual int         rED()                          { return 0; }
    virtual int         rPD()                          { return 0; }
    virtual int         place()                        { return 1; }

    virtual void        checked(bool)    { }
    virtual void        numeric(QString) { }
    virtual QJsonObject toJson()         { QJsonObject obj; obj["id"] = mGuid; return obj; }

    QWidget* sender() const { return mSender; }
    QString  id() const     { return mGuid; }

    void callback(QCheckBox*);
    void callback(QLineEdit*);
    bool createForm(QWidget*, QVBoxLayout*);

    static QList<QString>      Available();
    static void                ClearForm(QVBoxLayout*);
    static QList<QString>      SkillsAvailable();
    static QList<QString>      TalentsAvailable();
    static QList<QString>      PerksAvailable();

    static shared_ptr<SkillTalentOrPerk> ByName(QString);
    static shared_ptr<SkillTalentOrPerk> FromJson(QString, QJsonObject&);

    bool isNumber(QString);

    static constexpr int BaseRoll = 11;

protected:
    QString mGuid;

    void    id(const QJsonObject json) { mGuid = json["id"].toString(); if (mGuid.isEmpty()) mGuid = QUuid::createUuid().toString(QUuid::WithoutBraces); }

private:
    static QMap<QString, skillBase*>    sSkills;    // NOLINT
    static QMap<QString, talentBase*>   sTalents;   // NOLINT
    static QMap<QString, perkBase*>     sPerks;     // NOLINT
    static QMap<QString, enhancerBase*> sEnhancers; // NOLINT
};

#endif // SKILLTALENTORPERK_H
