#ifndef MISCSKILL_H
#define MISCSKILL_H

#include "character.h"
#include "sheet.h"
#include "skilltalentorperk.h"

class MiscSkills: public SkillTalentOrPerk {
public:
    MiscSkills(): SkillTalentOrPerk() { }
    MiscSkills(QString name)
        : v { name } { }
    MiscSkills(const MiscSkills& s)
        : SkillTalentOrPerk()
        , v(s.v) { }
    MiscSkills(MiscSkills&& s)
        : SkillTalentOrPerk()
        , v(s.v) { }
    MiscSkills(const QJsonObject& json)
        : SkillTalentOrPerk()
        , v { json["name"].toString("") } { }

    virtual MiscSkills& operator=(const MiscSkills& s) {
        if (this != &s) {
            v._name = s.v._name;
        }
        return *this;
    }
    virtual MiscSkills& operator=(MiscSkills&& s) {
        v._name = s.v._name;
        return *this;
    }

    QString description(bool showRoll = false) override { return v._name +  (showRoll ? "" : ""); }
    void form(QWidget*, QVBoxLayout*) override          { }
    int points(bool noStore = false) override           { if (!noStore) store(); return 0; }
    void restore() override                             { }
    QString roll() override                             { return ""; }
    void    store() override                            { }

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
    class x: public MiscSkills {\
    public:\
        x(): MiscSkills(#x) { }\
        x(const x& s): MiscSkills(s) { }\
        x(x&& s): MiscSkills(s) { }\
        x(const QJsonObject& json): MiscSkills(json) { }\
    };
#define CLASS_SPACE(x,y)\
    class x: public MiscSkills {\
    public:\
        x(): MiscSkills(y) { }\
        x(const x& s): MiscSkills(s) { }\
        x(x&& s): MiscSkills(s) { }\
        x(const QJsonObject& json): MiscSkills(json) { }\
    };

class MSL: public MiscSkills {
public:
    MSL(): MiscSkills("Movement Skill Levels")     { }
    MSL(const MSL& s): MiscSkills(s)               { }
    MSL(MSL&& s): MiscSkills(s)                    { }
    MSL(const QJsonObject& json): MiscSkills(json) { v._plus = json["plus"].toInt(1);
                                                     v._for  = json["for"].toString("");
                                                     v._size = json["size"].toInt(0);
                                                   }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { plus = createLineEdit(parent, layout, "How many pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  size = createComboBox(parent, layout, "Type of skill?", { "One mode of movement",
                                                                                                                            "Any mode of movement" });
                                                                  forwhat = createLineEdit(parent, layout, "Applies to what?");
                                                                }
    int     points(bool noStore = false) override               { if (!noStore) store();
                                                                  return v._plus * QList<int>{ 0, 2, 3 }[v._size + 1]; }
    void    restore() override                                  { vars s = v;
                                                                  plus->setText(QString("%1").arg(s._plus));
                                                                  size->setCurrentIndex(s._size);
                                                                  forwhat->setText(s._for);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._plus = plus->text().toInt(0);
                                                                  v._for  = forwhat->text();
                                                                  v._size = size->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = MiscSkills::toJson();
                                                                  obj["plus"] = v._plus;
                                                                  obj["for"]  = v._for;
                                                                  obj["size"] = v._size;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     _plus = 0;
        QString _for = "";
        int     _size = -1;
    } v;

    QLineEdit* plus;
    QLineEdit* forwhat;
    QComboBox* size;

    QString optOut() {
        if (v._plus < 0) return "<incomplete>";
        QString res = "Movement Skill Levels: ";
        switch (v._size) {
        case 0: res += QString("+%1 with %2").arg(v._plus).arg(v._for);           break;
        case 1: res += QString("+%1 with all movement").arg(v._plus).arg(v._for); break;
        default: return "<incomplete>";
        }

        return res;
    }

    void numeric(QString) override {
        QString txt = plus->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        plus->undo();
    }
};

class PowerSkill: public MiscSkills {
public:
    PowerSkill(): MiscSkills("Power Skill")               { }
    PowerSkill(const PowerSkill& s): MiscSkills(s)        { }
    PowerSkill(PowerSkill&& s): MiscSkills(s)             { }
    PowerSkill(const QJsonObject& json): MiscSkills(json) { v._what = json["what"].toString("");
                                                            v._plus = json["plus"].toInt(0);
                                                            v._stat = json["stat"].toInt(-1);
                                                          }

    QString description(bool showRoll = false) override         { return (showRoll ? "(" + QString("+%1").arg(v._plus) + ") ": "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { what = createLineEdit(parent, layout, "What power?");
                                                                  plus = createLineEdit(parent, layout, "Pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  stat = createComboBox(parent, layout, "Base on a stat?", { "STR", "DEX", "CON", "INT", "EGO", "PRE"});
                                                                }
    int     points(bool noStore = false) override               { if (!noStore) store();
                                                                  return v._plus * 2 + 3;
                                                                }
    void    restore() override                                  { vars s = v;
                                                                  what->setText(s._what);
                                                                  plus->setText(QString("%1").arg(s._plus));
                                                                  stat->setCurrentIndex(s._stat);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return (v._stat >= 0) ? add(Sheet::ref().character().characteristic(v._stat).roll(), v._plus)
                                                                                        : ""; }
    void    store() override                                    { v._what = what->text();
                                                                  v._plus = plus->text().toInt(0);
                                                                  v._stat = stat->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = MiscSkills::toJson();
                                                                  obj["what"] = v._what;
                                                                  obj["plus"] = v._plus;
                                                                  obj["stat"] = v._stat;
                                                                  return obj;
                                                                }

private:
    struct vars {
        QString _what = "";
        int     _plus = 0;
        int     _stat  = -1;
    } v;

    QLineEdit* what;
    QLineEdit* plus;
    QComboBox* stat;

    QString optOut() {
        if (v._plus <= 0 || v._what.isEmpty() || v._stat < 0) return "<incomplete>";
        return QString("+%1 with ").arg(v._plus) + v._what + QStringList { " (STR", " (DEX", " (CON", " (INT", " (EGO", " (PRE" }[v._stat] + " based)";
    }

    void numeric(QString) override {
        QString txt = plus->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        plus->undo();
    }
};

class SkillLevels: public MiscSkills {
public:
    SkillLevels(): MiscSkills("Skill Levels") { }
    SkillLevels(const SkillLevels& s): MiscSkills(s)        { }
    SkillLevels(SkillLevels&& s): MiscSkills(s)             { }
    SkillLevels(const QJsonObject& json): MiscSkills(json)  { v._plus = json["plus"].toInt(1);
                                                              v._for  = json["for"].toString("");
                                                              v._size = json["size"].toInt(0);
                                                            }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { plus = createLineEdit(parent, layout, "How many pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  forwhat = createLineEdit(parent, layout, "Applies to what?");
                                                                  size = createComboBox(parent, layout, "For?", { "One Skill or Characteristic",
                                                                                                                  "Any three pre-defined Skills",
                                                                                                                  "All Intellect, Interaction or similar broad group",
                                                                                                                  "All Agility Skills",
                                                                                                                  "All Non-Combat Skills",
                                                                                                                  "Overall" });
                                                                }
    int     points(bool noStore = false) override               { if (!noStore) store();
                                                                  return v._plus * QList<int>{ 0, 2, 3, 4, 6, 10, 12 }[v._size + 1]; }
    void    restore() override                                  { vars s = v;
                                                                  plus->setText(QString("%1").arg(s._plus));
                                                                  size->setCurrentIndex(s._size);
                                                                  forwhat->setText(s._for);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._plus = plus->text().toInt(0);
                                                                  v._for  = forwhat->text();
                                                                  v._size = size->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = MiscSkills::toJson();
                                                                  obj["plus"] = v._plus;
                                                                  obj["for"]  = v._for;
                                                                  obj["size"] = v._size;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     _plus = 0;
        QString _for = "";
        int     _size = -1;
    } v;

    QLineEdit* plus;
    QLineEdit* forwhat;
    QComboBox* size;

    QString optOut() {
        if (v._plus < 0 || (v._size < 3 && v._for.isEmpty()) || v._size < 0) return "<incomplete>";
        QString res;
        switch (v._size) {
        case 0:
        case 1:
        case 2: res += QString("+%1 with %2").arg(v._plus).arg(v._for);                         break;
        case 3: res += QString("+%1 With All Agility Skills").arg(v._plus);                     break;
        case 4: res += QString("+%1 With All Non-Combat Skills").arg(v._plus);                  break;
        case 5: res += QString("+%1 Overall Level%2").arg(v._plus).arg(v._plus > 1 ? "s" : ""); break;
        default: return "<incomplete>";
        }

        return res;
    }

    void numeric(QString) override {
        QString txt = plus->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        plus->undo();
    }
};

#undef CLASS
#undef CLASS_SPACE

#endif // MISCSKILL_H