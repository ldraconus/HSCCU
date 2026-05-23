#ifndef BACKGROUNDSKILLS_H
#define BACKGROUNDSKILLS_H

#include "character.h"
#ifndef ISHSC
#include "sheet.h"
#endif
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
    ~BackgroundSkill() override { }

    BackgroundSkill& operator=(const BackgroundSkill& s) {
        if (this != &s) v = s.v;
        return *this;
    }
    BackgroundSkill& operator=(BackgroundSkill&& s) {
        v = s.v;
        return *this;
    }

    bool isSkill() override { return true; }

    QString description(bool showRoll = false) override { return showRoll ? v._name : v._name; }
    bool form(QWidget*, QVBoxLayout*) override          { return true; }
    QString name() override                             { return v._name; }
    Points points(bool noStore = false) override      { return noStore ? 0_cp : 0_cp; }
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

// NOLINTNEXTLINE
#define CLASS(x)\
    class x: public BackgroundSkill {\
    public:\
        x(): BackgroundSkill(#x) { }\
        x(const x& s): BackgroundSkill(s) { }\
        x(x&& s): BackgroundSkill(s) { }\
        x(const QJsonObject& json): BackgroundSkill(json) { }\
    };
// NOLINTNEXTLINE
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
    ~KS() override { }

    KS& operator=(const KS& s) {
        if (this != &s) {
            BackgroundSkill::operator=(s);
            v = s.v;
        }
        return *this;
    }
    KS& operator=(KS&& s) {
        BackgroundSkill::operator=(s);
        v = s.v;
        return *this;
    }

    QString description(bool showRoll = false) override         { return (showRoll ? (
#ifndef ISHSC
                                                                                v._introll ? add(Sheet::ref().character().INT().roll(), v._plus)
                                                                                                 :
#endif
                                                                                    QString("%1-").arg(11 + v._plus)) + " " // NOLINT
                                                                                   : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { introll = createCheckBox(parent, layout, "Base off INT");
                                                                  plus = createLineEdit(parent, layout, "How many pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  type = createComboBox(parent, layout, "Type of Knowledge Skill", { "Group", "People", "Areas", "City", "Things" });
                                                                  forwhat = createLineEdit(parent, layout, "Applies to what?");
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override              { if (!noStore) store();
                                                                  auto pts = v._plus * 2_cp + (v._introll ? 3_cp : 2_cp);
#ifndef ISHSC
                                                                  if (Sheet::ref().character().hasScholar()  && (v._type == 1 || v._type == 4)) pts -= 1_cp;
                                                                  if (Sheet::ref().character().hasTraveler() && v._type != 1  && v._type != 4)  pts -= 1_cp;
#endif
                                                                  if (pts < 1_cp) pts = 1_cp;
                                                                  return pts;
                                                                }
    void    restore() override                                  { vars s = v;
                                                                  plus->setText(QString("%1").arg(s._plus));
                                                                  type->setCurrentIndex(s._type);
                                                                  forwhat->setText(s._for);
                                                                  introll->setChecked(s._introll);
                                                                  v = s;
                                                                }
#ifndef ISHSC
    QString roll() override                                     { return v._introll ? add(Sheet::ref().character().INT().roll(), v._plus) : QString("%1-").arg(11 + v._plus); } // NOLINT
#else
    QString roll() override                                     { return QString("%1-").arg(11 + v._plus); }
#endif
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
        bool    _introll = false;
        int     _plus    = 0;
        QString _for     = "";
        int     _type    = -1;
    } v;

    QCheckBox* introll = nullptr;
    QLineEdit* plus = nullptr;
    QLineEdit* forwhat = nullptr;
    QComboBox* type = nullptr;

    QString optOut() {
        if (v._for.isEmpty()) return "<incomplete>";
        QString res;
        QString prefix = "";
        switch (v._type) {
        case 0:
        case 1:
        case 4: prefix = "KS: "; break;
        case 2: prefix = "AK: "; break;
        case 3: prefix = "CK: "; break;
        default: return "<incomplete>";
        }
        res += prefix + v._for;
        QString sep = " (";
        if (v._introll) { res += sep + "INT"; sep = ", "; }
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

class Language: public BackgroundSkill {
public:
    Language(): BackgroundSkill("Language")            { }
    Language(const Language& s): BackgroundSkill(s)    { }
    Language(Language&& s): BackgroundSkill(s)         { }
    Language(const QJsonObject& json): BackgroundSkill(json) { v._which    = json["which"].toString("");
                                                               v._level    = json["level"].toInt(0);
                                                               v._literate = json["literate"].toBool(false);
                                                             }
    ~Language() override { }

    Language& operator=(const Language& s) {
        if (this != &s) {
            BackgroundSkill::operator=(s);
            v = s.v;
       }
       return *this;
    }
    Language& operator=(Language&& s) {
        BackgroundSkill::operator=(s);
        v = s.v;
        return *this;
    }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { which    = createLineEdit(parent, layout, "Which language?");
                                                                  level    = createComboBox(parent, layout, "Fluency", { "Basic", "Fluent", "Fluent w/accent",
                                                                                                                         "Idiomatic", "Imitate Dialects" });
                                                                  literate = createCheckBox(parent, layout, "Literate (If not standard)");
                                                                  return true;
                                                                }
    Points   points(bool noStore = false) override              { if (!noStore) store();
                                                                  auto pts = v._level + 1_cp + (v._literate ? 1_cp : 0_cp);
#ifndef ISHSC
                                                                  if (Sheet::ref().character().hasLinguist()) pts -= 1_cp;
#endif
                                                                  if (pts < 1_cp) pts = 1_cp;
                                                                  return pts;
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

    QLineEdit* which = nullptr;
    QComboBox* level = nullptr;
    QCheckBox* literate = nullptr;

    QString optOut() {
        if (v._level < 1 || v._which.isEmpty()) return "<incomplete>";
        return "Language: " + v._which + " (" + QStringList{ "Basic", "Fluent", "Fluent w/accent", "Idiomatic", "Imitate Dialects" }[v._level] + (v._literate ? ", Literate)" : ")");
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
    ~PS() override { }

    PS& operator=(const PS& s) {
        if (this != &s) {
            BackgroundSkill::operator=(s);
            v = s.v;
        }
        return *this;
    }
    PS& operator=(PS&& s) {
        BackgroundSkill::operator=(s);
        v = s.v;
        return *this;
    }

    QString description(bool showRoll = false) override         { return (showRoll ? "(" + QString("+%1").arg(v._plus) + ") ": "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { what = createLineEdit(parent, layout, "What profession?");
                                                                  plus = createLineEdit(parent, layout, "Pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  stat = createComboBox(parent, layout, "Base on a stat?", { "", "STR", "DEX", "CON", "INT", "EGO", "PRE"});
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override              { if (!noStore) store();
                                                                  auto pts = v._plus * 1_cp + (2_cp + ((v._stat >= 1) ? 1_cp : 0_cp));
#ifndef ISHSC
                                                                  if (Sheet::ref().character().hasJackOfAllTrades()) pts -= 1_cp;
#endif
                                                                  if (pts < 1_cp) pts = 1_cp;
                                                                  return pts;
                                                                }
    void    restore() override                                  { vars s = v;
                                                                  what->setText(s._what);
                                                                  plus->setText(QString("%1").arg(s._plus));
                                                                  stat->setCurrentIndex(s._stat);
                                                                  v = s;
                                                                }
#ifndef ISHSC
    QString roll() override                                     { return (v._stat >= 1) ? add(Sheet::ref().character().characteristic(v._stat - 1).roll(), v._plus)
                                                                                        : QString("%1-").arg(11 + v._plus); } // NOLINT
#else
    QString roll() override                                     { return QString("%1-").arg(11 + v._plus); }
#endif
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

    QLineEdit* what = nullptr;
    QLineEdit* plus = nullptr;
    QComboBox* stat = nullptr;

    QString optOut() {
        if (v._what.isEmpty()) return "<incomplete>";
        QString res = "PS: " + v._what + ((v._stat > 0) ? QStringList { "", " (STR", " (DEX", " (CON", " (INT", " (EGO", " (PRE" }[v._stat] : "");
        if (v._plus != 0) {
            if (v._stat > 0) res += ", ";
            else res += " (";
            res += QString("+%1").arg(v._plus);
        }
        if (v._plus > 0 || v._stat > 0) res += ")";
        return res;
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
    ~SS() override { }

    SS& operator=(const SS& s) {
        if (this != &s) {
            BackgroundSkill::operator=(s);
            v = s.v;
        }
        return *this;
    }
    SS& operator=(SS&& s) {
        BackgroundSkill::operator=(s);
        v = s.v;
        return *this;
    }

    QString description(bool showRoll = false) override         { return (showRoll ? "(" + QString("+%1").arg(v._plus) + ") ": "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { what    = createLineEdit(parent, layout, "What profession?");
                                                                  plus    = createLineEdit(parent, layout, "Pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  intstat = createCheckBox(parent, layout, "Based on INT");
                                                                  return true;
                                                                }
    Points   points(bool noStore = false) override              { if (!noStore) store();
                                                                  auto pts = v._plus * 1_cp + (2_cp + (v._int ? 1_cp : 0_cp));
#ifndef ISHSC
                                                                  if (Sheet::ref().character().hasScientist()) pts -= 1_cp;
#endif
                                                                  if (pts < 1_cp) pts = 1_cp;
                                                                  return pts;
                                                                }
    void    restore() override                                  { vars s = v;
                                                                  what->setText(s._what);
                                                                  plus->setText(QString("%1").arg(s._plus));
                                                                  intstat->setChecked(s._int);
                                                                  v = s;
                                                                }
#ifndef ISHSC
    QString roll() override                                     { return v._int ? add(Sheet::ref().character().INT().roll(), v._plus)
                                                                                : QString("%1-").arg(11 + v._plus); } // NOLINT
#else
    QString roll() override                                     { return QString("%1-").arg(11 + v._plus); }
#endif
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

    QLineEdit* what = nullptr;
    QLineEdit* plus = nullptr;
    QCheckBox* intstat = nullptr;

    QString optOut() {
        if (v._what.isEmpty()) return "<incomplete>";
        QString res = "SS: " + v._what + (v._int ? " (INT" : "");
        if (v._plus != 0) {
            if (v._int) res += ", ";
            else res += " (";
            res += QString("+%1").arg(v._plus);
        }
        if (v._plus > 0 || v._int) res += ")";
        return res;
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
    ~TF() override { }

    TF& operator=(const TF& s) {
        if (this != &s) {
            BackgroundSkill::operator=(s);
            v = s.v;
        }
        return *this;
    }
    TF& operator=(TF&& s) {
        BackgroundSkill::operator=(s);
        v = s.v;
        return *this;
    }
    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { what = createComboBox(parent, layout, "Familar with?", { "One class of conveyances",
                                                                                                                           "Broad category of conveyances"});
                                                                  with = createLineEdit(parent, layout, "Applies to what?");
                                                                  return true;
                                                                }
    Points   points(bool noStore = false) override              { if (!noStore) store();
                                                                  QList<Points> cost { 0_cp, 1_cp, 2_cp };
                                                                  return cost[v._what + 1]; }
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

    QComboBox* what = nullptr;
    QLineEdit* with = nullptr;

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
