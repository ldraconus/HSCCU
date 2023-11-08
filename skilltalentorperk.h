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
#include <QVBoxLayout>

class SkillTalentOrPerk
{
private:
    QWidget* _sender {};

protected:
    QCheckBox* createCheckBox(QWidget*, QVBoxLayout*, QString, std::_Mem_fn<void (SkillTalentOrPerk::*)(bool)>);
    QCheckBox* createCheckBox(QWidget*, QVBoxLayout*, QString);
    QComboBox* createComboBox(QWidget*, QVBoxLayout*, QString, QList<QString>, std::_Mem_fn<void (SkillTalentOrPerk::*)(int)>);
    QComboBox* createComboBox(QWidget*, QVBoxLayout*, QString, QList<QString>);
    QLabel*    createLabel(QWidget*, QVBoxLayout*, QString);
    QLineEdit* createLineEdit(QWidget*, QVBoxLayout*, QString, std::_Mem_fn<void (SkillTalentOrPerk::*)(QString)> callback);
    QLineEdit* createLineEdit(QWidget*, QVBoxLayout*, QString);

    QMap<QCheckBox*, std::_Mem_fn<void (SkillTalentOrPerk::*)(bool)>>     _callbacksCB;   // NOLINT
    QMap<QComboBox*, std::_Mem_fn<void (SkillTalentOrPerk::*)(int)>>      _callbacksCBox; // NOLINT
    QMap<QLineEdit*, std::_Mem_fn<void (SkillTalentOrPerk::*)(QString)>>  _callbacksEdit; // NOLINT
    void empty(bool)      { }

    QString add(QString n, int p) {
        QList<QString> vals = n.split("/");
        QString res;
        bool first = true;
        for (const auto& val: vals) {
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
        skillBase() { }
        skillBase(const skillBase&) { }
        skillBase(skillBase&&) { }
        skillBase(skillBase*) { }
        virtual ~skillBase() { }

        skillBase& operator=(const skillBase&) = delete;
        skillBase& operator=(skillBase&&) = delete;

        virtual shared_ptr<SkillTalentOrPerk> create()                        = 0;
        virtual shared_ptr<SkillTalentOrPerk> create(const QJsonObject& json) = 0;
    };

    template <typename T>
    class skill: public skillBase {
    public:
        skill(): skillBase() { }
        skill(const skill& b): skillBase(b) { }
        skill(skill&& b): skillBase(b) { }
        skill(skill* b): skillBase(b) { }
        ~skill() override { }

        skill& operator=(const skill& b) = delete;
        skill& operator=(skill&& b) = delete;

        shared_ptr<SkillTalentOrPerk> create() override                        { return make_shared<T>(); }
        shared_ptr<SkillTalentOrPerk> create(const QJsonObject& json) override { return make_shared<T>(json); }
    };

    class perkBase {
    public:
        perkBase() { }
        perkBase(const perkBase&) { }
        perkBase(perkBase&&) { }
        perkBase(perkBase*) { }
        virtual ~perkBase() { }

        perkBase& operator=(const perkBase&) = delete;
        perkBase& operator=(perkBase&&) = delete;

        virtual shared_ptr<SkillTalentOrPerk> create()                        = 0;
        virtual shared_ptr<SkillTalentOrPerk> create(const QJsonObject& json) = 0;
    };

    template <typename T>
    class perk: public perkBase {
    public:
        perk(): perkBase() { }
        perk(const perk& b): perkBase(b) { }
        perk(perk&& b): perkBase(b) { }
        perk(perk* b): perkBase(b) { }
        ~perk() override { }

        perk& operator=(const perk& b) = delete;
        perk& operator=(perk&& b) = delete;

        shared_ptr<SkillTalentOrPerk> create() override                        { return make_shared<T>(); }
        shared_ptr<SkillTalentOrPerk> create(const QJsonObject& json) override { return make_shared<T>(json); }
    };

    class talentBase {
    public:
        talentBase() { }
        talentBase(const talentBase&) { }
        talentBase(talentBase&&) { }
        talentBase(talentBase*) { }
        virtual ~talentBase() { }

        talentBase& operator=(const talentBase&) = delete;
        talentBase&& operator=(talentBase&&) = delete;

        virtual shared_ptr<SkillTalentOrPerk> create()                        = 0;
        virtual shared_ptr<SkillTalentOrPerk> create(const QJsonObject& json) = 0;
    };

    template <typename T>
    class talent: public talentBase {
    public:
        talent(): talentBase() { }
        talent(const talent& b): talentBase(b) { }
        talent(talent&& b): talentBase(b) { }
        talent(talentBase* b): talentBase(b) { }
        ~talent() override {}

        talent& operator=(const talent&) = delete;
        talent& operator=(talent&&) = delete;

        shared_ptr<SkillTalentOrPerk> create() override                        { return make_shared<T>(); }
        shared_ptr<SkillTalentOrPerk> create(const QJsonObject& json) override { return make_shared<T>(json); }
    };

    class enhancerBase {
    public:
        enhancerBase() { }
        enhancerBase(const enhancerBase&) { }
        enhancerBase(enhancerBase&&) { }
        enhancerBase(enhancerBase*) { }
        virtual ~enhancerBase() { }

        enhancerBase& operator=(const enhancerBase&) = delete;
        enhancerBase& operator=(enhancerBase&&) = delete;

        virtual shared_ptr<SkillTalentOrPerk> create()                        = 0;
        virtual shared_ptr<SkillTalentOrPerk> create(const QJsonObject& json) = 0;
    };

    template <typename T>
    class enhancer: public enhancerBase {
    public:
        enhancer(): enhancerBase() { }
        enhancer(const enhancer& b): enhancerBase(b) { }
        enhancer(enhancer&& b): enhancerBase(b) { }
        enhancer(enhancer* b): enhancerBase(b) { }
        ~enhancer() override {}

        enhancer& operator=(const enhancer&) = delete;
        enhancer& operator=(enhancer&&) = delete;

        shared_ptr<SkillTalentOrPerk> create() override                        { return make_shared<T>(); }
        shared_ptr<SkillTalentOrPerk> create(const QJsonObject& json) override { return make_shared<T>(json); }
    };

    SkillTalentOrPerk();
    SkillTalentOrPerk(const SkillTalentOrPerk&) = delete;
    SkillTalentOrPerk(SkillTalentOrPerk&&) = delete;
    virtual ~SkillTalentOrPerk() { }

    SkillTalentOrPerk& operator=(const SkillTalentOrPerk&) = delete;
    SkillTalentOrPerk& operator=(SkillTalentOrPerk&&) = delete;

    static const bool NoStore = true;
    static const bool ShowRoll = true;

    virtual QString     description(bool roll = false) = 0;
    virtual bool        form(QWidget*, QVBoxLayout*)   = 0;
    virtual QString     name()                         = 0;
    virtual Points      points(bool noStore = false)   = 0;
    virtual void        restore()                      = 0;
    virtual QString     roll()                         = 0;
    virtual void        store()                        = 0;
    virtual QJsonObject toJson()                       = 0;
    virtual bool        isSkill()                      { return false; }
    virtual bool        isPerk()                       { return false; }
    virtual bool        isTalent()                     { return false; }
    virtual int         rED()                          { return 0; }
    virtual int         rPD()                          { return 0; }
    virtual int         place()                        { return 1; }

    virtual void checked(bool) { }
    virtual void numeric(QString) { }

    QWidget* sender() { return _sender; }

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

private:
    static QMap<QString, skillBase*>    _skills;    // NOLINT
    static QMap<QString, talentBase*>   _talents;   // NOLINT
    static QMap<QString, perkBase*>     _perks;     // NOLINT
    static QMap<QString, enhancerBase*> _enhancers; // NOLINT
};

#endif // SKILLTALENTORPERK_H
