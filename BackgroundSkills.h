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

    QString description(bool showRoll = false) override { return showRoll ? v.mName : v.mName; }
    bool form(QWidget*, QVBoxLayout*) override          { return true; }
    QString name() override                             { return v.mName; }
    Points points(bool noStore = false) override      { return noStore ? 0_cp : 0_cp; }
    void restore() override                             { }
    QString roll() override                             { return ""; }
    void    store() override                            { }

    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"] = v.mName;
        return obj;
    }

private:
    struct vars {
        QString mName;
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
    KS(const QJsonObject& json): BackgroundSkill(json) { v.mIntRoll = json["introll"].toBool(false);
                                                         v.mPlus    = json["plus"].toInt(1);
                                                         v.mFor     = json["for"].toString("");
                                                         v.mType    = json["type"].toInt(0);
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
                                                                                v.mIntRoll ? add(Sheet::ref().character().INT().roll(), v.mPlus)
                                                                                                 :
#endif
                                                                                    QString("%1-").arg(11 + v.mPlus)) + " " // NOLINT
                                                                                   : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { introll = createCheckBox(parent, layout, "Base off INT");
                                                                  plus = createLineEdit(parent, layout, "How many pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  type = createComboBox(parent, layout, "Type of Knowledge Skill", { "Group", "People", "Areas", "City", "Things" });
                                                                  forwhat = createLineEdit(parent, layout, "Applies to what?");
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override              { if (!noStore) store();
                                                                  auto pts = v.mPlus * 2_cp + (v.mIntRoll ? 3_cp : 2_cp);
#ifndef ISHSC
                                                                  if (Sheet::ref().character().hasScholar()  && (v.mType == 1 || v.mType == 4)) pts -= 1_cp;
                                                                  if (Sheet::ref().character().hasTraveler() && v.mType != 1  && v.mType != 4)  pts -= 1_cp;
#endif
                                                                  if (pts < 1_cp) pts = 1_cp;
                                                                  return pts;
                                                                }
    void    restore() override                                  { vars s = v;
                                                                  plus->setText(QString("%1").arg(s.mPlus));
                                                                  type->setCurrentIndex(s.mType);
                                                                  forwhat->setText(s.mFor);
                                                                  introll->setChecked(s.mIntRoll);
                                                                  v = s;
                                                                }
#ifndef ISHSC
    QString roll() override                                     { return v.mIntRoll ? add(Sheet::ref().character().INT().roll(), v.mPlus) : QString("%1-").arg(11 + v.mPlus); } // NOLINT
#else
    QString roll() override                                     { return QString("%1-").arg(11 + v._plus); }
#endif
    void    store() override                                    { v.mPlus    = plus->text().toInt(0);
                                                                  v.mFor     = forwhat->text();
                                                                  v.mType    = type->currentIndex();
                                                                  v.mIntRoll = introll->isChecked();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = BackgroundSkill::toJson();
                                                                  obj["plus"]    = v.mPlus;
                                                                  obj["for"]     = v.mFor;
                                                                  obj["type"]    = v.mType;
                                                                  obj["introll"] = v.mIntRoll;
                                                                  return obj;
                                                                }

private:
    struct vars {
        bool    mIntRoll = false;
        int     mPlus    = 0;
        QString mFor     = "";
        int     mType    = -1;
    } v;

    QCheckBox* introll = nullptr;
    QLineEdit* plus = nullptr;
    QLineEdit* forwhat = nullptr;
    QComboBox* type = nullptr;

    QString optOut() {
        if (v.mFor.isEmpty()) return "<incomplete>";
        QString res;
        QString prefix = "";
        switch (v.mType) {
        case 0:
        case 1:
        case 4: prefix = "KS: "; break;
        case 2: prefix = "AK: "; break;
        case 3: prefix = "CK: "; break;
        default: return "<incomplete>";
        }
        res += prefix + v.mFor;
        QString sep = " (";
        if (v.mIntRoll) { res += sep + "INT"; sep = ", "; }
        if (v.mPlus > 0) { res += sep + QString("+%1").arg(v.mPlus); sep = ", "; }
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
    Language(const QJsonObject& json): BackgroundSkill(json) { v.mWhich    = json["which"].toString("");
                                                               v.mLevel    = json["level"].toInt(0);
                                                               v.mLiterate = json["literate"].toBool(false);
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
                                                                  auto pts = v.mLevel + 1_cp + (v.mLiterate ? 1_cp : 0_cp);
#ifndef ISHSC
                                                                  if (Sheet::ref().character().hasLinguist()) pts -= 1_cp;
#endif
                                                                  if (pts < 1_cp) pts = 1_cp;
                                                                  return pts;
                                                                }
    void    restore() override                                  { vars s = v;
                                                                  which->setText(s.mWhich);
                                                                  level->setCurrentIndex(s.mLevel);
                                                                  literate->setChecked(s.mLiterate);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mWhich    = which->text();
                                                                  v.mLevel    = level->currentIndex();
                                                                  v.mLiterate = literate->isChecked();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = BackgroundSkill::toJson();
                                                                  obj["which"]    = v.mWhich;
                                                                  obj["level"]    = v.mLevel;
                                                                  obj["literate"] = v.mLiterate;
                                                                  return obj;
                                                                }

private:
    struct vars {
        QString mWhich    = "";
        int     mLevel    = -1;
        bool    mLiterate = "";
    } v;

    QLineEdit* which = nullptr;
    QComboBox* level = nullptr;
    QCheckBox* literate = nullptr;

    QString optOut() {
        if (v.mLevel < 1 || v.mWhich.isEmpty()) return "<incomplete>";
        return "Language: " + v.mWhich + " (" + QStringList{ "Basic", "Fluent", "Fluent w/accent", "Idiomatic", "Imitate Dialects" }[v.mLevel] + (v.mLiterate ? ", Literate)" : ")");
    }
};

class PS: public BackgroundSkill {
public:
    PS(): BackgroundSkill("Professional Skill")           { }
    PS(const PS& s): BackgroundSkill(s)    { }
    PS(PS&& s): BackgroundSkill(s)         { }
    PS(const QJsonObject& json): BackgroundSkill(json) { v.mWhat = json["what"].toString("");
                                                         v.mPlus = json["plus"].toInt(0);
                                                         v.mStat = json["stat"].toInt(-1);
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

    QString description(bool showRoll = false) override         { return (showRoll ? "(" + QString("+%1").arg(v.mPlus) + ") ": "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { what = createLineEdit(parent, layout, "What profession?");
                                                                  plus = createLineEdit(parent, layout, "Pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  stat = createComboBox(parent, layout, "Base on a stat?", { "", "STR", "DEX", "CON", "INT", "EGO", "PRE"});
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override              { if (!noStore) store();
                                                                  auto pts = v.mPlus * 1_cp + (2_cp + ((v.mStat >= 1) ? 1_cp : 0_cp));
#ifndef ISHSC
                                                                  if (Sheet::ref().character().hasJackOfAllTrades()) pts -= 1_cp;
#endif
                                                                  if (pts < 1_cp) pts = 1_cp;
                                                                  return pts;
                                                                }
    void    restore() override                                  { vars s = v;
                                                                  what->setText(s.mWhat);
                                                                  plus->setText(QString("%1").arg(s.mPlus));
                                                                  stat->setCurrentIndex(s.mStat);
                                                                  v = s;
                                                                }
#ifndef ISHSC
    QString roll() override                                     { return (v.mStat >= 1) ? add(Sheet::ref().character().characteristic(v.mStat - 1).roll(), v.mPlus)
                                                                                        : QString("%1-").arg(11 + v.mPlus); } // NOLINT
#else
    QString roll() override                                     { return QString("%1-").arg(11 + v._plus); }
#endif
    void    store() override                                    { v.mWhat = what->text();
                                                                  v.mPlus = plus->text().toInt(0);
                                                                  v.mStat = stat->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = BackgroundSkill::toJson();
                                                                  obj["what"] = v.mWhat;
                                                                  obj["plus"] = v.mPlus;
                                                                  obj["stat"] = v.mStat;
                                                                  return obj;
                                                                }

private:
    struct vars {
        QString mWhat = "";
        int     mPlus = 0;
        int     mStat  = -1;
    } v;

    QLineEdit* what = nullptr;
    QLineEdit* plus = nullptr;
    QComboBox* stat = nullptr;

    QString optOut() {
        if (v.mWhat.isEmpty()) return "<incomplete>";
        QString res = "PS: " + v.mWhat + ((v.mStat > 0) ? QStringList { "", " (STR", " (DEX", " (CON", " (INT", " (EGO", " (PRE" }[v.mStat] : "");
        if (v.mPlus != 0) {
            if (v.mStat > 0) res += ", ";
            else res += " (";
            res += QString("+%1").arg(v.mPlus);
        }
        if (v.mPlus > 0 || v.mStat > 0) res += ")";
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
    SS(const QJsonObject& json): BackgroundSkill(json) { v.mWhat = json["what"].toString("");
                                                         v.mPlus = json["plus"].toInt(0);
                                                         v.mInt  = json["int"].toBool(false);
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

    QString description(bool showRoll = false) override         { return (showRoll ? "(" + QString("+%1").arg(v.mPlus) + ") ": "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { what    = createLineEdit(parent, layout, "What profession?");
                                                                  plus    = createLineEdit(parent, layout, "Pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  intstat = createCheckBox(parent, layout, "Based on INT");
                                                                  return true;
                                                                }
    Points   points(bool noStore = false) override              { if (!noStore) store();
                                                                  auto pts = v.mPlus * 1_cp + (2_cp + (v.mInt ? 1_cp : 0_cp));
#ifndef ISHSC
                                                                  if (Sheet::ref().character().hasScientist()) pts -= 1_cp;
#endif
                                                                  if (pts < 1_cp) pts = 1_cp;
                                                                  return pts;
                                                                }
    void    restore() override                                  { vars s = v;
                                                                  what->setText(s.mWhat);
                                                                  plus->setText(QString("%1").arg(s.mPlus));
                                                                  intstat->setChecked(s.mInt);
                                                                  v = s;
                                                                }
#ifndef ISHSC
    QString roll() override                                     { return v.mInt ? add(Sheet::ref().character().INT().roll(), v.mPlus)
                                                                                : QString("%1-").arg(11 + v.mPlus); } // NOLINT
#else
    QString roll() override                                     { return QString("%1-").arg(11 + v._plus); }
#endif
    void    store() override                                    { v.mWhat = what->text();
                                                                  v.mPlus = plus->text().toInt(0);
                                                                  v.mInt  = intstat->isChecked();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = BackgroundSkill::toJson();
                                                                  obj["what"] = v.mWhat;
                                                                  obj["plus"] = v.mPlus;
                                                                  obj["int"]  = v.mInt;
                                                                  return obj;
                                                                }

private:
    struct vars {
        QString mWhat = "";
        int     mPlus = 0;
        bool    mInt = false;
    } v;

    QLineEdit* what = nullptr;
    QLineEdit* plus = nullptr;
    QCheckBox* intstat = nullptr;

    QString optOut() {
        if (v.mWhat.isEmpty()) return "<incomplete>";
        QString res = "SS: " + v.mWhat + (v.mInt ? " (INT" : "");
        if (v.mPlus != 0) {
            if (v.mInt) res += ", ";
            else res += " (";
            res += QString("+%1").arg(v.mPlus);
        }
        if (v.mPlus > 0 || v.mInt) res += ")";
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
    TF(const QJsonObject& json): BackgroundSkill(json) { v.mWhat = json["what"].toInt(0);
                                                         v.mWith = json["with"].toString("");
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
                                                                  return cost[v.mWhat + 1]; }
    void    restore() override                                  { vars s = v;
                                                                  what->setCurrentIndex(s.mWhat);
                                                                  with->setText(s.mWith);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mWhat = what->currentIndex();
                                                                  v.mWith = with->text();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = BackgroundSkill::toJson();
                                                                  obj["what"] = v.mWhat;
                                                                  obj["with"] = v.mWith;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     mWhat = -1;
        QString mWith = "";
    } v;

    QComboBox* what = nullptr;
    QLineEdit* with = nullptr;

    QString optOut() {
        if (v.mWith.isEmpty()) return "<incomplete>";
        QString res = "TF: ";
        switch (v.mWhat) {
        case 0: res += QString("(One Class of Conveyances: %1)").arg(v.mWith); break;
        case 1: res += QString("(Broad Category of Conveyancess: %1)").arg(v.mWith);   break;
        default: return "<incomplete>";
        }

        return res;
    }
};

#undef CLASS
#undef CLASS_SPACE

#endif // BACKGROUNDSKILLS_H
