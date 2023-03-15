#ifndef INTELLECTSKILLS_H
#define INTELLECTSKILLS_H

#include "character.h"
#ifndef ISHSC
#include "sheet.h"
#endif
#include "skilltalentorperk.h"

class IntellectSkills: public SkillTalentOrPerk {
public:
    IntellectSkills(): SkillTalentOrPerk() { }
    IntellectSkills(QString name)
        : v { name, "", 0 } { }
    IntellectSkills(const IntellectSkills& s)
        : SkillTalentOrPerk()
        , v(s.v) { }
    IntellectSkills(IntellectSkills&& s)
        : SkillTalentOrPerk()
        , v(s.v) { }
    IntellectSkills(const QJsonObject& json)
        : SkillTalentOrPerk()
        , v { json["name"].toString(""), json["topic"].toString(""), json["plus"].toInt(0) } { }

    virtual IntellectSkills& operator=(const IntellectSkills& s) {
        if (this != &s) v = s.v;
        return *this;
    }
    virtual IntellectSkills& operator=(IntellectSkills&& s) {
        v = s.v;
        return *this;
    }

    bool isSkill() override { return true; }

    QString description(bool showRoll = false) override      { return v._name + " (" + (v._topic.isEmpty() ? "" : v._topic + "; ") +
                                                                                       (showRoll ? roll() : "+" + QString("%1").arg(v._plus)) + ")"; }
    void form(QWidget* parent, QVBoxLayout* layout) override { topic = createLineEdit(parent, layout, "Specific Topic?");
                                                               plus  = createLineEdit(parent, layout, "Pluses?", std::mem_fn(&SkillTalentOrPerk::numeric)); }
    QString name() override                                  { return v._name; }
    Points points(bool noStore = false) override             { if (!noStore) store(); return 3_cp + v._plus * 2_cp; }
    void restore() override                                  { vars s = v;
                                                               topic->setText(v._topic);
                                                               plus->setText(QString("%1").arg(s._plus));
                                                               v = s; }
    QString roll() override                                  { return
#ifndef ISHSC
                                                                   add(Sheet::ref().character().INT().roll(), v._plus); }
#else
                                                                   QString("+%1").arg(v._plus); }
#endif
    void    store() override                                 { v._topic = topic->text();
                                                               v._plus  = plus->text().toInt(0); }

    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]  = v._name;
        obj["topic"] = v._topic;
        obj["plus"]  = v._plus;
        return obj;
    }

private:
    struct vars {
        QString _name;
        QString _topic;
        int     _plus;
    } v;

    QLineEdit* topic;
    QLineEdit* plus;

    void numeric(QString) override {
        QString txt = plus->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        plus->undo();
    }
};

#define CLASS(x)\
    class x: public IntellectSkills {\
    public:\
        x(): IntellectSkills(#x) { }\
        x(const x& s): IntellectSkills(s) { }\
        x(x&& s): IntellectSkills(s) { }\
        x(const QJsonObject& json): IntellectSkills(json) { }\
    };
#define CLASS_SPACE(x,y)\
    class x: public IntellectSkills {\
    public:\
        x(): IntellectSkills(y) { }\
        x(const x& s): IntellectSkills(s) { }\
        x(x&& s): IntellectSkills(s) { }\
        x(const QJsonObject& json): IntellectSkills(json) { }\
    };

CLASS(Analyze);
CLASS(Bugging);
CLASS_SPACE(ComputerProgramming, "Computer Programming");
CLASS(Concealment);
CLASS(Cramming);
CLASS(Criminology);
CLASS(Cryptography);
CLASS(Deduction);
CLASS(Demolitions);
CLASS(Disguise);
CLASS(Electronics);
CLASS_SPACE(ForensicMedicine, "Forensic Medicine");
CLASS(Forgery);
CLASS(Gambling);
CLASS(Inventor);
CLASS(Lipreading);
CLASS(Mechanics);
CLASS(Mimicry);
CLASS(Navigation);
CLASS(Paramedics);
CLASS_SPACE(SecuritySystems, "Security Systems");
CLASS(Shadowing);
CLASS(Survival);
CLASS_SPACE(SystemsOperation, "Systems Operation");
CLASS(Tactics);
CLASS(Tracking);
CLASS(Ventriloquism);
CLASS(Weaponsmith);

#undef CLASS
#undef CLASS_SPACE

#endif // INTELLECTSKILLS_H
