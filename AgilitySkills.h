#ifndef AGILITYSKILLS_H
#define AGILITYSKILLS_H

#include "character.h"
#ifndef ISHSC
#include "sheet.h"
#endif
#include "skilltalentorperk.h"

class AgilitySkills: public SkillTalentOrPerk {
public:
    AgilitySkills(): SkillTalentOrPerk() { }
    AgilitySkills(QString name)
        : v { name, 0 } { }
    AgilitySkills(const AgilitySkills& s)
        : SkillTalentOrPerk()
        , v(s.v) { }
    AgilitySkills(AgilitySkills&& s)
        : SkillTalentOrPerk()
        , v(s.v) { }
    AgilitySkills(const QJsonObject& json)
        : SkillTalentOrPerk()
        , v { json["name"].toString(""), json["plus"].toInt(0) } { }
    ~AgilitySkills() override { }

    AgilitySkills& operator=(const AgilitySkills& s) {
        if (this != &s) v = s.v;
        return *this;
    }
    AgilitySkills& operator=(AgilitySkills&& s) {
        v = s.v;
        return *this;
    }

    bool isSkill() override { return true; }

    QString  description(bool showRoll = false) override         { return v._name + " (" + (showRoll ? roll() : "+" + QString("%1").arg(v._plus)) + ")"; }
    bool     form(QWidget* parent, QVBoxLayout* layout) override { plus = createLineEdit(parent, layout, "Pluses?", std::mem_fn(&SkillTalentOrPerk::numeric)); return true; }
    QString  name() override                                     { return v._name; }
    Points   points(bool noStore = false) override               { if (!noStore) store(); return 3_cp + v._plus * 2_cp; }
    void     restore() override                                  { vars s = v; plus->setText(QString("%1").arg(s._plus)); v = s; }
#ifndef ISHSC
    QString  roll() override                                     { return add(Sheet::ref().character().DEX().roll(), v._plus); }
#else
    QString  roll() override                                     { return QString("+%1").arg(v._plus); }
#endif
    void     store() override                                    { v._plus = plus->text().toInt(0); }

    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"] = v._name;
        obj["plus"] = v._plus;
        return obj;
    }

private:
    struct vars {
        QString _name = "";
        int     _plus = 0;
    } v;

    QLineEdit* plus = nullptr;

    void numeric(QString) override {
        QString txt = plus->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        plus->undo();
    }
};

// NOLINTNEXTLINE
#define CLASS(x)                                         \
    class x: public AgilitySkills {                      \
    public:                                              \
        x()                                              \
            : AgilitySkills(#x) { }                      \
        x(const x& s)                                    \
            : AgilitySkills(s) { }                       \
        x(x&& s)                                         \
            : AgilitySkills(s) { }                       \
        x(const QJsonObject& json)                       \
            : AgilitySkills(json) { }                    \
        ~x() { }                                         \
        x& operator=(const x& s) {                       \
            if (this != &s) AgilitySkills::operator=(s); \
            return *this;                                \
        }                                                \
        x& operator=(x&& s) {                            \
            AgilitySkills::operator=(s);                 \
            return *this;                                \
        }                                                \
    };
// NOLINTNEXTLINE
#define CLASS_SPACE(x, y)                                \
    class x: public AgilitySkills {                      \
    public:                                              \
        x()                                              \
            : AgilitySkills(y) { }                       \
        x(const x& s)                                    \
            : AgilitySkills(s) { }                       \
        x(x&& s)                                         \
            : AgilitySkills(s) { }                       \
        x(const QJsonObject& json)                       \
            : AgilitySkills(json) { }                    \
        ~x() { }                                         \
        x& operator=(const x& s) {                       \
            if (this != &s) AgilitySkills::operator=(s); \
            return *this;                                \
        }                                                \
        x& operator=(x&& s) {                            \
            AgilitySkills::operator=(s);                 \
            return *this;                                \
        }                                                \
    };

CLASS(Acrobatics);
CLASS(Breakfall);
CLASS(Climbing);
CLASS_SPACE(CombatDriving, "Combat Driving");
CLASS_SPACE(CombatPiloting, "Combat Piloting");
CLASS(Contortionist);
CLASS_SPACE(FastDraw, "Fast Draw");
CLASS(Lockpicking);
CLASS(Riding);
CLASS_SPACE(SleightOfHand, "Sleight Of Hand");
CLASS(Stealth);
CLASS(Teamwork);

#undef CLASS
#undef CLASS_SPACE

#endif // AGILITYSKILLS_H
