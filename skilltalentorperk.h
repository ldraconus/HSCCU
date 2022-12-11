#ifndef SKILLTALENTORPERK_H
#define SKILLTALENTORPERK_H

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
protected:
    QCheckBox* createCheckBox(QWidget*, QVBoxLayout*, QString, std::_Mem_fn<void (SkillTalentOrPerk::*)(bool)>);
    QCheckBox* createCheckBox(QWidget*, QVBoxLayout*, QString);
    QComboBox* createComboBox(QWidget*, QVBoxLayout*, QString, QList<QString>, std::_Mem_fn<void (SkillTalentOrPerk::*)(int)>);
    QComboBox* createComboBox(QWidget*, QVBoxLayout*, QString, QList<QString>);
    QLabel*    createLabel(QWidget*, QVBoxLayout*, QString);
    QLineEdit* createLineEdit(QWidget*, QVBoxLayout*, QString, std::_Mem_fn<void (SkillTalentOrPerk::*)(QString)> callback);
    QLineEdit* createLineEdit(QWidget*, QVBoxLayout*, QString);

    QMap<QCheckBox*, std::_Mem_fn<void (SkillTalentOrPerk::*)(bool)>>     _callbacksCB;
    QMap<QComboBox*, std::_Mem_fn<void (SkillTalentOrPerk::*)(int)>>      _callbacksCBox;
    QMap<QLineEdit*, std::_Mem_fn<void (SkillTalentOrPerk::*)(QString)>>  _callbacksEdit;
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

    QWidget* _sender;

public:
    class skillBase {
    public:
        skillBase() { }
        skillBase(const skillBase&) { }
        skillBase(skillBase*) { }

        virtual SkillTalentOrPerk* create()                        = 0;
        virtual SkillTalentOrPerk* create(const QJsonObject& json) = 0;
    };

    template <typename T>
    class skill: public skillBase {
    public:
        skill(): skillBase() { }
        skill(const skill& b): skillBase(b) { }
        skill(skill* b): skillBase(b) { }

        SkillTalentOrPerk* create() override                        { return new T; }
        SkillTalentOrPerk* create(const QJsonObject& json) override { return new T(json); }
    };

    class perkBase {
    public:
        perkBase() { }
        perkBase(const perkBase&) { }
        perkBase(perkBase*) { }

        virtual SkillTalentOrPerk* create()                        = 0;
        virtual SkillTalentOrPerk* create(const QJsonObject& json) = 0;
    };

    template <typename T>
    class perk: public perkBase {
    public:
        perk(): perkBase() { }
        perk(const perk& b): perkBase(b) { }
        perk(perk* b): perkBase(b) { }

        SkillTalentOrPerk* create() override                        { return new T; }
        SkillTalentOrPerk* create(const QJsonObject& json) override { return new T(json); }
    };

    class talentBase {
    public:
        talentBase() { }
        talentBase(const talentBase&) { }
        talentBase(talentBase*) { }

        virtual SkillTalentOrPerk* create()                        = 0;
        virtual SkillTalentOrPerk* create(const QJsonObject& json) = 0;
    };

    template <typename T>
    class talent: public talentBase {
    public:
        talent(): talentBase() { }
        talent(const talent& b): talentBase(b) { }
        talent(talentBase* b): talentBase(b) { }

        SkillTalentOrPerk* create() override                        { return new T; }
        SkillTalentOrPerk* create(const QJsonObject& json) override { return new T(json); }
    };

    SkillTalentOrPerk();
    virtual ~SkillTalentOrPerk() { }

    static const bool NoStore = true;
    static const bool ShowRoll = true;

    virtual QString     description(bool roll = false) = 0;
    virtual void        form(QWidget*, QVBoxLayout*)   = 0;
    virtual QString     name()                         = 0;
    virtual int         points(bool noStore = false)   = 0;
    virtual void        restore()                      = 0;
    virtual QString     roll()                         = 0;
    virtual void        store()                        = 0;
    virtual QJsonObject toJson()                       = 0;

    virtual void checked(bool) { }
    virtual void numeric(QString) { }

    QWidget* sender() { return _sender; }

    void callback(QCheckBox*);
    void callback(QLineEdit*);
    void createForm(QWidget*, QVBoxLayout*);

    static QList<QString>      Available();
    static void                ClearForm(QVBoxLayout*);
    static QList<QString>      SkillsAvailable();
    static QList<QString>      TalentsAvailable();
    static QList<QString>      PerksAvailable();
    static SkillTalentOrPerk*  ByName(QString);
    static SkillTalentOrPerk*  FromJson(QString, const QJsonObject&);

    bool isNumber(QString);

private:
    QLabel* _points;
    QLabel* _description;

    static QMap<QString, skillBase*>  _skills;
    static QMap<QString, talentBase*> _talents;
    static QMap<QString, perkBase*>   _perks;
};

#endif // SKILLTALENTORPERK_H
