#ifndef MISCSKILL_H
#define MISCSKILL_H

#include "character.h"
#include "sheet.h"
#include "skilltalentorperk.h"

class MiscSkills: public SkillTalentOrPerk {
public:
    MiscSkills(): SkillTalentOrPerk() { }
    MiscSkills(QString name)
        : _name(name) { }
    MiscSkills(const MiscSkills& s)
        : SkillTalentOrPerk()
        , _name(s._name) { }
    MiscSkills(MiscSkills&& s)
        : SkillTalentOrPerk()
        , _name(s._name) { }
    MiscSkills(const QJsonObject& json)
        : SkillTalentOrPerk()
        , _name(json["name"].toString("")) { }

    virtual MiscSkills& operator=(const MiscSkills& s) {
        if (this != &s) {
            _name = s._name;
        }
        return *this;
    }
    virtual MiscSkills& operator=(MiscSkills&& s) {
        _name = s._name;
        return *this;
    }

    QString description(bool showRoll = false) override      { return _name +  (showRoll ? "" : ""); }
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
    MSL(const QJsonObject& json): MiscSkills(json) { _plus = json["plus"].toInt(1);
                                                     _for  = json["for"].toString("");
                                                     _size = json["size"].toInt(0);
                                                   }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { plus = createLineEdit(parent, layout, "How many pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  size = createComboBox(parent, layout, "Type of skill?", { "One mode of movement",
                                                                                                                            "Any mode of movement" });
                                                                  forwhat = createLineEdit(parent, layout, "Applies to what?");
                                                                }
    int     points(bool noStore = false) override               { if (!noStore) store();
                                                                  return _plus * QList<int>{ 0, 2, 3 }[_size + 1]; }
    void    restore() override                                  { plus->setText(QString("%1").arg(_plus));
                                                                  size->setCurrentIndex(_size);
                                                                  forwhat->setText(_for);
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { _plus = plus->text().toInt(0);
                                                                  _for  = forwhat->text();
                                                                  _size = size->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = MiscSkills::toJson();
                                                                  obj["plus"] = _plus;
                                                                  obj["for"]  = _for;
                                                                  obj["size"] = _size;
                                                                  return obj;
                                                                }

private:
    int     _plus = 0;
    QString _for = "";
    int     _size = -1;

    QLineEdit* plus;
    QLineEdit* forwhat;
    QComboBox* size;

    QString optOut() {
        if (_plus < 0) return "<incomplete>";
        QString res = "Movement Skill Levels: ";
        switch (_size) {
        case 0: res += QString("+%1 with %2").arg(_plus).arg(_for);           break;
        case 1: res += QString("+%1 with all movement").arg(_plus).arg(_for); break;
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
    PowerSkill(const QJsonObject& json): MiscSkills(json) { _what = json["what"].toString("");
                                                            _plus = json["plus"].toInt(0);
                                                            _stat = json["stat"].toInt(-1);
                                                       }

    QString description(bool showRoll = false) override         { return (showRoll ? "(" + QString("+%1").arg(_plus) + ") ": "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { what = createLineEdit(parent, layout, "What power?");
                                                                  plus = createLineEdit(parent, layout, "Pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  stat = createComboBox(parent, layout, "Base on a stat?", { "STR", "DEX", "CON", "INT", "EGO", "PRE"});
                                                                }
    int     points(bool noStore = false) override               { if (!noStore) store();
                                                                  return _plus * 2 + 3;
                                                                }
    void    restore() override                                  { what->setText(_what);
                                                                  plus->setText(QString("%1").arg(_plus));
                                                                  stat->setCurrentIndex(_stat);
                                                                }
    QString roll() override                                     { return (_stat >= 0) ? add(Sheet::ref().character().characteristic(_stat).roll(), _plus)
                                                                                      : ""; }
    void    store() override                                    { _what = what->text();
                                                                  _plus = plus->text().toInt(0);
                                                                  _stat = stat->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = MiscSkills::toJson();
                                                                  obj["what"] = _what;
                                                                  obj["plus"] = _plus;
                                                                  obj["stat"] = _stat;
                                                                  return obj;
                                                                }

private:
    QString _what = "";
    int     _plus = 0;
    int     _stat  = -1;

    QLineEdit* what;
    QLineEdit* plus;
    QComboBox* stat;

    QString optOut() {
        if (_plus <= 0 || _what.isEmpty() || _stat < 0) return "<incomplete>";
        return QString("+%1 with ").arg(_plus) + _what + QStringList { " (STR", " (DEX", " (CON", " (INT", " (EGO", " (PRE" }[_stat] + " based)";
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
    SkillLevels(const QJsonObject& json): MiscSkills(json)  { _plus = json["plus"].toInt(1);
                                                              _for  = json["for"].toString("");
                                                              _size = json["size"].toInt(0);
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
                                                                  return _plus * QList<int>{ 0, 2, 3, 4, 6, 10, 12 }[_size + 1]; }
    void    restore() override                                  { plus->setText(QString("%1").arg(_plus));
                                                                  size->setCurrentIndex(_size);
                                                                  forwhat->setText(_for);
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { _plus = plus->text().toInt(0);
                                                                  _for  = forwhat->text();
                                                                  _size = size->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = MiscSkills::toJson();
                                                                  obj["plus"] = _plus;
                                                                  obj["for"]  = _for;
                                                                  obj["size"] = _size;
                                                                  return obj;
                                                                }

private:
    int     _plus = 0;
    QString _for = "";
    int     _size = -1;

    QLineEdit* plus;
    QLineEdit* forwhat;
    QComboBox* size;

    QString optOut() {
        if (_plus < 0 || (_size < 3 && _for.isEmpty()) || _size < 0) return "<incomplete>";
        QString res;
        switch (_size) {
        case 0:
        case 1:
        case 2: res += QString("+%1 with %2").arg(_plus).arg(_for);                         break;
        case 3: res += QString("+%1 With All Agility Skills").arg(_plus);                   break;
        case 4: res += QString("+%1 With All Non-Combat Skills").arg(_plus);                break;
        case 5: res += QString("+%1 Overall Level%2").arg(_plus).arg(_plus > 1 ? "s" : ""); break;
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
