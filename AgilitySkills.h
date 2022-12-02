#ifndef AGILITYSKILLS_H
#define AGILITYSKILLS_H

#include "character.h"
#include "sheet.h"
#include "skilltalentorperk.h"

class AgilitySkills: public SkillTalentOrPerk {
public:
    AgilitySkills(): SkillTalentOrPerk() { }
    AgilitySkills(QString name)
        : _name(name)
        , _plus(0) { }
    AgilitySkills(const AgilitySkills& s)
        : SkillTalentOrPerk()
        , _name(s._name)
        , _plus(s._plus) { }
    AgilitySkills(AgilitySkills&& s)
        : SkillTalentOrPerk()
        , _name(s._name)
        , _plus(s._plus) { }
    AgilitySkills(const QJsonObject& json)
        : SkillTalentOrPerk()
        , _name(json["name"].toString(""))
        , _plus(json["plus"].toInt(0)) { }

    virtual AgilitySkills& operator=(const AgilitySkills& s) {
        if (this != &s) {
            _name = s._name;
            _plus = s._plus;
        }
        return *this;
    }
    virtual AgilitySkills& operator=(AgilitySkills&& s) {
        _name = s._name;
        _plus = s._plus;
        return *this;
    }

    QString description(bool showRoll = false) override      { return _name + " (" + (showRoll ? roll() : "+" + QString("%1").arg(_plus)) + ")"; }
    void form(QWidget* parent, QVBoxLayout* layout) override { plus = createLineEdit(parent, layout, "Pluses?", std::mem_fn(&SkillTalentOrPerk::numeric)); }
    int points(bool noStore = false) override                { if (!noStore) store(); return 3 + _plus * 2; }
    void restore() override                                  { plus->setText(QString("%1").arg(_plus)); }
    QString roll() override                                  { return add(Sheet::ref().character().DEX().roll(), _plus); }
    void    store() override                                 { _plus = plus->text().toInt(0); }

    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"] = _name;
        obj["plus"] = _plus;
        return obj;
    }

private:
    QString _name;
    int     _plus;

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
