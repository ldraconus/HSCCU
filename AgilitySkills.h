#ifndef AGILITYSKILLS_H
#define AGILITYSKILLS_H

#include "character.h"
#include "sheet.h"
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

    virtual AgilitySkills& operator=(const AgilitySkills& s) {
        if (this != &s) v = s.v;
        return *this;
    }
    virtual AgilitySkills& operator=(AgilitySkills&& s) {
        v = s.v;
        return *this;
    }

    bool isSkill() override { return true; }

    QString  description(bool showRoll = false) override         { return v._name + " (" + (showRoll ? roll() : "+" + QString("%1").arg(v._plus)) + ")"; }
    void     form(QWidget* parent, QVBoxLayout* layout) override { plus = createLineEdit(parent, layout, "Pluses?", std::mem_fn(&SkillTalentOrPerk::numeric)); }
    QString  name() override                                     { return v._name; }
    Points<> points(bool noStore = false) override               { if (!noStore) store(); return 3_cp + v._plus * 2_cp; }
    void     restore() override                                  { vars s = v; plus->setText(QString("%1").arg(s._plus)); v = s; }
    QString  roll() override                                     { return add(Sheet::ref().character().DEX().roll(), v._plus); }
    void     store() override                                    { v._plus = plus->text().toInt(0); }

    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"] = v._name;
        obj["plus"] = v._plus;
        return obj;
    }

private:
    struct vars {
        QString _name;
        int     _plus;
    } v;

    QLineEdit* plus;

    void numeric(QString) override {
        QString txt = plus->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        plus->undo();
    }
};

#define CLASS(x)\
    class x: public AgilitySkills {\
    public:\
        x(): AgilitySkills(#x) { }\
        x(const x& s): AgilitySkills(s) { }\
        x(x&& s): AgilitySkills(s) { }\
        x(const QJsonObject& json): AgilitySkills(json) { }\
    };
#define CLASS_SPACE(x,y)\
    class x: public AgilitySkills {\
    public:\
        x(): AgilitySkills(y) { }\
        x(const x& s): AgilitySkills(s) { }\
        x(x&& s): AgilitySkills(s) { }\
        x(const QJsonObject& json): AgilitySkills(json) { }\
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
