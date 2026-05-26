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

    QString description(bool showRoll = false) override { return v.mName + (showRoll ? "" : ""); }
    bool    form(QWidget*, QVBoxLayout*) override       { return false; }
    QString name() override                             { return v.mName; }
    Points points(bool noStore = false) override        { if (!noStore) store(); return 0_cp; }
    void    restore() override                          { }
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
    Access(const QJsonObject& json): Perks(json)  { v.mCost = json["cost"].toInt(1);
                                                    v.mFor  = json["for"].toString("");
                                                    v.mHide = json["hide"].toInt(0);
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
                                                                  return (v.mCost + 1) * 1_cp + v.mHide; }
    void    restore() override                                  { vars s = v;
                                                                  cost->setCurrentIndex(s.mCost);
                                                                  hide->setText(QString("%1").arg(s.mHide));
                                                                  forwhat->setText(s.mFor);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mCost = cost->currentIndex();
                                                                  v.mFor  = forwhat->text();
                                                                  v.mHide = hide->text().toInt(0);
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Perks::toJson();
                                                                  obj["cost"] = v.mCost;
                                                                  obj["for"]  = v.mFor;
                                                                  obj["hide"] = v.mHide;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     mCost = -1;
        QString mFor = "";
        int     mHide = 0;
    } v;

    QComboBox* cost = nullptr;
    QLineEdit* forwhat = nullptr;
    QLineEdit* hide = nullptr;

    QString optOut() {
        if (v.mCost < 0 || v.mFor.isEmpty()) return "<incomplete>";
        QString res = "Access: " + v.mFor;
        if (v.mHide > 0) res += QString(" (-%1 to discover)").arg(v.mHide);
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
        v.mExtra = json["extra"].toInt(1);
    }
    ~Anonymity() override { }

    Anonymity& operator=(const Anonymity&) = delete;
    Anonymity& operator=(Anonymity&&) = delete;

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    bool    form(QWidget* parent, QVBoxLayout* layout) override { extra = createLineEdit(parent, layout, "Extra cost", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  return true;
                                                                }
    Points points(bool noStore = false) override                { if (!noStore) store();
                                                                  return 3_cp + v.mExtra; }
    void    restore() override                                  { vars s = v;
                                                                  extra->setText(QString("%1").arg(s.mExtra));
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mExtra = extra->text().toInt(0);
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Perks::toJson();
                                                                  obj["extra"] = v.mExtra;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int mExtra = 0;
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
    ComputerLink(const QJsonObject& json): Perks(json)  { v.mValue = json["value"].toInt(1);
        v.mFor = json["for"].toString("");
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
                                                                  return v.mValue + 1_cp; }
    void    restore() override                                  { vars s = v;
                                                                  value->setText(QString("%1").arg(s.mValue));
                                                                  forwhat->setText(s.mFor);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mFor   = forwhat->text();
                                                                  v.mValue = value->text().toInt(0);
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Perks::toJson();
                                                                  obj["for"]   = v.mFor;
                                                                  obj["value"] = v.mValue;
                                                                  return obj;
                                                                }

private:
    struct vars {
        QString mFor = "";
        int     mValue = 0;
    } v;

    QLineEdit* forwhat = nullptr;
    QLineEdit* value = nullptr;

    QString optOut() {
        if (v.mFor.isEmpty()) return "<incomplete>";
        QString res = "Computer Link: " + v.mFor;
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
    Contact(const QJsonObject& json): Perks(json) { v.mBase     = json["base"].toInt(0);
                                                    v.mPlus     = json["plus"].toInt(0);
                                                    v.mLimited  = json["limited"].toBool(false);
                                                    v.mWho      = json["who"].toString("");
                                                    v.mUseful   = json["useful"].toInt(0);
                                                    v.mAccess   = json["access"].toBool(false);
                                                    v.mContacts = json["contacts"].toBool(false);
                                                    v.mRelate   = json["relate"].toInt(0);
                                                    v.mOrg = json["org"].toBool(false);
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
                                                                  auto pts = (v.mBase + 1_cp + v.mPlus - (v.mLimited ? 1_cp : 0_cp) + (v.mUseful > 0 ? 1_cp * v.mUseful: 0_cp) + (v.mAccess ? 1_cp : 0_cp) +
                                                                          (v.mContacts ? 1_cp : 0_cp) + ((v.mRelate == 0) ? -2 : v.mRelate - 1)) * (v.mOrg ? 3_cp : 1_cp);
#ifndef ISHSC
                                                                  if (Sheet::ref().character().hasWellConnected()) pts -= 1_cp;
#endif
                                                                  if (pts < 1_cp) pts = 1_cp;
                                                                  return pts;
                                                                }
    void    restore() override                                  { vars s = v;
                                                                  base->setCurrentIndex(s.mBase);
                                                                  plus->setText(QString("%1").arg(s.mPlus));
                                                                  limited->setChecked(s.mLimited);
                                                                  who->setText(s.mWho);
                                                                  useful->setCurrentIndex(s.mUseful);
                                                                  access->setChecked(s.mAccess);
                                                                  contacts->setChecked(s.mContacts);
                                                                  relate->setCurrentIndex(s.mRelate);
                                                                  org->setChecked(s.mOrg);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return QString("%1-").arg(8 + v.mBase * 3 + v.mPlus); } // NOLINT
    void    store() override                                    { v.mBase     = base->currentIndex();
                                                                  v.mPlus     = plus->text().toInt(0);
                                                                  v.mLimited  = limited->isChecked();
                                                                  v.mWho      = who->text();
                                                                  v.mUseful   = useful->currentIndex();
                                                                  v.mAccess   = access->isChecked();
                                                                  v.mContacts = contacts->isChecked();
                                                                  v.mRelate   = relate->currentIndex();
                                                                  v.mOrg      = org->isChecked();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Perks::toJson();
                                                                  obj["base"]     = v.mBase;
                                                                  obj["plus"]     = v.mPlus;
                                                                  obj["limited"]  = v.mLimited;
                                                                  obj["who"]      = v.mWho;
                                                                  obj["useful"]   = v.mUseful;
                                                                  obj["access"]   = v.mAccess;
                                                                  obj["contacts"] = v.mContacts;
                                                                  obj["relate"]   = v.mRelate;
                                                                  obj["org"]      = v.mOrg;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     mBase = -1;
        int     mPlus = 0;
        bool    mLimited = false;
        QString mWho = "";
        int     mUseful = -1;
        bool    mAccess = false;
        bool    mContacts = false;
        int     mRelate = -1;
        bool    mOrg = false;
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
        if (v.mBase < 0 || v.mUseful < 0 || v.mRelate < 0 || v.mWho.isEmpty()) return "<incomplete>";
        QString res;
        if (v.mPlus) res += QString("+%1 ").arg(v.mPlus);
        res += "Contact: " + v.mWho;
        QString sep = " (";
        if (v.mLimited) { res += sep + "Limited"; sep = "; "; }
        QStringList useful { "Useful", "Very Useful", "More Userful", "Extremely Useful" };
        res += sep + useful[v.mUseful];
        sep = "; ";
        if (v.mAccess) res += sep + "Access";
        if (v.mContacts) res += sep + "Contacts";
        QStringList relate { "Unfriendly", "Neutral", "Good", "Very Good", "Slavishly Loyal" };
        res += sep + relate[v.mRelate];
        if (v.mOrg) res += sep + "Organization";
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
        v.mAs = json["as"].toString("");
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
    void    restore() override                                  { vars s = v; as->setText(v.mAs); v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mAs = as->text();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Perks::toJson();
                                                                  obj["as"] = v.mAs;
                                                                  return obj;
                                                                }

private:
    struct vars {
        QString mAs = "";
    } v;

    QLineEdit* as = nullptr;

    QString optOut() {
        if (v.mAs.isEmpty()) return "<incomplete>";
        return "Deep Cover: " + v.mAs;
    }
};

class Favor: public Perks {
public:
    Favor(): Perks("Favor")                     { }
    Favor(const Favor& s): Perks(s)             { }
    Favor(Favor&& s): Perks(s)                  { }
    Favor(const QJsonObject& json): Perks(json) { v.mWho  = json["who"].toString("");
        v.mCost = json["cost"].toInt(1);
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
                                                                  auto pts = (v.mCost > 0) ? Points(v.mCost) : 1_cp;
#ifndef ISHSC
                                                                  if (Sheet::ref().character().hasWellConnected()) pts -= 1_cp;
#endif
                                                                  if (pts < 1_cp) pts = 1_cp;
                                                                  return pts; }
    void    restore() override                                  { vars s = v;
                                                                  who->setText(v.mWho);
                                                                  cost->setText(QString("%1").arg(v.mCost));
                                                                  v = s;
                                                                }
    QString roll() override                                     { return "14-"; }
    void    store() override                                    { v.mWho  = who->text();
                                                                  v.mCost = cost->text().toInt();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Perks::toJson();
                                                                  obj["who"]  = v.mWho;
                                                                  obj["cost"] = v.mCost;
                                                                  return obj;
                                                                }

private:
    struct vars {
        QString mWho = "";
        int     mCost = 1;
    } v;

    QLineEdit* who = nullptr;
    QLineEdit* cost = nullptr;

    QString optOut() {
        if (v.mWho.isEmpty()) return "<incomplete>";
        return "Favor: " + v.mWho + QString(" (%1 pt)").arg(v.mCost);
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
    Follower(const QJsonObject& json): Perks(json) { v.mWho  = json["who"].toString("");
                                                     v.mPnts = json["pnts"].toInt(1);
                                                     v.mMult = json["mult"].toInt(0);
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
                                                                  return (v.mPnts + 3) / 5 + v.mMult * 5_cp; } // NOLINT
    void    restore() override                                  { vars s= v;
                                                                  who->setText(s.mWho);
                                                                  pnts->setText(QString("%1").arg(s.mPnts));
                                                                  mult->setText(QString("%1").arg(s.mMult));
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mWho  = who->text();
                                                                  v.mPnts = pnts->text().toInt();
                                                                  v.mMult = mult->text().toInt();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Perks::toJson();
                                                                  obj["who"]  = v.mWho;
                                                                  obj["pnts"] = v.mPnts;
                                                                  obj["mult"] = v.mMult;
                                                                  return obj;
                                                                }

private:
    struct vars {
    QString mWho = "";
    int     mPnts = 0;
    int     mMult = 0;
    } v;

    QLineEdit* who = nullptr;
    QLineEdit* pnts = nullptr;
    QLineEdit* mult = nullptr;

    QString optOut() {
        if (v.mWho.isEmpty() || v.mPnts == 0) return "<incomplete>";
        // NOLINTNEXTLINE
        return QString("%1Follower%2: ").arg(v.mMult != 0 ? QString("x%1 ").arg(pow(2.0, v.mMult)) : "", v.mMult != 0 ? "s" : "") + v.mWho + QString(" (%1 points)").arg(v.mPnts);
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
        v.mCost = json["cost"].toInt(1);
        v.mFor = json["for"].toString("");
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
                                                                  return Points(v.mCost); }
    void    restore() override                                  { vars s = v;
                                                                  cost->setText(QString("%1").arg(s.mCost));
                                                                  forwhat->setText(s.mFor);
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mFor  = forwhat->text();
                                                                  v.mCost = cost->text().toInt(0);
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Perks::toJson();
                                                                  obj["for"]  = v.mFor;
                                                                  obj["cost"] = v.mCost;
                                                                  return obj;
                                                                }

private:
    struct vars {
        QString mFor  = "";
        int     mCost = 0;
    } v;

    QLineEdit* forwhat = nullptr;
    QLineEdit* cost = nullptr;

    QString optOut() {
        if (v.mFor.isEmpty()) return "<incomplete>";
        QString res = "Fringe Benefit: " + v.mFor;
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
        v.mAmount = json["amount"].toInt(0);
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
                                                                  return ((v.mAmount < 0) ? 0_cp : ((v.mAmount < 10) ? (v.mAmount + 1) * 1_cp : 15_cp)); } // NOLINT
    void    restore() override                                  { vars s = v; amount->setCurrentIndex(s.mAmount); v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mAmount = amount->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Perks::toJson();
                                                                  obj["amount"] = v.mAmount;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int mAmount = 0;
    } v;

    QComboBox* amount = nullptr;

    QString optOut() {
        if (v.mAmount < 0) return "<incomplete>";
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
        return amount[v.mAmount];
    }
};

class PositiveReputation: public Perks {
public:
    PositiveReputation(): Perks("Positive Reputation")        { }
    PositiveReputation(const PositiveReputation& s): Perks(s) { }
    PositiveReputation(PositiveReputation&& s): Perks(s)      { }
    PositiveReputation(const QJsonObject& json): Perks(json)  { v.mLvl  = json["level"].toInt(1);
                                                                v.mFor  = json["for"].toString("");
                                                                v.mKnown = json["known"].toInt(0);
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
                                                                  return (v.mLvl + 1_cp) + v.mKnown - 1_cp; }
    void    restore() override                                  { vars s = v;
                                                                  level->setCurrentIndex(s.mLvl);
                                                                  known->setCurrentIndex(s.mKnown);
                                                                  forwhat->setText(s.mFor);
                                                                  v = s;
                                                                }
    QString roll() override                                     { QStringList known { "8-", "11-", "14-"};
                                                                  return known[v.mKnown];
                                                                }
    void    store() override                                    { v.mLvl  = level->currentIndex();
                                                                  v.mFor  = forwhat->text();
                                                                  v.mKnown = known->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Perks::toJson();
                                                                  obj["level"] = v.mLvl;
                                                                  obj["for"]   = v.mFor;
                                                                  obj["known"] = v.mKnown;
                                                                  return obj;
                                                                }

private:
    struct vars {
        int     mLvl = -1;
        QString mFor = "";
        int     mKnown = -1;
    } v;

    QComboBox* level = nullptr;
    QLineEdit* forwhat = nullptr;
    QComboBox* known = nullptr;

    QString optOut() {
        if (v.mLvl < 0 || v.mKnown < 0 || v.mFor.isEmpty()) return "<incomplete>";
        QString res = "Positive Reputation: " + v.mFor;
        QStringList level { "Small to medium size group",
                            "Medium sized group",
                            "Large group" };
        return res + " (Known to a " + level[v.mLvl] + ")";
    }
};

class VehiclesAndBases: public Perks {
public:
    VehiclesAndBases(): Perks("Vehicles And Bases")        { }
    VehiclesAndBases(const VehiclesAndBases& s): Perks(s)  { }
    VehiclesAndBases(VehiclesAndBases&& s): Perks(s)       { }
    VehiclesAndBases(const QJsonObject& json): Perks(json) { v.mWhat = json["what"].toString("");
                                                             v.mPnts = json["pnts"].toInt(1);
                                                             v.mMult = json["mult"].toInt(0);
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
                                                                  return (v.mPnts + 3) / 5 + v.mMult * 5_cp; } // NOLINT
    void    restore() override                                  { vars s = v;
                                                                  what->setText(s.mWhat);
                                                                  pnts->setText(QString("%1").arg(s.mPnts));
                                                                  mult->setText(QString("%1").arg(s.mMult));
                                                                  v = s;
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { v.mWhat = what->text();
                                                                  v.mPnts = pnts->text().toInt();
                                                                  v.mMult = mult->text().toInt();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = Perks::toJson();
                                                                  obj["what"] = v.mWhat;
                                                                  obj["pnts"] = v.mPnts;
                                                                  obj["mult"] = v.mMult;
                                                                  return obj;
                                                                }

private:
    struct vars {
        QString mWhat = "";
        int     mPnts = 0;
        int     mMult = 0;
    } v;

    QLineEdit* what = nullptr;
    QLineEdit* pnts = nullptr;
    QLineEdit* mult = nullptr;

    QString optOut() {
        if (v.mWhat.isEmpty() || v.mPnts == 0) return "<incomplete>";
        return QString("%1Vehicles and Bases: ").arg(v.mMult != 0 ? QString("x%1 ").arg(pow(2.0, v.mMult)) : "") + v.mWhat + QString(" (%1 points)").arg(v.mPnts); // NOLINT
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
