#ifndef INTELLECTSKILLS_H
#define INTELLECTSKILLS_H

#include "character.h"
#include "sheet.h"
#include "skilltalentorperk.h"

class IntellectSkills: public SkillTalentOrPerk {
public:
    IntellectSkills(): SkillTalentOrPerk() { }
    IntellectSkills(QString name)
        : _name(name)
        , _plus(0) { }
    IntellectSkills(const IntellectSkills& s)
        : SkillTalentOrPerk()
        , _name(s._name)
        , _plus(s._plus) { }
    IntellectSkills(IntellectSkills&& s)
        : SkillTalentOrPerk()
        , _name(s._name)
        , _plus(s._plus) { }
    IntellectSkills(const QJsonObject& json)
        : SkillTalentOrPerk()
        , _name(json["name"].toString(""))
        , _plus(json["plus"].toInt(0)) { }

    virtual IntellectSkills& operator=(const IntellectSkills& s) {
        if (this != &s) {
            _name = s._name;
            _plus = s._plus;
        }
        return *this;
    }
    virtual IntellectSkills& operator=(IntellectSkills&& s) {
        _name = s._name;
        _plus = s._plus;
        return *this;
    }

    QString description(bool showRoll = false) override      { return _name + " (" + (showRoll ? roll() : "+" + QString("%1").arg(_plus)) + ")"; }
    void form(QWidget* parent, QVBoxLayout* layout) override { plus = createLineEdit(parent, layout, "Pluses?", std::mem_fn(&SkillTalentOrPerk::numeric)); }
    int points(bool noStore = false) override                { if (!noStore) store(); return 3 + _plus * 2; }
    void restore() override                                  { plus->setText(QString("%1").arg(_plus)); }
    QString roll() override                                  { return add(Sheet::ref().character().INT().roll(), _plus); }
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
