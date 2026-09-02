#ifndef SKILLTALENTORPERK_H
#define SKILLTALENTORPERK_H

#include "shared.h"

#include <functional>

#include <QCheckBox>
#include <QComboBox>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QUuid>
#include <QVBoxLayout>

class SkillTalentOrPerk {
private:
    QWidget* mSender {};

protected:
    using BoolCallback   = std::function<void (SkillTalentOrPerk*, bool)>;
    using IntCallback    = std::function<void (SkillTalentOrPerk*, int)>;
    using StringCallback = std::function<void (SkillTalentOrPerk*, QString)>;

    QCheckBox* createCheckBox(QWidget*, QVBoxLayout*, QString, BoolCallback);
    QCheckBox* createCheckBox(QWidget*, QVBoxLayout*, QString);
    QComboBox* createComboBox(QWidget*, QVBoxLayout*, QString, QList<QString>, IntCallback);
    QComboBox* createComboBox(QWidget*, QVBoxLayout*, QString, QList<QString>);
    QLabel*    createLabel(QWidget*, QVBoxLayout*, QString);
    QLineEdit* createLineEdit(QWidget*, QVBoxLayout*, QString, StringCallback);
    QLineEdit* createLineEdit(QWidget*, QVBoxLayout*, QString);

    QMap<QCheckBox*, BoolCallback>   mCallbacksCB;
    QMap<QComboBox*, IntCallback>    mCallbacksCBox;
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

public:
    class skillBase {
    public:
        skillBase() = default;

        virtual shared_ptr<SkillTalentOrPerk> create()                        = 0;
        virtual shared_ptr<SkillTalentOrPerk> create(const QJsonObject& json) = 0;
    };

    template <typename T>
    class skill: public skillBase {
    public:
        skill() = default;
\
        shared_ptr<SkillTalentOrPerk> create() override                        { auto x = make_shared<T>();     x->id();     return x; }
        shared_ptr<SkillTalentOrPerk> create(const QJsonObject& json) override { auto x = make_shared<T>(json); x->id(json); return x; }
    };

    class perkBase {
    public:
        perkBase() = default;

        virtual shared_ptr<SkillTalentOrPerk> create()                        = 0;
        virtual shared_ptr<SkillTalentOrPerk> create(const QJsonObject& json) = 0;
    };

    template <typename T>
    class perk: public perkBase {
    public:
        perk() = default;

        shared_ptr<SkillTalentOrPerk> create() override                        { auto x = make_shared<T>();     x->id();     return x; }
        shared_ptr<SkillTalentOrPerk> create(const QJsonObject& json) override { auto x = make_shared<T>(json); x->id(json); return x; }
    };

    class talentBase {
    public:
        talentBase() = default;

        virtual shared_ptr<SkillTalentOrPerk> create()                        = 0;
        virtual shared_ptr<SkillTalentOrPerk> create(const QJsonObject& json) = 0;
    };

    template <typename T>
    class talent: public talentBase {
    public:
        talent() = default;

        shared_ptr<SkillTalentOrPerk> create() override                        { auto x = make_shared<T>();     x->id();     return x; }
        shared_ptr<SkillTalentOrPerk> create(const QJsonObject& json) override { auto x = make_shared<T>(json); x->id(json); return x; }
    };

    class enhancerBase {
    public:
        enhancerBase() = default;

        virtual shared_ptr<SkillTalentOrPerk> create()                        = 0;
        virtual shared_ptr<SkillTalentOrPerk> create(const QJsonObject& json) = 0;
    };

    template <typename T>
    class enhancer: public enhancerBase {
    public:
        enhancer() = default;

        shared_ptr<SkillTalentOrPerk> create() override                        { auto x = make_shared<T>();     x->id();     return x; }
        shared_ptr<SkillTalentOrPerk> create(const QJsonObject& json) override { auto x = make_shared<T>(json); x->id(json); return x; }
    };

    SkillTalentOrPerk() = default;

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
    static shared_ptr<SkillTalentOrPerk> FromJson(QString, const QJsonObject&);

    bool isNumber(QString);

    static constexpr int BaseRoll = 11;

protected:
    QString mGuid;

    void    id(const QJsonObject& json) { mGuid = json["id"].toString(); if (mGuid.isEmpty()) mGuid = QUuid::createUuid().toString(QUuid::WithoutBraces); }

private:
    static QMap<QString, skillBase*>    sSkills;    // NOLINT
    static QMap<QString, talentBase*>   sTalents;   // NOLINT
    static QMap<QString, perkBase*>     sPerks;     // NOLINT
    static QMap<QString, enhancerBase*> sEnhancers; // NOLINT
};

#endif // SKILLTALENTORPERK_H
