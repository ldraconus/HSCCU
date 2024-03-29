#ifndef PERK_H
#define PERK_H

#ifndef ISHSC
#include "sheet.h"
#endif
#include "skilltalentorperk.h"

class Perks: public SkillTalentOrPerk {
public:
    Perks(): SkillTalentOrPerk() { }
    Perks(QString name)
        : v { name } { }
    Perks(const Perks& s)
        : SkillTalentOrPerk()
        , v(s.v) { }
    Perks(Perks&& s)
        : SkillTalentOrPerk()
        , v(s.v) { }
    Perks(const QJsonObject& json)
        : SkillTalentOrPerk()
        , v { json["name"].toString("") } { }
    ~Perks() override { }

    Perks& operator=(const Perks& s) = delete;
    Perks& operator=(Perks&& s) = delete;

    bool isPerk() override { return true; }

    QString description(bool showRoll = false) override { return v._name + (showRoll ? "" : ""); }
    bool    form(QWidget*, QVBoxLayout*) override       { return false; }
    QString name() override                             { return v._name; }
    Points points(bool noStore = false) override        { if (!noStore) store(); return 0_cp; }
    void    restore() override                          { }
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
    } v {};
};

// NOLINTNEXTLINE
#define CLASS(x)\
    class x: public AgilitySkills {\
    public:\
        x(): AgilitySkills(#x) { }\
        x(const x& s): AgilitySkills(s) { }\
        x(x&& s): AgilitySkills(s) { }\
        x(const QJsonObject& json): AgilitySkills(json) { }\
    };
// NOLINTNEXTLINE
#define CLASS_SPACE(x,y)\
    class x: public AgilitySkills {\
    public:\
        x(): AgilitySkills(y) { }\
        x(const x& s): AgilitySkills(s) { }\
        x(x&& s): AgilitySkills(s) { }\
        x(const QJsonObject& json): AgilitySkills(json) { }\
    };

class Access: public Perks {
public:
    Access(): Perks("Access") { }
    Access(const Access& s): Perks(s)             { }
    Access(Access&& s): Perks(s)                  { }
    Access(const QJsonObject& json): Perks(json)  { v._cost = json["cost"].toInt(1);
                                                    v._for  = json["for"].toString("");
                                                    v._hide = json["hide"].toInt(0);
    }
    ~Access() override { }

    Access& operator=(const Access&) = delete;
    Access& operator=(Access&&) = delete;

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { forwhat = createLineEdit(parent, layout, "Applies to what?");
                                                                  cost    = createComboBox(parent, layout, "How many Points?", { "1", "2", "3", "4", "5" });
                                                                  hide    = createLineEdit(parent, layout, "Bonus to hiding access", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override                { if (!noStore) store();
                                                                  return (v._cost + 1) * 1_cp + v._hide; }
    void    restore() override                                  { vars s = v;
                                                                  cost->setCurrentIndex(s._cost);
                                                                  hide->setText(QString("%1").arg(s._hide));
                                                                  forwhat->setText(s._for);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._cost = cost->currentIndex();
                                                                  v._for  = forwhat->text();
                                                                  v._hide = hide->text().toInt(0);
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Perks::toJson();
                                                                  obj["cost"] = v._cost;
                                                                  obj["for"]  = v._for;
                                                                  obj["hide"] = v._hide;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     _cost = -1;
        QString _for = "";
        int     _hide = 0;
    } v;

    QComboBox* cost = nullptr;
    QLineEdit* forwhat = nullptr;
    QLineEdit* hide = nullptr;

    QString optOut() {
        if (v._cost < 0 || v._for.isEmpty()) return "<incomplete>";
        QString res = "Access: " + v._for;
        if (v._hide > 0) res += QString(" (-%1 to discover)").arg(v._hide);
        return res;
    }

    void numeric(QString) override {
        QString txt = hide->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        hide->undo();
    }
};

class Anonymity: public Perks {
public:
    Anonymity(): Perks("Anonymity")                  { }
    Anonymity(const Anonymity& s): Perks(s)          { }
    Anonymity(Anonymity&& s): Perks(s)               { }
    Anonymity(const QJsonObject& json)
        : Perks(json) {
        v._extra = json["extra"].toInt(1);
    }
    ~Anonymity() override { }

    Anonymity& operator=(const Anonymity&) = delete;
    Anonymity& operator=(Anonymity&&) = delete;

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { extra = createLineEdit(parent, layout, "Extra cost", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override                { if (!noStore) store();
                                                                  return 3_cp + v._extra; }
    void    restore() override                                  { vars s = v;
                                                                  extra->setText(QString("%1").arg(s._extra));
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._extra = extra->text().toInt(0);
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Perks::toJson();
                                                                  obj["extra"] = v._extra;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int _extra = 0;
    } v;

    QLineEdit* extra = nullptr;

    QString optOut() {
        return "Anonymity";
    }

    void numeric(QString) override {
        QString txt = extra->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        extra->undo();
    }
};

class ComputerLink: public Perks {
public:
    ComputerLink(): Perks("Computer Link")              { }
    ComputerLink(const ComputerLink& s): Perks(s)       { }
    ComputerLink(ComputerLink&& s): Perks(s)            { }
    ComputerLink(const QJsonObject& json): Perks(json)  { v._value = json["value"].toInt(1);
        v._for = json["for"].toString("");
    }
    ~ComputerLink() override { }

    ComputerLink& operator=(const ComputerLink&) = delete;
    ComputerLink& operator=(ComputerLink&&) = delete;

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { forwhat = createLineEdit(parent, layout, "Applies to what?");
                                                                  value   = createLineEdit(parent, layout, "Bonus for value", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override                { if (!noStore) store();
                                                                  return v._value + 1_cp; }
    void    restore() override                                  { vars s = v;
                                                                  value->setText(QString("%1").arg(s._value));
                                                                  forwhat->setText(s._for);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._for   = forwhat->text();
                                                                  v._value = value->text().toInt(0);
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Perks::toJson();
                                                                  obj["for"]   = v._for;
                                                                  obj["value"] = v._value;
                                                                  return obj;
                                                                }

private:
    struct vars {
        QString _for = "";
        int     _value = 0;
    } v;

    QLineEdit* forwhat = nullptr;
    QLineEdit* value = nullptr;

    QString optOut() {
        if (v._for.isEmpty()) return "<incomplete>";
        QString res = "Computer Link: " + v._for;
        return res;
    }

    void numeric(QString) override {
        QString txt = value->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        value->undo();
    }
};

class Contact: public Perks {
public:
    Contact(): Perks("Contact")                   { }
    Contact(const Contact& s): Perks(s)           { }
    Contact(Contact&& s): Perks(s)                { }
    Contact(const QJsonObject& json): Perks(json) { v._base     = json["base"].toInt(0);
                                                    v._plus     = json["plus"].toInt(0);
                                                    v._limited  = json["limited"].toBool(false);
                                                    v._who      = json["who"].toString("");
                                                    v._useful   = json["useful"].toInt(0);
                                                    v._access   = json["access"].toBool(false);
                                                    v._contacts = json["contacts"].toBool(false);
                                                    v._relate   = json["relate"].toInt(0);
                                                    v._org = json["org"].toBool(false);
    }
    ~Contact() override { }

    Contact& operator=(const Contact&) = delete;
    Contact& operator=(Contact&&) = delete;

    QString description(bool showRoll = false) override         { return (showRoll ? roll() + " " : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { who      = createLineEdit(parent, layout, "Contact is?");
                                                                  base     = createComboBox(parent, layout, "Base Contact Roll", { "8-", "11-" });
                                                                  plus     = createLineEdit(parent, layout, "Bonus to Contact Roll", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  limited  = createCheckBox(parent, layout, "Limited by PC identity");
                                                                  useful   = createComboBox(parent, layout, "How useful is contact?", { "Useful Skills or resources",
                                                                                                                                        "Very Useful Skills or reesources",
                                                                                                                                        "Even more Userful SKills or resources",
                                                                                                                                        "Extremely Useful Skills or resources" });
                                                                  access   = createCheckBox(parent, layout, "Contact has access to major institutions");
                                                                  contacts = createCheckBox(parent, layout, "Contact has significant contacts of their own");
                                                                  relate   = createComboBox(parent, layout, "Relationship with contact", { "Unfriendly",
                                                                                                                                           "Neutral",
                                                                                                                                           "Good",
                                                                                                                                           "Very Good",
                                                                                                                                           "Slavishly Loyal" });
                                                                  org = createCheckBox(parent, layout, "Contact is an organization");
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override                { if (!noStore) store();
                                                                  auto pts = (v._base + 1_cp + v._plus - (v._limited ? 1_cp : 0_cp) + (v._useful > 0 ? 1_cp * v._useful: 0_cp) + (v._access ? 1_cp : 0_cp) +
                                                                          (v._contacts ? 1_cp : 0_cp) + ((v._relate == 0) ? -2 : v._relate - 1)) * (v._org ? 3_cp : 1_cp);
#ifndef ISHSC
                                                                  if (Sheet::ref().character().hasWellConnected()) pts -= 1_cp;
#endif
                                                                  if (pts < 1_cp) pts = 1_cp;
                                                                  return pts;
                                                                }
    void    restore() override                                  { vars s = v;
                                                                  base->setCurrentIndex(s._base);
                                                                  plus->setText(QString("%1").arg(s._plus));
                                                                  limited->setChecked(s._limited);
                                                                  who->setText(s._who);
                                                                  useful->setCurrentIndex(s._useful);
                                                                  access->setChecked(s._access);
                                                                  contacts->setChecked(s._contacts);
                                                                  relate->setCurrentIndex(s._relate);
                                                                  org->setChecked(s._org);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return QString("%1-").arg(8 + v._base * 3 + v._plus); } // NOLINT
    void    store() override                                    { v._base     = base->currentIndex();
                                                                  v._plus     = plus->text().toInt(0);
                                                                  v._limited  = limited->isChecked();
                                                                  v._who      = who->text();
                                                                  v._useful   = useful->currentIndex();
                                                                  v._access   = access->isChecked();
                                                                  v._contacts = contacts->isChecked();
                                                                  v._relate   = relate->currentIndex();
                                                                  v._org      = org->isChecked();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Perks::toJson();
                                                                  obj["base"]     = v._base;
                                                                  obj["plus"]     = v._plus;
                                                                  obj["limited"]  = v._limited;
                                                                  obj["who"]      = v._who;
                                                                  obj["useful"]   = v._useful;
                                                                  obj["access"]   = v._access;
                                                                  obj["contacts"] = v._contacts;
                                                                  obj["relate"]   = v._relate;
                                                                  obj["org"]      = v._org;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     _base = -1;
        int     _plus = 0;
        bool    _limited = false;
        QString _who = "";
        int     _useful = -1;
        bool    _access = false;
        bool    _contacts = false;
        int     _relate = -1;
        bool    _org = false;
    } v;

    QComboBox* base = nullptr;
    QLineEdit* plus = nullptr;
    QCheckBox* limited = nullptr;
    QLineEdit* who = nullptr;
    QComboBox* useful = nullptr;
    QCheckBox* access = nullptr;
    QCheckBox* contacts = nullptr;
    QComboBox* relate = nullptr;
    QCheckBox* org = nullptr;

    QString optOut() {
        if (v._base < 0 || v._useful < 0 || v._relate < 0 || v._who.isEmpty()) return "<incomplete>";
        QString res;
        if (v._plus) res += QString("+%1 ").arg(v._plus);
        res += "Contact: " + v._who;
        QString sep = " (";
        if (v._limited) { res += sep + "Limited"; sep = "; "; }
        QStringList useful { "Useful", "Very Useful", "More Userful", "Extremely Useful" };
        res += sep + useful[v._useful];
        sep = "; ";
        if (v._access) res += sep + "Access";
        if (v._contacts) res += sep + "Contacts";
        QStringList relate { "Unfriendly", "Neutral", "Good", "Very Good", "Slavishly Loyal" };
        res += sep + relate[v._relate];
        if (v._org) res += sep + "Organization";
        return res + ")";
    }

    void numeric(QString) override {
        QString txt = plus->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        plus->undo();
    }
};

class DeepCover: public Perks {
public:
    DeepCover(): Perks("Deep Cover")                 { }
    DeepCover(const DeepCover& s): Perks(s)          { }
    DeepCover(DeepCover&& s): Perks(s)               { }
    DeepCover(const QJsonObject& json)
        : Perks(json) {
        v._as = json["as"].toString("");
    }
    ~DeepCover() override { }

    DeepCover& operator=(const DeepCover&) = delete;
    DeepCover& operator=(DeepCover&&) = delete;

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { as = createLineEdit(parent, layout, "What is the cover identity?");
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override                { if (!noStore) store();
                                                                  return 2_cp; }
    void    restore() override                                  { vars s = v; as->setText(v._as); v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._as = as->text();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Perks::toJson();
                                                                  obj["as"] = v._as;
                                                                  return obj;
                                                                }

private:
    struct vars {
        QString _as = "";
    } v;

    QLineEdit* as = nullptr;

    QString optOut() {
        if (v._as.isEmpty()) return "<incomplete>";
        return "Deep Cover: " + v._as;
    }
};

class Favor: public Perks {
public:
    Favor(): Perks("Favor")                     { }
    Favor(const Favor& s): Perks(s)             { }
    Favor(Favor&& s): Perks(s)                  { }
    Favor(const QJsonObject& json): Perks(json) { v._who  = json["who"].toString("");
        v._cost = json["cost"].toInt(1);
    }
    ~Favor() override { }

    Favor& operator=(const Favor&) = delete;
    Favor& operator=(Favor&&) = delete;

    QString description(bool showRoll = false) override         { return (showRoll ? roll() + " " : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { who  = createLineEdit(parent, layout, "Who owes the favor?");
                                                                  cost = createLineEdit(parent, layout, "How much does the favor cost?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override                { if (!noStore) store();
                                                                  auto pts = (v._cost > 0) ? Points(v._cost) : 1_cp;
#ifndef ISHSC
                                                                  if (Sheet::ref().character().hasWellConnected()) pts -= 1_cp;
#endif
                                                                  if (pts < 1_cp) pts = 1_cp;
                                                                  return pts; }
    void    restore() override                                  { vars s = v;
                                                                  who->setText(v._who);
                                                                  cost->setText(QString("%1").arg(v._cost));
                                                                  v = s;
                                                                }
    QString roll() override                                     { return "14-"; }
    void    store() override                                    { v._who  = who->text();
                                                                  v._cost = cost->text().toInt();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Perks::toJson();
                                                                  obj["who"]  = v._who;
                                                                  obj["cost"] = v._cost;
                                                                  return obj;
                                                                }

private:
    struct vars {
        QString _who = "";
        int     _cost = 1;
    } v;

    QLineEdit* who = nullptr;
    QLineEdit* cost = nullptr;

    QString optOut() {
        if (v._who.isEmpty()) return "<incomplete>";
        return "Favor: " + v._who + QString(" (%1 pt)").arg(v._cost);
    }

    void numeric(QString) override {
        QString txt = cost->text();
        if (txt.isEmpty()) return;
        if (isNumber(txt)) {
            if (txt.toInt() < 1) cost->setText("1");
            return;
        }
        cost->undo();
    }
};

class Follower: public Perks {
public:
    Follower(): Perks("Follower")                  { }
    Follower(const Follower& s): Perks(s)          { }
    Follower(Follower&& s): Perks(s)               { }
    Follower(const QJsonObject& json): Perks(json) { v._who  = json["who"].toString("");
                                                     v._pnts = json["pnts"].toInt(1);
                                                     v._mult = json["mult"].toInt(0);
    }
    ~Follower() override { }

    Follower& operator=(const Follower&) = delete;
    Follower& operator=(Follower&&) = delete;

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { who  = createLineEdit(parent, layout, "Who is/are the follower(s)?");
                                                                  pnts = createLineEdit(parent, layout, "How many points in the follower?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  mult = createLineEdit(parent, layout, "How many multiples of followers?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override                { if (!noStore) store();
                                                                  return (v._pnts + 3) / 5 + v._mult * 5_cp; } // NOLINT
    void    restore() override                                  { vars s= v;
                                                                  who->setText(s._who);
                                                                  pnts->setText(QString("%1").arg(s._pnts));
                                                                  mult->setText(QString("%1").arg(s._mult));
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._who  = who->text();
                                                                  v._pnts = pnts->text().toInt();
                                                                  v._mult = mult->text().toInt();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Perks::toJson();
                                                                  obj["who"]  = v._who;
                                                                  obj["pnts"] = v._pnts;
                                                                  obj["mult"] = v._mult;
                                                                  return obj;
                                                                }

private:
    struct vars {
    QString _who = "";
    int     _pnts = 0;
    int     _mult = 0;
    } v;

    QLineEdit* who = nullptr;
    QLineEdit* pnts = nullptr;
    QLineEdit* mult = nullptr;

    QString optOut() {
        if (v._who.isEmpty() || v._pnts == 0) return "<incomplete>";
        // NOLINTNEXTLINE
        return QString("%1Follower%2: ").arg(v._mult != 0 ? QString("x%1 ").arg(pow(2.0, v._mult)) : "", v._mult != 0 ? "s" : "") + v._who + QString(" (%1 points)").arg(v._pnts);
    }

    void numeric(QString) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }
};

class FringeBenefit: public Perks {
public:
    FringeBenefit(): Perks("Fringe Benefit")            { }
    FringeBenefit(const FringeBenefit& s): Perks(s)     { }
    FringeBenefit(FringeBenefit&& s): Perks(s)          { }
    FringeBenefit(const QJsonObject& json)
        : Perks(json) {
        v._cost = json["cost"].toInt(1);
        v._for = json["for"].toString("");
    }
    ~FringeBenefit() override { }

    FringeBenefit& operator=(const FringeBenefit&) = delete;
    FringeBenefit& operator=(FringeBenefit&&) = delete;

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { forwhat = createLineEdit(parent, layout, "What is it?");
                                                                  cost    = createLineEdit(parent, layout, "Cost of the fringe benefit", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override                { if (!noStore) store();
                                                                  return Points(v._cost); }
    void    restore() override                                  { vars s = v;
                                                                  cost->setText(QString("%1").arg(s._cost));
                                                                  forwhat->setText(s._for);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._for  = forwhat->text();
                                                                  v._cost = cost->text().toInt(0);
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Perks::toJson();
                                                                  obj["for"]  = v._for;
                                                                  obj["cost"] = v._cost;
                                                                  return obj;
                                                                }

private:
    struct vars {
        QString _for  = "";
        int     _cost = 0;
    } v;

    QLineEdit* forwhat = nullptr;
    QLineEdit* cost = nullptr;

    QString optOut() {
        if (v._for.isEmpty()) return "<incomplete>";
        QString res = "Fringe Benefit: " + v._for;
        return res;
    }

    void numeric(QString) override {
        QString txt = cost->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        cost->undo();
    }
};

class Money: public Perks {
public:
    Money(): Perks("Money")                     { }
    Money(const Money& s): Perks(s)             { }
    Money(Money&& s): Perks(s)                  { }
    Money(const QJsonObject& json)
        : Perks(json) {
        v._amount = json["amount"].toInt(0);
    }
    ~Money() override { }

    Money& operator=(const Money&) = delete;
    Money& operator=(Money&&) = delete;

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { amount = createComboBox(parent, layout, "How much money do you make a year?", { "Well Off ($100,000 or less)",
                                                                                                                                                  "Well Off ($200,000 or less)",
                                                                                                                                                  "Well Off ($300,000 or less)",
                                                                                                                                                  "Well Off ($400,000 or less)",
                                                                                                                                                  "Well Off ($500,000 or less)",
                                                                                                                                                  "Wealthy ($1,000,000 or less)",
                                                                                                                                                  "Wealthy ($2,000,000 or less)",
                                                                                                                                                  "Wealthy ($3,000,000 or less)",
                                                                                                                                                  "Wealthy ($4,000,000 or less)",
                                                                                                                                                  "Wealthy ($5,000,000 or less)",
                                                                                                                                                  "Filthy Rich (Unlimited)"
                                                                                                                                                });
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override                { if (!noStore) store();
                                                                  return ((v._amount < 0) ? 0_cp : ((v._amount < 10) ? (v._amount + 1) * 1_cp : 15_cp)); } // NOLINT
    void    restore() override                                  { vars s = v; amount->setCurrentIndex(s._amount); v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._amount = amount->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Perks::toJson();
                                                                  obj["amount"] = v._amount;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int _amount = 0;
    } v;

    QComboBox* amount = nullptr;

    QString optOut() {
        if (v._amount < 0) return "<incomplete>";
        QStringList amount { "Well Off ($100,000 or less)",
                             "Well Off ($200,000 or less)",
                             "Well Off ($300,000 or less)",
                             "Well Off ($400,000 or less)",
                             "Well Off ($500,000 or less)",
                             "Wealthy ($1,000,000 or less)",
                             "Wealthy ($2,000,000 or less)",
                             "Wealthy ($3,000,000 or less)",
                             "Wealthy ($4,000,000 or less)",
                             "Wealthy ($6,000,000 or less)",
                             "Filthy Rich (Unlimited)"
                           };
        return amount[v._amount];
    }
};

class PositiveReputation: public Perks {
public:
    PositiveReputation(): Perks("Positive Reputation")        { }
    PositiveReputation(const PositiveReputation& s): Perks(s) { }
    PositiveReputation(PositiveReputation&& s): Perks(s)      { }
    PositiveReputation(const QJsonObject& json): Perks(json)  { v._lvl  = json["level"].toInt(1);
                                                                v._for  = json["for"].toString("");
                                                                v._knwn = json["known"].toInt(0);
    }
    ~PositiveReputation() override { }

    PositiveReputation& operator=(const PositiveReputation&) = delete;
    PositiveReputation& operator=(PositiveReputation&&) = delete;

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { forwhat = createLineEdit(parent, layout, "Reputation for what?");
                                                                  level   = createComboBox(parent, layout, "How widely known?", { "Small to medium size group",
                                                                                                                                  "Medium sized group",
                                                                                                                                  "Large group" });
                                                                  known   = createComboBox(parent, layout, "How well known?", { "8-", "11-", "14-"});
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override                { if (!noStore) store();
                                                                  return (v._lvl + 1_cp) + v._knwn - 1_cp; }
    void    restore() override                                  { vars s = v;
                                                                  level->setCurrentIndex(s._lvl);
                                                                  known->setCurrentIndex(s._knwn);
                                                                  forwhat->setText(s._for);
                                                                  v = s;
                                                                }
    QString roll() override                                     { QStringList known { "8-", "11-", "14-"};
                                                                  return known[v._knwn];
                                                                }
    void    store() override                                    { v._lvl  = level->currentIndex();
                                                                  v._for  = forwhat->text();
                                                                  v._knwn = known->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Perks::toJson();
                                                                  obj["level"] = v._lvl;
                                                                  obj["for"]   = v._for;
                                                                  obj["known"] = v._knwn;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     _lvl = -1;
        QString _for = "";
        int     _knwn = -1;
    } v;

    QComboBox* level = nullptr;
    QLineEdit* forwhat = nullptr;
    QComboBox* known = nullptr;

    QString optOut() {
        if (v._lvl < 0 || v._knwn < 0 || v._for.isEmpty()) return "<incomplete>";
        QString res = "Positive Reputation: " + v._for;
        QStringList level { "Small to medium size group",
                            "Medium sized group",
                            "Large group" };
        return res + " (Known to a " + level[v._lvl] + ")";
    }
};

class VehiclesAndBases: public Perks {
public:
    VehiclesAndBases(): Perks("Vehicles And Bases")        { }
    VehiclesAndBases(const VehiclesAndBases& s): Perks(s)  { }
    VehiclesAndBases(VehiclesAndBases&& s): Perks(s)       { }
    VehiclesAndBases(const QJsonObject& json): Perks(json) { v._what = json["what"].toString("");
                                                             v._pnts = json["pnts"].toInt(1);
                                                             v._mult = json["mult"].toInt(0);
    }
    ~VehiclesAndBases() override { }

    VehiclesAndBases& operator=(const VehiclesAndBases&) = delete;
    VehiclesAndBases& operator=(VehiclesAndBases&&) = delete;

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { what  = createLineEdit(parent, layout, "What is the thing?");
                                                                  pnts = createLineEdit(parent, layout, "How many points in the thing?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  mult = createLineEdit(parent, layout, "How many multiples of things?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  return true;
                                                                }
    Points  points(bool noStore = false) override               { if (!noStore) store();
                                                                  return (v._pnts + 3) / 5 + v._mult * 5_cp; } // NOLINT
    void    restore() override                                  { vars s = v;
                                                                  what->setText(s._what);
                                                                  pnts->setText(QString("%1").arg(s._pnts));
                                                                  mult->setText(QString("%1").arg(s._mult));
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v._what = what->text();
                                                                  v._pnts = pnts->text().toInt();
                                                                  v._mult = mult->text().toInt();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Perks::toJson();
                                                                  obj["what"] = v._what;
                                                                  obj["pnts"] = v._pnts;
                                                                  obj["mult"] = v._mult;
                                                                  return obj;
                                                                }

private:
    struct vars {
        QString _what = "";
        int     _pnts = 0;
        int     _mult = 0;
    } v;

    QLineEdit* what = nullptr;
    QLineEdit* pnts = nullptr;
    QLineEdit* mult = nullptr;

    QString optOut() {
        if (v._what.isEmpty() || v._pnts == 0) return "<incomplete>";
        return QString("%1Vehicles and Bases: ").arg(v._mult != 0 ? QString("x%1 ").arg(pow(2.0, v._mult)) : "") + v._what + QString(" (%1 points)").arg(v._pnts); // NOLINT
    }

    void numeric(QString) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }
};

#undef CLASS
#undef CLASS_SPACE

#endif // PERK_H
