#ifndef INTERACTIONSKILLS_H
#define INTERACTIONSKILLS_H

#include "character.h"
#ifndef ISHSC
#include "sheet.h"
#endif
#include "skilltalentorperk.h"

class InteractionSkills: public SkillTalentOrPerk {
public:
    InteractionSkills(): SkillTalentOrPerk() { }
    InteractionSkills(QString name)
        : v { name, 0 } { }
    InteractionSkills(const InteractionSkills& s)
        : SkillTalentOrPerk()
        , v(s.v) { }
    InteractionSkills(InteractionSkills&& s)
        : SkillTalentOrPerk()
        , v(s.v) { }
    InteractionSkills(const QJsonObject& json)
        : SkillTalentOrPerk()
        , v { json["name"].toString(""), json["plus"].toInt(0) } { }

    virtual InteractionSkills& operator=(const InteractionSkills& s) {
        if (this != &s) v = s.v;
        return *this;
    }
    virtual InteractionSkills& operator=(InteractionSkills&& s) {
        v = s.v;
        return *this;
    }

    bool isSkill() override { return true; }

    QString description(bool showRoll = false) override      { return v._name + " (" + (showRoll ? roll() : "+" + QString("%1").arg(v._plus)) + ")"; }
    bool form(QWidget* parent, QVBoxLayout* layout) override { plus = createLineEdit(parent, layout, "Pluses?", std::mem_fn(&SkillTalentOrPerk::numeric)); return true; }
    QString name() override                                  { return v._name; }
    Points points(bool noStore = false) override           { if (!noStore) store(); return 3_cp + v._plus * 2_cp; }
    void restore() override                                  { vars s = v; plus->setText(QString("%1").arg(s._plus)); v = s; }
#ifndef ISHSC
    QString roll() override                                  { return add(Sheet::ref().character().PRE().roll(), v._plus); }
#else
    QString roll() override                                  { return QString("+%1").arg(v._plus); }
#endif
    void    store() override                                 { v._plus = plus->text().toInt(0); }

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
    class x: public InteractionSkills {\
    public:\
        x(): InteractionSkills(#x) { }\
        x(const x& s): InteractionSkills(s) { }\
        x(x&& s): InteractionSkills(s) { }\
        x(const QJsonObject& json): InteractionSkills(json) { }\
    };
#define CLASS_SPACE(x,y)\
    class x: public InteractionSkills {\
    public:\
        x(): InteractionSkills(y) { }\
        x(const x& s): InteractionSkills(s) { }\
        x(x&& s): InteractionSkills(s) { }\
        x(const QJsonObject& json): InteractionSkills(json) { }\
    };

CLASS(Acting);
CLASS(Bribery);
CLASS(Bureaucratics);
CLASS(Charm);
CLASS(Conversation);
CLASS_SPACE(HighSociety, "High Society");
CLASS(Interrogation);
CLASS(Oratory);
CLASS(Persuasion);
CLASS(Streetwise);
CLASS(Trading);

#undef CLASS
#undef CLASS_SPACE

#endif // INTERACTIONSKILLS_H
