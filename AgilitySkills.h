#pragma once

#include "character.h"
#ifndef ISHSC
#include "sheet.h"
#endif
#include "skilltalentorperk.h"

class AgilitySkills: public SkillTalentOrPerk {
public:
    AgilitySkills(): SkillTalentOrPerk() { }
    AgilitySkills(QString name)
        : SkillTalentOrPerk()
        , v { name, 0 } { }
    AgilitySkills(QJsonObject& json)
        : SkillTalentOrPerk(json)
        , v { json["name"].toString(""), json["plus"].toInt(0) } { }

    bool isSkill() override { return true; }

    QString  description(bool showRoll = false) override         { return v.mName + " (" + (showRoll ? roll() : "+" + QString("%1").arg(v.mPlus)) + ")"; }
    bool     form(QWidget* parent, QVBoxLayout* layout) override { plus = createLineEdit(parent, layout, "Pluses?", std::mem_fn(&SkillTalentOrPerk::numeric)); return true; }
    QString  name() override                                     { return v.mName; }
    Points   points(bool noStore = false) override               { if (!noStore) store(); return 3_cp + v.mPlus * 2_cp; }
    void     restore() override                                  { vars s = v; plus->setText(QString("%1").arg(s.mPlus)); v = s; }
#ifndef ISHSC
    QString  roll() override                                     { return add(Sheet::ref().character().DEX().roll(), v.mPlus); }
#else
    QString  roll() override                                     { return QString("+%1").arg(v._plus); }
#endif
    void     store() override                                    { v.mPlus = plus->text().toInt(0); }

    QJsonObject toJson() override {
        QJsonObject obj = SkillTalentOrPerk::toJson();
        obj["name"]     = v.mName;
        obj["plus"]     = v.mPlus;
        return obj;
    }

private:
    struct vars {
        QString mName = "";
        int     mPlus = 0;
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
        x(QJsonObject& json)                             \
            : AgilitySkills(json) { }                    \
    };
// NOLINTNEXTLINE
#define CLASS_SPACE(x, y)                                \
    class x: public AgilitySkills {                      \
    public:                                              \
        x()                                              \
            : AgilitySkills(y) { }                       \
        x(QJsonObject& json)                             \
            : AgilitySkills(json) { }                    \
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
