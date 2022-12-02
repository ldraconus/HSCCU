#ifndef PERK_H
#define PERK_H

#include "skilltalentorperk.h"

class Perks: public SkillTalentOrPerk {
public:
    Perks(): SkillTalentOrPerk() { }
    Perks(QString name)
        : _name(name) { }
    Perks(const Perks& s)
        : SkillTalentOrPerk()
        , _name(s._name) { }
    Perks(Perks&& s)
        : SkillTalentOrPerk()
        , _name(s._name) { }
    Perks(const QJsonObject& json)
        : SkillTalentOrPerk()
        , _name(json["name"].toString("")) { }

    virtual Perks& operator=(const Perks& s) {
        if (this != &s) {
            _name = s._name;
        }
        return *this;
    }
    virtual Perks& operator=(Perks&& s) {
        _name = s._name;
        return *this;
    }

    QString description(bool showRoll = false) override      { return _name + (showRoll ? "" : ""); }
    void form(QWidget* parent, QVBoxLayout* layout) override { }
    int points(bool noStore = false) override                { if (!noStore) store(); return 0; }
    void restore() override                                  { }
    QString roll() override                                  { return ""; }
    void    store() override                                 { }

    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"] = _name;
        return obj;
    }

private:
    QString _name;
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

#undef CLASS
#undef CLASS_SPACE

#endif // PERK_H
