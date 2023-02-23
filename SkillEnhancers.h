#ifndef SKILLENHANCERS_H
#define SKILLENHANCERS_H

#include "skilltalentorperk.h"

class SkillEnhancers: public SkillTalentOrPerk {
public:
    SkillEnhancers(): SkillTalentOrPerk() { }
    SkillEnhancers(QString name)
        : v { name } { }
    SkillEnhancers(const SkillEnhancers& s)
        : SkillTalentOrPerk()
        , v(s.v) { }
    SkillEnhancers(SkillEnhancers&& s)
        : SkillTalentOrPerk()
        , v(s.v) { }
    SkillEnhancers(const QJsonObject& json)
        : SkillTalentOrPerk()
        , v { json["name"].toString("") } { }

    virtual SkillEnhancers& operator=(const SkillEnhancers& s) {
        if (this != &s) v = s.v;
        return *this;
    }
    virtual SkillEnhancers& operator=(SkillEnhancers&& s) {
        v = s.v;
        return *this;
    }

    bool isSkill() override { return true; }

    QString  description(bool) override            { return v._name; }
    void     form(QWidget*, QVBoxLayout*) override { throw "just accept"; }
    QString  name() override                       { return v._name; }
    Points points(bool noStore = false) override { if (!noStore) store(); return 3_cp; }
    void     restore() override                    { }
    QString  roll() override                       { return ""; }
    void     store() override                      { }

    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"] = v._name;
        return obj;
    }

private:
    struct vars {
        QString _name;
    } v;
};

#define CLASS(x)\
    class x: public SkillEnhancers {\
    public:\
        x(): SkillEnhancers(#x) { }\
        x(const x& s): SkillEnhancers(s) { }\
        x(x&& s): SkillEnhancers(s) { }\
        x(const QJsonObject& json): SkillEnhancers(json) { }\
    };
#define CLASS_SPACE(x,y)\
    class x: public SkillEnhancers {\
    public:\
        x(): SkillEnhancers(y) { }\
        x(const x& s): SkillEnhancers(s) { }\
        x(x&& s): SkillEnhancers(s) { }\
        x(const QJsonObject& json): SkillEnhancers(json) { }\
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
