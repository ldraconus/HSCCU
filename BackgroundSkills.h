#ifndef BACKGROUNDSKILLS_H
#define BACKGROUNDSKILLS_H

#include "character.h"
#include "sheet.h"
#include "skilltalentorperk.h"

class BackgroundSkill: public SkillTalentOrPerk {
public:
    BackgroundSkill(): SkillTalentOrPerk() { }
    BackgroundSkill(QString name)
        : v { name } { }
    BackgroundSkill(const BackgroundSkill& s)
        : SkillTalentOrPerk()
        , v(s.v) { }
    BackgroundSkill(BackgroundSkill&& s)
        : SkillTalentOrPerk()
        , v(s.v) { }
    BackgroundSkill(const QJsonObject& json)
        : SkillTalentOrPerk()
        , v { json["name"].toString("") } { }

    virtual BackgroundSkill& operator=(const BackgroundSkill& s) {
        if (this != &s) v = s.v;
        return *this;
    }
    virtual BackgroundSkill& operator=(BackgroundSkill&& s) {
        v = s.v;
        return *this;
    }

    QString description(bool showRoll = false) override { return showRoll ? v._name : v._name; }
    void form(QWidget*, QVBoxLayout*) override          { }
    int points(bool noStore = false) override           { return noStore ? 0 : 0; }
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
    class x: public BackgroundSkill {\
    public:\
        x(): BackgroundSkill(#x) { }\
        x(const x& s): BackgroundSkill(s) { }\
        x(x&& s): BackgroundSkill(s) { }\
        x(const QJsonObject& json): BackgroundSkill(json) { }\
    };
#define CLASS_SPACE(x,y)\
    class x: public BackgroundSkill {\
    public:\
        x(): BackgroundSkill(y) { }\
        x(const x& s): BackgroundSkill(s) { }\
        x(x&& s): BackgroundSkill(s) { }\
        x(const QJsonObject& json): BackgroundSkill(json) { }\
    };

class KS: public BackgroundSkill {
public:
    KS(): BackgroundSkill("Knowledge Skill")           { }
    KS(const KS& s): BackgroundSkill(s)                { }
    KS(KS&& s): BackgroundSkill(s)                     { }
    KS(const QJsonObject& json): BackgroundSkill(json) { v._introll = json["introll"].toBool(false);
                                                         v._plus    = json["plus"].toInt(1);
                                                         v._for     = json["for"].toString("");
                                                         v._type    = json["type"].toInt(0);
                                                       }

    QString description(bool showRoll = false) override         { return (showRoll ? (v._introll ? add(Sheet::ref().character().DEX().roll(), v._plus)
                                                                                                 : QString("%1-").arg(11 + v._plus)) + " "
                                                                                   : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { introll = createCheckBox(parent, layout, "Base off INT");
                                                                  plus = createLineEdit(parent, layout, "How many pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  type = createComboBox(parent, layout, "Type of Knowledge Skill", { "Group", "People", "Areas", "City", "Things" });
                                                                  forwhat = createLineEdit(parent, layout, "Applies to what?");
                                                                }
    int     points(bool noStore = false) override               { if (!noStore) store();
                                                                  return v._plus * 2 + (v._introll ? 3 : 2);
    }
    void    restore() override                                  { vars s = v;
                                                                  plus->setText(QString("%1").arg(s._plus));
                                                                  type->setCurrentIndex(s._type);
                                                                  forwhat->setText(s._for);
                                                                  introll->setChecked(s._introll);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return v._introll ? add(Sheet::ref().character().DEX().roll(), v._plus) : QString("%1-").arg(11 + v._plus); }
    void    store() override                                    { v._plus    = plus->text().toInt(0);
                                                                  v._for     = forwhat->text();
                                                                  v._type    = type->currentIndex();
                                                                  v._introll = introll->isChecked();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = BackgroundSkill::toJson();
                                                                  obj["plus"]    = v._plus;
                                                                  obj["for"]     = v._for;
                                                                  obj["type"]    = v._type;
                                                                  obj["introll"] = v._introll;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     _introll = -1;
        int     _plus    = 0;
        QString _for     = "";
        int     _type    = -1;
    } v;

    QCheckBox* introll;
    QLineEdit* plus;
    QLineEdit* forwhat;
    QComboBox* type;

    QString optOut() {
        if (v._plus < 0 || v._for.isEmpty()) return "<incomplete>";
        QString res;
        switch (v._type) {
        case 0:
        case 1:
        case 4: res += QString("KS: %2 (+%1)").arg(v._plus).arg(v._for); break;
        case 2: res += QString("AK: %2 (+%1)").arg(v._plus).arg(v._for); break;
        case 3: res += QString("CK: %2 (+%1)").arg(v._plus).arg(v._for); break;
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

class Language: public BackgroundSkill {
public:
    Language(): BackgroundSkill("Language")           { }
    Language(const Language& s): BackgroundSkill(s)    { }
    Language(Language&& s): BackgroundSkill(s)         { }
    Language(const QJsonObject& json): BackgroundSkill(json) { v._which    = json["which"].toString("");
                                                               v._level    = json["level"].toInt(0);
                                                               v._literate = json["literate"].toBool(false);
                                                             }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { which    = createLineEdit(parent, layout, "Which language?");
                                                                  level    = createComboBox(parent, layout, "Fluency", { });
                                                                  literate = createCheckBox(parent, layout, "Literate (If not standard)");
                                                                }
    int     points(bool noStore = false) override               { if (!noStore) store();
                                                                  return v._level + 1 + (v._literate ? 1 : 0);
    }
    void    restore() override                                  { vars s = v;
                                                                  which->setText(s._which);
                                                                  level->setCurrentIndex(s._level);
                                                                  literate->setChecked(s._literate);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._which    = which->text();
                                                                  v._level    = level->currentIndex();
                                                                  v._literate = literate->isChecked();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = BackgroundSkill::toJson();
                                                                  obj["which"]    = v._which;
                                                                  obj["level"]    = v._level;
                                                                  obj["literate"] = v._literate;
                                                                  return obj;
                                                                }

private:
    struct vars {
        QString _which    = "";
        int     _level    = -1;
        bool    _literate = "";
    } v;

    QLineEdit* which;
    QComboBox* level;
    QCheckBox* literate;

    QString optOut() {
        if (v._level < 0 || v._which.isEmpty()) return "<incomplete>";
        return "Language: " + v._which + " (" + QStringList{ "Basic", "Fluent", "Fluent w/accent1", "Idiomatic", "Imitate Dialects" }[v._level] + (v._literate ? ", Literate)" : ")");
    }
};

class PS: public BackgroundSkill {
public:
    PS(): BackgroundSkill("Professional Skill")           { }
    PS(const PS& s): BackgroundSkill(s)    { }
    PS(PS&& s): BackgroundSkill(s)         { }
    PS(const QJsonObject& json): BackgroundSkill(json) { v._what = json["what"].toString("");
                                                         v._plus = json["plus"].toInt(0);
                                                         v._stat = json["stat"].toInt(-1);
                                                       }

    QString description(bool showRoll = false) override         { return (showRoll ? "(" + QString("+%1").arg(v._plus) + ") ": "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { what = createLineEdit(parent, layout, "What profession?");
                                                                  plus = createLineEdit(parent, layout, "Pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  stat = createComboBox(parent, layout, "Base on a stat?", { "STR", "DEX", "CON", "INT", "EGO", "PRE"});
                                                                }
    int     points(bool noStore = false) override               { if (!noStore) store();
                                                                  return v._plus * 2 + (2 + ((v._stat >= 0) ? 1 : 0));
                                                                }
    void    restore() override                                  { vars s = v;
                                                                  what->setText(s._what);
                                                                  plus->setText(QString("%1").arg(s._plus));
                                                                  stat->setCurrentIndex(s._stat);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return (v._stat >= 0) ? add(Sheet::ref().character().characteristic(v._stat).roll(), v._plus)
                                                                                        : QString("%1-").arg(11 + v._plus); }
    void    store() override                                    { v._what = what->text();
                                                                  v._plus = plus->text().toInt(0);
                                                                  v._stat = stat->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = BackgroundSkill::toJson();
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
        if (v._plus <= 0 || v._what.isEmpty()) return "<incomplete>";
        return "PS: " + v._what + ((v._stat != -1) ? QStringList { " (STR)", " (DEX)", " (CON)", " (INT)", " (EGO)", " (PRE)" }[v._stat] : "");
    }

    void numeric(QString) override {
        QString txt = plus->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        plus->undo();
    }
};

class SS: public BackgroundSkill {
public:
    SS(): BackgroundSkill("Science Skill")           { }
    SS(const SS& s): BackgroundSkill(s)    { }
    SS(SS&& s): BackgroundSkill(s)         { }
    SS(const QJsonObject& json): BackgroundSkill(json) { v._what = json["what"].toString("");
                                                         v._plus = json["plus"].toInt(0);
                                                         v._int  = json["int"].toBool(false);
                                                       }

    QString description(bool showRoll = false) override         { return (showRoll ? "(" + QString("+%1").arg(v._plus) + ") ": "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { what    = createLineEdit(parent, layout, "What profession?");
                                                                  plus    = createLineEdit(parent, layout, "Pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  intstat = createCheckBox(parent, layout, "Based on INT");
                                                                }
    int     points(bool noStore = false) override               { if (!noStore) store();
                                                                  return v._plus * 2 + (2 + (v._int ? 1 : 0));
                                                                }
    void    restore() override                                  { vars s = v;
                                                                  what->setText(s._what);
                                                                  plus->setText(QString("%1").arg(s._plus));
                                                                  intstat->setChecked(s._int);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return v._int ? add(Sheet::ref().character().INT().roll(), v._plus)
                                                                                : QString("%1-").arg(11 + v._plus); }
    void    store() override                                    { v._what = what->text();
                                                                  v._plus = plus->text().toInt(0);
                                                                  v._int  = intstat->isChecked();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = BackgroundSkill::toJson();
                                                                  obj["what"] = v._what;
                                                                  obj["plus"] = v._plus;
                                                                  obj["int"]  = v._int;
                                                                  return obj;
                                                                }

private:
    struct vars {
        QString _what = "";
        int     _plus = 0;
        bool    _int = false;
    } v;

    QLineEdit* what;
    QLineEdit* plus;
    QCheckBox* intstat;

    QString optOut() {
        if (v._plus <= 0 || v._what.isEmpty()) return "<incomplete>";
        return "SS: " + v._what + (v._int ? " (INT)" : "");
    }

    void numeric(QString) override {
        QString txt = plus->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        plus->undo();
    }
};

class TF: public BackgroundSkill {
public:
    TF(): BackgroundSkill("Transport Familiarity")     { }
    TF(const TF& s): BackgroundSkill(s)                { }
    TF(TF&& s): BackgroundSkill(s)                     { }
    TF(const QJsonObject& json): BackgroundSkill(json) { v._what = json["what"].toInt(0);
                                                         v._with = json["with"].toString("");
                                                       }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { what = createComboBox(parent, layout, "Familar with?", { "One class of conveyances",
                                                                                                                           "Broad category of conveyances"});
                                                                  with = createLineEdit(parent, layout, "Applies to what?");
                                                                }
    int     points(bool noStore = false) override               { if (!noStore) store();
                                                                  return QList<int>{ 0, 1, 2 }[v._what + 1]; }
    void    restore() override                                  { vars s = v;
                                                                  what->setCurrentIndex(s._what);
                                                                  with->setText(s._with);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._what = what->currentIndex();
                                                                  v._with = with->text();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = BackgroundSkill::toJson();
                                                                  obj["what"] = v._what;
                                                                  obj["with"] = v._with;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     _what = -1;
        QString _with = "";
    } v;

    QComboBox* what;
    QLineEdit* with;

    QString optOut() {
        if (v._with.isEmpty()) return "<incomplete>";
        QString res = "TF: ";
        switch (v._what) {
        case 0: res += QString("(One Class of Conveyances: %1)").arg(v._with); break;
        case 1: res += QString("(Broad Category of Conveyancess: %1)").arg(v._with);   break;
        default: return "<incomplete>";
        }

        return res;
    }
};

#undef CLASS
#undef CLASS_SPACE

#endif // BACKGROUNDSKILLS_H