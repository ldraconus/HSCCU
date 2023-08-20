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
    bool form(QWidget* parent, QVBoxLayout* layout) override { topic = createLineEdit(parent, layout, "Specific Topic?");
                                                               plus  = createLineEdit(parent, layout, "Pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                               return true; }
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

class Analyze: public IntellectSkills {
public:
    Analyze(): IntellectSkills("Analyze")                   { }
    Analyze(const Analyze& s): IntellectSkills(s)           { }
    Analyze(Analyze&& s): IntellectSkills(s)                { }
    Analyze(const QJsonObject& json): IntellectSkills(json) {
        v._plus    = json["plus"].toInt(1);
        v._what    = json["what"].toString("");
    }

    QString description(bool showRoll = false) override {
        return (showRoll ?
#ifndef ISHSC
                               add(Sheet::ref().character().INT().roll(), v._plus)
#else
                               QString("+%1").arg(v._plus)
#endif
                         : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override {
        plus = createLineEdit(parent, layout, "How many pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
        what = createLineEdit(parent, layout, "Analyze what?");
        return true;
    }
    Points points(bool noStore = false) override              {
        if (!noStore) store();
        auto pts = v._plus * 2_cp + 3_cp;
        if (pts < 1_cp) pts = 1_cp;
        return pts;
    }
    void    restore() override                                  {
        vars s = v;
        plus->setText(QString("%1").arg(s._plus));
        what->setText(s._what);
        v = s;
    }
#ifndef ISHSC
    QString roll() override                                     { return add(Sheet::ref().character().INT().roll(), v._plus); }
#else
    QString roll() override                                     { return QString("%1-").arg(11 + v._plus); }
#endif
    void    store() override                                    {
        v._plus    = plus->text().toInt(0);
        v._what    = what->text();
    }
    QJsonObject toJson() override                               {
        QJsonObject obj = IntellectSkills::toJson();
        obj["plus"]    = v._plus;
        obj["what"]    = v._what;
        return obj;
    }

private:
    struct vars {
        int     _plus    = 0;
        QString _what    = "";
    } v;

    QLineEdit* plus;
    QLineEdit* what;

    QString optOut() {
        if (v._what.isEmpty()) return "<incomplete>";
        QString res;
        res += "Analyze " + v._what;
        QString sep = " (";
        if (v._plus > 0) { res += sep + QString("+%1").arg(v._plus); sep = ", "; }
        if (sep != " (") res += ")";
        return res;
    }

    void numeric(QString) override {
        QString txt = plus->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        plus->undo();
    }
};

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
