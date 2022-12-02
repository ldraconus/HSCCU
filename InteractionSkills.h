#ifndef INTERACTIONSKILLS_H
#define INTERACTIONSKILLS_H

#include "character.h"
#include "sheet.h"
#include "skilltalentorperk.h"

class InteractionSkills: public SkillTalentOrPerk {
public:
    InteractionSkills(): SkillTalentOrPerk() { }
    InteractionSkills(QString name)
        : _name(name)
        , _plus(0) { }
    InteractionSkills(const InteractionSkills& s)
        : SkillTalentOrPerk()
        , _name(s._name)
        , _plus(s._plus) { }
    InteractionSkills(InteractionSkills&& s)
        : SkillTalentOrPerk()
        , _name(s._name)
        , _plus(s._plus) { }
    InteractionSkills(const QJsonObject& json)
        : SkillTalentOrPerk()
        , _name(json["name"].toString(""))
        , _plus(json["plus"].toInt(0)) { }

    virtual InteractionSkills& operator=(const InteractionSkills& s) {
        if (this != &s) {
            _name = s._name;
            _plus = s._plus;
        }
        return *this;
    }
    virtual InteractionSkills& operator=(InteractionSkills&& s) {
        _name = s._name;
        _plus = s._plus;
        return *this;
    }

    QString description(bool showRoll = false) override      { return _name + " (" + (showRoll ? roll() : "+" + QString("%1").arg(_plus)) + ")"; }
    void form(QWidget* parent, QVBoxLayout* layout) override { plus = createLineEdit(parent, layout, "Pluses?", std::mem_fn(&SkillTalentOrPerk::numeric)); }
    int points(bool noStore = false) override                { if (!noStore) store(); return 3 + _plus * 2; }
    void restore() override                                  { plus->setText(QString("%1").arg(_plus)); }
    QString roll() override                                  { return add(Sheet::ref().character().PRE().roll(), _plus); }
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

#endif // INTERACTIONSKILLS_H
