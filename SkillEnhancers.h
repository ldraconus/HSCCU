#ifndef SKILLENHANCERS_H
#define SKILLENHANCERS_H

#include "skilltalentorperk.h"

class SkillEnhancers: public SkillTalentOrPerk {
public:
    SkillEnhancers(): SkillTalentOrPerk() { }
    SkillEnhancers(QString name)
        : SkillTalentOrPerk()
        , v { name } { }
    SkillEnhancers(QJsonObject& json)
        : SkillTalentOrPerk(json)
        , v { json["name"].toString("") } { }

    bool isSkill() override { return true; }

    QString  description(bool) override            { return v.mName; }
    bool     form(QWidget*, QVBoxLayout*) override { return false; }
    QString  name() override                       { return v.mName; }
    Points   points(bool noStore = false) override { if (!noStore) store(); return 3_cp; }
    void     restore() override                    { }
    QString  roll() override                       { return ""; }
    void     store() override                      { }

    QJsonObject toJson() override {
        QJsonObject obj = SkillTalentOrPerk::toJson();
        obj["name"] = v.mName;
        return obj;
    }

private:
    struct vars {
        QString mName;
    } v;
};

class BlankSkill: public SkillEnhancers {
public:
    BlankSkill(): SkillEnhancers("Blank Line") { }
    BlankSkill(QJsonObject& json): SkillEnhancers(json) { }

    QString  description(bool) override            { return "-"; }
    Points   points(bool) override                 { return 0_cp; }
    void     restore() override                    { }
    QString  roll() override                       { return ""; }
    void     store() override                      { }
};

#define CLASS(x)\
    class x: public SkillEnhancers {\
    public:\
        x(): SkillEnhancers(#x) { }\
        x(QJsonObject& json): SkillEnhancers(json) { }\
    };
#define CLASS_SPACE(x,y)\
    class x: public SkillEnhancers {\
    public:\
        x(): SkillEnhancers(y) { }\
        x(QJsonObject& json): SkillEnhancers(json) { }\
    };

CLASS_SPACE(JackOfAllTrades, "Jack Of All Trades");
CLASS(Linguist);
CLASS(Scholar);
CLASS(Scientist);
CLASS(Traveler);
CLASS_SPACE(WellConnected, "Well-Connected");

#undef CLASS
#undef CLASS_SPACE

#endif // SKILLENHANCERS_H
