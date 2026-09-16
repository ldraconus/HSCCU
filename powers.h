#ifndef POWERS_H
#define POWERS_H

#include "fraction.h"
#include "modifier.h"
#include "skilltalentorperk.h"

#include <map>

#include <QCheckBox>
#include <QComboBox>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QObject>
#include <QPushButton>
#include <QTableWidget>
#include <QTreeWidget>
#include <QVBoxLayout>

class Power {
protected:
    typedef std::function<void (Power*, int)>            lineEditCallback;
    typedef std::function<void (Power*, bool)>           checkBoxCallback;
    typedef std::function<void (Power*, int)>            comboBoxCallback;
    typedef std::function<void (Power*, int, int)>       activatedCallback;
    typedef std::function<void (Power*)>                 pushButtonCallback;
    typedef std::function<void (Power*, int, int, bool)> treeCallback;

    QCheckBox*   createCheckBox(QWidget*, QVBoxLayout*, QString, checkBoxCallback, int before = -1);
    QCheckBox*   createCheckBox(QWidget*, QVBoxLayout*, QString, int before = -1);
    QComboBox*   createComboBox(QWidget*, QVBoxLayout*, QString, QList<QString>, comboBoxCallback, int before = -1);
    QComboBox*   createComboBox(QWidget*, QVBoxLayout*, QString, QList<QString>, activatedCallback, int before = -1);
    QComboBox*   createComboBox(QWidget*, QVBoxLayout*, QString, QList<QString>, int before = -1);
    QLabel*      createLabel(QWidget*, QVBoxLayout*, QString, int before = -1);
    QLineEdit*   createLineEdit(QWidget*, QVBoxLayout*, QString, lineEditCallback callback, int before = -1);
    QLineEdit*   createLineEdit(QWidget*, QVBoxLayout*, QString, int before = -1);
    QPushButton* createPushButton(QWidget* parent, QVBoxLayout* layout, QString prompt, pushButtonCallback callback, int before = -1);
    QPushButton* createPushButton(QWidget* parent, QVBoxLayout* layout, QString prompt, int before = -1);
    QTreeWidget* createTreeWidget(QWidget*, QVBoxLayout*, QMap<QString, QStringList>, treeCallback callabck, int before = -1);
    QTreeWidget* createTreeWidget(QWidget*, QVBoxLayout*, QMap<QString, QStringList>, int before = -1);
    QWidget*     createLabeledEdit(QWidget*, QVBoxLayout*, QString, lineEditCallback callback, int before = -1);
    QWidget*     createLabeledEdit(QWidget*, QVBoxLayout*, QString, int before = -1);

    static std::map<QWidget*, QLineEdit*> mLabeledEdits;

    QTableWidget* createAdvantages(QWidget* parent, QVBoxLayout* layout);
    QTableWidget* createLimitations(QWidget* parent, QVBoxLayout* layout);

    QList<shared_ptr<Modifier>>::iterator findModifier(QString);

    std::map<QCheckBox*,   checkBoxCallback>   mCallbacksCB;
    std::map<QComboBox*,   comboBoxCallback>   mCallbacksCBox;
    std::map<QComboBox*,   activatedCallback>  mCallbacksActivatedCBox;
    std::map<QLineEdit*,   lineEditCallback>   mCallbacksEdit;
    std::map<QTreeWidget*, treeCallback>       mCallbacksTree;
    std::map<QPushButton*, pushButtonCallback> mCallbacksBtn;

    void empty(bool) { }

    QString add(QString n, int p) {
        QList<QString> vals = n.split("/");
        QString res;
        bool first = true;
        for (int i = 0; i < vals.count(); ++i) {
            auto& val = vals[i];
            if (first) first = false;
            else res += "/";
            QList<QString> num = val.split("-");
            res += QString("%1-").arg(num[0].toInt(0) + p);
        }
        return res;
    }

    QList<shared_ptr<Modifier>> mAdvantagesList;
    QList<shared_ptr<Modifier>> mLimitationsList;

    bool     mInMultipower = false;
    QWidget* mSender;

    Power* mParent = nullptr;
    int    mRow    = -1;

    static constexpr auto     Accurate = "accurate";
    static constexpr auto          Adj = "adj";
    static constexpr auto       Affect = "affect";
    static constexpr auto      Against = "against";
    static constexpr auto          All = "all";
    static constexpr auto      Altered = "altered";
    static constexpr auto       Amount = "amount";
    static constexpr auto       Anchor = "anchor";
    static constexpr auto          Any = "any";
    static constexpr auto         Anlz = "anlz";
    static constexpr auto        Armor = "armor";
    static constexpr auto      AsPower = "power";
    static constexpr auto      AsSkill = "skill";
    static constexpr auto       Attack = "attack";
    static constexpr auto      Average = "average";
    static constexpr auto        Bcast = "bcast";
    static constexpr auto        Black = "black";
    static constexpr auto         Body = "body";
    static constexpr auto         Bond = "bond";
    static constexpr auto        Bonus = "bonus";
    static constexpr auto        Boost = "boost";
    static constexpr auto       Bright = "bright";
    static constexpr auto         Cant = "cant";
    static constexpr auto        Cause = "cause";
    static constexpr auto     Cellular = "cellular";
    static constexpr auto         Cham = "cham";
    static constexpr auto       Change = "change";
    static constexpr auto         Circ = "circ";
    static constexpr auto        Clair = "clair";
    static constexpr auto        Class = "class";
    static constexpr auto          Cmd = "cmd";
    static constexpr auto         Conc = "conc";
    static constexpr auto       Config = "config";
    static constexpr auto      Control = "control";
    static constexpr auto      Correct = "correct";
    static constexpr auto         Crit = "crit";
    static constexpr auto       Damage = "damage";
    static constexpr auto         Decr = "decr";
    static constexpr auto          Def = "def";
    static constexpr auto    Defensive = "defensive";
    static constexpr auto       Degree = "degree";
    static constexpr auto       Depend = "depend";
    static constexpr auto      Desolid = "desolid";
    static constexpr auto       Detect = "detect";
    static constexpr auto         Dice = "dice";
    static constexpr auto         Diff = "diff";
    static constexpr auto          Dim = "dim";
    static constexpr auto       Direct = "direct";
    static constexpr auto        Discr = "discr";
    static constexpr auto      Distort = "distort";
    static constexpr auto          Dmg = "dmg";
    static constexpr auto       Dreams = "dreams";
    static constexpr auto         Easy = "easy";
    static constexpr auto      Effects = "effects";
    static constexpr auto       Effect = "effect";
    static constexpr auto          Emp = "emp";
    static constexpr auto          Emo = "emo";
    static constexpr auto          End = "end";
    static constexpr auto      Englobe = "englobe";
    static constexpr auto         Envs = "envs";
    static constexpr auto       Enhanc = "enhanc";
    static constexpr auto EquipmentTag = "equipment";
    static constexpr auto         Excl = "excl";
    static constexpr auto        Extra = "extra";
    static constexpr auto     Feedback = "feedback";
    static constexpr auto         Feed = "feed";
    static constexpr auto       FillIn = "fillin";
    static constexpr auto         Fine = "fine";
    static constexpr auto        Fixed = "fixed";
    static constexpr auto        Float = "float";
    static constexpr auto         Foci = "foci";
    static constexpr auto        Forms = "forms";
    static constexpr auto         Form = "form";
    static constexpr auto         From = "from";
    static constexpr auto        Given = "given";
    static constexpr auto        Glide = "glide";
    static constexpr auto      Grouped = "group";
    static constexpr auto       Groups = "groups";
    static constexpr auto         Harm = "harm";
    static constexpr auto         Heal = "heal";
    static constexpr auto      Hearing = "hearing";
    static constexpr auto       Height = "height";
    static constexpr auto        Hover = "hover";
    static constexpr auto          How = "how";
    static constexpr auto           Id = "id";
    static constexpr auto          Idx = "idx";
    static constexpr auto       Imperm = "imperm";
    static constexpr auto      Imitate = "imitate";
    static constexpr auto         Incr = "incr";
    static constexpr auto    IndNonTgt = "indNonTgt";
    static constexpr auto       IndTgt = "indTgt";
    static constexpr auto      Instant = "instant";
    static constexpr auto         Into = "into";
    static constexpr auto          Inv = "inv";
    static constexpr auto         Lang = "lang";
    static constexpr auto      Lasting = "lasting";
    static constexpr auto       Length = "length";
    static constexpr auto       Levels = "levels";
    static constexpr auto        Level = "level";
    static constexpr auto        Limbs = "limbs";
    static constexpr auto      Limited = "limited";
    static constexpr auto        Limit = "limit";
    static constexpr auto         Link = "link";
    static constexpr auto          Lim = "lim";
    static constexpr auto          Lit = "lit";
    static constexpr auto         Lock = "lock";
    static constexpr auto          Loc = "loc";
    static constexpr auto         Loss = "loss";
    static constexpr auto          Los = "los";
    static constexpr auto         Mass = "mass";
    static constexpr auto     Makeover = "makeover";
    static constexpr auto       Mental = "mental";
    static constexpr auto       Meters = "meters";
    static constexpr auto       Method = "method";
    static constexpr auto          Mic = "mic";
    static constexpr auto        Minds = "minds";
    static constexpr auto       Mobile = "mobile";
    static constexpr auto    Modifiers = "modifiers";
    static constexpr auto          Mod = "mod";
    static constexpr auto       Moment = "moment";
    static constexpr auto         Mult = "mult";
    static constexpr auto   Multipwers = "Multipowers";
    static constexpr auto         Name = "name";
    static constexpr auto        NoDef = "nodef";
    static constexpr auto        NoDir = "nodir";
    static constexpr auto       NonCom = "noncom";
    static constexpr auto        NoNon = "nonon";
    static constexpr auto      NoSkill = "noSkill";
    static constexpr auto        NoStr = "nostr";
    static constexpr auto       NonTgt = "nonTgt";
    static constexpr auto       NoPDED = "nopded";
    static constexpr auto          Not = "not";
    static constexpr auto           No = "no";
    static constexpr auto       NumDim = "numDim";
    static constexpr auto          One = "one";
    static constexpr auto         Only = "only";
    static constexpr auto         PDED = "pded";
    static constexpr auto      Partial = "partial";
    static constexpr auto        Parts = "parts";
    static constexpr auto         Part = "part";
    static constexpr auto         Pass = "pass";
    static constexpr auto          Pen = "pen";
    static constexpr auto         Perc = "perc";
    static constexpr auto          Per = "per";
    static constexpr auto     PointsOf = "points";
    static constexpr auto       PoolOf = "pool";
    static constexpr auto         PorR = "porr";
    static constexpr auto    PowerName = "powerName";
    static constexpr auto       Powers = "powers";
    static constexpr auto          Pre = "pre";
    static constexpr auto      Protect = "protect";
    static constexpr auto          Pts = "pts";
    static constexpr auto          Put = "put";
    static constexpr auto        Radio = "radio";
    static constexpr auto          Rad = "rad";
    static constexpr auto       Ranged = "ranged";
    static constexpr auto        Range = "range";
    static constexpr auto        Rapid = "rapid";
    static constexpr auto         Rate = "rate";
    static constexpr auto        Recom = "recom";
    static constexpr auto         Recv = "recv";
    static constexpr auto          Rec = "rec";
    static constexpr auto        Restr = "restr";
    static constexpr auto       Resist = "resist";
    static constexpr auto       Result = "result";
    static constexpr auto        Retro = "retro";
    static constexpr auto       Revert = "revert";
    static constexpr auto         Safe = "save";
    static constexpr auto         Self = "self";
    static constexpr auto       Senses = "senses";
    static constexpr auto        Sense = "sense";
    static constexpr auto          Set = "set";
    static constexpr auto        Sight = "sight";
    static constexpr auto         Size = "size";
    static constexpr auto         Slow = "slow";
    static constexpr auto        Smell = "smell";
    static constexpr auto        Space = "space";
    static constexpr auto         Span = "span";
    static constexpr auto      Spatial = "spatial";
    static constexpr auto        Speed = "speed";
    static constexpr auto        Solid = "solid";
    static constexpr auto        Spatl = "spatl";
    static constexpr auto          Str = "str";
    static constexpr auto         Stun = "stun";
    static constexpr auto     Suppress = "suppress";
    static constexpr auto      Surface = "surface";
    static constexpr auto         Susc = "susc";
    static constexpr auto       SuscTo = "suscTo";
    static constexpr auto       Target = "target";
    static constexpr auto         Tele = "tele";
    static constexpr auto      Terrain = "terrain";
    static constexpr auto          Tgt = "tgt";
    static constexpr auto        TheDC = "TheDC";
    static constexpr auto        Thick = "thick";
    static constexpr auto        Thing = "thing";
    static constexpr auto         Time = "time";
    static constexpr auto       TimLoc = "timLoc";
    static constexpr auto       TimWhr = "timLWhr";
    static constexpr auto           To = "to";
    static constexpr auto         ToPD = "pd";
    static constexpr auto         ToED = "ed";
    static constexpr auto        Touch = "touch";
    static constexpr auto        Track = "track";
    static constexpr auto        Trans = "trans";
    static constexpr auto        Vague = "vague";
    static constexpr auto          Val = "val";
    static constexpr auto        VarPP = "Variable Power Pool";
    static constexpr auto       Varies = "varies";
    static constexpr auto      Varying = "varying";
    static constexpr auto        Veloc = "veloc";
    static constexpr auto         Vuln = "vuln";
    static constexpr auto       VulnTo = "vulnTo";
    static constexpr auto       Weapon = "weapon";
    static constexpr auto         What = "what";
    static constexpr auto        When2 = "when2";
    static constexpr auto         When = "when";
    static constexpr auto        Where = "where";
    static constexpr auto        Which = "which";
    static constexpr auto          Who = "who";
    static constexpr auto        Whole = "whole";

public:
    QLineEdit* labeledEdit(QWidget* w)       { return mLabeledEdits[w]; }
    bool       labeledEditExists(QWidget* w) { return mLabeledEdits.find(w) != mLabeledEdits.end(); }

    bool hasModifier(QString);

    class allBase {
    public:
        allBase() { }
        allBase(allBase*) { }

        virtual shared_ptr<Power> create() const                  = 0;
        virtual shared_ptr<Power> create(QJsonObject& json) const = 0;
    };

    template <typename T>
    class allPower: public allBase {
    public:
        allPower(): allBase()                   { }
        allPower(allPower* b): allBase(b)       { }

        shared_ptr<Power> create() const override                  { return make_shared<T>(); }
        shared_ptr<Power> create(QJsonObject& json) const override { return make_shared<T>(json); }
    };

    Power()                  { id({ }); }
    Power(QJsonObject& json) { id(json); }

    static const bool NoStore = true;
    static const bool ShowEND = true;
    static const bool NoUpdate = false;
    static const bool DoUpdate = true;

    virtual QString     abbreviation(bool roll = false) { return description(roll); }
    virtual QString     description(bool roll = false)  { return roll ? "" : ""; };
    virtual void        form(QWidget*, QVBoxLayout*)    { }
    virtual QString     name()                          { return ""; }
    virtual Points      points(bool noStore = false)    { return noStore ? 0_cp : 0_cp; }
    virtual QString     nickname()                      { return ""; }
    virtual void        restore()                       { }
    virtual void        store()                         { }
    virtual bool        varying()                       { return false; }

    virtual QJsonObject toJson()                 { QJsonObject obj;
                                                   QJsonObject mods;
                                                   obj[Id] = mGuid;
                                                   for (const auto& mod: std::as_const(mAdvantagesList))  mods[mod->name()] = mod->toJson();
                                                   for (const auto& mod: std::as_const(mLimitationsList)) mods[mod->name()] = mod->toJson();
                                                   obj[Modifiers] = mods;
                                                   return obj;
                                                 }
    virtual void        activate(int, int)       { }
    virtual Fraction    adv()                    { return Fraction(0); }
    virtual void        checked(bool)            { }
    virtual void        clicked(void)            { }
    virtual QString     end();
    virtual QString     noEnd();
    virtual void        index(int)               { }
    virtual Fraction    lim()                    { return Fraction(0); }
    virtual void        numeric(int)             { }
    virtual void        selected(int, int, bool) { }

    Fraction endLessActing();

    Power* parent()         { return mParent; }
    Power* parent(Power* p) { mParent = p; return mParent; }
    int    row()            { return mRow; }
    int    row(int r)       { mRow = r; return mRow; }

    struct sizeMods {
        int mSTR = 0;
        int mCON = 0;
        int mPRE = 0;
        int mPD = 0;
        int mED = 0;
        int mBODY = 0;
        int mSTUN = 0;
        int mReach = 0;
        int mRunning = 0;
        int mKBRes = 0;
        int mAOERad = 0;
        QString mComp {};
    };

    virtual int characteristic(int) { return 0; }

    virtual void                      append(shared_ptr<Power>)      { }
    virtual Points                    display(int&, QTableWidget*)   { return 0_cp; }
    virtual Points                    display(QString&)              { return 0_cp; }
    virtual int                       count()                        { return -1; }
    virtual QString                   id() const                     { return mGuid; }
    virtual void                      inMultipower()                 { mInMultipower = true; }
    virtual bool                      isEquipment()                  { return false; }
    virtual bool                      isFramework()                  { return false; }
    virtual bool                      isMultipower()                 { return false; }
    virtual bool                      isVPP()                        { return false; }
    virtual bool                      isValid(shared_ptr<Power>)     { return true; }
    virtual void                      insert(int, shared_ptr<Power>) { }
    virtual QList<shared_ptr<Power>>& list()                         { static QList<shared_ptr<Power>> l; return l; }
    virtual Points                    pool()                         { return 0_cp; }
    virtual void                      remove(int)                    { }
    virtual void                      remove(shared_ptr<Power>)      { }

    virtual int                           FD()          { return 0; }
    virtual int                           MD()          { return 0; }
    virtual int                           PowD()        { return 0; }
    virtual int                           rED()         { return 0; }
    virtual int                           rPD()         { return 0; }
    virtual int                           ED()          { return 0; }
    virtual int                           PD()          { return 0; }
    virtual shared_ptr<SkillTalentOrPerk> skill()       { return nullptr; }
    virtual int                           str()         { return 0; }
    virtual sizeMods&                     growthStats() { static sizeMods sm; return sm; }
    virtual int                           move()        { return 0; }
    virtual QString                       units()       { return "m"; }
    virtual int                           place()       { return 1; }

    int doubling();

    Points real(Fraction a = Fraction(0), Points mod = 0_cp, Fraction s = Fraction(0));
    Points acting(Fraction a = Fraction(0), Points mod = 0_cp);
    Points active();

    QList<shared_ptr<Modifier>>& modifiers()       { return mModifiers; }
    QList<shared_ptr<Modifier>>& advantagesList()  { return mAdvantagesList; }
    QList<shared_ptr<Modifier>>& limitationsList() { return mLimitationsList; }

    QWidget* sender() { return mSender; }

    void callback(QCheckBox*);
    void callback(QComboBox*);
    void callback(QComboBox*, bool);
    void callback(QLineEdit*);
    void callback(QTreeWidget* tree);
    void callback(QPushButton* btn);

    void createForm(QWidget*, QVBoxLayout*);

    static QList<QString>    Available();
    static void              ClearForm(QVBoxLayout*);
    static void              ClearLabeledEdits() { mLabeledEdits.clear(); }
    static bool              LoadEquipment();
    static QList<QString>    AdjustmentPowers();
    static QList<QString>    AttackPowers();
    static QList<QString>    AutomatonPowers();
    static QList<QString>    BodyAffectingPowers();
    static QList<QString>    DefensePowers();
    static QList<QString>    Equipment();
    static QList<QString>    FrameworkPowers();
    static QList<QString>    MentalPowers();
    static QList<QString>    MovementPowers();
    static QList<QString>    SenseAffectingPowers();
    static QList<QString>    SensoryPowers();
    static QList<QString>    SpecialPowers();
    static QList<QString>    StandardPowers();

    static shared_ptr<Power> ByName(QString);
    static shared_ptr<Power> FromJson(QString, QJsonObject&);

    bool isNumber(QString);

private:
    QList<shared_ptr<Modifier>> mModifiers;
    QString                     mGuid;

    void id(const QJsonObject& json) { mGuid = json[Id].toString(); if (mGuid.isEmpty()) mGuid = QUuid::createUuid().toString(QUuid::WithoutBraces); }

    static const QMap<QString, QString> mAdjustmentPower;
    static const QMap<QString, QString> mAttackPower;
    static const QMap<QString, QString> mAutomatonPower;
    static const QMap<QString, QString> mBodyAffectingPower;
    static const QMap<QString, QString> mDefensePower;
    static const QMap<QString, QString> mFrameworkPower;
    static const QMap<QString, QString> mMentalPower;
    static const QMap<QString, QString> mMovementPower;
    static const QMap<QString, QString> mSenseAffectingPower;
    static const QMap<QString, QString> mSensoryPower;
    static const QMap<QString, QString> mSpecialPower;
    static const QMap<QString, QString> mStandardPower;
    static const QMap<QString, QString> mEquipment;
};

class AllPowers: public Power {
protected:
    int countCommas(QString x) {
        if (x.isEmpty()) return 0;
        QStringList words = x.split(",");
        return int(words.count());
    }

    QJsonArray toArray(const QStringList& list) const {
        QJsonArray array;
        for (const auto& str: std::as_const(list)) array.append(str);
        return array;
    }

    QStringList toStringList(const QJsonArray& array) const {
        QStringList list;
        for (const auto& str: std::as_const(array)) list.append(str.toString());
        return list;
    }

    void setTreeWidget(QTreeWidget* tree, const QStringList& list) {
        int count = tree->topLevelItemCount();
        for (int i = 0; i < count; ++i) {
            auto item = tree->topLevelItem(i);
            item->setCheckState(0, Qt::Unchecked);
            int children = item->childCount();
            for (int j = 0; j < children; ++j) {
                auto child = item->child(j);
                child->setCheckState(0, Qt::Unchecked);
            }
        }
        for (const auto& str: list) {
            for (int i = 0; i < count; ++i) {
                auto item = tree->topLevelItem(i);
                if (item->text(0) == str) {
                    item->setCheckState(0, Qt::Checked);
                    int children = item->childCount();
                    for (int j = 0; j < children; ++j) {
                        auto child = item->child(j);
                        child->setCheckState(0, Qt::Checked);
                    }
                    break;
                } else {
                    int children = item->childCount();
                    int j = 0;
                    for (; j < children; ++j) {
                        auto child = item->child(j);
                        if (child->text(0) == str) {
                            child->setCheckState(0, Qt::Checked);
                            break;
                        }
                    }
                    if (j != children) break;
                }
            }
        }
    }

    QStringList treeWidget(QTreeWidget* tree) {
        QStringList list;
        int count = tree->topLevelItemCount();
        for (int i = 0; i < count; ++i) {
            auto item = tree->topLevelItem(i);
            if (item->checkState(0) == Qt::Checked) list.append(item->text(0));
            else {
                int children = item->childCount();
                for (int j = 0; j < children; ++j) {
                    auto child = item->child(j);
                    if (child->checkState(0) == Qt::Checked) list.append(child->text(0));
                }
            }
        }
        return list;
    }

public:
    AllPowers(): Power()         { }
    AllPowers(QString name)
        : Power()
        , v { name, "" }         { }
    AllPowers(QJsonObject& json)
        : Power(json)            { load(json); }

    void load(const QJsonObject& json, const QString& name = "") {
        if (name.isEmpty()) v.mName = json[Name].toString();
        else v.mName = name;
        v.mPowerName = json[PowerName].toString("");
        v.mVaries = json[Varies].toBool(false);
    }

    QString     abbreviation(bool roll = false) override { return description(roll); }
    QString     description(bool roll = false) override = 0;
    Points      points(bool noStore = false) override   = 0;

    Fraction adv() override         { return Fraction(0); }
    void     checked(bool) override { }
    Fraction lim() override         { return Fraction(0); }
    void     numeric(int) override  { }

    void form(QWidget* widget, QVBoxLayout* layout) override {
        if (isEquipment()) powerName = nullptr;
        else powerName = createLineEdit(widget, layout, "Nickname of power");
        if (mParent != nullptr && mParent->isMultipower()) varies = createCheckBox(widget, layout, "Varies");
        else if (mInMultipower) varies = createCheckBox(widget, layout, "Varies");
        else varies = nullptr;
    }
    QString     name() override                                     { return v.mName;
                                                                    }
    QString     nickname() override                                 { return v.mPowerName;
                                                                    }
    void        restore() override                                  { vars s = v;
                                                                      if (powerName) powerName->setText(s.mPowerName);
                                                                      if (varies != nullptr) varies->setChecked(s.mVaries);
                                                                      v = s;
                                                                    }
    void        store() override                                    { v.mPowerName = powerName ? powerName->text() : "";
                                                                      if (varies != nullptr) v.mVaries = varies->isChecked();
                                                                    }
    QJsonObject toJson() override                                   { QJsonObject obj  = Power::toJson();
                                                                      obj[Name]      = v.mName;
                                                                      obj[PowerName] = v.mPowerName;
                                                                      obj[Varies]    = v.mVaries;
                                                                      return obj;
                                                                    }
    bool        varying() override                                  { return v.mVaries; }

private:
    struct vars {
        QString mName      = "";
        QString mPowerName = "";
        bool    mVaries    = false;
    } v;

    QLineEdit* powerName = nullptr;
    QCheckBox* varies = nullptr;
};

#endif // POWERS_H

