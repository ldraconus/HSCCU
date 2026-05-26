#ifndef MISCSKILL_H
#define MISCSKILL_H

#include "character.h"
#ifndef ISHSC
#include "sheet.h"
#endif
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
    ~MiscSkills() override { }
    MiscSkills& operator=(const MiscSkills& s) {
        if (this != &s) {
            v.mName = s.v.mName;
        }
        return *this;
    }
    MiscSkills& operator=(MiscSkills&& s) {
        v.mName = s.v.mName;
        return *this;
    }

    bool isSkill() override { return true; }

    QString description(bool showRoll = false) override { return v.mName +  (showRoll ? "" : ""); }
    bool form(QWidget*, QVBoxLayout*) override          { return false; }
    QString name() override                             { return v.mName; }
    Points points(bool noStore = false) override        { if (!noStore) store(); return 0_cp; }
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
        QString mName = "";
    } v;

};

// NOLINTNEXTLINE
#define CLASS(x)                                     \
    class x: public MiscSkills {                     \
    public:                                          \
        x()                                          \
            : MiscSkills(#x) { }                     \
        x(const x& s)                                \
            : MiscSkills(s) { }                      \
        x(x&& s)                                     \
            : MiscSkills(s) { }                      \
        x(const QJsonObject& json)                   \
            : MiscSkills(json) { }                   \
        ~x() override { }                            \
        x& operator=(const x& s) {                   \
            if (this != &s) { v._name = s.v._name; } \
            return *this;                            \
        }                                            \
        x& operator=(x&& s) {                        \
            v._name = s.v._name;                     \
            return *this;                            \
        }                                            \
    }
// NOLINTNEXTLINE
#define CLASS_SPACE(x, y)                            \
    class x: public MiscSkills {                     \
    public:                                          \
        x()                                          \
            : MiscSkills(y) { }                      \
        x(const x& s)                                \
            : MiscSkills(s) { }                      \
        x(x&& s)                                     \
            : MiscSkills(s) { }                      \
        x(const QJsonObject& json)                   \
            : MiscSkills(json) { }                   \
        ~x() override { }                            \
        x& operator=(const x& s) {                   \
            if (this != &s) { v._name = s.v._name; } \
            return *this;                            \
        }                                            \
        x& operator=(x&& s) {                        \
            v._name = s.v._name;                     \
            return *this;                            \
        }                                            \
    }

class MSL: public MiscSkills {
public:
    MSL(): MiscSkills("Movement Skill Levels")     { }
    MSL(const MSL& s): MiscSkills(s)               { }
    MSL(MSL&& s): MiscSkills(s)                    { }
    MSL(const QJsonObject& json): MiscSkills(json) { v.mPlus = json["plus"].toInt(1);
                                                     v.mFor  = json["for"].toString("");
                                                     v.mSize = json["size"].toInt(0);
    }
    ~MSL() override { }
    MSL& operator=(const MSL& s) {
        if (this != &s) v = s.v;
        return *this;
    }
    MSL& operator=(MSL&& s) {
        v = s.v;
        return *this;
    }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { plus = createLineEdit(parent, layout, "How many pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  size = createComboBox(parent, layout, "Type of skill?", { "One mode of movement",
                                                                                                                            "Any mode of movement" });
                                                                  forwhat = createLineEdit(parent, layout, "Applies to what?");
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override                { if (!noStore) store();
                                                                  QList<Points> size { 0_cp, 2_cp, 3_cp };
                                                                  return v.mPlus * size[v.mSize + 1]; }
    void    restore() override                                  { vars s = v;
                                                                  plus->setText(QString("%1").arg(s.mPlus));
                                                                  size->setCurrentIndex(s.mSize);
                                                                  forwhat->setText(s.mFor);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mPlus = plus->text().toInt(0);
                                                                  v.mFor  = forwhat->text();
                                                                  v.mSize = size->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = MiscSkills::toJson();
                                                                  obj["plus"] = v.mPlus;
                                                                  obj["for"]  = v.mFor;
                                                                  obj["size"] = v.mSize;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     mPlus = 0;
        QString mFor = "";
        int     mSize = -1;
    } v;

    QLineEdit* plus = nullptr;
    QLineEdit* forwhat = nullptr;
    QComboBox* size = nullptr;

    QString optOut() {
        if (v.mPlus < 1 || v.mSize < 0) return "<incomplete>";
        QString res = "Movement Skill Levels: ";
        switch (v.mSize) {
        case 0: res += QString("+%1 with %2").arg(v.mPlus).arg(v.mFor);           break;
        case 1: res += QString("+%1 with all movement").arg(v.mPlus).arg(v.mFor); break;
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
    PowerSkill(const QJsonObject& json): MiscSkills(json) { v.mWhat = json["what"].toString("");
                                                            v.mPlus = json["plus"].toInt(0);
                                                            v.mStat = json["stat"].toInt(-1);
                                                          }
    ~PowerSkill() override { }
    PowerSkill& operator=(const PowerSkill& s) {
        if (this != &s) v = s.v;
        return *this;
    }
    PowerSkill& operator=(PowerSkill&& s) {
        v = s.v;
        return *this;
    }

    QString description(bool showRoll = false) override         { return (showRoll ? "(" + QString("+%1").arg(v.mPlus) + ") ": "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { what = createLineEdit(parent, layout, "What power?");
                                                                  plus = createLineEdit(parent, layout, "Pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  stat = createComboBox(parent, layout, "Base on a stat?", { "No", "STR", "DEX", "CON", "INT", "EGO", "PRE"});
                                                                  return true;
                                                                }
    Points   points(bool noStore = false) override              { if (!noStore) store();
                                                                  return v.mPlus * 2_cp + 3_cp;
                                                                }
    void    restore() override                                  { vars s = v;
                                                                  what->setText(s.mWhat);
                                                                  plus->setText(QString("%1").arg(s.mPlus));
                                                                  stat->setCurrentIndex(s.mStat);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return (v.mStat >= 1) ?
#ifndef ISHSC
                                                                             add(Sheet::ref().character().characteristic(v.mStat).roll(), v.mPlus)
#else
                                                                             QString("+%1").arg(v._plus)
#endif
                                                                                        : ""; }
    void    store() override                                    { v.mWhat = what->text();
                                                                  v.mPlus = plus->text().toInt(0);
                                                                  v.mStat = stat->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = MiscSkills::toJson();
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
        if (v.mPlus <= 0 || v.mWhat.isEmpty() || v.mStat < 1) return "<incomplete>";
        QStringList stat { "", " (STR", " (DEX", " (CON", " (INT", " (EGO", " (PRE" };
        return QString("+%1 with ").arg(v.mPlus) + v.mWhat + stat[v.mStat] + ((v.mStat != 0) ? " based)" : "");
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
    SkillLevels(const QJsonObject& json): MiscSkills(json)  { v.mPlus = json["plus"].toInt(1);
                                                              v.mFor  = json["for"].toString("");
                                                              v.mSize = json["size"].toInt(0);
                                                            }
    ~SkillLevels() override { }
    SkillLevels& operator=(const SkillLevels& s) {
        if (this != &s) v = s.v;
        return *this;
    }
    SkillLevels& operator=(SkillLevels&& s) {
        v = s.v;
        return *this;
    }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { plus = createLineEdit(parent, layout, "How many pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  forwhat = createLineEdit(parent, layout, "Applies to what?");
                                                                  size = createComboBox(parent, layout, "For?", { "One Skill or Characteristic",
                                                                                                                  "Any three pre-defined Skills",
                                                                                                                  "All Intellect, Interaction or similar broad group",
                                                                                                                  "All Agility Skills",
                                                                                                                  "All Non-Combat Skills",
                                                                                                                  "Overall" });
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override                { if (!noStore) store();
                                                                  QList<Points> size { 0_cp, 2_cp, 3_cp, 4_cp, 6_cp, 10_cp, 12_cp }; // NOLINT
                                                                  return v.mPlus * size[v.mSize + 1]; }
    void    restore() override                                  { vars s = v;
                                                                  plus->setText(QString("%1").arg(s.mPlus));
                                                                  size->setCurrentIndex(s.mSize);
                                                                  forwhat->setText(s.mFor);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mPlus = plus->text().toInt(0);
                                                                  v.mFor  = forwhat->text();
                                                                  v.mSize = size->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = MiscSkills::toJson();
                                                                  obj["plus"] = v.mPlus;
                                                                  obj["for"]  = v.mFor;
                                                                  obj["size"] = v.mSize;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     mPlus = 0;
        QString mFor = "";
        int     mSize = -1;
    } v;

    QLineEdit* plus = nullptr;
    QLineEdit* forwhat = nullptr;
    QComboBox* size = nullptr;

    QString optOut() {
        if (v.mPlus < 1 || (v.mSize < 3 && v.mFor.isEmpty()) || v.mSize < 0) return "<incomplete>";
        QString res;
        switch (v.mSize) {
        case 0:
        case 1:
        case 2: res += QString("+%1 with %2").arg(v.mPlus).arg(v.mFor);                         break;
        case 3: res += QString("+%1 With All Agility Skills").arg(v.mPlus);                     break;
        case 4: res += QString("+%1 With All Non-Combat Skills").arg(v.mPlus);                  break;
        case 5: res += QString("+%1 Overall Level%2").arg(v.mPlus).arg(v.mPlus > 1 ? "s" : ""); break; // NOLINT
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
