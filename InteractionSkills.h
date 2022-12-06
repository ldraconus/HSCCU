#ifndef INTERACTIONSKILLS_H
#define INTERACTIONSKILLS_H

#include "character.h"
#include "sheet.h"
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

    QString description(bool showRoll = false) override      { return v._name + " (" + (showRoll ? roll() : "+" + QString("%1").arg(v._plus)) + ")"; }
    void form(QWidget* parent, QVBoxLayout* layout) override { plus = createLineEdit(parent, layout, "Pluses?", std::mem_fn(&SkillTalentOrPerk::numeric)); }
    int points(bool noStore = false) override                { if (!noStore) store(); return 3 + v._plus * 2; }
    void restore() override                                  { vars s = v; plus->setText(QString("%1").arg(s._plus)); v = s; }
    QString roll() override                                  { return add(Sheet::ref().character().PRE().roll(), v._plus); }
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
