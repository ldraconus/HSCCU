#ifndef MODIFIER_H
#define MODIFIER_H

#include "fraction.h"
#ifndef ISHSC
#include "modifiersdialog.h"
#else
#include "shared.h"
#endif

#include <QCheckBox>
#include <QComboBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidget>

#include <variant>
#include <functional>
#include <memory>

using std::function;
using std::variant;
using std::shared_ptr;

#ifdef ISHSC
class ModifiersDialog {
public:
    ModifiersDialog() {}
    static ModifiersDialog& ref() { static ModifiersDialog md; return md; }
    void updateForm() { }
    void update() { }
};
#endif

typedef variant<Fraction, int> value;

class ModifierBase {
public:
    enum ModifierType { isLimitation = 1, isAdvantage = 2, isBoth = 3 };
    enum AdderType { isModifier = false, isAnAdder = true };

    class base {
    public:
        base() { }
        base(const base&) { }
        base(base&&) { }
        base(base*) { }
        virtual ~base() { }

        base& operator=(const base&) { return *this; }
        base& operator=(base&&) { return *this; }

        virtual shared_ptr<class Modifier> create()                   { return nullptr; }
        virtual shared_ptr<class Modifier> create(const QJsonObject&) { return nullptr; }
    };

    template <typename T>
    class mod: public base {
    public:
        mod(): base() { }
        mod(const mod& b): base(b) { }
        mod(mod&& b): base(b) { }
        mod(mod* b): base(b) { }
        ~mod() override { }

        mod& operator=(const mod&) = delete;
        mod& operator=(mod&&) = delete;

        shared_ptr<class Modifier> create() override                        { return make_shared<T>(); }
        shared_ptr<class Modifier> create(const QJsonObject& json) override { return make_shared<T>(json); }
    };

    ModifierBase()
        : mName("")
        , mType(isLimitation)
        , mAdder(isModifier)
        , mBase(nullptr) { }
    ModifierBase(QString nm, ModifierType type, bool adder, base* base = nullptr)
        : mName(nm)
        , mType(type)
        , mAdder(adder)
        , mBase(base) { }
    ModifierBase(const ModifierBase& m)
        : mName(m.mName)
        , mType(m.mType)
        , mAdder(m.mAdder)
        , mBase(m.mBase) { }
    ModifierBase(const ModifierBase* m)
        : mName(m->mName)
        , mType(m->mType)
        , mAdder(m->mAdder)
        , mBase(m->mBase) { }
    ModifierBase(ModifierBase&& m)
        : mName(m.mName)
        , mType(m.mType)
        , mAdder(m.mAdder)
        , mBase(m.mBase) { }

    ModifierBase& operator=(const ModifierBase& m) = delete;
    ModifierBase& operator=(ModifierBase&&) = delete;

    virtual ~ModifierBase() { }

    QWidget* mSender = nullptr; // NOLINT

    bool             isAdder() const  { return mAdder; }
    base             modifier() const { return mBase; }
    ModifierType     type() const     { return mType; }
    QString          name() const     { return mName; }

    void callback(QCheckBox*);
    void callback(QLineEdit*);
    void callback(QTreeWidget*);
    void callback(QComboBox*);

    virtual shared_ptr<class Modifier> create();
    virtual shared_ptr<class Modifier> create(const QJsonObject&);

    virtual bool          createForm(QWidget*, QVBoxLayout*) { return false; }
    virtual QString       abbreviation(bool show = false)    { return description(show); }
    virtual QString       description(bool show = false)     { return show ? "<incomplete>" : "<incomplete>"; }

private:
    QString       mName = "";
    ModifierType  mType = isLimitation;
    bool          mAdder = false;
    base*         mBase = nullptr;

 protected:
    QMap<QCheckBox*,   std::_Mem_fn<void (ModifierBase::*)(bool)>>            mCallbacksCB; // NNOLINT
    QMap<QComboBox*,   std::_Mem_fn<void (ModifierBase::*)(int)>>             mCallbacksCBox; // NOLINT
    QMap<QLineEdit*,   std::_Mem_fn<void (ModifierBase::*)(QString)>>         mCallbacksEdit; // NOLINT
    QMap<QTreeWidget*, std::_Mem_fn<void (ModifierBase::*)(int, int, bool)>>  mCallbacksTree; // NOLINT
    void empty(bool) { }

public:
    virtual void numeric(QString)         { }
    virtual void changed(QString)         { }
    virtual void checked(bool)            { }
    virtual void selected(int, int, bool) { }
    virtual void index(int)               { }
};

class Modifier: public ModifierBase {
protected:
    QCheckBox*   createCheckBox(QWidget*, QVBoxLayout*, QString, std::_Mem_fn<void (ModifierBase::*)(bool)>);
    QCheckBox*   createCheckBox(QWidget*, QVBoxLayout*, QString);
    QComboBox*   createComboBox(QWidget*, QVBoxLayout*, QString, QList<QString>, std::_Mem_fn<void (ModifierBase::*)(int)>);
    QComboBox*   createComboBox(QWidget*, QVBoxLayout*, QString, QList<QString>);
    QLabel*      createLabel(QWidget*, QVBoxLayout*, QString);
    QLineEdit*   createLineEdit(QWidget*, QVBoxLayout*, QString, std::_Mem_fn<void (ModifierBase::*)(QString)>);
    QLineEdit*   createLineEdit(QWidget*, QVBoxLayout*, QString);
    QTreeWidget* createTreeWidget(QWidget*, QVBoxLayout*, QMap<QString, QStringList>, std::_Mem_fn<void (ModifierBase::*)(int, int, bool)>);
    QTreeWidget* createTreeWidget(QWidget*, QVBoxLayout*, QMap<QString, QStringList>);

    QJsonArray toArray(QStringList list) {
        QJsonArray array;
        for (const auto& str: std::as_const(list)) array.append(str);
        return array;
    }
    QStringList toStringList(QJsonArray array) {
        QStringList list;
        for (const auto& str: std::as_const(array)) list.append(str.toString());
        return list;
    }
    void setTreeWidget(QTreeWidget* tree, QStringList list) {
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
        for (const auto& str: std::as_const(list)) {
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
#ifndef ISHSC
    Ui::ModifiersDialog* ui = nullptr;
#endif

    Modifier(): ModifierBase() { }
    Modifier(QString nm, ModifierType type, bool adder, base* base = nullptr)
        : ModifierBase(nm, type, adder, base) { }
    Modifier(const Modifier* m): ModifierBase(m) { }
    Modifier(Modifier&& m): ModifierBase(m) { }
    Modifier(const Modifier& m): ModifierBase(m) { }
    ~Modifier() override { }

    Modifier& operator=(const Modifier&) = delete;
    Modifier& operator=(Modifier&&) = delete;

    QWidget* sender() const { return mSender; }

    bool createForm(QWidget*, QVBoxLayout*) override;

    void checked(bool) override            { }
    void selected(int, int, bool) override { }
    void changed(QString) override         { }
    void index(int) override               { }

    void numeric(QString) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
        if (txt.isEmpty() || isNumber(txt)) {
            store();
#ifndef ISHSC
            ModifiersDialog::ref().updateForm();
#endif
            return;
        }
        edit->undo();
    }

    static const bool NoStore = true;
    static const bool Show = true;
    static const bool Hide = false;

    virtual bool        form(QWidget*, QVBoxLayout*)   { return false; }
    virtual void        restore()                      { }
    virtual void        store()                        { }
    virtual QJsonObject toJson()                       { QJsonObject obj; return obj; }

    virtual int         doubling()                     { return 2; }

    QString abbreviation(bool show = false) override { return description(show); }
    QString description(bool show = false) override  { return ModifierBase::description(show); }
    virtual Fraction fraction(bool noStore = false)  { return noStore ? Fraction(0) : Fraction(0); }
    virtual Points   points(bool noStore = false)    { return noStore ? 0_cp : 0_cp; }
    virtual Fraction endChange()                     { return Fraction(1); }

    static void ClearForm(QVBoxLayout* layout);

    bool isNumber(QString);
};

class Modifiers {
private:
    static QMap<QString, shared_ptr<Modifier>> sModifiers; // NOLINT

public:
    Modifiers();
    Modifiers(const Modifiers&) { }
    Modifiers(Modifiers&&) { }
    ~Modifiers() { }

    Modifiers& operator=(const Modifiers&) = delete;
    Modifiers& operator=(Modifiers&&) = delete;

    QList<shared_ptr<Modifier>> operator()(Modifier::ModifierType) const;

    static shared_ptr<Modifier> ByName(QString);
};

class NoFormModifier: public Modifier {
public:
    NoFormModifier(QString n, ModifierType m, Fraction v)
        : Modifier(n, m, isModifier)
        , mValue(v)
        , mPoints(0_cp) { }
    NoFormModifier(QString n, ModifierType m, Points p)
        : Modifier(n, m, isAnAdder)
        , mValue(Fraction(0))
        , mPoints(p) { }
    NoFormModifier(const NoFormModifier& m)
        : Modifier(m)
        , mValue(m.mValue)
        , mPoints(m.mPoints) { }
    NoFormModifier(NoFormModifier&& m)
        : Modifier(m)
        , mValue(m.mValue)
        , mPoints(m.mPoints) { }
    NoFormModifier(QJsonObject json)
        : Modifier(json["name"].toString(""),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false))
        , mValue(Fraction(json["value"].toArray()[0].toInt(0), json["value"].toArray()[1].toInt(1)))
        , mPoints(json["points"].toInt()) { }
    ~NoFormModifier() override { }

    NoFormModifier& operator=(const NoFormModifier& n) = delete;
    NoFormModifier& operator=(NoFormModifier&& n) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<NoFormModifier>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<NoFormModifier>(json); }

    QString       abbreviation(bool show = false) override { return description(show); }
    QString       description(bool show = false) override  { return QString(show ? fraction(Modifier::NoStore).toString() + " ": "") + name(); }
    Fraction      fraction(bool noStore = false) override  { return noStore ? mValue : mValue; }
    Points        points(bool noStore = false) override    { return noStore ? mPoints : mPoints; }
    void          restore() override                       { }
    void          store() override                         { }
    QJsonObject   toJson() override                        { QJsonObject obj;
                                                             obj["name"]  = name();
                                                             obj["type"]  = type();
                                                             obj["adder"] = isAdder();
                                                             QJsonArray arr;
                                                             arr.append((int) mValue.numerator());
                                                             arr.append((int) mValue.denominator());
                                                             obj["value"] = arr;
                                                             obj["points"] = (int) mPoints.points;
                                                             return obj;
                                                           }
private:
    Fraction mValue;
    Points mPoints;
};

class Ablative: public Modifier {
public:
    Ablative()
        : Modifier("Ablative", isLimitation, isModifier)
        , v({ false }) {
    }
    Ablative(const Ablative& m)
        : Modifier(m)
        , v(m.v) {
    }
    Ablative(Ablative&& m)
        : Modifier(m)
        , v(m.v) {
    }
    Ablative(QJsonObject json)
        : Modifier(json["name"].toString("Ablative"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) {
        v._aquiresRoll = json["aquiresRoll"].toBool(false);
    }
    ~Ablative() override { }

    Ablative& operator=(const Ablative&) = delete;
    Ablative& operator=(Ablative&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<Ablative>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<Ablative>(json); }

    void        changed(QString) override                 { store();
#ifndef ISHSC
                                                            ModifiersDialog::ref().updateForm();
#endif
                                                          }
    void        checked(bool) override                    { store();
#ifndef ISHSC
                                                            ModifiersDialog::ref().updateForm();
#endif
                                                          }
    QString     abbreviation(bool show = false) override  { return optOut(show, true); }
    QString     description(bool show = false) override   { return optOut(show); }
    bool        form(QWidget* p, QVBoxLayout* l) override { aquiresRoll = createCheckBox(p, l, "Aquires a Skill Roll when Exceeded", std::mem_fn(&ModifierBase::checked));
                                                            return true; }
    Fraction    fraction(bool noStore = false) override   { if (!noStore) store();
                                                            return v._aquiresRoll ? Fraction(1) : Fraction(1, 2); }
    void        restore() override                        { vars s = v;
                                                            aquiresRoll->setChecked(s._aquiresRoll);
                                                            v = s; }
    void        store() override                          { v._aquiresRoll = aquiresRoll->isChecked(); }
    QJsonObject toJson() override                         { QJsonObject obj;
                                                            obj["name"] = name();
                                                            obj["type"] = type();
                                                            obj["adder"] = isAdder();
                                                            obj["aquiresRoll"] = v._aquiresRoll;
                                                            return obj; }

private:
    struct vars {
        bool _aquiresRoll = false;
    } v;

    QCheckBox* aquiresRoll = nullptr;

    QString optOut(bool show, bool abbr = false) {
        Fraction half(1, 2);
        if (v._aquiresRoll) {
            if (abbr) return QString(show ? "(-1) " : "") + "Ablates, Skill Roll After";
            else return QString(show ? "(-1) " : "") + "Ablative, Aquires a Skill Roll When Exceeded";
        }
        return (show ? "(-" + half.toString() + ") " : "") + "Ablative";
    }
};

class AffectedAsAnotherSense: public Modifier {
public:
    AffectedAsAnotherSense()
        : Modifier("Affected As Another Sense", isLimitation, isModifier)
        , v({ -1 }) { }
    AffectedAsAnotherSense(const AffectedAsAnotherSense& m)
        : Modifier(m)
        , v(m.v) { }
    AffectedAsAnotherSense(AffectedAsAnotherSense&& m)
        : Modifier(m)
        , v(m.v) { }
    AffectedAsAnotherSense(QJsonObject json)
        : Modifier(json["name"].toString(""),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mSense = json["sense"].toInt(0); }
    ~AffectedAsAnotherSense() override { }

    AffectedAsAnotherSense& operator=(const AffectedAsAnotherSense&) = delete;
    AffectedAsAnotherSense& operator=(AffectedAsAnotherSense&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<AffectedAsAnotherSense>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<AffectedAsAnotherSense>(json); }

    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { sense = createComboBox(p, l, "Affect as?", { "", "Hearing", "Mental", "Radio", "Sight", "Smell/Taste", "Touch" },
                                                                                     std::mem_fn(&ModifierBase::index));
                                                              return true; }
    Fraction      fraction(bool noStore = false) override   { if (!noStore) store();
                                                              return Fraction(1, 4); }
    void          restore() override                        { vars s = v;
                                                              sense->setCurrentIndex(s.mSense);
                                                              v = s; }
    void          store() override                          { v.mSense = sense->currentIndex(); }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"] = name();
                                                              obj["type"] = type();
                                                              obj["adder"] = isAdder();
                                                              obj["sense"] = v.mSense;
                                                              return obj; }

private:
    struct vars {
        int mSense = 0;
    } v;

    QComboBox* sense = nullptr;

    QString optOut(bool show) {
        Fraction quarter(1, 4);
        if (v.mSense < 1) return "<incomplete>";
        QStringList sense { "", "Hearing", "Mental", "Radio", "Sight", "Smell/Taste", "Touch" };
        return (show ? "(-" + quarter.toString() + ") " : "") + "Affected As Another Sense (" + sense[v.mSense] + ")";
    }
};

class AffectedAsMoreThanOneSense: public Modifier {
public:
    AffectedAsMoreThanOneSense()
        : Modifier("Affected as More Than One Sense", isLimitation, isModifier)
        , v({ false, "" }) { }
    AffectedAsMoreThanOneSense(const AffectedAsMoreThanOneSense& m)
        : Modifier(m)
        , v(m.v) { }
    AffectedAsMoreThanOneSense(AffectedAsMoreThanOneSense&& m)
        : Modifier(m)
        , v(m.v) { }
    AffectedAsMoreThanOneSense(QJsonObject json)
        : Modifier(json["name"].toString("Affected as More Than One Sense"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mVeryUncommon = json["veryUncommon"].toBool();
                                                  v.mSenses       = json["senses"].toString(""); }
    ~AffectedAsMoreThanOneSense() override { }

    AffectedAsMoreThanOneSense& operator=(const AffectedAsMoreThanOneSense&) = delete;
    AffectedAsMoreThanOneSense& operator=(AffectedAsMoreThanOneSense&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<AffectedAsMoreThanOneSense>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<AffectedAsMoreThanOneSense>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { veryUncommon = createCheckBox(p, l, "Senses are very uncommon", std::mem_fn(&ModifierBase::checked));
                                                              senses       = createLineEdit(p, l, "Affect as?", std::mem_fn(&ModifierBase::changed));
                                                              return true; }
    Fraction      fraction(bool noStore = false) override   { if (!noStore) store();
                                                              return v.mVeryUncommon ? Fraction(1, 2) : Fraction(1, 4); }
    void          restore() override                        { vars s = v;
                                                              veryUncommon->setChecked(s.mVeryUncommon);
                                                              senses->setText(s.mSenses);
                                                              v = s; }
    void          store() override                          { v.mSenses       = senses->text();
                                                              v.mVeryUncommon = veryUncommon->isChecked(); }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]         = name();
                                                              obj["type"]         = type();
                                                              obj["adder"]        = isAdder();
                                                              obj["senses"]       = v.mSenses;
                                                              obj["veryUncommon"] = v.mVeryUncommon;
                                                              return obj; }

private:
    struct vars {
        bool    mVeryUncommon = false;
        QString mSenses = "";
    } v;

    QCheckBox* veryUncommon = nullptr;
    QLineEdit* senses = nullptr;

    QString optOut(bool show) {
        Fraction quarter(1, 4);
        Fraction half(1, 2);
        if (v.mSenses.isEmpty()) return "<incomplete>";
        return (show ? "(-" + (v.mVeryUncommon ? half.toString() : quarter.toString()) + ") " : "") + "Affected As More Than One Sense: " + v.mSenses +
                (v.mVeryUncommon ? " (Senses are very uncommon)" : "");
    }
};

class AffectsDesolid: public Modifier {
public:
    AffectsDesolid()
        : Modifier("Affects Desolid", isAdvantage, isModifier)
        , v({ false, "" }) { }
    AffectsDesolid(const AffectsDesolid& m)
        : Modifier(m)
        , v(m.v) { }
    AffectsDesolid(AffectsDesolid&& m)
        : Modifier(m)
        , v(m.v) { }
    AffectsDesolid(QJsonObject json)
        : Modifier(json["name"].toString("Affects Desolid"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mLimited = json["limited"].toBool();
                                                  v.mWhat    = json["what"].toString(""); }
    ~AffectsDesolid() override { }

    AffectedAsMoreThanOneSense& operator=(const AffectsDesolid&) = delete;
    AffectedAsMoreThanOneSense& operator=(AffectsDesolid&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<AffectsDesolid>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<AffectsDesolid>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { limited = createCheckBox(p, l, "Limited to a single SFX", std::mem_fn(&ModifierBase::checked));
                                                              what    = createLineEdit(p, l, "Limited to?", std::mem_fn(&ModifierBase::changed));
                                                              return true; }
    Fraction      fraction(bool noStore = false) override   { if (!noStore) store();
                                                              return v.mLimited ? Fraction(1, 4) : Fraction(1, 2); }
    void          restore() override                        { vars s = v;
                                                              limited->setChecked(s.mLimited);
                                                              what->setText(s.mWhat);
                                                              v = s; }
    void          store() override                          { v.mWhat    = what->text();
                                                              v.mLimited = limited->isChecked(); }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]    = name();
                                                              obj["type"]    = type();
                                                              obj["adder"]   = isAdder();
                                                              obj["limited"] = v.mLimited;
                                                              obj["what"]    = v.mWhat;
                                                              return obj; }

private:
    struct vars {
        bool    mLimited = false;
        QString mWhat = "";
    } v;

    QCheckBox* limited = nullptr;
    QLineEdit* what = nullptr;

    QString optOut(bool show) {
        Fraction quarter(1, 4);
        Fraction half(1, 2);
        if (v.mWhat.isEmpty() && v.mLimited) return "<incomplete>";
        return (show ? "(+" + (v.mLimited ? quarter.toString() : half.toString()) + ") " : "") + "Affects Desolid" +
                (v.mLimited ? " (only versus " + v.mWhat + ")" : "");
    }
};

class AffectsPhysicalWorld: public NoFormModifier {
public:
    AffectsPhysicalWorld()
        : NoFormModifier("Affects Physical World", isAdvantage, Fraction(2)) { }
    AffectsPhysicalWorld(QJsonObject json)
        : NoFormModifier(json) { }
    AffectsPhysicalWorld(const AffectsPhysicalWorld& m)
        : NoFormModifier(m) { }
    AffectsPhysicalWorld(AffectsPhysicalWorld&& m)
        : NoFormModifier(m) { }
    ~AffectsPhysicalWorld() override { }

    AffectsPhysicalWorld& operator=(const AffectsPhysicalWorld&) = delete;
    AffectsPhysicalWorld& operator=(AffectsPhysicalWorld&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<AffectsPhysicalWorld>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<AffectsPhysicalWorld>(json); }
};

class Allocatable: public NoFormModifier {
public:
    Allocatable()
        : NoFormModifier("Allocatable▲", isAdvantage, Fraction(1, 4)) { }
    Allocatable(QJsonObject json)
        : NoFormModifier(json) { }
    Allocatable(const Allocatable& m)
        : NoFormModifier(m) { }
    Allocatable(Allocatable&& m)
        : NoFormModifier(m) { }
    ~Allocatable() override { }

    Allocatable& operator=(const Allocatable&) = delete;
    Allocatable& operator=(Allocatable&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<Allocatable>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<Allocatable>(json); }
};

class AlterableOriginPoint: public NoFormModifier {
public:
    AlterableOriginPoint()
        : NoFormModifier("Alterable Origin Point", isAdvantage, 5_cp) { } // NOLINT
    AlterableOriginPoint(QJsonObject json)
        : NoFormModifier(json) { }
    AlterableOriginPoint(const AlterableOriginPoint& m)
        : NoFormModifier(m) { }
    AlterableOriginPoint(AlterableOriginPoint&& m)
        : NoFormModifier(m) { }
    ~AlterableOriginPoint() override { }

    AlterableOriginPoint& operator=(const AlterableOriginPoint&) = delete;
    AlterableOriginPoint& operator=(AlterableOriginPoint&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<AlterableOriginPoint>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<AlterableOriginPoint>(json); }
};

class AlterableSize: public NoFormModifier {
public:
    AlterableSize()
        : NoFormModifier("Alterable Size", isAdvantage, 5_cp) { } // NOLINT
    AlterableSize(QJsonObject json)
        : NoFormModifier(json) { }
    AlterableSize(const AlterableSize& m)
        : NoFormModifier(m) { }
    AlterableSize(AlterableSize&& m)
        : NoFormModifier(m) { }
    ~AlterableSize() override { }

    AlterableSize& operator=(const AlterableSize&) = delete;
    AlterableSize& operator=(AlterableSize&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<AlterableSize>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<AlterableSize>(json); }
};

class AlternateCombatValue: public Modifier {
public:
    AlternateCombatValue()
        : Modifier("Alternate Combat Value▲", isBoth, isModifier)
        , v({ -1 }) { }
    AlternateCombatValue(const AlternateCombatValue& m)
        : Modifier(m)
        , v(m.v) { }
    AlternateCombatValue(AlternateCombatValue&& m)
        : Modifier(m)
        , v(m.v) { }
    AlternateCombatValue(QJsonObject json)
        : Modifier(json["name"].toString("Alternate Combat Value▲"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mIs = json["is"].toInt(0); }
    ~AlternateCombatValue() override { }

    AlternateCombatValue& operator=(const AlternateCombatValue&) = delete;
    AlternateCombatValue& operator=(AlternateCombatValue&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<AlternateCombatValue>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<AlternateCombatValue>(json); }

    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { is = createComboBox(p, l, "Kind of ACV?", { "",
                                                                                                          "Mental Power uses OCV instead of OMCV",
                                                                                                          "Mental Power attacks against DCV instead of DMCV",
                                                                                                          "Non-Mental Power uses OMCV instead of OCV",
                                                                                                          "Non-Mental Power attacks against DMCV instead of DCV" },
                                                                                  std::mem_fn(&ModifierBase::index));
                                                              return true; }
    Fraction      fraction(bool noStore = false) override   { if (!noStore) store();
                                                              switch (v.mIs) {
                                                              case 0:  return Fraction(1, 4);
                                                              case 1:  return Fraction(-1, 4);
                                                              case 2:  return Fraction(0, 1);
                                                              case 3:  return Fraction(1, 4);
                                                              default: return Fraction(0, 1);
                                                              }
                                                            }
    void          restore() override                        { vars s = v;
                                                              is->setCurrentIndex(s.mIs);
                                                              v = s; }
    void          store() override                          { v.mIs = is->currentIndex(); }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["is"]    = v.mIs;
                                                              return obj; }

private:
    struct vars {
        int    mIs = 0;
    } v;

    QComboBox* is = nullptr;

    QString optOut(bool show) {
        QStringList is = {
            "",
            "Mental Power uses OCV instead of OMCV",
            "Mental Power attacks against DCV instead of DMCV",
            "Non-Mental Power uses OMCV instead of OCV",
            "Non-Mental Power attacks against DMCV instead of DCV"
        };
        if (v.mIs < 1) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        return (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "ACV▲: " + is[v.mIs];
    }
};

class AlwaysOn: public NoFormModifier {
public:
    AlwaysOn()
        : NoFormModifier("Always On", isLimitation, Fraction(1, 2)) { }
    AlwaysOn(QJsonObject json)
        : NoFormModifier(json) { }
    AlwaysOn(const AlwaysOn& m)
        : NoFormModifier(m) { }
    AlwaysOn(AlwaysOn&& m)
        : NoFormModifier(m) { }
    ~AlwaysOn() override { }

    AlwaysOn& operator=(const AlwaysOn&) = delete;
    AlwaysOn& operator=(AlwaysOn&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<AlwaysOn>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<AlwaysOn>(json); }
};

class AreaOfEffect: public Modifier {
public:
    AreaOfEffect()
        : Modifier("Area Of Effect", isAdvantage, isModifier)
        , v({ -1, false, "", false, false, false, false, false, false, false, 1 }) { }
    AreaOfEffect(const AreaOfEffect& m)
        : Modifier(m)
        , v(m.v) { }
    AreaOfEffect(AreaOfEffect&& m)
        : Modifier(m)
        , v(m.v) { }
    AreaOfEffect(QJsonObject json)
        : Modifier(json["name"].toString("Area Of Effect"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mKind = json["kind"].toInt(0);
                                                  v.mExplosion = json["explosion"].toBool(false);
                                                  v.mFixedShape = json["fixedShape"].toBool(false);
                                                  v.mShape = json["shape"].toString("");
                                                  v.mNonselective = json["nonselective"].toBool(false);
                                                  v.mSelective = json["selective"].toBool(false);
                                                  v.mMobile = json["mobile"].toBool(false);
                                                  v.mAccurate = json["accurate"].toBool(false);
                                                  v.mThinCone = json["thinCone"].toBool(false);
                                                  v.mDamageShield = json["damageShield"].toBool(false);
                                                  v.mMultiplier = json["multiplier"].toInt(0);
                                                }
    ~AreaOfEffect() override { }

    AreaOfEffect& operator=(const AreaOfEffect&) = delete;
    AreaOfEffect& operator=(AreaOfEffect&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<AreaOfEffect>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<AreaOfEffect>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { kind = createComboBox(p, l, "Kind of Area?", { "Radius",
                                                                                                             "Cone",
                                                                                                             "Line",
                                                                                                             "Surface",
                                                                                                             "Any Area" },
                                                                                  std::mem_fn(&ModifierBase::index));
                                                              fixedShape = createCheckBox(p, l, "Fixed Shape", std::mem_fn(&ModifierBase::checked));
                                                              shape = createLineEdit(p, l, "What shape?", std::mem_fn(&ModifierBase::changed));
                                                              explosion = createCheckBox(p, l, "Explosion", std::mem_fn(&ModifierBase::checked));
                                                              nonselective = createCheckBox(p, l, "Non-selective Target", std::mem_fn(&ModifierBase::checked));
                                                              selective = createCheckBox(p, l, "Selective Target", std::mem_fn(&ModifierBase::checked));
                                                              mobile = createCheckBox(p, l, "Mobile", std::mem_fn(&ModifierBase::checked));
                                                              accurate = createCheckBox(p, l, "Accurate", std::mem_fn(&ModifierBase::checked));
                                                              thinCone = createCheckBox(p, l, "Thin Cone", std::mem_fn(&ModifierBase::checked));
                                                              damageShield = createCheckBox(p, l, "Damage Shield", std::mem_fn(&ModifierBase::checked));
                                                              multiplier = createLineEdit(p, l, "m Length?", std::mem_fn(&ModifierBase::changed));
                                                              fixedShape->setEnabled(false);
                                                              shape->setEnabled(false);
                                                              explosion->setEnabled(false);
                                                              nonselective->setEnabled(false);
                                                              selective->setEnabled(false);
                                                              mobile->setEnabled(false);
                                                              accurate->setEnabled(false);
                                                              thinCone->setEnabled(false);
                                                              damageShield->setEnabled(false);
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              kind->setCurrentIndex(s.mKind);
                                                              fixedShape->setChecked(s.mFixedShape);
                                                              shape->setText(s.mShape);
                                                              explosion->setChecked(s.mExplosion);
                                                              nonselective->setChecked(s.mNonselective);
                                                              selective->setChecked(s.mSelective);
                                                              mobile->setChecked(s.mMobile);
                                                              accurate->setChecked(s.mAccurate);
                                                              thinCone->setChecked(s.mThinCone);
                                                              damageShield->setChecked(s.mDamageShield);
                                                              multiplier->setText(QString("%1").arg(s.mMultiplier));
                                                              v = s;
                                                            }
    void          store() override                          { v.mKind = kind->currentIndex();
                                                              v.mFixedShape = fixedShape->isChecked();
                                                              v.mShape = shape->text();
                                                              v.mExplosion = explosion->isChecked();
                                                              v.mNonselective = nonselective->isChecked();
                                                              v.mSelective = selective->isChecked();
                                                              v.mMobile = mobile->isChecked();
                                                              v.mAccurate = mobile->isChecked();
                                                              v.mThinCone = thinCone->isChecked();
                                                              v.mDamageShield = damageShield->isChecked();
                                                              v.mMultiplier = multiplier->text().toInt(0);
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["kind"]  = v.mKind;
                                                              obj["fixedShape"] = v.mFixedShape;
                                                              obj["shape"] = v.mShape;
                                                              obj["explosion"] = v.mExplosion;
                                                              obj["nonselective"] = v.mNonselective;
                                                              obj["selective"] = v.mSelective;
                                                              obj["mobile"] = v.mMobile;
                                                              obj["accurate"] = v.mAccurate;
                                                              obj["thinCone"] = v.mThinCone;
                                                              obj["damageShield"] = v.mDamageShield;
                                                              obj["multiplier"] = v.mMultiplier;
                                                              return obj;
                                                            }
    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        int steps = 0;
        if (v.mMultiplier < 1) v.mMultiplier = 1;
        switch (v.mKind) {
        case 0: steps = (int) (log((double) (v.mMultiplier - 1) / 4 + 1)  / log(2.0)); break; // NOLINT
        case 1: steps = (int) (log((double) (v.mMultiplier - 1) / 8 + 1)  / log(2.0)); break; // NOLINT
        case 2: steps = (int) (log((double) (v.mMultiplier - 1) / 16 + 1) / log(2.0)); break; // NOLINT
        case 3: steps = (int) (log((double) (v.mMultiplier - 1) / 2 + 1)  / log(2.0)); break; // NOLINT
        case 4: steps = (int) (log((double) (v.mMultiplier - 1) / 2 + 1)  / log(2.0)); break; // NOLINT
        }
        f *= steps + 1;
        if (v.mExplosion) f -= Fraction(1, 2);
        if (v.mNonselective) f -= Fraction(1, 4);
        if (v.mSelective) f += Fraction(1, 4);
        if (v.mMobile) f += Fraction(1, 2);
        if (v.mAccurate) f += Fraction(1, 4);
        if (v.mThinCone) f -= Fraction(1, 4);
        if (v.mFixedShape) f -= Fraction(1, 4);
        if (f < Fraction(1, 4)) f = Fraction(1, 4);
        return f;
    }

private:
    struct vars {
        int     mKind = 0;
        bool    mFixedShape = false;
        QString mShape = "";
        bool    mExplosion = false;
        bool    mNonselective = false;
        bool    mSelective = false;
        bool    mMobile = false;
        bool    mAccurate = false;
        bool    mThinCone = false;
        bool    mDamageShield = false;
        int     mMultiplier = 0;
    } v;

    QComboBox* kind = nullptr;
    QCheckBox* fixedShape = nullptr;
    QLineEdit* shape = nullptr;
    QCheckBox* explosion = nullptr;
    QCheckBox* nonselective = nullptr;
    QCheckBox* selective = nullptr;
    QCheckBox* mobile = nullptr;
    QCheckBox* accurate = nullptr;
    QCheckBox* thinCone = nullptr;
    QCheckBox* damageShield = nullptr;
    QLineEdit* multiplier = nullptr;

    void changed(QString) override {
        QString txt = multiplier->text();
        if (txt.isEmpty() || isNumber(txt)) {
            if (txt.toInt(nullptr) < 1 && !txt.isEmpty()) multiplier->undo();
            v.mMultiplier = txt.toInt(nullptr);
            return;
        }
        multiplier->undo();
    }

    void index(int) override {
        store();
        if (sender() == static_cast<QWidget*>(kind)) {
            fixedShape->setEnabled(false);
            shape->setEnabled(false);
            explosion->setEnabled(true);
            nonselective->setEnabled(true);
            selective->setEnabled(true);
            mobile->setEnabled(true);
            accurate->setEnabled(false);
            thinCone->setEnabled(false);
            damageShield->setEnabled(false);
            switch (kind->currentIndex()) {
            case 0: accurate->setEnabled(true);
                    multiplier->setPlaceholderText("m radius");
                    break;
            case 1: thinCone->setEnabled(true);
                    multiplier->setPlaceholderText("m side");
                    break;
            case 2: multiplier->setPlaceholderText("m length");
                    break;
            case 3: damageShield->setEnabled(true);
                    multiplier->setPlaceholderText("m area");
                    break;
            case 4: fixedShape->setEnabled(true);
                    multiplier->setPlaceholderText("x m areas");
                    shape->setEnabled(true);
                    break;
            default: return;
            }
            multiplier->setText(QString("%1").arg(v.mMultiplier));
        }
        ModifiersDialog::ref().updateForm();
    }

    QString optOut(bool show) {
        QStringList kind = {
            "Radius", "Cone", "Line", "Surface", "Any Area"
        };
        QStringList size = {
            "m radius", "m sides", "m long", "m area", "m area"
        };
        if (v.mKind < 0 || (v.mFixedShape && v.mShape.isEmpty())) return "<incomplete>";
        if (v.mMultiplier < 1) v.mMultiplier = 1;
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "Area Of Effect: " + kind[v.mKind];
        if (v.mKind == 4) desc += QString(" (%1x%1%2)").arg(v.mMultiplier).arg(size[v.mKind]);
        else desc += QString(" (%1%2)").arg(v.mMultiplier).arg(size[v.mKind]);
        if (v.mFixedShape) desc += "; Fixed Shape(" + v.mShape + ")";
        if (v.mAccurate) desc += "; Accurate";
        if (v.mExplosion) desc += "; Explosion";
        if (v.mMobile) desc += "; Mobile";
        if (v.mNonselective) desc += "; Non-selective Targeting";
        if (v.mSelective) desc += "; Selective Targeting";
        if (v.mThinCone) desc += "; Thin Cone";
        if (v.mDamageShield) desc += "; Damage Shield";
        return desc;
    }
};

class ArmorPiercing: public Modifier {
public:
    ArmorPiercing()
        : Modifier("Armor Piercing", isAdvantage, isModifier)
        , v({ 1 }) { }
    ArmorPiercing(const ArmorPiercing& m)
        : Modifier(m)
        , v(m.v) { }
    ArmorPiercing(ArmorPiercing&& m)
        : Modifier(m)
        , v(m.v) { }
    ArmorPiercing(QJsonObject json)
        : Modifier(json["name"].toString(""),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mTimes = json["times"].toInt(0);
                                                }
    ~ArmorPiercing() override { }

    ArmorPiercing& operator=(const ArmorPiercing& m) {
        v = m.v;
        return *this;
    }
    ArmorPiercing& operator=(ArmorPiercing&& m) {
        v = m.v;
        return *this;
    }

    shared_ptr<Modifier> create() override                         { return make_shared<ArmorPiercing>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<ArmorPiercing>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { times = createLineEdit(p, l, "Times?", std::mem_fn(&ModifierBase::changed));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              times->setText(QString("%1").arg(s.mTimes));
                                                              v = s;
                                                            }
    void          store() override                          { v.mTimes = times->text().toInt(0);
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["times"] = v.mTimes;
                                                              return obj;
                                                            }
    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        f *= v.mTimes;
        return f;
    }

private:
    struct vars {
        int mTimes = 0;
    } v;

    QLineEdit* times = nullptr;

    void changed(QString) override {
        QString txt = times->text();
        if (txt.isEmpty() || isNumber(txt)) {
            if (txt.toInt(nullptr) < 1 && !txt.isEmpty()) times->undo();
            v.mTimes = txt.toInt(nullptr);
            if (v.mTimes < 1) v.mTimes = 1;
            return;
        }
        times->undo();
    }

    QString optOut(bool show) {
        if (v.mTimes < 0) return "<incomplete>";
        Fraction f(fraction(NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                ((v.mTimes > 1) ? QString("%1x").arg(v.mTimes) : "") +"Armor Piercing";
        return desc;
    }
};

class AVAD: public Modifier {
public:
    AVAD()
        : Modifier("Attack Versus Alternate Defense▲", isBoth, isModifier)
        , v({ -1, -1, "", false }) { }
    AVAD(const AVAD& m)
        : Modifier(m)
        , v(m.v) { }
    AVAD(AVAD&& m)
        : Modifier(m)
        , v(m.v) { }
    AVAD(QJsonObject json)
        : Modifier(json["name"].toString("Attack Versus Alternate Defense▲"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v._original = json["original"].toInt(0);
                                                  v.mNewOne = json["newone"].toInt(0);
                                                  v.mVersus = json["versus"].toString();
                                                  v.mNND = json["nnd"].toBool(false);
                                                }
    ~AVAD() override { }

    AVAD& operator=(const AVAD&) = delete;
    AVAD& operator=(AVAD&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<AVAD>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<AVAD>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { original = createComboBox(p, l, "Original Defense is?", { "",
                                                                                                                        "Very Common (PD/ED) for ex.",
                                                                                                                        "Common (Reistant PD/ED)",
                                                                                                                        "Uncommon (Sight Flash Defense)",
                                                                                                                        "Rare (Life Support)" },
                                                                                       std::mem_fn(&ModifierBase::index));
                                                              newone = createComboBox(p, l, "New Defense is?", { "",
                                                                                                                 "Very Common (PD/ED) for ex.",
                                                                                                                 "Common (Reistant PD/ED)",
                                                                                                                 "Uncommon (Sight Flash Defense)",
                                                                                                                 "Rare (Life Support)" },
                                                                                      std::mem_fn(&ModifierBase::index));
                                                              versus = createLineEdit(p, l, "Defense?", std::mem_fn(&ModifierBase::changed));
                                                              nnd = createCheckBox(p, l, "All or nothing (NND)", std::mem_fn(&ModifierBase::checked));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              original->setCurrentIndex(s._original);
                                                              newone->setCurrentIndex(s.mNewOne);
                                                              versus->setText(s.mVersus);
                                                              nnd->setChecked(s.mNND);
                                                              v = s;
                                                            }
    void          store() override                          { v._original = original->currentIndex();
                                                              v.mNewOne = newone->currentIndex();
                                                              v.mVersus = versus->text();
                                                              v.mNND = nnd->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["original"] = v._original;
                                                              obj["newone"] = v.mNewOne;
                                                              obj["versus"] = v.mVersus;
                                                              obj["nnd"] = v.mNND;
                                                              return obj;
                                                            }
    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 2);
        int diff = v.mNewOne - v._original;
        f *= diff;
        if (v.mNND) f -= Fraction(1, 2);
        return f;
    }

private:
    struct vars {
        int _original = 0;
        int mNewOne = 0;
        QString mVersus = "";
        bool mNND = false;
    } v;

    QComboBox* original = nullptr;
    QComboBox* newone = nullptr;
    QLineEdit* versus = nullptr;
    QCheckBox* nnd = nullptr;

    QString optOut(bool show) {
        if (v._original < 1 || v.mNewOne < 1 || v.mVersus.isEmpty()) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 + (v.mNND ? "NND: Defense is " : "Attack Versus Alternate Defense: ") + v.mVersus;
        return desc;
    }
};

class Autofire: public Modifier {
public:
    Autofire()
        : Modifier("Autofire", isAdvantage, isModifier)
        , v({ false, 0, false }) { }
    Autofire(const ModifierBase* m)
        : Modifier(dynamic_cast<const Modifier*>(m))
        , v(dynamic_cast<const Autofire*>(m)->v) { }
    Autofire(const Autofire& m)
        : Modifier(m)
        , v(m.v) { }
    Autofire(Autofire&& m)
        : Modifier(m)
        , v(m.v) { }
    Autofire(QJsonObject json)
        : Modifier(json["name"].toString("Autofire"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mFive = json["five"].toBool(false);
                                                  v.mDoubling = json["doubling"].toInt(0);
                                                  v.mByPass = json["bypass"].toBool(false);
                                                }
    ~Autofire() override { }

    Autofire& operator=(const Autofire&) = delete;
    Autofire& operator=(Autofire&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<Autofire>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<Autofire>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { bypass = createCheckBox(p, l, "All or nothing (NND)", std::mem_fn(&ModifierBase::checked));
                                                              doubling = createLineEdit(p, l, "Doublings of shots?", std::mem_fn(&ModifierBase::numeric));
                                                              bypass = createCheckBox(p, l, "All or nothing (NND)", std::mem_fn(&ModifierBase::checked));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              five->setChecked(s.mFive);
                                                              doubling->setText(QString("%1").arg(s.mDoubling));
                                                              bypass->setChecked(s.mByPass);
                                                              v = s;
                                                            }
    void          store() override                          { v.mFive = five->isChecked();
                                                              v.mDoubling = doubling->text().toInt(nullptr);
                                                              v.mByPass = bypass->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["five"] = v.mFive;
                                                              obj["doubling"] = v.mDoubling;
                                                              obj["bypass"] = v.mByPass;
                                                              return obj;
                                                            }
    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v.mFive) f = Fraction(1, 2);
        if (v.mDoubling > 0) f += Fraction(1, 2) * v.mDoubling;
        if (v.mByPass) f += 1;
        return f;
    }

private:
    struct vars {
        bool mFive = false;
        int mDoubling = 0;
        bool mByPass = false;
    } v;

    QCheckBox* five = nullptr;
    QLineEdit* doubling = nullptr;
    QCheckBox* bypass = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 QString("Autofire: %1 shots").arg((3 + 5) * std::pow(2, v.mDoubling)); // NOLINT
        return desc;
    }

    void numeric(QString) override {
        QString txt = doubling->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        doubling->undo();
    }
};

class Backlash: public NoFormModifier {
public:
    Backlash()
        : NoFormModifier("Backlash", isAdvantage, Fraction(1, 2)) { }
    Backlash(QJsonObject json)
        : NoFormModifier(json) { }
    Backlash(const Backlash& m)
        : NoFormModifier(m) { }
    Backlash(Backlash&& m)
        : NoFormModifier(m) { }
    ~Backlash() override { }

    Backlash& operator=(const Backlash&) = delete;
    Backlash& operator=(Backlash&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<Backlash>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<Backlash>(json); }
};

class BasedOnCON: public NoFormModifier {
public:
    BasedOnCON()
        : NoFormModifier("Based On CON", isLimitation, Fraction(1)) { }
    BasedOnCON(QJsonObject json)
        : NoFormModifier(json) { }
    BasedOnCON(const BasedOnCON& m)
        : NoFormModifier(m) { }
    BasedOnCON(BasedOnCON&& m)
        : NoFormModifier(m) { }
    ~BasedOnCON() override { }

    BasedOnCON& operator=(const BasedOnCON&) = delete;
    BasedOnCON& operator=(BasedOnCON&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<BasedOnCON>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<BasedOnCON>(json); }
};

class Beam: public NoFormModifier {
public:
    Beam()
        : NoFormModifier("Beam", isLimitation, Fraction(1, 4)) { }
    Beam(QJsonObject json)
        : NoFormModifier(json) { }
    Beam(const Beam& m)
        : NoFormModifier(m) { }
    Beam(Beam&& m)
        : NoFormModifier(m) { }
    ~Beam() override { }

    Beam& operator=(const Beam&) = delete;
    Beam& operator=(Beam&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<Beam>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<Beam>(json); }
};

class BodyOnly: public NoFormModifier {
public:
    BodyOnly()
        : NoFormModifier("BODY Only", isLimitation, Fraction(1, 2)) { }
    BodyOnly(QJsonObject json)
        : NoFormModifier(json) { }
    BodyOnly(const BodyOnly& m)
        : NoFormModifier(m) { }
    BodyOnly(BodyOnly&& m)
        : NoFormModifier(m) { }
    ~BodyOnly() override { }

    BodyOnly& operator=(const BodyOnly&) = delete;
    BodyOnly& operator=(BodyOnly&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<BodyOnly>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<BodyOnly>(json); }
};

class CanApplyRemoveAdders: public NoFormModifier {
public:
    CanApplyRemoveAdders()
        : NoFormModifier("Can Apply/Remove Adders", isAdvantage, Fraction(1)) { }
    CanApplyRemoveAdders(QJsonObject json)
        : NoFormModifier(json) { }
    CanApplyRemoveAdders(const CanApplyRemoveAdders& m)
        : NoFormModifier(m) { }
    CanApplyRemoveAdders(CanApplyRemoveAdders&& m)
        : NoFormModifier(m) { }
    ~CanApplyRemoveAdders() override { }

    CanApplyRemoveAdders& operator=(const CanApplyRemoveAdders&) = delete;
    CanApplyRemoveAdders& operator=(CanApplyRemoveAdders&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<CanApplyRemoveAdders>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<CanApplyRemoveAdders>(json); }
};

class CanBeDeflectedReflected: public NoFormModifier {
public:
    CanBeDeflectedReflected()
        : NoFormModifier("Can Be Deflected/Reflected", isLimitation, Fraction(1, 4)) { }
    CanBeDeflectedReflected(QJsonObject json)
        : NoFormModifier(json) { }
    CanBeDeflectedReflected(const CanBeDeflectedReflected& m)
        : NoFormModifier(m) { }
    CanBeDeflectedReflected(CanBeDeflectedReflected&& m)
        : NoFormModifier(m) { }
    ~CanBeDeflectedReflected() override { }

    CanBeDeflectedReflected& operator=(const CanBeDeflectedReflected&) = delete;
    CanBeDeflectedReflected& operator=(CanBeDeflectedReflected&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<CanBeDeflectedReflected>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<CanBeDeflectedReflected>(json); }
};

class CanBeDispelled: public NoFormModifier {
public:
    CanBeDispelled()
        : NoFormModifier("Can Be Dispelled", isLimitation, Fraction(1, 4)) { }
    CanBeDispelled(QJsonObject json)
        : NoFormModifier(json) { }
    CanBeDispelled(const CanBeDispelled& m)
        : NoFormModifier(m) { }
    CanBeDispelled(CanBeDispelled&& m)
        : NoFormModifier(m) { }
    ~CanBeDispelled() override { }

    CanBeDispelled& operator=(const CanBeDispelled&) = delete;
    CanBeDispelled& operator=(CanBeDispelled&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<CanBeDispelled>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<CanBeDispelled>(json); }
};

class CanHealLimbs: public NoFormModifier {
public:
    CanHealLimbs()
        : NoFormModifier("Can Heal Limbs", isAdvantage, 5_cp) { } // NOLINT
    CanHealLimbs(QJsonObject json)
        : NoFormModifier(json) { }
    CanHealLimbs(const CanHealLimbs& m)
        : NoFormModifier(m) { }
    CanHealLimbs(CanHealLimbs&& m)
        : NoFormModifier(m) { }
    ~CanHealLimbs() override { }

    CanHealLimbs& operator=(const CanHealLimbs&) = delete;
    CanHealLimbs& operator=(CanHealLimbs&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<CanHealLimbs>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<CanHealLimbs>(json); }
};

class CanOnlyBeUsedThroughMindLink: public Modifier {
public:
    CanOnlyBeUsedThroughMindLink()
        : Modifier("Can Only Be Used Through MindLink", isLimitation, isModifier)
        , v({ -1 }) { }
    CanOnlyBeUsedThroughMindLink(const CanOnlyBeUsedThroughMindLink& m)
        : Modifier(m)
        , v(m.v) { }
    CanOnlyBeUsedThroughMindLink(CanOnlyBeUsedThroughMindLink&& m)
        : Modifier(m)
        , v(m.v) { }
    CanOnlyBeUsedThroughMindLink(QJsonObject json)
        : Modifier(json["name"].toString("CanOnlyBeUsedThroughMindLink"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mValue = json["value"].toInt(0);
                                                }
    ~CanOnlyBeUsedThroughMindLink() override { }

    CanOnlyBeUsedThroughMindLink& operator=(const CanOnlyBeUsedThroughMindLink&) = delete;
    CanOnlyBeUsedThroughMindLink& operator=(CanOnlyBeUsedThroughMindLink&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<CanOnlyBeUsedThroughMindLink>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<CanOnlyBeUsedThroughMindLink>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }   // ¹²³₂₄⁄
    bool          form(QWidget* p, QVBoxLayout* l) override { value = createComboBox(p, l, "Value of the limitation?", { "", "-¹⁄₄⁄", "-¹⁄₂⁄", "-³⁄₄", "-1", "-1¹⁄₄⁄", "-1¹⁄₂⁄", "-1³⁄₄", "-2"
                                                                                                                       }, std::mem_fn(&ModifierBase::index));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              value->setCurrentIndex(s.mValue);
                                                              v = s;
                                                            }
    void          store() override                          { v.mValue = value->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["value"] = v.mValue;
                                                              return obj;
                                                            }
    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        static QList<Fraction> fractions { { 0, 1 }, { 1, 4 }, { 1, 2 }, { 3, 4 }, { 1, 1 }, { 1, { 1, 4 } }, { 1, { 1, 2 } }, { 1, { 3, 4 } }, { 2, 1 } };
        return fractions[v.mValue];
    }

private:
    struct vars {
        int mValue = 0;
    } v;

    QComboBox* value = nullptr;

    QString optOut(bool show) {
        static QStringList values { "", "¹⁄₄⁄", "¹⁄₂⁄", "³⁄₄", "1", "1¹⁄₄⁄", "1¹⁄₂⁄", "1³⁄₄", "2" };

        if (v.mValue < 1) return "<incomplete>";
        return (show ? QString("(%1").arg(values[v.mValue]) + ") " : "") + "Can Only Be Used Through Mind Link";
    }
};

class CannotBeEscapedWithTeleportation: public NoFormModifier {
public:
    CannotBeEscapedWithTeleportation()
        : NoFormModifier("Cannot Be Escaped With Teleportation", isAdvantage, Fraction(1, 4)) { }
    CannotBeEscapedWithTeleportation(QJsonObject json)
        : NoFormModifier(json) { }
    CannotBeEscapedWithTeleportation(const CannotBeEscapedWithTeleportation& m)
        : NoFormModifier(m) { }
    CannotBeEscapedWithTeleportation(CannotBeEscapedWithTeleportation&& m)
        : NoFormModifier(m) { }
    ~CannotBeEscapedWithTeleportation() override { }

    CannotBeEscapedWithTeleportation& operator=(const CannotBeEscapedWithTeleportation&) = delete;
    CannotBeEscapedWithTeleportation& operator=(CannotBeEscapedWithTeleportation&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<CannotBeEscapedWithTeleportation>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<CannotBeEscapedWithTeleportation>(json); }
};

class CannotBeUsedThroughMindLink: public NoFormModifier {
public:
    CannotBeUsedThroughMindLink()
        : NoFormModifier("Cannot Be Used Through Mind Link", isLimitation, Fraction(1, 4)) { }
    CannotBeUsedThroughMindLink(QJsonObject json)
        : NoFormModifier(json) { }
    CannotBeUsedThroughMindLink(const CannotBeUsedThroughMindLink& m)
        : NoFormModifier(m) { }
    CannotBeUsedThroughMindLink(CannotBeUsedThroughMindLink&& m)
        : NoFormModifier(m) { }
    ~CannotBeUsedThroughMindLink() override { }

    CannotBeUsedThroughMindLink& operator=(const CannotBeUsedThroughMindLink&) = delete;
    CannotBeUsedThroughMindLink& operator=(CannotBeUsedThroughMindLink&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<CannotBeUsedThroughMindLink>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<CannotBeUsedThroughMindLink>(json); }
};

class CannotBeUsedWith: public Modifier {
public:
    CannotBeUsedWith()
        : Modifier("Cannot Be Used With ...", isLimitation, isModifier)
        , v({ false, "" }) { }
    CannotBeUsedWith(const CannotBeUsedWith& m)
        : Modifier(m)
        , v(m.v) { }
    CannotBeUsedWith(CannotBeUsedWith&& m)
        : Modifier(m)
        , v(m.v) { }
    CannotBeUsedWith(QJsonObject json)
        : Modifier(json["name"].toString("Cannot Be Used With ..."),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mVeryEffective = json["veryEffective"].toBool();
                                                  v.mManeuver = json["maneuver"].toString();
                                                }
    ~CannotBeUsedWith() override { }

    CannotBeUsedWith& operator=(const CannotBeUsedWith&) = delete;
    CannotBeUsedWith& operator=(CannotBeUsedWith&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<CannotBeUsedWith>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<CannotBeUsedWith>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { veryEffective = createCheckBox(p, l, "Very Effective", std::mem_fn(&ModifierBase::checked));
                                                              maneuver = createLineEdit(p, l, "Maneuver", std::mem_fn(&ModifierBase::changed));
                                                              return true; }
    Fraction      fraction(bool noStore = false) override   { if (!noStore) store();
                                                              return v.mVeryEffective ? Fraction(1, 2) : Fraction(1, 4); }
    void          restore() override                        { vars s = v;
                                                              veryEffective->setChecked(s.mVeryEffective);
                                                              maneuver->setText(s.mManeuver);
                                                              v = s; }
    void          store() override                          { v.mVeryEffective = veryEffective->isChecked();
                                                              v.mManeuver = maneuver->text();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]          = name();
                                                              obj["type"]          = type();
                                                              obj["adder"]         = isAdder();
                                                              obj["maneuver"]      = v.mManeuver;
                                                              obj["veryEffective"] = v.mVeryEffective;
                                                              return obj; }

private:
    struct vars {
        bool    mVeryEffective = false;
        QString mManeuver = "";
    } v;

    QCheckBox* veryEffective = nullptr;
    QLineEdit* maneuver = nullptr;

    QString optOut(bool show) {
        Fraction quarter(1, 4);
        Fraction half(1, 2);
        return (show ? "(-" + (v.mVeryEffective ? half.toString() : quarter.toString()) + ") " : "") + "Cannot Be used With " + v.mManeuver +
                (v.mVeryEffective ? " (very effective)" : "");
    }
};

class CannotUseTargeting: public NoFormModifier {
public:
    CannotUseTargeting()
        : NoFormModifier("Cannot Use Targeting", isLimitation, Fraction(1, 2)) { }
    CannotUseTargeting(QJsonObject json)
        : NoFormModifier(json) { }
    CannotUseTargeting(const CannotUseTargeting& m)
        : NoFormModifier(m) { }
    CannotUseTargeting(CannotUseTargeting&& m)
        : NoFormModifier(m) { }
    ~CannotUseTargeting() override { }

    CannotUseTargeting& operator=(const CannotUseTargeting&) = delete;
    CannotUseTargeting& operator=(CannotUseTargeting&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<CannotUseTargeting>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<CannotUseTargeting>(json); }
};

class Charges: public Modifier {
private:
    int _charges[12] { 1,         2,         3,         4,         6,         8, // NOLINT
                        12,        16,        32,        64,        128,       250 }; // NOLINT
    Fraction _lim[12] { { -2, 1 }, { -1, { 1, 2 } }, { -1, { 1, 4 } }, { -1, 1 }, { -3, 4 }, { -1, 2 }, // NOLINT
                        { -1, 4 }, { 0, 1 },  { 1, 4 },  { 1, 2 },  { 3, 4 },  { 1, 1 } };

    Fraction chargeFraction() {
        int lim = 0;
        int max = sizeof(_charges) / sizeof(int);
        if (v.mCharges > 0) {
            for (lim = 0; lim < max; ++lim) if (_charges[lim] >= v.mCharges) break; // NOLINT
            if (lim >= max) lim = max - 1;
        }

        return _lim[lim]; // NOLINT
    }

public:
    Charges()
        : Modifier("Charges", isBoth, isModifier)
        , v({ 0, 0, false, 0, false, false, -1, false, -1 }) { }
    Charges(const Charges& m)
        : Modifier(m)
        , v(m.v) { }
    Charges(Charges&& m)
        : Modifier(m)
        , v(m.v) { }
    Charges(QJsonObject json)
        : Modifier(json["name"].toString("Charges"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mCharges      = json["charges"].toInt(0);
                                                  v.mClips        = json["clips"].toInt(0);
                                                  v.mBoostable    = json["boostable"].toBool(false);
                                                  v.mContinuing   = json["continuing"].toInt(0);
                                                  v.mFuelCharge   = json["fuelCharge"].toBool(false);
                                                  v.mRecoverable  = json["recoverable"].toBool(false);
                                                  v.mRecoveryTime = json["recoveryTime"].toInt(0);
                                                  v.mExpensive    = json["expensive"].toBool(false);
                                                  v.mReload       = json["reload"].toInt(0);
                                                }
    ~Charges() override { }

    Charges& operator=(const Charges&) = delete;
    Charges& operator=(Charges&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<Charges>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<Charges>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { charges      = createLineEdit(p, l, "Number of Charges", std::mem_fn(&ModifierBase::numeric));
                                                              clips        = createLineEdit(p, l, "Number of Clips", std::mem_fn(&ModifierBase::numeric));
                                                              reload       = createComboBox(p, l, "Extra Time to Reload", { "", "2 Phases", "Turn", "1 Minute", "5 Minutes", "20 Minutes",
                                                                                                                            "1 Hour", "6 Hours", "1 Day", "1 Week", "1 Month", "1 Season",
                                                                                                                            "1 Year", "5 Years" }, std::mem_fn(&ModifierBase::index));
                                                              boostable    = createCheckBox(p, l, "Boostable", std::mem_fn(&ModifierBase::checked));
                                                              continuing   = createComboBox(p, l, "Continuing?", { "", "2 Phases", "Turn", "1 Minute", "5 Minutes", "20 Minutes",
                                                                                                                   "1 Hour", "6 Hours", "1 Day", "1 Week", "1 Month", "1 Season",
                                                                                                                   "1 Year", "5 Years" }, std::mem_fn(&ModifierBase::index));
                                                              recoverable  = createCheckBox(p, l, "Recoverable?", std::mem_fn(&ModifierBase::checked));
                                                              recoveryTime = createComboBox(p, l, "Extra Recovery Time?", { "", "1 Week", "1 Month", "1 Season",
                                                                                                                            "1 Year", "5 Years", "10 Years", "25 Years",
                                                                                                                            "Never" }, std::mem_fn(&ModifierBase::index));
                                                              expensive    = createCheckBox(p, l, "Expensive or Difficult to Replace", std::mem_fn(&ModifierBase::checked));
                                                              fuelCharge   = createCheckBox(p, l, "Fuel Charge", std::mem_fn(&ModifierBase::checked));
                                                              return true; }
    Fraction      fraction(bool noStore = false) override   { if (!noStore) store();
                                                              auto res = chargeFraction();
                                                              if (v.mBoostable) res += Fraction(1, 4);
                                                              if (v.mClips) {
                                                                  int steps = (int) (log((double) v.mClips) / log(2.0)); // NOLINT
                                                                  res += steps * Fraction(1, 4);
                                                              }
                                                              res += v.mContinuing * Fraction(1, 4);
                                                              if (v.mFuelCharge) res += Fraction(1, 4);
                                                              if (v.mRecoverable) res += Fraction(1, 2);
                                                              if (v.mRecoveryTime >= 0) res -= v.mRecoveryTime * Fraction(1, 4);
                                                              if (v.mExpensive) res -= Fraction(1, 4);
                                                              if (v.mReload > -1) res -= v.mReload * Fraction(1, 4);
                                                              return res;
                                                            }
    void          restore() override                        { vars s = v;
                                                              charges->setText(QString("%1").arg(s.mCharges));
                                                              clips->setText(QString("%1").arg(s.mClips));
                                                              boostable->setEnabled(s.mClips > 1);
                                                              boostable->setChecked(s.mBoostable);
                                                              continuing->setCurrentIndex(s.mContinuing);
                                                              fuelCharge->setChecked(s.mFuelCharge);
                                                              recoverable->setChecked(s.mRecoverable);
                                                              recoveryTime->setCurrentIndex(s.mRecoveryTime);
                                                              expensive->setChecked(s.mExpensive);
                                                              reload->setCurrentIndex(s.mReload);
                                                              v = s;
                                                            }
    void          store() override                          { v.mCharges      = charges->text().toInt();
                                                              v.mClips        = clips->text().toInt();
                                                              v.mBoostable    = boostable->isChecked();
                                                              v.mContinuing   = continuing->currentIndex();
                                                              v.mFuelCharge   = fuelCharge->isChecked();
                                                              v.mRecoverable  = recoverable->isChecked();
                                                              v.mRecoveryTime = recoveryTime->currentIndex();
                                                              v.mExpensive    = expensive->isChecked();
                                                              v.mReload       = reload->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]         = name();
                                                              obj["type"]         = type();
                                                              obj["charges"]      = v.mCharges;
                                                              obj["clips"]        = v.mClips;
                                                              obj["boostable"]    = v.mBoostable;
                                                              obj["continuing"]   = v.mContinuing;
                                                              obj["fuelCharge"]   = v.mFuelCharge;
                                                              obj["recoverable"]  = v.mRecoverable;
                                                              obj["recoveryTime"] = v.mRecoveryTime;
                                                              obj["expensive"]    = v.mExpensive;
                                                              obj["reload"]       = v.mReload;
                                                              return obj;
                                                            }

private:
    struct vars {
        int  mCharges = 0;
        int  mClips = 0;
        bool mBoostable = false;
        int  mContinuing = 0;
        bool mFuelCharge = false;
        bool mRecoverable = false;
        int  mRecoveryTime = 0;
        bool mExpensive = false;
        int  mReload = 0;
    } v;

    QLineEdit* charges = nullptr;
    QLineEdit* clips = nullptr;
    QCheckBox* boostable = nullptr;
    QComboBox* continuing = nullptr;
    QCheckBox* fuelCharge = nullptr;
    QCheckBox* recoverable = nullptr;
    QComboBox* recoveryTime = nullptr;
    QCheckBox* expensive = nullptr;
    QComboBox* reload = nullptr;

    void numeric(QString) override {
        QLineEdit* text = dynamic_cast<QLineEdit*>(sender());
        QString txt = text->text();
        if (txt.isEmpty() || isNumber(txt)) {
            if (text == charges) {
                if (txt.toInt() > 1) {
                    reload->setEnabled(true);
                    boostable->setEnabled(true);
                }
                else {
                    reload->setEnabled(false);
                    reload->setCurrentIndex(-1);
                    v.mReload = -1;
                    boostable->setEnabled(false);
                    boostable->setChecked(false);
                    v.mBoostable = false;
                }
            }
            store();
            ModifiersDialog::ref().updateForm();
            return;
        }
        text->undo();
    }

    QString optOut(bool show) {
        static QStringList conTime { "", "2 Phases", "Turn", "1 Minute", "5 Minutes", "20 Minutes",
                                     "1 Hour", "6 Hours", "1 Day", "1 Week", "1 Month", "1 Season",
                                     "1 Year", "5 Years" };
        static QStringList recTime {
            "", "1 Week", "1 Month", "1 Season", "1 Year", "5 Years", "10 Years", "25 Years", "Never"
        };
        static QStringList extraTime {
            "", "2 Phases", "Turn", "1 Minute", "5 Minutes", "20 Minutes",  "1 Hour", "6 Hours", "1 Day", "1 Week", "1 Month", "1 Season", "1 Year", "5 Years"
        };
        if (v.mCharges < 1) return "<incomplete>";
        Fraction mod = fraction(NoStore);
        QString res = (show ? QString("(%1").arg((mod > 0) ? "+" : "") + fraction(NoStore).toString() + ") " : "") + "Charges (" +
                QString("%1").arg(v.mCharges) + QString(" charge%1").arg((v.mCharges > 1) ? "s" : "");
        if (v.mFuelCharge) res += "; Fuel Charge";
        if (v.mRecoverable) res += "; Recoverable";
        if (v.mRecoveryTime > 0) res += "; Recovery Time: " + recTime[v.mRecoveryTime];
        if (v.mContinuing > 0) res += "Continuing: " + conTime[v.mContinuing];
        if (v.mClips > 1) res += QString("; %1 clips").arg(v.mClips);
        if (v.mReload > 0) res += "; Extra Time to Reload - " + extraTime[v.mReload];
        if (v.mExpensive) res += "; Charges are Expensive or Difficult to Obtain";
        return res + ")";
    }
};

class CombatAccelerationDecelleration: public NoFormModifier {
public:
    CombatAccelerationDecelleration()
        : NoFormModifier("Combat Acceleration/Decelleration", isAdvantage, Fraction(1, 4)) { }
    CombatAccelerationDecelleration(QJsonObject json)
        : NoFormModifier(json) { }
    CombatAccelerationDecelleration(const CombatAccelerationDecelleration& m)
        : NoFormModifier(m) { }
    CombatAccelerationDecelleration(CombatAccelerationDecelleration&& m)
        : NoFormModifier(m) { }
    ~CombatAccelerationDecelleration() override { }

    CombatAccelerationDecelleration& operator=(const CombatAccelerationDecelleration&) = delete;
    CombatAccelerationDecelleration& operator=(CombatAccelerationDecelleration&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<CombatAccelerationDecelleration>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<CombatAccelerationDecelleration>(json); }
};

class Concentration: public Modifier {
public:
    Concentration()
        : Modifier("Concentration", isLimitation, isModifier)
        , v({ false, false, false }) { }
    Concentration(const Concentration& m)
        : Modifier(m)
        , v(m.v) { }
    Concentration(Concentration&& m)
        : Modifier(m)
        , v(m.v) { }
    Concentration(QJsonObject json)
        : Modifier(json["name"].toString("Concentration"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mZeroDCV  = json["zeroDCV"].toBool(false);
                                                  v.mUnaware  = json["unaware"].toBool(false);
                                                  v.mConstant = json["constant"].toBool(false);
                                                }
    ~Concentration() override { }

    Concentration& operator=(const Concentration&) = delete;
    Concentration& operator=(Concentration&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<Concentration>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<Concentration>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { zeroDCV  = createCheckBox(p, l, "0 DCV", std::mem_fn(&ModifierBase::checked));
                                                              unaware  = createCheckBox(p, l, "Unaware (no Perception roll)", std::mem_fn(&ModifierBase::checked));
                                                              constant = createCheckBox(p, l, "Constant concentration", std::mem_fn(&ModifierBase::checked));
                                                              return true; }
    Fraction      fraction(bool noStore = false) override   { if (!noStore) store();
                                                              auto res = Fraction(1, 4);
                                                              if (v.mZeroDCV) res += Fraction(1, 4);
                                                              if (v.mUnaware) res += Fraction(1, 4);
                                                              return res;
                                                            }
    void          restore() override                        { vars s = v;
                                                              zeroDCV->setChecked(s.mZeroDCV);
                                                              unaware->setChecked(s.mUnaware);
                                                              constant->setChecked(s.mConstant);
                                                              v = s;
                                                            }
    void          store() override                          { v.mZeroDCV  = zeroDCV->isChecked();
                                                              v.mUnaware  = unaware->isChecked();
                                                              v.mConstant = constant->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]     = name();
                                                              obj["type"]     = type();
                                                              obj["zeroDCV"]  = v.mZeroDCV;
                                                              obj["unaware"]  = v.mUnaware;
                                                              obj["constant"] = v.mConstant;
                                                              return obj;
                                                            }

private:
    struct vars {
        bool mZeroDCV = false;
        bool mUnaware = false;
        bool mConstant = false;
    } v;

    QCheckBox* zeroDCV = nullptr;
    QCheckBox* unaware = nullptr;
    QCheckBox* constant = nullptr;

    QString optOut(bool show) {
        Fraction mod = fraction(Modifier::NoStore);
        QString res = (show ? QString("(%1").arg((mod > 0) ? "+" : "") + fraction(false).toString() + ") " : "") + "Concentrataion";
        QString sep = " (";
        if (zeroDCV) { res += sep + "0 DCV"; sep = "; "; }
        if (unaware) { res += sep + "No Perception Roll"; sep = "; "; }
        if (constant) { res += sep + "Constant concentration"; sep = "; "; }
        if (sep != " (") res += ")";
        return res;
    }
};

class Constant: public NoFormModifier {
public:
    Constant()
        : NoFormModifier("Constant", isAdvantage, Fraction(1, 2)) { }
    Constant(QJsonObject json)
        : NoFormModifier(json) { }
    Constant(const Constant& m)
        : NoFormModifier(m) { }
    Constant(Constant&& m)
        : NoFormModifier(m) { }
    ~Constant() override { }

    Constant& operator=(const Constant&) = delete;
    Constant& operator=(Constant&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<Constant>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<Constant>(json); }
};

class CostsEndurance: public Modifier {
public:
    CostsEndurance()
        : Modifier("Costs Endurance", isLimitation, isModifier)
        , v({ -1 }) { }
    CostsEndurance(const CostsEndurance& m)
        : Modifier(m)
        , v(m.v) { }
    CostsEndurance(CostsEndurance&& m)
        : Modifier(m)
        , v(m.v) { }
    CostsEndurance(QJsonObject json)
        : Modifier(json["name"].toString("CostsEndurance"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mHowMuch  = json["howMuch"].toInt(0);
                                                }
    ~CostsEndurance() override { }

    CostsEndurance& operator=(const CostsEndurance&) = delete;
    CostsEndurance& operator=(CostsEndurance&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<CostsEndurance>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<CostsEndurance>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { howMuch  = createComboBox(p, l, "How Much END?", { "",
                                                                                                                 "Half END",
                                                                                                                 "Normal END" }, std::mem_fn(&ModifierBase::index));
                                                              return true; }
    Fraction      fraction(bool noStore = false) override   { if (!noStore) store();
                                                              auto res = Fraction(1, 4);
                                                              if (v.mHowMuch > 0) res += Fraction(1, 4);
                                                              return res;
                                                            }
    void          restore() override                        { vars s = v;
                                                              howMuch->setCurrentIndex(s.mHowMuch);
                                                              v = s;
                                                            }
    void          store() override                          { v.mHowMuch  = howMuch->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]     = name();
                                                              obj["type"]     = type();
                                                              obj["howMuch"]  = v.mHowMuch;
                                                              return obj;
                                                            }

    Fraction endChange() override { if (v.mHowMuch == 0) return Fraction(1, 2); return Fraction(1); }

private:
    struct vars {
        int  mHowMuch = 0;
    } v;

    QComboBox* howMuch = nullptr;

    QString optOut(bool show) {
        if (v.mHowMuch < 1) return "<incomplete>";
        Fraction mod = fraction(Modifier::NoStore);
        QString res = (show ? QString("(%1").arg((mod > 0) ? "+" : "") + fraction(Modifier::NoStore).toString() + ") " : "") + "Costs Endurance (";
        if (v.mHowMuch == 1) res += Fraction(1, 2).toString();
        else res += "Full";
        return res + " END)";
    }
};

class CostsEnduranceOnlyToActivate: public NoFormModifier {
public:
    CostsEnduranceOnlyToActivate()
        : NoFormModifier("Costs Endurance Only To Activate▲", isAdvantage, Fraction(1, 4)) { }
    CostsEnduranceOnlyToActivate(QJsonObject json)
        : NoFormModifier(json) { }
    CostsEnduranceOnlyToActivate(const CostsEnduranceOnlyToActivate& m)
        : NoFormModifier(m) { }
    CostsEnduranceOnlyToActivate(CostsEnduranceOnlyToActivate&& m)
        : NoFormModifier(m) { }
    ~CostsEnduranceOnlyToActivate() override { }

    CostsEnduranceOnlyToActivate& operator=(const CostsEnduranceOnlyToActivate&) = delete;
    CostsEnduranceOnlyToActivate& operator=(CostsEnduranceOnlyToActivate&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<CostsEnduranceOnlyToActivate>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<CostsEnduranceOnlyToActivate>(json); }
};

class CostsEnduranceToMaintain: public Modifier {
public:
    CostsEnduranceToMaintain()
        : Modifier("Costs Endurance To Maintain", isLimitation, isModifier)
        , v({ -1 }) { }
    CostsEnduranceToMaintain(const CostsEnduranceToMaintain& m)
        : Modifier(m)
        , v(m.v) { }
    CostsEnduranceToMaintain(CostsEnduranceToMaintain&& m)
        : Modifier(m)
        , v(m.v) { }
    CostsEnduranceToMaintain(QJsonObject json)
        : Modifier(json["name"].toString("CostsEnduranceToMaintain"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mHowMuch  = json["howMuch"].toInt(0);
                                                }
    ~CostsEnduranceToMaintain() override { }

    CostsEnduranceToMaintain& operator=(const CostsEnduranceToMaintain&) = delete;
    CostsEnduranceToMaintain& operator=(CostsEnduranceToMaintain&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<CostsEnduranceToMaintain>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<CostsEnduranceToMaintain>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { howMuch  = createComboBox(p, l, "How Much END?", { "", "Half END",
                                                                                                                 "Normal END" }, std::mem_fn(&ModifierBase::index));
                                                              return true; }
    Fraction      fraction(bool noStore = false) override   { if (!noStore) store();
                                                              auto res = Fraction(1, 4);
                                                              if (v.mHowMuch > 0) res += Fraction(1, 4);
                                                              return res;
                                                            }
    void          restore() override                        { vars s = v;
                                                              howMuch->setCurrentIndex(s.mHowMuch);
                                                              v = s;
                                                            }
    void          store() override                          { v.mHowMuch  = howMuch->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]     = name();
                                                              obj["type"]     = type();
                                                              obj["howMuch"]  = v.mHowMuch;
                                                              return obj;
                                                            }

    Fraction endChange() override { return v.mHowMuch == 0 ? Fraction(1, 2) : Fraction(1); }

    struct vars {
        int  mHowMuch = 0;
    } v;

    QComboBox* howMuch = nullptr;

    QString optOut(bool show) {
        if (v.mHowMuch < 1) return "<incomplete>";
        Fraction mod = fraction(Modifier::NoStore);
        QString res = (show ? QString("(%1").arg((mod > 0) ? "+" : "") + fraction(Modifier::NoStore).toString() + ") " : "") + "Costs Endurance To Maintain (";
        if (v.mHowMuch == 1) res += Fraction(1, 2).toString();
        else res += "Full";
        return res + " END)";
    }
};

class Cumulative: public NoFormModifier {
public:
    Cumulative()
        : NoFormModifier("Cumulative▲", isAdvantage, Fraction(1, 2)) { }
    Cumulative(QJsonObject json)
        : NoFormModifier(json) { }
    Cumulative(const Cumulative& m)
        : NoFormModifier(m) { }
    Cumulative(Cumulative&& m)
        : NoFormModifier(m) { }
    ~Cumulative() override { }

    Cumulative& operator=(const Cumulative&) = delete;
    Cumulative& operator=(Cumulative&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<Cumulative>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<Cumulative>(json); }
};

class DamageOverTime: public Modifier {
private:
    int      _time[7] { 1,        2,        3,        4,        6,        8,               12  }; // NOLINT
    Fraction _adv[7]  { { 0, 1 }, { 1, 4 }, { 1, 2 }, { 3, 4 }, { 1, 1 }, { 1, { 1, 4 } }, { 1, { 1, 2 } } }; // NOLINT

    // 2^x=C
    // log(2^x)=log(C)
    // x*log(2)=log(C)
    // x=log(C)/log(2)
    Fraction timeFraction() {
        int adv = 0;
        int max = sizeof(_time) / sizeof(int);
        if (v._times > 0) {
            for (adv = 0; adv < max; ++adv) if (_time[adv] <= v._times) break; // NOLINT
            if (adv >= max) {
                int C = v._times / 16; // NOLINT
                int x = (int) (log((double) C) / log(2.0)); // NOLINT
                return Fraction(5, 2) + Fraction(1, 4) * x; // NOLINT
            }
        }

        return _adv[adv]; // NOLINT
    }

    Fraction _duration[8] { { 0, 1 }, { 2, 1 }, { 1, { 1, 2 } }, { 1, 1 }, { 1, 2 }, { 1, 4 }, { 0, 1 }, { -1, 4 } }; // NOLINT

    Fraction durationFraction() {
        if (v._duration == -1) return Fraction(1, 1);
        if (v._duration < 8) return _duration[v._duration]; // NOLINT
        if (v._duration > 6 && v._oneUse) return Fraction(0, 1); // NOLINT
        return Fraction(1, 2) * (v._duration - 7); // NOLINT
    }

public:
    DamageOverTime()
        : Modifier("Damage Over Time▲", isBoth, isModifier)
        , v({ 0, -1, false, false }) { }
    DamageOverTime(const DamageOverTime& m)
        : Modifier(m)
        , v(m.v) { }
    DamageOverTime(DamageOverTime&& m)
        : Modifier(m)
        , v(m.v) { }
    DamageOverTime(QJsonObject json)
        : Modifier(json["name"].toString("Damage Over Time▲"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v._times    = json["times"].toInt(0);
                                                  v._duration = json["duration"].toInt(0);
                                                  v._once     = json["once"].toBool(false);
                                                  v._oneUse   = json["oneUse"].toBool(false);
                                                }
    ~DamageOverTime() override { }

    DamageOverTime& operator=(const DamageOverTime&) = delete;
    DamageOverTime& operator=(DamageOverTime&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<DamageOverTime>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<DamageOverTime>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { times       = createLineEdit(p, l, "Number of Damage Increments", std::mem_fn(&ModifierBase::numeric));
                                                              duration    = createComboBox(p, l, "Duration Between Damage Increments", { "", "Every Segment", "Every Other Segement",
                                                                                                                                         "Every 3 Segments", "Every 4 Segments",
                                                                                                                                         "Every 6 Segments", "Every Turn",
                                                                                                                                         "Every 30 Segments", "Every Minute",
                                                                                                                                         "Every 5 Minutes", "Every 20 Minutes",
                                                                                                                                         "Every Hour", "Every 6 Hours", "Every Day",
                                                                                                                                         "Every Week", "Every Month", "Every Season",
                                                                                                                                         "Every Year", "Every 5 Years" },
                                                                                                                                         std::mem_fn(&ModifierBase::index));
                                                              once  = createCheckBox(p, l, "Target's Defenses Only Apply Once", std::mem_fn(&ModifierBase::checked));
                                                              oneUse = createCheckBox(p, l, "One Use at a Time", std::mem_fn(&ModifierBase::checked));
                                                              return true; }
    Fraction      fraction(bool noStore = false) override   { if (!noStore) store();
                                                              auto res = Fraction(1, 1);
                                                              res += (v._once ? Fraction(2, 1) : Fraction(1, 1)) * timeFraction();
                                                              res += durationFraction();
                                                              return res;
                                                            }
    void          restore() override                        { vars s = v;
                                                              times->setText(QString("%1").arg(s._times));
                                                              duration->setCurrentIndex(s._duration);
                                                              once->setChecked(s._once);
                                                              oneUse->setChecked(s._oneUse);
                                                              v = s;
                                                            }
    void          store() override                          { v._times    = times->text().toInt();
                                                              v._duration = duration->currentIndex();
                                                              v._once     = once->isChecked();
                                                              v._oneUse   = oneUse->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]     = name();
                                                              obj["type"]     = type();
                                                              obj["times"]    = v._times;
                                                              obj["duration"] = v._duration;
                                                              obj["omce"]     = v._once;
                                                              obj["oneUSe"]   = v._oneUse;
                                                              return obj;
                                                            }

private:
    struct vars {
        int  _times = 0;
        int  _duration = 0;
        bool _once = false;
        bool _oneUse = false;
    } v;

    QLineEdit* times = nullptr;
    QComboBox* duration = nullptr;
    QCheckBox* once = nullptr;
    QCheckBox* oneUse = nullptr;

    QString optOut(bool show) {
        static QStringList duration { "", "Every Segment", "Every Other Segement",
                                      "Every 3 Segments", "Every 4 Segments",
                                      "Every 6 Segments", "Every Turn",
                                      "Every 30 Segments", "Every Minute",
                                      "Every 5 Minutes", "Every 20 MInutes",
                                      "Every Hour", "Every 6 Hours", "Every Day",
                                      "Every Week", "Every Month", "Every Season",
                                      "Every Year", "Every 5 Years" };
        Fraction mod = fraction(Modifier::NoStore);
        if (v._duration < 1) return "<incomplete>";
        QString res = (show ? QString("(%1").arg((mod > 0) ? "+" : "") + mod.toString() + ") " : "") + "Damage Over Time▲ (";
        res += QString("%1 Damage Increments %1").arg(v._times).arg(duration[v._duration]);
        if (v._once) res += "; Target's Defenses Apply Only Once";
        if (v._oneUse) res += "; One Use at a Time";
        return res + ")";
    }
};

class DecreasedAccelerationDeceleration: public Modifier {
public:
    DecreasedAccelerationDeceleration()
        : Modifier("Decreased Acceleration/Deceleration", isLimitation, isModifier)
        , v({ -1 }) { }
    DecreasedAccelerationDeceleration(const DecreasedAccelerationDeceleration& m)
        : Modifier(m)
        , v(m.v) { }
    DecreasedAccelerationDeceleration(DecreasedAccelerationDeceleration&& m)
        : Modifier(m)
        , v(m.v) { }
    DecreasedAccelerationDeceleration(QJsonObject json)
        : Modifier(json["name"].toString("Decreased Acceleration/Deceleration"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v._howMuch  = json["howMuch"].toInt(0);
                                                }
    ~DecreasedAccelerationDeceleration() override { }

    DecreasedAccelerationDeceleration& operator=(const DecreasedAccelerationDeceleration&) = delete;
    DecreasedAccelerationDeceleration& operator=(DecreasedAccelerationDeceleration&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<DecreasedAccelerationDeceleration>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<DecreasedAccelerationDeceleration>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { howMuch  = createComboBox(p, l, "Reduced to?", { "", "4m per m", "3m per m",
                                                                                                               "2m per m", "1m per m" }, std::mem_fn(&ModifierBase::index));
                                                              return true; }
    Fraction      fraction(bool noStore = false) override   { if (!noStore) store();
                                                              auto res = Fraction(1, 4);
                                                              if (v._howMuch > 2) res += Fraction(1, 4);
                                                              return res;
                                                            }
    void          restore() override                        { vars s = v;
                                                              howMuch->setCurrentIndex(s._howMuch);
                                                              v = s;
                                                            }
    void          store() override                          { v._howMuch  = howMuch->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]     = name();
                                                              obj["type"]     = type();
                                                              obj["howMuch"]  = v._howMuch;
                                                              return obj;
                                                            }

private:
    struct vars {
        int  _howMuch = 0;
    } v;

    QComboBox* howMuch = nullptr;

    QString optOut(bool show) {
        static QStringList by { "", "4m per m", "3m per m", "2m per m", "1m per m" };

        if (v._howMuch < 1) return "<incomplete>";
        Fraction mod = fraction(Modifier::NoStore);
        QString res = (show ? QString("(%1").arg((mod > 0) ? "+" : "") + mod.toString() + ") " : "") + "Decreased Acceleration/Deceleration (";
        res += by[v._howMuch];
        return res + ")";
    }
};

class DelayedEffect: public Modifier {
public:
    DelayedEffect()
        : Modifier("Delayed EffectꚚ", isAdvantage, isModifier)
        , v({ 0 }) { }
    DelayedEffect(const DelayedEffect& m)
        : Modifier(m)
        , v(m.v) { }
    DelayedEffect(DelayedEffect&& m)
        : Modifier(m)
        , v(m.v) { }
    DelayedEffect(QJsonObject json)
        : Modifier(json["name"].toString("Delayed EffectꚚ"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mDoubling = json["doubling"].toInt(0);
                                                }
    ~DelayedEffect() override { }

    DelayedEffect& operator=(const DelayedEffect&) = delete;
    DelayedEffect& operator=(DelayedEffect&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<DelayedEffect>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<DelayedEffect>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { doubling = createLineEdit(p, l, "Doublings of powers?", std::mem_fn(&ModifierBase::numeric));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              doubling->setText(QString("%1").arg(s.mDoubling));
                                                              v = s;
                                                            }
    void          store() override                          { v.mDoubling = doubling->text().toInt(nullptr);
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["doubling"] = v.mDoubling;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(0, 1);
        if (v.mDoubling > 0) f = Fraction(1, 4) * v.mDoubling;
        return f;
    }

private:
    struct vars {
        int mDoubling = 0;
    } v;

    QLineEdit* doubling = nullptr;

    QString optOut(bool show) {
        if (v.mDoubling == 0) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 QString("Delayed EffectꚚ: x%1").arg((int) pow(2.0, (double) v.mDoubling)); // NOLINT
        return desc;
    }
};

class DelayedFadeReturnRate: public Modifier {
public:
    DelayedFadeReturnRate()
        : Modifier("Delayed Fade/Return Rate▲", isAdvantage, isModifier)
        , v({ -1 }) { }
    DelayedFadeReturnRate(const DelayedFadeReturnRate& m)
        : Modifier(m)
        , v(m.v) { }
    DelayedFadeReturnRate(DelayedFadeReturnRate&& m)
        : Modifier(m)
        , v(m.v) { }
    DelayedFadeReturnRate(QJsonObject json)
        : Modifier(json["name"].toString("Delayed Fade/Return Rate▲"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v._duration = json["duration"].toInt(0);
                                                }
    ~DelayedFadeReturnRate() override { }

    DelayedFadeReturnRate& operator=(const DelayedFadeReturnRate&) = delete;
    DelayedFadeReturnRate& operator=(DelayedFadeReturnRate&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<DelayedFadeReturnRate>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<DelayedFadeReturnRate>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { duration    = createComboBox(p, l, "5 CP Per Time ", { "", "Turn", "Minute", "5 Minutes", "20 Minutes", "Hour", "6 Hours",
                                                                                                                     "Day", "Week", "Month", "Season", "Year", "5 Years" },
                                                                                                                     std::mem_fn(&ModifierBase::index));
                                                              return true; }
    Fraction      fraction(bool noStore = false) override   { if (!noStore) store();
                                                              auto res = Fraction(1, 1);
                                                              res += v._duration * Fraction(1, 4);
                                                              return res;
                                                            }
    void          restore() override                        { vars s = v;
                                                              duration->setCurrentIndex(s._duration);
                                                              v = s;
                                                            }
    void          store() override                          { v._duration = duration->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]     = name();
                                                              obj["type"]     = type();
                                                              obj["duration"] = v._duration;
                                                              return obj;
                                                            }

    struct vars {
        int  _duration = 0;
    } v;

    QComboBox* duration = nullptr;

    QString optOut(bool show) {
        static QStringList duration { "", "Turn", "Minute", "5 Minutes", "20 Minutes", "Hour", "6 Hours",
                                      "Day", "Week", "Month", "Season", "Year", "5 Years" };
        if (v._duration < 1) return "<incomplete>";
        Fraction mod = fraction(Modifier::NoStore);
        QString res = (show ? QString("(%1").arg((mod > 0) ? "+" : "") + mod.toString() + ") " : "") + "Delayed Fade/Return Rate▲ (";
        res += QString("5 CP per %1").arg(duration[v._duration]);
        return res + ")";
    }
};

class DifficultToDispel: public Modifier {
public:
    DifficultToDispel()
        : Modifier("Difficult To Dispel", isAdvantage, isModifier)
        , v({ 0 }) { }
    DifficultToDispel(const DifficultToDispel& m)
        : Modifier(m)
        , v(m.v) { }
    DifficultToDispel(DifficultToDispel&& m)
        : Modifier(m)
        , v(m.v) { }
    DifficultToDispel(QJsonObject json)
        : Modifier(json["name"].toString("Difficult To Dispel"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mDoubling = json["doubling"].toInt(0);
                                                }
    ~DifficultToDispel() override { }

    DifficultToDispel& operator=(const DifficultToDispel&) = delete;
    DifficultToDispel& operator=(DifficultToDispel&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<DifficultToDispel>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<DifficultToDispel>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { doubling = createLineEdit(p, l, "Doublings of active points?", std::mem_fn(&ModifierBase::numeric));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              doubling->setText(QString("%1").arg(s.mDoubling));
                                                              v = s;
                                                            }
    void          store() override                          { v.mDoubling = doubling->text().toInt(nullptr);
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]     = name();
                                                              obj["type"]     = type();
                                                              obj["adder"]    = isAdder();
                                                              obj["doubling"] = v.mDoubling;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(0, 1);
        if (v.mDoubling > 0) f = Fraction(1, 4) * v.mDoubling;
        return f;
    }

private:
    struct vars {
        int mDoubling = 0;
    } v;

    QLineEdit* doubling = nullptr;

    QString optOut(bool show) {
        if (v.mDoubling == 0) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 QString("Difficult To Dispel: x%1 Active Points").arg((int) pow(2.0, (double) v.mDoubling)); // NOLINT
        return desc;
    }
};

class Dismissible: public NoFormModifier {
public:
    Dismissible()
        : NoFormModifier("Dismissible▲", isAdvantage, 5_cp) { } // NOLINT
    Dismissible(QJsonObject json)
        : NoFormModifier(json) { }
    Dismissible(const Dismissible& m)
        : NoFormModifier(m) { }
    Dismissible(Dismissible&& m)
        : NoFormModifier(m) { }
    ~Dismissible() override { }

    Dismissible& operator=(const Dismissible&) = delete;
    Dismissible& operator=(Dismissible&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<Dismissible>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<Dismissible>(json); }
};

class DividedEffect: public NoFormModifier {
public:
    DividedEffect()
        : NoFormModifier("DividedEffect", isLimitation, Fraction(1, 4)) { }
    DividedEffect(QJsonObject json)
        : NoFormModifier(json) { }
    DividedEffect(const DividedEffect& m)
        : NoFormModifier(m) { }
    DividedEffect(DividedEffect&& m)
        : NoFormModifier(m) { }
    ~DividedEffect() override { }

    DividedEffect& operator=(const DividedEffect&) = delete;
    DividedEffect& operator=(DividedEffect&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<DividedEffect>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<DividedEffect>(json); }
};

class DoesBODY: public NoFormModifier {
public:
    DoesBODY()
        : NoFormModifier("Does BODYϴ", isAdvantage, Fraction(1, 1)) { }
    DoesBODY(QJsonObject json)
        : NoFormModifier(json) { }
    DoesBODY(const DoesBODY& m)
        : NoFormModifier(m) { }
    DoesBODY(DoesBODY&& m)
        : NoFormModifier(m) { }
    ~DoesBODY() override { }

    DoesBODY& operator=(const DoesBODY&) = delete;
    DoesBODY& operator=(DoesBODY&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<DoesBODY>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<DoesBODY>(json); }
};

class DoesKnockback: public NoFormModifier {
public:
    DoesKnockback()
        : NoFormModifier("Does Knockback", isAdvantage, Fraction(1, 4)) { }
    DoesKnockback(QJsonObject json)
        : NoFormModifier(json) { }
    DoesKnockback(const DoesKnockback& m)
        : NoFormModifier(m) { }
    DoesKnockback(DoesKnockback&& m)
        : NoFormModifier(m) { }
    ~DoesKnockback() override { }

    DoesKnockback& operator=(const DoesKnockback&) = delete;
    DoesKnockback& operator=(DoesKnockback&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<DoesKnockback>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<DoesKnockback>(json); }
};

class DoesntWorkOnDefinedDamage: public Modifier {
public:
    DoesntWorkOnDefinedDamage()
        : Modifier("Doesn't Work On [Defined] Damage", isLimitation, isModifier)
        , v({ "", -1 }) { }
    DoesntWorkOnDefinedDamage(const DoesntWorkOnDefinedDamage& m)
        : Modifier(m)
        , v(m.v) { }
    DoesntWorkOnDefinedDamage(DoesntWorkOnDefinedDamage&& m)
        : Modifier(m)
        , v(m.v) { }
    DoesntWorkOnDefinedDamage(QJsonObject json)
        : Modifier(json["name"].toString("Doesn't Work On [Defined] Damage"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mDoesnt = json["doesnt"].toString("");
                                                  v.mHowCommon = json["howCommon"].toInt(0);
                                                }
    ~DoesntWorkOnDefinedDamage() override { }

    DoesntWorkOnDefinedDamage& operator=(const DoesntWorkOnDefinedDamage&) = delete;
    DoesntWorkOnDefinedDamage& operator=(DoesntWorkOnDefinedDamage&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<DoesntWorkOnDefinedDamage>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<DoesntWorkOnDefinedDamage>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { doesnt    = createLineEdit(p, l, "Doesn't Work On?", std::mem_fn(&ModifierBase::changed));
                                                              howCommon = createComboBox(p, l, "How Common?", { "", "Rare", "Uncommon", "Common", "Very Common" },
                                                                                         std::mem_fn(&ModifierBase::index));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              doesnt->setText(s.mDoesnt);
                                                              howCommon->setCurrentIndex(s.mHowCommon);
                                                              v = s;
                                                            }
    void          store() override                          { v.mDoesnt    = doesnt->text();
                                                              v.mHowCommon = howCommon->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]      = name();
                                                              obj["type"]      = type();
                                                              obj["adder"]     = isAdder();
                                                              obj["doesnt"]    = v.mDoesnt;
                                                              obj["howCommon"] = v.mHowCommon;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        return v.mHowCommon * Fraction(1, 4);
    }

private:
    struct vars {
        QString mDoesnt = "";
        int     mHowCommon = 0;
    } v;

    QLineEdit* doesnt = nullptr;
    QComboBox* howCommon = nullptr;

    QString optOut(bool show) {
        if (v.mDoesnt.isEmpty() || v.mHowCommon < 1) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Doesn't Work On " + v.mDoesnt;
        return desc;
    }
};

class DoesntWorkWhileDuplicatExists: public NoFormModifier {
public:
    DoesntWorkWhileDuplicatExists()
        : NoFormModifier("Doesn't Work While Duplicat Exists", isLimitation, Fraction(1, 4)) { }
    DoesntWorkWhileDuplicatExists(QJsonObject json)
        : NoFormModifier(json) { }
    DoesntWorkWhileDuplicatExists(const DoesntWorkWhileDuplicatExists& m)
        : NoFormModifier(m) { }
    DoesntWorkWhileDuplicatExists(DoesntWorkWhileDuplicatExists&& m)
        : NoFormModifier(m) { }
    ~DoesntWorkWhileDuplicatExists() override { }

    DoesntWorkWhileDuplicatExists& operator=(const DoesntWorkWhileDuplicatExists&) = delete;
    DoesntWorkWhileDuplicatExists& operator=(DoesntWorkWhileDuplicatExists&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<DoesntWorkWhileDuplicatExists>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<DoesntWorkWhileDuplicatExists>(json); }
};

class DoubleKnockback: public NoFormModifier {
public:
    DoubleKnockback()
        : NoFormModifier("Double Knockback", isAdvantage, Fraction(1, 2)) { }
    DoubleKnockback(QJsonObject json)
        : NoFormModifier(json) { }
    DoubleKnockback(const DoubleKnockback& m)
        : NoFormModifier(m) { }
    DoubleKnockback(DoubleKnockback&& m)
        : NoFormModifier(m) { }
    ~DoubleKnockback() override { }

    DoubleKnockback& operator=(const DoubleKnockback&) = delete;
    DoubleKnockback& operator=(DoubleKnockback&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<DoubleKnockback>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<DoubleKnockback>(json); }
};

class ExpandedEffect: public Modifier {
public:
    ExpandedEffect()
        : Modifier("Expanded Effect▲", isAdvantage, isModifier)
        , v({ "", -1 }) { }
    ExpandedEffect(const ExpandedEffect& m)
        : Modifier(m)
        , v(m.v) { }
    ExpandedEffect(ExpandedEffect&& m)
        : Modifier(m)
        , v(m.v) { }
    ExpandedEffect(QJsonObject json)
        : Modifier(json["name"].toString("Expanded Effect▲"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mEffects = json["effects"].toString("");
                                                  v.mHowMany = json["howMany"].toInt(0);
                                                }
    ~ExpandedEffect() override { }

    ExpandedEffect& operator=(const ExpandedEffect&) = delete;
    ExpandedEffect& operator=(ExpandedEffect&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<ExpandedEffect>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<ExpandedEffect>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { effects = createLineEdit(p, l, "Effects?", std::mem_fn(&ModifierBase::changed));
                                                              howMany = createComboBox(p, l, "How Many?", { "", "Two", "Three", "Four", "Five", "Six", "Seven", "All" },
                                                                                       std::mem_fn(&ModifierBase::index));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              effects->setText(s.mEffects);
                                                              howMany->setCurrentIndex(s.mHowMany);
                                                              v = s;
                                                            }
    void          store() override                          { v.mEffects = effects->text();
                                                              v.mHowMany = howMany->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]    = name();
                                                              obj["type"]    = type();
                                                              obj["adder"]   = isAdder();
                                                              obj["effects"] = v.mEffects;
                                                              obj["howMany"] = v.mHowMany;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        return (v.mHowMany) * Fraction(1, 2);
    }

private:
    struct vars {
        QString mEffects = "";
        int     mHowMany = 0;
    } v;

    QLineEdit* effects = nullptr;
    QComboBox* howMany = nullptr;

    QString optOut(bool show) {
        if (v.mEffects.isEmpty() || v.mHowMany < 1) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Expanded Effect▲: " + v.mEffects;
        return desc;
    }
};

class ExtraTime: public Modifier {
public:
    ExtraTime()
        : Modifier("Extra Time", isLimitation, isModifier)
        , v({ -1, false, false }) { }
    ExtraTime(const ExtraTime& m)
        : Modifier(m)
        , v(m.v) { }
    ExtraTime(ExtraTime&& m)
        : Modifier(m)
        , v(m.v) { }
    ExtraTime(QJsonObject json)
        : Modifier(json["name"].toString("Extra Time"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mTime     = json["time"].toInt(-1);
                                                  v.mLockout  = json["lockout"].toBool(false);
                                                  v.mActivate = json["activate"].toBool(false);
                                                }
    ~ExtraTime() override { }

    ExtraTime& operator=(const ExtraTime&) = delete;
    ExtraTime& operator=(ExtraTime&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<ExtraTime>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<ExtraTime>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { time = createComboBox(p, l, "How Much Extra Time?", { "", "Delayed Phase", "Extra Segment", "Full Phase", "Extra Phase",
                                                                                                                    "Turn", "Minute", "5 Minutes", "20 Minutes", "Hour", "6 Hours",
                                                                                                                    "Day", "Week", "Month", "Season", "Year", "5 Years" },
                                                                                         std::mem_fn(&ModifierBase::index));
                                                              lockout = createCheckBox(p, l, "Cannot Activate Other Powers While Waiting", std::mem_fn(&ModifierBase::checked));
                                                              activate = createCheckBox(p, l, "Only to Activate", std::mem_fn(&ModifierBase::checked));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              time->setCurrentIndex(s.mTime);
                                                              lockout->setChecked(s.mLockout);
                                                              activate->setChecked(s.mActivate);
                                                              v = s;
                                                            }
    void          store() override                          { v.mTime     = time->currentIndex();
                                                              v.mLockout  = lockout->isChecked();
                                                              v.mActivate = activate->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]     = name();
                                                              obj["type"]     = type();
                                                              obj["adder"]    = isAdder();
                                                              obj["time"]     = v.mTime;
                                                              obj["lockout"]  = v.mLockout;
                                                              obj["activate"] = v.mActivate;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v.mTime <= 5) switch (v.mTime) { // NOLINT
        case 2:
        case 3: f = Fraction(1, 2);    break;
        case 4: f = Fraction(3, 4);    break;
        case 5: f = Fraction(1, Fraction(1, 4)); break; // NOLINT
        } else f = Fraction(1) + (v.mTime - 5) * Fraction(1, 2); // NOLINT
        if (v.mLockout) f += Fraction(1, 4);
        if (v.mActivate) f = f / 2;
        return f;
    }

private:
    struct vars {
        int mTime = 0;
        int mLockout = 0;
        int mActivate = 0;
    } v;

    QComboBox* time = nullptr;
    QCheckBox* lockout = nullptr;
    QCheckBox* activate = nullptr;

    QString optOut(bool show) {
        static QStringList extra { "", "Delayed Phase", "Extra Segment", "Full Phase", "Extra Phase",
                                   "Turn", "Minute", "5 Minutes", "20 Minutes", "Hour", "6 Hours",
                                   "Day", "Week", "Month", "Season", "Year", "5 Years" };
        if (v.mTime < 1) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Extra Time: " + extra[v.mTime];
        QString sep = " (";
        if (v.mLockout) { desc += sep + "Cannot Activate Other Powers"; sep = "; "; }
        if (v.mActivate) { desc += sep + "Only to Activate"; sep = "; "; }
        if (sep == "; ") desc += ")";
        return desc;
    }
};

class EyeContactRequired: public Modifier {
public:
    EyeContactRequired()
        : Modifier("Eye Contact Required", isLimitation, isModifier)
        , v({ false }) { }
    EyeContactRequired(const EyeContactRequired& m)
        : Modifier(m)
        , v(m.v) { }
    EyeContactRequired(EyeContactRequired&& m)
        : Modifier(m)
        , v(m.v) { }
    EyeContactRequired(QJsonObject json)
        : Modifier(json["name"].toString("Eye Contact Required"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mThroughout  = json["throughout"].toBool(false);
                                                }
    ~EyeContactRequired() override { }

    EyeContactRequired& operator=(const EyeContactRequired&) = delete;
    EyeContactRequired& operator=(EyeContactRequired&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<EyeContactRequired>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<EyeContactRequired>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { throughout = createCheckBox(p, l, "Must Keep Eye Contact Throuhgout", std::mem_fn(&ModifierBase::checked));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              throughout->setChecked(s.mThroughout);
                                                              v = s;
                                                            }
    void          store() override                          { v.mThroughout = throughout->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]     = name();
                                                              obj["type"]     = type();
                                                              obj["adder"]    = isAdder();
                                                              obj["throughout"] = v.mThroughout;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 2);
        if (v.mThroughout) f = Fraction(1, 1);
        return f;
    }

private:
    struct vars {
        bool mThroughout = false;
    } v;

    QCheckBox* throughout = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Eye Contact Required";
        if (v.mThroughout) desc += "(Throughout)";
        return desc;
    }
};

class Focus: public Modifier {
public:
    Focus()
        : Modifier("Focus", isLimitation, isModifier)
        , v({ "", -1, -1, -1, -1, false }) { }
    Focus(const Focus& m)
        : Modifier(m)
        , v(m.v) { }
    Focus(Focus&& m)
        : Modifier(m)
        , v(m.v) { }
    Focus(QJsonObject json)
        : Modifier(json["name"].toString("Focus"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mWhat          = json["what"].toString();
                                                  v.mType          = json["Type"].toInt(-1);
                                                  v.mMobility      = json["mobility"].toInt(0);
                                                  v.mExpendability = json["expendability"].toInt(0);
                                                  v.mDurability    = json["durability"].toInt(0);
                                                  v.mUniversal     = json["universal"].toBool(false);
                                                }
    ~Focus() override { }

    Focus& operator=(const Focus&) = delete;
    Focus& operator=(Focus&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<Focus>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<Focus>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { what          = createLineEdit(p, l, "What is the fopcus?", std::mem_fn(&ModifierBase::changed));
                                                              ntype         = createComboBox(p, l, "Type Of Focus?", { "Inobvious, Inaccessable", "Inobvious, Accessable",
                                                                                                                       "Obvious, Inaccessable", "Obvious, Accessable" },
                                                                                                                       std::mem_fn(&ModifierBase::index));
                                                              mobility      = createComboBox(p, l, "Mobility?", { "", "Bulky", "Immobile", "Arrangement" },
                                                                                                                  std::mem_fn(&ModifierBase::index));
                                                              expendability = createComboBox(p, l, "Expendability?", { "", "Difficult To Obtain",
                                                                                                                       "Very Difficult To Obtain", "Extremely Difficult To Obtain" },
                                                                                                                       std::mem_fn(&ModifierBase::index));
                                                              durability    = createComboBox(p, l, "Durability?", { "", "Fragile", "Durable", "Unbreakable" },
                                                                                                                    std::mem_fn(&ModifierBase::index));
                                                              universal     = createCheckBox(p, l, "Universal Focus", std::mem_fn(&ModifierBase::checked));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              what->setText(s.mWhat);
                                                              ntype->setCurrentIndex(s.mType);
                                                              mobility->setCurrentIndex(s.mMobility);
                                                              expendability->setCurrentIndex(s.mExpendability);
                                                              durability->setCurrentIndex(s.mDurability);
                                                              universal->setChecked(s.mUniversal);
                                                              v = s;
                                                            }
    void          store() override                          { v.mWhat          = what->text();
                                                              v.mType          = ntype->currentIndex();
                                                              v.mMobility      = mobility->currentIndex();
                                                              v.mExpendability = expendability->currentIndex();
                                                              v.mDurability    = durability->currentIndex();
                                                              v.mUniversal     = universal->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]          = name();
                                                              obj["type"]          = type();
                                                              obj["what"]          = v.mWhat;
                                                              obj["adder"]         = isAdder();
                                                              obj["Type"]          = v.mType;
                                                              obj["mobiliity"]     = v.mMobility;
                                                              obj["expendability"] = v.mExpendability;
                                                              obj["durability"]    = v.mDurability;
                                                              obj["universal"]     = v.mUniversal;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        static QList<Fraction> Type          { { 0, 1 }, { 1, 4 }, { 1, 2 }, { 1, 2 }, { 1, 1 } };
        static QList<Fraction> Mobility      { { 0, 1 }, { 0, 1 }, { 1, 2 }, { 1, 1 }, { 1, 4 } };
        static QList<Fraction> Expendability { { 0, 1 }, { 0, 1 }, { 1, 4 }, { 1, 2 }, { 1, 1 } };
        static QList<Fraction> Durability    { { 0, 1 }, { 0, 1 }, { 1, 4 }, { 0, 1 }, { 0, 1 } };
        try { return Type[v.mType + 1] + Mobility[v.mMobility + 1] + Expendability[v.mExpendability + 1] + Durability[v.mDurability + 1]; }
        catch (...) { return Fraction(0); }
    }

private:
    struct vars {
        QString mWhat;
        int     mType;
        int     mMobility;
        int     mExpendability;
        int     mDurability;
        bool    mUniversal;
    } v {};

    QLineEdit* what = nullptr;
    QComboBox* ntype = nullptr;
    QComboBox* mobility = nullptr;
    QComboBox* expendability = nullptr;
    QComboBox* durability = nullptr;
    QCheckBox* universal = nullptr;

    QString optOut(bool show) {
        if (v.mWhat.isEmpty() || v.mType < 0) return "<incomplete>";
        QStringList Type { "IIF", "IAF", "OIF", "OAF" };
        QStringList Mobiillity { "", "Bulky", "Immobile", "Arrangement" };
        QStringList Expendability { "", "Difficult To Obtain", "Very Difficult To Obtain", "Extremely Difficult To Obtain" };
        QStringList Durability { "", "Fragile", "Durable", "Unbreakable" };
        Fraction f = fraction(Modifier::NoStore);
        try {
            QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + Type[v.mType] + " (" + v.mWhat;
            QString sep = "; ";
            if (v.mMobility > 0) desc += sep + Mobiillity[v.mMobility];
            if (v.mExpendability > 0) desc += sep + Expendability[v.mExpendability];
            if (v.mDurability > 0) desc += sep + Durability[v.mDurability];
            desc += ")";
            return desc;
        } catch (...) { return ""; }
    }
};

class Gestures: public Modifier {
public:
    Gestures()
        : Modifier("Gestures", isLimitation, isModifier)
        , v({ false, false }) { }
    Gestures(const Gestures& m)
        : Modifier(m)
        , v(m.v) { }
    Gestures(Gestures&& m)
        : Modifier(m)
        , v(m.v) { }
    Gestures(QJsonObject json)
        : Modifier(json["name"].toString("Gestures"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mThroughout  = json["throughout"].toBool(false);
                                                  v.mBothHands   = json["bothHands"].toBool(false);
                                                }
    ~Gestures() override { }

    Gestures& operator=(const Gestures&) = delete;
    Gestures& operator=(Gestures&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<Gestures>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<Gestures>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { bothHands  = createCheckBox(p, l, "Both Hands", std::mem_fn(&ModifierBase::checked));
                                                              throughout = createCheckBox(p, l, "Must Gesture Throuhgout", std::mem_fn(&ModifierBase::checked));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              bothHands->setChecked(s.mBothHands);
                                                              throughout->setChecked(s.mThroughout);
                                                              v = s;
                                                            }
    void          store() override                          { v.mThroughout = throughout->isChecked();
                                                              v.mBothHands = bothHands->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]       = name();
                                                              obj["type"]       = type();
                                                              obj["adder"]      = isAdder();
                                                              obj["throughout"] = v.mThroughout;
                                                              obj["bothHands"]  = v.mBothHands;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v.mBothHands) f = Fraction(1, 2);
        if (v.mThroughout) f *= 2;
        return f;
    }

private:
    struct vars {
        bool mBothHands;
        bool mThroughout;
    } v {};

    QCheckBox* bothHands = nullptr;
    QCheckBox* throughout = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Gesstures";
        QString sep = " (";
        if (v.mBothHands) { desc += sep + "Both Hands"; sep = "; "; }
        if (v.mThroughout) { desc += sep + "Throughout"; sep = "; "; }
        if (sep == "; ") desc += ")";
        return desc;
    }
};

class Hardened: public NoFormModifier {
public:
    Hardened()
        : NoFormModifier("Hardened", isAdvantage, Fraction(1, 4)) { }
    Hardened(QJsonObject json)
        : NoFormModifier(json) { }
    Hardened(const Hardened& m)
        : NoFormModifier(m) { }
    Hardened(Hardened&& m)
        : NoFormModifier(m) { }
    ~Hardened() override { }

    Hardened& operator=(const Hardened&) = delete;
    Hardened& operator=(Hardened&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<Hardened>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<Hardened>(json); }
};

class HoleInTheMiddle: public Modifier {
public:
    HoleInTheMiddle()
        : Modifier("Hole In The Middle", isAdvantage, isModifier)
        , v({ false, "" }) { }
    HoleInTheMiddle(const HoleInTheMiddle& m)
        : Modifier(m)
        , v(m.v) { }
    HoleInTheMiddle(HoleInTheMiddle&& m)
        : Modifier(m)
        , v(m.v) { }
    HoleInTheMiddle(QJsonObject json)
        : Modifier(json["name"].toString("Hole In The Middle"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mChangeable = json["changeable"].toBool(false);
                                                  v.mShape      = json["shape"].toString();
                                                }
    ~HoleInTheMiddle() override { }

    HoleInTheMiddle& operator=(const HoleInTheMiddle&) = delete;
    HoleInTheMiddle& operator=(HoleInTheMiddle&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<HoleInTheMiddle>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<HoleInTheMiddle>(json); }

    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { changeable = createCheckBox(p, l, "Shae is changeable", std::mem_fn(&ModifierBase::checked));
                                                              shape      = createLineEdit(p, l, "Hole shape?", std::mem_fn(&ModifierBase::changed));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              changeable->setChecked(s.mChangeable);
                                                              shape->setText(s.mShape);
                                                              v = s;
                                                            }
    void          store() override                          { v.mChangeable = changeable->isChecked();
                                                              v.mShape      = shape->text();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]       = name();
                                                              obj["type"]       = type();
                                                              obj["adder"]      = isAdder();
                                                              obj["changeable"] = v.mChangeable;
                                                              obj["shape"]      = v.mShape;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v.mChangeable) f = Fraction(1, 2);
        return f;
    }

private:
    struct vars {
        bool    mChangeable;
        QString mShape;
    } v {};

    QCheckBox* changeable = nullptr;
    QLineEdit* shape = nullptr;

    QString optOut(bool show) {
        if (!v.mChangeable && v.mShape.isEmpty()) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Hole In The Middle";
        if (v.mChangeable) desc += " (Any Shape)";
        else desc += " (" + v.mShape + ")";
        return desc;
    }

    void checked(bool) override {
        store();
        shape->setEnabled(!v.mChangeable);
        ModifiersDialog::ref().updateForm();
    }
};

class Impenetrable: public NoFormModifier {
public:
    Impenetrable()
        : NoFormModifier("Impenetrable", isAdvantage, Fraction(1, 4)) { }
    Impenetrable(QJsonObject json)
        : NoFormModifier(json) { }
    Impenetrable(const Impenetrable& m)
        : NoFormModifier(m) { }
    Impenetrable(Impenetrable&& m)
        : NoFormModifier(m) { }
    ~Impenetrable() override { }

    Impenetrable& operator=(const Impenetrable&) = delete;
    Impenetrable& operator=(Impenetrable&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<Impenetrable>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<Impenetrable>(json); }
};

class ImprovedNoncombatMovement: public Modifier {
public:
    ImprovedNoncombatMovement()
        : Modifier("Improved Noncombat Movement", isAdvantage, isAnAdder)
        , v({ 0 }) { }
    ImprovedNoncombatMovement(const ImprovedNoncombatMovement& m)
        : Modifier(m)
        , v(m.v) { }
    ImprovedNoncombatMovement(ImprovedNoncombatMovement&& m)
        : Modifier(m)
        , v(m.v) { }
    ImprovedNoncombatMovement(QJsonObject json)
        : Modifier(json["name"].toString("Improved Noncombat Movement"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(true)) { v.mDoubling  = json["doubling"].toInt(0);
                                               }
    ~ImprovedNoncombatMovement() override { }

    ImprovedNoncombatMovement& operator=(const ImprovedNoncombatMovement&) = delete;
    ImprovedNoncombatMovement& operator=(ImprovedNoncombatMovement&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<ImprovedNoncombatMovement>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<ImprovedNoncombatMovement>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { dbling = createLineEdit(p, l, "How many doublings?", std::mem_fn(&ModifierBase::numeric));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              dbling->setText(QString("%1").arg(s.mDoubling));
                                                              v = s;
                                                            }
    void          store() override                          { v.mDoubling = dbling->text().toInt();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]     = name();
                                                              obj["type"]     = type();
                                                              obj["adder"]    = isAdder();
                                                              obj["doubling"] = v.mDoubling;
                                                              return obj;
                                                            }

    Points points(bool noStore = false) override {
        if (!noStore) store();
        return 5_cp * v.mDoubling; // NOLINT
    }

    int doubling() override { return v.mDoubling; }

private:
    struct vars {
        int mDoubling;
    } v {};

    QLineEdit* dbling = nullptr;

    QString optOut(bool show) {
        Points p(points(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((p < 0) ? "" : "+") + QString("%1").arg(p.points) + ") " : "") +
                 "Improved Noncombat Movement (" + QString("x%1").arg(2 * (int) pow(2, v.mDoubling)) + ")";
        return desc;
    }
};

class Inaccurate: public Modifier {
public:
    Inaccurate()
        : Modifier("Inaccurate", isLimitation, isModifier)
        , v({ false }) { }
    Inaccurate(const Inaccurate& m)
        : Modifier(m)
        , v(m.v) { }
    Inaccurate(Inaccurate&& m)
        : Modifier(m)
        , v(m.v) { }
    Inaccurate(QJsonObject json)
        : Modifier(json["name"].toString("Inaccurate"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mZero  = json["zero"].toBool(false);
                                                }
    ~Inaccurate() override { }

    Inaccurate& operator=(const Inaccurate&) = delete;
    Inaccurate& operator=(Inaccurate&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<Inaccurate>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<Inaccurate>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { zero = createCheckBox(p, l, "0 OCV", std::mem_fn(&ModifierBase::checked));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              zero->setChecked(s.mZero);
                                                              v = s;
                                                            }
    void          store() override                          { v.mZero = zero->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["zero"]  = v.mZero;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v.mZero) f = Fraction(1, 2);
        return f;
    }

private:
    struct vars {
        bool mZero;
    } v {};

    QCheckBox* zero = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Inaccurate";
        if (v.mZero) desc += " (0 OCV, Range Mods start at 6m)";
        else { Fraction(1, 2); desc += " (" + f.toString() + " OCV, Range Mods start at 4m)"; }
        return desc;
    }
};

class Incantations: public Modifier {
public:
    Incantations()
        : Modifier("Incantations", isLimitation, isModifier)
        , v({ false }) { }
    Incantations(const Incantations& m)
        : Modifier(m)
        , v(m.v) { }
    Incantations(Incantations&& m)
        : Modifier(m)
        , v(m.v) { }
    Incantations(QJsonObject json)
        : Modifier(json["name"].toString("Incantations"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mThroughout = json["throughout"].toBool(false);
                                                }
    ~Incantations() override { }

    Incantations& operator=(const Incantations&) = delete;
    Incantations& operator=(Incantations&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<Incantations>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<Incantations>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { throughout = createCheckBox(p, l, "Must Incant Throuhgout", std::mem_fn(&ModifierBase::checked));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              throughout->setChecked(s.mThroughout);
                                                              v = s;
                                                            }
    void          store() override                          { v.mThroughout = throughout->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]       = name();
                                                              obj["type"]       = type();
                                                              obj["adder"]      = isAdder();
                                                              obj["throughout"] = v.mThroughout;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v.mThroughout) f *= 2;
        return f;
    }

private:
    struct vars {
        bool mThroughout;
    } v {};

    QCheckBox* throughout = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Incantations";
        QString sep = " (";
        if (v.mThroughout) { desc += sep + "Throughout"; sep = "; "; }
        if (sep == "; ") desc += ")";
        return desc;
    }
};

class IncreasedENDCost: public Modifier {
public:
    IncreasedENDCost()
        : Modifier("Increased END Cost", isLimitation, isModifier)
        , v({ -1, -1, "" }) { }
    IncreasedENDCost(const IncreasedENDCost& m)
        : Modifier(m)
        , v(m.v) { }
    IncreasedENDCost(IncreasedENDCost&& m)
        : Modifier(m)
        , v(m.v) { }
    IncreasedENDCost(QJsonObject json)
        : Modifier(json["name"].toString("Increased END Cost"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mAmount        = json["amount"].toInt(-1);
                                                  v.mCircumstances = json["circumstances"].toInt(0);
                                                  v.mWhat          = json["what"].toString();
                                                }
    ~IncreasedENDCost() override { }

    IncreasedENDCost& operator=(const IncreasedENDCost&) = delete;
    IncreasedENDCost& operator=(IncreasedENDCost&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<IncreasedENDCost>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<IncreasedENDCost>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { amount        = createComboBox(p, l, "Amount of increased END?", { "", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10" },
                                                                                                                                 std::mem_fn(&ModifierBase::index));
                                                              circumstances = createComboBox(p, l, "How common the circumstances?", { "", "Very Common", "Common", "Uncommon" },
                                                                                                                                       std::mem_fn(&ModifierBase::index));
                                                              what          = createLineEdit(p, l, "What circumstances?", std::mem_fn(&ModifierBase::changed));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              amount->setCurrentIndex(s.mAmount);
                                                              circumstances->setCurrentIndex(s.mCircumstances);
                                                              v = s;
                                                            }
    void          store() override                          { v.mAmount        = amount->currentIndex();
                                                              v.mCircumstances = circumstances->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]          = name();
                                                              obj["type"]          = type();
                                                              obj["adder"]         = isAdder();
                                                              obj["amount"]        = v.mAmount;
                                                              obj["circumstances"] = v.mCircumstances;
                                                              obj["what"]          = v.mWhat;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f;
        if (v.mCircumstances < 0) return f;
        if (v.mAmount < 8) f = Fraction(1, 2) * v.mAmount; // NOLINT
        else if (v.mAmount == 8) f = Fraction(3, Fraction(1, 2)); // NOLINT
        else f = Fraction(4);
        f -= v.mCircumstances * Fraction(1, 4);
        if (f < Fraction(0)) f = Fraction(0);
        return f;
    }

    Fraction endChange() override { return Fraction(v.mAmount + 1); }

private:
    struct vars {
        int     mAmount;
        int     mCircumstances;
        QString mWhat;
    } v {};

    QComboBox* amount = nullptr;
    QComboBox* circumstances = nullptr;
    QLineEdit* what = nullptr;

    QString optOut(bool show) {
        if (v.mAmount < 1) return "<incomplete>";
        QStringList Amount { "", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10" };
        QStringList Circumstances { "", "Very Common", "Common", "Uncommon" };
        Fraction f = fraction(Modifier::NoStore);
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "Increased END Cost (" + Amount[v.mAmount];
        if (Circumstances[v.mCircumstances] != "") desc += "; " + Circumstances[v.mCircumstances] + ((v.mCircumstances != 0) ? ": " + v.mWhat : "");
        return desc + ")";
    }

    void index(int) override {
        store();
        QComboBox* box = dynamic_cast<QComboBox*>(sender());
        if (box == circumstances) what->setEnabled(box->currentIndex() != 0);
        ModifiersDialog::ref().updateForm();
    }
};

class IncreasedMass: public Modifier {
public:
    IncreasedMass()
        : Modifier("Increased Mass", isAdvantage, isAnAdder)
        , v({ 0 }) { }
    IncreasedMass(const IncreasedMass& m)
        : Modifier(m)
        , v(m.v) { }
    IncreasedMass(IncreasedMass&& m)
        : Modifier(m)
        , v(m.v) { }
    IncreasedMass(QJsonObject json)
        : Modifier(json["name"].toString("Increased Mass"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mDoubling  = json["doubling"].toInt(0);
                                                }
    ~IncreasedMass() override { }

    IncreasedMass& operator=(const IncreasedMass&) = delete;
    IncreasedMass& operator=(IncreasedMass&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<IncreasedMass>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<IncreasedMass>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { doubling  = createLineEdit(p, l, "How mnay doublings?", std::mem_fn(&ModifierBase::numeric));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              doubling->setText(QString("%1").arg(s.mDoubling));
                                                              v = s;
                                                            }
    void          store() override                          { v.mDoubling = doubling->text().toInt();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]     = name();
                                                              obj["type"]     = type();
                                                              obj["adder"]    = isAdder();
                                                              obj["doubling"] = v.mDoubling;
                                                              return obj;
                                                            }

    Points points(bool noStore = false) override {
        if (!noStore) store();
        return 5_cp * v.mDoubling; // NOLINT
    }

private:
    struct vars {
        int mDoubling;
    } v {};

    QLineEdit* doubling = nullptr;

    QString optOut(bool show) {
        Points p(points(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((p < 0) ? "" : "+") + QString("%1").arg(p.points) + ") " : "") +
                 "Increased Mass (" + QString("x%1, %2 kg").arg((int) pow(2, v.mDoubling)).arg(200 * (int) pow(2, v.mDoubling)) + ")"; // NOLINT
        return desc;
    }
};

class IncreasedMaximumEffect: public Modifier {
public:
    IncreasedMaximumEffect()
        : Modifier("Increased Maximum Effect", isAdvantage, isModifier)
        , v({ 0 }) { }
    IncreasedMaximumEffect(const IncreasedMaximumEffect& m)
        : Modifier(m)
        , v(m.v) { }
    IncreasedMaximumEffect(IncreasedMaximumEffect&& m)
        : Modifier(m)
        , v(m.v) { }
    IncreasedMaximumEffect(QJsonObject json)
        : Modifier(json["name"].toString("Increased Maximum Effect"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mDoubling  = json["doubling"].toInt(0);
                                                }
    ~IncreasedMaximumEffect() override { }

    IncreasedMaximumEffect& operator=(const IncreasedMaximumEffect&) = delete;
    IncreasedMaximumEffect& operator=(IncreasedMaximumEffect&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<IncreasedMaximumEffect>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<IncreasedMaximumEffect>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { doubling  = createLineEdit(p, l, "How mnay doublings?", std::mem_fn(&ModifierBase::numeric));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              doubling->setText(QString("%1").arg(s.mDoubling));
                                                              v = s;
                                                            }
    void          store() override                          { v.mDoubling = doubling->text().toInt();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]     = name();
                                                              obj["type"]     = type();
                                                              obj["adder"]    = isAdder();
                                                              obj["doubling"] = v.mDoubling;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        return Fraction(1, 4) * v.mDoubling;
    }

private:
    struct vars {
        int mDoubling;
    } v {};

    QLineEdit* doubling = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Increased Maximum Effect (" + QString("x%1").arg((int) pow(2, v.mDoubling)) + ")";
        return desc;
    }
};

class IncreasedMaximumRange: public Modifier {
public:
    IncreasedMaximumRange()
        : Modifier("Increased Maximum Range", isAdvantage, isModifier)
        , v({ 0 }) { }
    IncreasedMaximumRange(const IncreasedMaximumRange& m)
        : Modifier(m)
        , v(m.v) { }
    IncreasedMaximumRange(IncreasedMaximumRange&& m)
        : Modifier(m)
        , v(m.v) { }
    IncreasedMaximumRange(QJsonObject json)
        : Modifier(json["name"].toString("Increased Maximum Range"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mDoubling  = json["doubling"].toInt(0);
                                                }
    ~IncreasedMaximumRange() override { }

    IncreasedMaximumRange& operator=(const IncreasedMaximumRange&) = delete;
    IncreasedMaximumRange& operator=(IncreasedMaximumRange&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<IncreasedMaximumRange>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<IncreasedMaximumRange>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { doubling  = createLineEdit(p, l, "How mnay doublings?", std::mem_fn(&ModifierBase::numeric));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              doubling->setText(QString("%1").arg(s.mDoubling));
                                                              v = s;
                                                            }
    void          store() override                          { v.mDoubling = doubling->text().toInt();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]     = name();
                                                              obj["type"]     = type();
                                                              obj["adder"]    = isAdder();
                                                              obj["doubling"] = v.mDoubling;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        return Fraction(1, 4) * v.mDoubling;
    }

private:
    struct vars {
        int mDoubling;
    } v {};

    QLineEdit* doubling = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Increased Maximum Range (" + QString("x%1").arg((int) pow(2, v.mDoubling)) + ")";
        return desc;
    }
};

class Indirect: public Modifier {
public:
    Indirect()
        : Modifier("Indirect", isLimitation, isModifier)
        , v({ -1, "", -1 }) { }
    Indirect(const Indirect& m)
        : Modifier(m)
        , v(m.v) { }
    Indirect(Indirect&& m)
        : Modifier(m)
        , v(m.v) { }
    Indirect(QJsonObject json)
        : Modifier(json["name"].toString("Indirect"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mPowerSource  = json["location"].toInt(0);
                                                  v.mLocAndDir = json["locAndDir"].toString();
                                                  v.mDirection = json["mobility"].toInt(0);
                                                }
    ~Indirect() override { }

    Indirect& operator=(const Indirect&) = delete;
    Indirect& operator=(Indirect&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<Indirect>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<Indirect>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { location  = createComboBox(p, l, "Power Source?", { "Always The Character",
                                                                                                                  "Always the Same",
                                                                                                                  "Variable" }, std::mem_fn(&ModifierBase::index));
                                                              locAndDir = createLineEdit(p, l, "Location and Path?", std::mem_fn(&ModifierBase::changed));
                                                              direction = createComboBox(p, l, "Path the Power Follows?", { "Directly from Source to Target",
                                                                                                                            "Always the Same",
                                                                                                                            "Variable" }, std::mem_fn(&ModifierBase::index));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              location->setCurrentIndex(s.mPowerSource);
                                                              locAndDir->setText(s.mLocAndDir);
                                                              direction->setCurrentIndex(s.mDirection);
                                                              v = s;
                                                            }
    void          store() override                          { v.mPowerSource  = location->currentIndex();
                                                              v.mLocAndDir = locAndDir->text();
                                                              v.mDirection = direction->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]      = name();
                                                              obj["type"]      = type();
                                                              obj["adder"]     = isAdder();
                                                              obj["location"]  = v.mPowerSource;
                                                              obj["locAndDir"] = v.mLocAndDir;
                                                              obj["direction"] = v.mDirection;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f;
        if (v.mPowerSource < 0 || v.mDirection < 0) return f;
        f = Fraction(1, 4) * (v.mPowerSource + v.mDirection);
        return f;
    }

private:
    struct vars {
        int     mPowerSource;
        QString mLocAndDir;
        int     mDirection;
    } v {};

    QComboBox* location = nullptr;
    QLineEdit* locAndDir = nullptr;
    QComboBox* direction = nullptr;

    QString optOut(bool show) {
        if (v.mPowerSource < 0 || v.mDirection < 0 || ((v.mPowerSource == 2 || v.mDirection == 2) && v.mLocAndDir.isEmpty())) return "<incomplete>";
        QStringList Location { "Always The Character", "Always the Same", "Variable" };
        QStringList Direction { "Directly from Source to Target", "Always the Same", "Variable" };
        Fraction f = fraction(Modifier::NoStore);
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "Indirect (" + Location[v.mPowerSource];
        if (v.mPowerSource == 1 && v.mDirection != 2) desc += ", " + v.mLocAndDir;
        desc += "; " + Direction[v.mDirection];
        if (v.mPowerSource == 2 || v.mDirection == 2) desc += ", " + v.mLocAndDir;
        return desc + ")";
    }

    void index(int) override {
        store();
        if (location->currentIndex() >= 1 || direction->currentIndex() >= 1) locAndDir->setEnabled(true);
        else locAndDir->setEnabled(false);
        ModifiersDialog::ref().updateForm();
    }
};

class Inherent: public NoFormModifier {
public:
    Inherent()
        : NoFormModifier("Inherent", isAdvantage, Fraction(1, 4)) { }
    Inherent(QJsonObject json)
        : NoFormModifier(json) { }
    Inherent(const Inherent& m)
        : NoFormModifier(m) { }
    Inherent(Inherent&& m)
        : NoFormModifier(m) { }
    ~Inherent() override { }

    Inherent& operator=(const Inherent&) = delete;
    Inherent& operator=(Inherent&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<Inherent>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<Inherent>(json); }
};

class Instant: public NoFormModifier {
public:
    Instant()
        : NoFormModifier("Instant", isLimitation, Fraction(1, 2)) { }
    Instant(QJsonObject json)
        : NoFormModifier(json) { }
    Instant(const Instant& m)
        : NoFormModifier(m) { }
    Instant(Instant&& m)
        : NoFormModifier(m) { }
    ~Instant() override { }

    Instant& operator=(const Instant&) = delete;
    Instant& operator=(Instant&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<Instant>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<Instant>(json); }
};

class InvisiblePowerEffects: public Modifier {
public:
    InvisiblePowerEffects()
        : Modifier("Invisible Power Effects", isAdvantage, isModifier)
        , v({ false, -1, "", -1 }) { }
    InvisiblePowerEffects(const InvisiblePowerEffects& m)
        : Modifier(m)
        , v(m.v) { }
    InvisiblePowerEffects(InvisiblePowerEffects&& m)
        : Modifier(m)
        , v(m.v) { }
    InvisiblePowerEffects(QJsonObject json)
        : Modifier(json["name"].toString("Invisible Power Effects"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mInobvious = json["inobvious"].toBool(false);
                                                  v.mHow       = json["how"].toInt(0);
                                                  v.mSense     = json["sense"].toString();
                                                  v.mEffect    = json["Effect"].toInt(0);
                                                }
    ~InvisiblePowerEffects() override { }

    InvisiblePowerEffects& operator=(const InvisiblePowerEffects&) = delete;
    InvisiblePowerEffects& operator=(InvisiblePowerEffects&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<InvisiblePowerEffects>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<InvisiblePowerEffects>(json); }

    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { inobvious = createCheckBox(p, l, "Power is Inobvious", std::mem_fn(&ModifierBase::checked));
                                                              how       = createComboBox(p, l, "How Invisible?", { "",
                                                                                                                   "Inobvious to One Sense Group",
                                                                                                                   "Inobvious to Two Sense Groups",
                                                                                                                   "Imperceptible to Onse Sense Group",
                                                                                                                   "Fully Invisible" }, std::mem_fn(&ModifierBase::index));
                                                              sense     = createLineEdit(p, l, "Sense(s)?", std::mem_fn(&ModifierBase::changed));
                                                              effect    = createComboBox(p, l, "Power Effect?", {  "",
                                                                                                                   "Inobvious to other characters but not to target",
                                                                                                                   "Invisible to other characters but not to target",
                                                                                                                   "Inobvious to target but not to other characters"
                                                                                                                   "Invisible to target but not to other characters" },
                                                                                                                   std::mem_fn(&ModifierBase::index));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              inobvious->setChecked(s.mInobvious);
                                                              how->setCurrentIndex(s.mHow);
                                                              sense->setText(s.mSense);
                                                              effect->setCurrentIndex(s.mEffect);
                                                              v = s;
                                                            }
    void          store() override                          { v.mInobvious = inobvious->isChecked();
                                                              v.mHow       = how->currentIndex();
                                                              v.mSense     = sense->text();
                                                              v.mEffect    = effect->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]      = name();
                                                              obj["type"]      = type();
                                                              obj["adder"]     = isAdder();
                                                              obj["inobvious"] = v.mInobvious;
                                                              obj["how"]       = v.mHow;
                                                              obj["sense"]     = v.mSense;
                                                              obj["effect"]    = v.mEffect;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(0, 1);
        if (v.mEffect > 0) f = (v.mEffect % 2 != 0) ? Fraction(1, 4) : Fraction(1, 2);
        if (v.mInobvious) f += Fraction(1, 4) * (v.mHow);
        else if (v.mHow > 2) f += Fraction(1, 2) * (v.mHow - 3);
        else f += Fraction(1, 4) * v.mHow;
        return f;
    }

private:
    struct vars {
        bool    mInobvious;
        int     mHow;
        QString mSense;
        int     mEffect;
    } v {};

    QCheckBox* inobvious = nullptr;
    QComboBox* how = nullptr;
    QLineEdit* sense = nullptr;
    QComboBox* effect = nullptr;

    QString optOut(bool show) {
        if (v.mHow < 1 || v.mEffect < 1) return "<incomplete>";
        if (v.mInobvious && v.mHow != 1 && v.mSense.isEmpty()) return "<incomplete>";
        if (!v.mInobvious && v.mHow != 3 && v.mSense.isEmpty()) return "<incomplete>";
        static QStringList How { "",
                                 "Inobvious to One Sense Group",
                                 "Inobvious to Two Sense Groups",
                                 "Imperceptible to Onse Sense Group",
                                 "Fully Invisible" };
        static QStringList Effect {  "",
                                     "Inobvious to other characters but not to target",
                                     "Invisible to other characters but not to target",
                                     "Inobvious to target but not to other characters"
                                     "Invisible to target but not to other characters" };
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Invisible Power Effects (";
        if (v.mInobvious) desc += How[v.mHow + 2];
        else desc += How[v.mHow];
        desc += (v.mSense.isEmpty() ? "" : ": " + v.mSense);
        if (v.mEffect > 0) desc += "; " + Effect[v.mEffect];
        return desc + ")";
    }

    void checked(bool) override {
        store();
        QStringList data { "",
                           "Inobvious to One Sense Group",
                           "Inobvious to Two Sense Groups",
                           "Imperceptible to Onse Sense Group",
                           "Fully Invisible" };
        int start = 0;
        if (v.mInobvious) start = 2;
        v.mHow = -1;
        how->setCurrentIndex(-1);
        how->clear();
        for (int i = start; i < data.count(); ++i) how->addItem(data[i]);
        ModifiersDialog::ref().updateForm();
    }

    void index(int) override {
        store();
        sense->setEnabled((v.mInobvious && v.mHow != 1) || (!v.mInobvious && v.mHow != 3));
        ModifiersDialog::ref().updateForm();
    }
};

class LimitedEffect: public Modifier {
public:
    LimitedEffect()
        : Modifier("Limited Effect", isLimitation, isModifier)
        , v({ "" }) { }
    LimitedEffect(const LimitedEffect& m)
        : Modifier(m)
        , v(m.v) { }
    LimitedEffect(LimitedEffect&& m)
        : Modifier(m)
        , v(m.v) { }
    LimitedEffect(QJsonObject json)
        : Modifier(json["name"].toString("Limited Effect"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mSense = json["sense"].toString();
                                                }
    ~LimitedEffect() override { }

    LimitedEffect& operator=(const LimitedEffect&) = delete;
    LimitedEffect& operator=(LimitedEffect&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<LimitedEffect>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<LimitedEffect>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { sense = createLineEdit(p, l, "Sense(s)?", std::mem_fn(&ModifierBase::changed));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              sense->setText(s.mSense);
                                                              v = s;
                                                            }
    void          store() override                          { v.mSense     = sense->text();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["sense"] = v.mSense;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        return Fraction(1, 4);
    }

private:
    struct vars {
        QString mSense;
    } v {};

    QLineEdit* sense = nullptr;

    QString optOut(bool show) {
        if (v.mSense.isEmpty()) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Limited Effect (" + v.mSense + ")";
        return desc;
    }
};

class LimitedManeuverability: public NoFormModifier {
public:
    LimitedManeuverability()
        : NoFormModifier("Limited Maneuverability", isLimitation, Fraction(1, 4)) { }
    LimitedManeuverability(QJsonObject json)
        : NoFormModifier(json) { }
    LimitedManeuverability(const LimitedManeuverability& m)
        : NoFormModifier(m) { }
    LimitedManeuverability(LimitedManeuverability&& m)
        : NoFormModifier(m) { }
    ~LimitedManeuverability() override { }

    LimitedManeuverability& operator=(const LimitedManeuverability&) = delete;
    LimitedManeuverability& operator=(LimitedManeuverability&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<LimitedManeuverability>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<LimitedManeuverability>(json); }
};

class LimitedPower: public Modifier {
public:
    LimitedPower()
        : Modifier("Limited Power", isLimitation, isModifier)
        , v({ false, -1, "" }) { }
    LimitedPower(const LimitedPower& m)
        : Modifier(m)
        , v(m.v) { }
    LimitedPower(LimitedPower&& m)
        : Modifier(m)
        , v(m.v) { }
    LimitedPower(QJsonObject json)
        : Modifier(json["name"].toString("Limited Power"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mConditional = json["conditional"].toBool(false);
                                                  v.mHow         = json["how"].toInt(0);
                                                  v.mWhat        = json["what"].toString();
                                                }
    ~LimitedPower() override { }

    LimitedPower& operator=(const LimitedPower&) = delete;
    LimitedPower& operator=(LimitedPower&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<LimitedPower>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<LimitedPower>(json); }

    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { conditional = createCheckBox(p, l, "Power is Conditional", std::mem_fn(&ModifierBase::checked));
                                                              how         = createComboBox(p, l, "How Limited?", { "", "Power loses less than a fourth of its overall effectiveness",
                                                                                                                   "Power loses about a fourth of its overall effectiveness",
                                                                                                                   "Power loses about a third of its overall effectiveness",
                                                                                                                   "Power loses about half of its overall effectiveness",
                                                                                                                   "Power loses about two-thirds of its overall effectiveness",
                                                                                                                   "Power loses almost all of its overall effectiveness" },
                                                                                                                   std::mem_fn(&ModifierBase::index));
                                                              what        = createLineEdit(p, l, "What?", std::mem_fn(&ModifierBase::changed));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              conditional->setChecked(s.mConditional);
                                                              how->setCurrentIndex(s.mHow);
                                                              what->setText(s.mWhat);
                                                              v = s;
                                                            }
    void          store() override                          { v.mConditional = conditional->isChecked();
                                                              v.mHow       = how->currentIndex();
                                                              v.mWhat     = what->text();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]        = name();
                                                              obj["type"]        = type();
                                                              obj["adder"]       = isAdder();
                                                              obj["conditional"] = v.mConditional;
                                                              obj["how"]         = v.mHow;
                                                              obj["what"]        = v.mWhat;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        switch (v.mHow + (v.mConditional ? 1 : 0)) {
        case 1: return Fraction(0, 1);
        case 2: return Fraction(1, 4);
        case 3: return Fraction(1, 2);
        case 4: return Fraction(1, 1);
        case 5: return Fraction(1, Fraction(1, 2)); // NOLINT
        case 6: return Fraction(2, 0); // NOLINT
        }
        Fraction f(0, 1);
        return f;
    }

private:
    struct vars {
        bool    mConditional;
        int     mHow;
        QString mWhat;
    } v {};

    QCheckBox* conditional = nullptr;
    QComboBox* how = nullptr;
    QLineEdit* what = nullptr;

    QString optOut(bool show) {
        if (v.mHow < 1 || v.mWhat.isEmpty()) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "-") + f.toString() + ") " : "") +
                 "Limited Power (";
        desc += v.mWhat;
        return desc + ")";
    }

    void checked(bool) override {
        store();
        QStringList limited { "", "Power loses less than a fourth of its overall effectiveness",
                              "Power loses about a fourth of its overall effectiveness",
                              "Power loses about a third of its overall effectiveness",
                              "Power loses about half of its overall effectiveness",
                              "Power loses about two-thirds of its overall effectiveness",
                              "Power loses almost all of its overall effectiveness" };
        QStringList conditional { "", "Very Common", "Common", "Uncommon", "Very Uncommon" };
        v.mHow = -1;
        how->setCurrentIndex(-1);
        how->clear();
        how->setCurrentText(v.mConditional ? "What Condition?" : "How Limited?");
        for (int i = 0; i < (v.mConditional ? conditional.count() : limited.count()); ++i) how->addItem(v.mConditional ? conditional[i] : limited[i]);
        ModifiersDialog::ref().updateForm();
    }
};

class LimitedRange: public Modifier {
public:
    LimitedRange()
        : Modifier("Limited Range", isBoth, isModifier)
        , v({ false }) { }
    LimitedRange(const LimitedRange& m)
        : Modifier(m)
        , v(m.v) { }
    LimitedRange(LimitedRange&& m)
        : Modifier(m)
        , v(m.v) { }
    LimitedRange(QJsonObject json)
        : Modifier(json["name"].toString("Limited Range"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mNoRange = json["noRange"].toBool(false);
                                                }
    ~LimitedRange() override { }

    LimitedRange& operator=(const LimitedRange&) = delete;
    LimitedRange& operator=(LimitedRange&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<LimitedRange>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<LimitedRange>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { noRange = createCheckBox(p, l, "Power Has No Range", std::mem_fn(&ModifierBase::checked));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              noRange->setChecked(s.mNoRange);
                                                              v = s;
                                                            }
    void          store() override                          { v.mNoRange = noRange->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]    = name();
                                                              obj["type"]    = type();
                                                              obj["adder"]   = isAdder();
                                                              obj["noRange"] = v.mNoRange;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v.mNoRange) f = Fraction(1, 4);
        return f;
    }

private:
    struct vars {
        bool mNoRange;
    } v {};

    QCheckBox* noRange = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Limited Range";
        return desc;
    }
};

class LimitedSpecialEffect: public Modifier {
public:
    LimitedSpecialEffect()
        : Modifier("Limited Special Effect", isLimitation, isModifier)
        , v({ -1, "" }) { }
    LimitedSpecialEffect(const LimitedSpecialEffect& m)
        : Modifier(m)
        , v(m.v) { }
    LimitedSpecialEffect(LimitedSpecialEffect&& m)
        : Modifier(m)
        , v(m.v) { }
    LimitedSpecialEffect(QJsonObject json)
        : Modifier(json["name"].toString("Limited Special Effect"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mHow         = json["how"].toInt(0);
                                                  v.mWhat        = json["what"].toString();
                                                }
    ~LimitedSpecialEffect() override { }

    LimitedSpecialEffect& operator=(const LimitedSpecialEffect&) = delete;
    LimitedSpecialEffect& operator=(LimitedSpecialEffect&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<LimitedSpecialEffect>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<LimitedSpecialEffect>(json); }

    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { how  = createComboBox(p, l, "How Common?", { "", "Very Common", "Common", "Uncommon" }, std::mem_fn(&ModifierBase::index));
                                                              what = createLineEdit(p, l, "What?", std::mem_fn(&ModifierBase::changed));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              how->setCurrentIndex(s.mHow);
                                                              what->setText(s.mWhat);
                                                              v = s;
                                                            }
    void          store() override                          { v.mHow       = how->currentIndex();
                                                              v.mWhat     = what->text();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]        = name();
                                                              obj["type"]        = type();
                                                              obj["adder"]       = isAdder();
                                                              obj["how"]         = v.mHow;
                                                              obj["what"]        = v.mWhat;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        switch (v.mHow) {
        case 1: return Fraction(1, 4);
        case 2: return Fraction(1, 2);
        case 3: return Fraction(1, 1);
        }
        Fraction f(0, 1);
        return f;
    }

private:
    struct vars {
        int     mHow;
        QString mWhat;
    } v {};

    QComboBox* how = nullptr;
    QLineEdit* what = nullptr;

    QString optOut(bool show) {
        if (v.mHow < 1 || v.mWhat.isEmpty()) return "<incomplete>";
        static QStringList Limited { "", "Very Common", "Common", "Uncommon" };
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Limited Special Effect (" + Limited[v.mHow] + ": " + v.mWhat + ")";
        return desc;
    }

};

class LineOfSight: public NoFormModifier {
public:
    LineOfSight()
        : NoFormModifier("Line Of Sight▲", isAdvantage, Fraction(1, 2)) { }
    LineOfSight(QJsonObject json)
        : NoFormModifier(json) { }
    LineOfSight(const LineOfSight& m)
        : NoFormModifier(m) { }
    LineOfSight(LineOfSight&& m)
        : NoFormModifier(m) { }
    ~LineOfSight() override { }

    LineOfSight& operator=(const LineOfSight&) = delete;
    LineOfSight& operator=(LineOfSight&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<LineOfSight>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<LineOfSight>(json); }
};

class Linked: public Modifier {
public:
    Linked()
        : Modifier("Linked", isBoth, isModifier)
        , v({ false, false, false, false, false, false, "" }) { }
    Linked(const Linked& m)
        : Modifier(m)
        , v(m.v) { }
    Linked(Linked&& m)
        : Modifier(m)
        , v(m.v) { }
    Linked(QJsonObject json)
        : Modifier(json["name"].toString("Linked"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mGreater  = json["greater"].toBool(false);
                                                  v.mBoth     = json["both"].toBool(false);
                                                  v.mProp     = json["prop"].toBool(false);
                                                  v.mFull     = json["full"].toBool(false);
                                                  v.mConstant = json["constant"].toBool(false);
                                                  v.mInstant  = json["instant"].toBool(false);
                                                  v.mTarget   = json["target"].toString();
                                                }
    ~Linked() override { }

    Linked& operator=(const Linked&) = delete;
    Linked& operator=(Linked&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<Linked>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<Linked>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { greater  = createCheckBox(p, l, "Greater Power Linked to Lesser▲", std::mem_fn(&ModifierBase::checked));
                                                              both     = createCheckBox(p, l, "Must Use Together and Lesser is Inconvenient", std::mem_fn(&ModifierBase::checked));
                                                              prop     = createCheckBox(p, l, "Need Not Use Powers Proportionally", std::mem_fn(&ModifierBase::checked));
                                                              full     = createCheckBox(p, l, "Greater must be at Full Power to use Lesser", std::mem_fn(&ModifierBase::checked));
                                                              constant = createCheckBox(p, l, "Greater Power is Constant", std::mem_fn(&ModifierBase::checked));
                                                              instant  = createCheckBox(p, l, "Instant Power can be Used any Time with Constant", std::mem_fn(&ModifierBase::checked));
                                                              target   = createLineEdit(p, l, "Linked to?", std::mem_fn(&ModifierBase::changed));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              greater->setChecked(s.mGreater);
                                                              both->setChecked(s.mBoth);
                                                              prop->setChecked(s.mProp);
                                                              full->setChecked(s.mFull);
                                                              constant->setChecked(s.mConstant);
                                                              instant->setChecked(s.mInstant);
                                                              target->setText(s.mTarget);
                                                              v = s;
                                                            }
    void          store() override                          { v.mGreater  = greater->isChecked();
                                                              v.mBoth     = both->isChecked();
                                                              v.mProp     = prop->isChecked();
                                                              v.mFull     = full->isChecked();
                                                              v.mConstant = constant->isChecked();
                                                              v.mInstant  = instant->isChecked();
                                                              v.mTarget   = target->text();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]     = name();
                                                              obj["type"]     = type();
                                                              obj["adder"]    = isAdder();
                                                              obj["greater"]  = v.mGreater;
                                                              obj["both"]     = v.mBoth;
                                                              obj["prop"]     = v.mProp;
                                                              obj["full"]     = v.mFull;
                                                              obj["constant"] = v.mConstant;
                                                              obj["instant"]  = v.mInstant;
                                                              obj["target"]   = v.mTarget;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(-1, 2);
        if (v.mGreater) f = Fraction(-1, 4);
        if (v.mBoth) f -= Fraction(1, 4);
        if (v.mProp) f += Fraction(1, 4);
        if (v.mFull) f -= Fraction(1, 4);
        if (v.mConstant) f += Fraction(1, 4);
        if (v.mInstant) f += Fraction(1, 4);
        return f;
    }

private:
    struct vars {
        bool    mGreater;
        bool    mBoth;
        bool    mProp;
        bool    mFull;
        bool    mConstant;
        bool    mInstant;
        QString mTarget;
    } v {};

    QCheckBox* greater = nullptr;
    QCheckBox* both = nullptr;
    QCheckBox* prop = nullptr;
    QCheckBox* full = nullptr;
    QCheckBox* constant = nullptr;
    QCheckBox* instant = nullptr;
    QLineEdit* target = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Linked to " + v.mTarget;
        QString sep = " (";
        if (v.mGreater) { desc += sep + "Greater Power Linked to Lesser▲"; sep = "; "; }
        if (v.mBoth) { desc += sep + "Must Use Together and Lesser is Inconvenient"; sep = "; "; }
        if (v.mProp) { desc += sep + "Need Not Use Powers Proportionally"; sep = "; "; }
        if (v.mFull) { desc += sep + "Greater must be at Full Power to use Lesser"; sep = "; "; }
        if (v.mConstant) { desc += sep + "Greater Power is Constant"; sep = "; "; }
        if (v.mInstant) { desc += sep + "Instant Power can be Used any Time with Constant"; sep = ": "; }
        if (sep == "; ") desc += ")";
        return desc;
    }
};

class Lockout: public NoFormModifier {
public:
    Lockout()
        : NoFormModifier("Lockout", isLimitation, Fraction(1, 2)) { }
    Lockout(QJsonObject json)
        : NoFormModifier(json) { }
    Lockout(const Lockout& m)
        : NoFormModifier(m) { }
    Lockout(Lockout&& m)
        : NoFormModifier(m) { }
    ~Lockout() override { }

    Lockout& operator=(const Lockout&) = delete;
    Lockout& operator=(Lockout&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<Lockout>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<Lockout>(json); }
};

class MandatoryEffect: public Modifier {
public:
    MandatoryEffect()
        : Modifier("Mandatory Effect", isLimitation, isModifier)
        , v({ -1, "" }) { }
    MandatoryEffect(const MandatoryEffect& m)
        : Modifier(m)
        , v(m.v) { }
    MandatoryEffect(MandatoryEffect&& m)
        : Modifier(m)
        , v(m.v) { }
    MandatoryEffect(QJsonObject json)
        : Modifier(json["name"].toString("Mandatory Effect"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mEffect = json["effect"].toInt(0);
                                                  v.mOther  = json["other"].toString();
                                                }
    ~MandatoryEffect() override { }

    MandatoryEffect& operator=(const MandatoryEffect&) = delete;
    MandatoryEffect& operator=(MandatoryEffect&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<MandatoryEffect>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<MandatoryEffect>(json); }

    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { effect = createComboBox(p, l, "What Effect?", { "", "EGO+10", "EGO+20", "EGO+30" }, std::mem_fn(&ModifierBase::index));
                                                              other  = createLineEdit(p, l, "Other Effects?", std::mem_fn(&ModifierBase::changed));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              effect->setCurrentIndex(s.mEffect);
                                                              other->setText(s.mOther);
                                                              v = s;
                                                            }
    void          store() override                          { v.mEffect = effect->currentIndex();
                                                              v.mOther  = other->text();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]   = name();
                                                              obj["type"]   = type();
                                                              obj["adder"]  = isAdder();
                                                              obj["effect"] = v.mEffect;
                                                              obj["other"]  = v.mOther;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f;
        if (!v.mOther.isEmpty()) f = Fraction(1, 4);
        switch (v.mEffect) {
        case 1: return f + Fraction(1, 4);
        case 2: return f + Fraction(1, 2);
        case 3: return f + Fraction(3, 4);
        }
        return Fraction(0, 1);
    }

private:
    struct vars {
        int     mEffect;
        QString mOther;
    } v {};

    QComboBox* effect = nullptr;
    QLineEdit* other = nullptr;

    QString optOut(bool show) {
        if (v.mEffect < 1) return "<incomplete>";
        static QStringList Limited { "", "EGO+10", "EGO+20", "EGO+30" };
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Mandatory Effect (" + Limited[v.mEffect];
        if (!v.mOther.isEmpty()) desc += " and " + v.mOther;
        return desc + ")";
    }
};

class Mass: public Modifier {
public:
    Mass()
        : Modifier("MassꚚ", isLimitation, isModifier)
        , v({ -1 }) { }
    Mass(const Mass& m)
        : Modifier(m)
        , v(m.v) { }
    Mass(Mass&& m)
        : Modifier(m)
        , v(m.v) { }
    Mass(QJsonObject json)
        : Modifier(json["name"].toString("MassꚚ"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mMass = json["mass"].toInt(0);
                                                }
    ~Mass() override { }

    Mass& operator=(const Mass&) = delete;
    Mass& operator=(Mass&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<Mass>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<Mass>(json); }

    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { mass = createComboBox(p, l, "How much?", { "", Fraction(1, 2).toString() + " Mass",
                                                                                                         "Normal Mass", "2x Mass" }, std::mem_fn(&ModifierBase::index));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              mass->setCurrentIndex(s.mMass);
                                                              v = s;
                                                            }
    void          store() override                          { v.mMass = mass->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["mass"]  = v.mMass;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        switch (v.mMass) {
        case 1: return Fraction(1, 2);
        case 2: return Fraction(1, 1);
        case 3: return Fraction(1, Fraction(1, 2));
        }
        return Fraction(0, 1);
    }

private:
    struct vars {
        int mMass;
    } v {};

    QComboBox* mass = nullptr;

    QString optOut(bool show) {
        if (v.mMass < 1) return "<incomplete>";
        static QStringList Limited { "", Fraction(1, 2).toString() + " Mass", "Normal Mass", "2x Mass" };
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "MassꚚ (" + Limited[v.mMass] + ")";
        return desc;
    }
};

class Megascale: public Modifier {
public:
    Megascale()
        : Modifier("Megascaleϴ", isAdvantage, isModifier)
        , v({ -1, false }) { }
    Megascale(const Megascale& m)
        : Modifier(m)
        , v(m.v) { }
    Megascale(Megascale&& m)
        : Modifier(m)
        , v(m.v) { }
    Megascale(QJsonObject json)
        : Modifier(json["name"].toString("Megascaleϴ"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mScale     = json["scale"].toInt(0);
                                                  v.mInvariant = json["invariant"].toBool(false);
                                                }
    ~Megascale() override { }

    Megascale& operator=(const Megascale&) = delete;
    Megascale& operator=(Megascale&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<Megascale>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<Megascale>(json); }

    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { scale = createComboBox(p, l, "How much?", { "", "1 km", "10 km", "100 km", "1,000 km", "10,000 km", "100,000 km",
                                                                                                          "1 million km", "100 million km", "1 billion km", "10 billion km",
                                                                                                          "100 billion km", "1 trillion km", "1 light year", "10 light years",
                                                                                                          "100,000 light-years", "100 billion light-years" },
                                                                                                          std::mem_fn(&ModifierBase::index));
                                                              invariant = createCheckBox(p, l, "Can't change scale");
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              scale->setCurrentIndex(s.mScale);
                                                              invariant->setChecked(s.mInvariant);
                                                              v = s;
                                                            }
    void          store() override                          { v.mScale     = scale->currentIndex();
                                                              v.mInvariant = invariant->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]      = name();
                                                              obj["type"]      = type();
                                                              obj["adder"]     = isAdder();
                                                              obj["scale"]     = v.mScale;
                                                              obj["invariant"] = v.mInvariant;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        static QList<Fraction> cost { Fraction(0, 1),
           Fraction(1), Fraction(1, Fraction(1, 4)), Fraction(1, Fraction(1, 2)), Fraction(1, Fraction(3, 4)), Fraction(2),
           Fraction(2, Fraction(1, 4)), Fraction(2, Fraction(1, 2)), Fraction(3), Fraction(3, Fraction(1, 4)), Fraction(3, Fraction(1, 2)),
           Fraction(3, Fraction(3, 4)), Fraction(4), Fraction(4, Fraction(1, 4)), Fraction(4, Fraction(1, 2)), Fraction(5, Fraction(1, 2)), // NOLINT
           Fraction(7) // NOLINT
        };
        if (v.mScale < 1) return Fraction(0);
        return cost[v.mScale] + (v.mInvariant ? Fraction(1, 4) : Fraction(0));
    }

private:
    struct vars {
        int  mScale;
        bool mInvariant;
    } v {};

    QComboBox* scale = nullptr;
    QCheckBox* invariant = nullptr;

    QString optOut(bool show) {
        if (v.mScale == -1) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QStringList scale { "", "1 km", "10 km", "100 km", "1,000 km", "10,000 km", "100,000 km",
                            "1 million km", "100 million km", "1 billion km", "10 billion km",
                            "100 billion km", "1 trillion km", "1 light year", "10 light years",
                            "100,000 light-years", "100 billion light-years" };
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Megascaleϴ (" + scale[v.mScale];
        if (v.mInvariant) desc += "; Can't change scale";
        return desc + ")";
    }
};

class MentalDefenseAddsToEGO: public NoFormModifier {
public:
    MentalDefenseAddsToEGO()
        : NoFormModifier("Mental Defense Adds To EGO", isLimitation, Fraction(1, 2)) { }
    MentalDefenseAddsToEGO(QJsonObject json)
        : NoFormModifier(json) { }
    MentalDefenseAddsToEGO(const MentalDefenseAddsToEGO& m)
        : NoFormModifier(m) { }
    MentalDefenseAddsToEGO(MentalDefenseAddsToEGO&& m)
        : NoFormModifier(m) { }
    ~MentalDefenseAddsToEGO() override { }

    MentalDefenseAddsToEGO& operator=(const MentalDefenseAddsToEGO&) = delete;
    MentalDefenseAddsToEGO& operator=(MentalDefenseAddsToEGO&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<MentalDefenseAddsToEGO>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<MentalDefenseAddsToEGO>(json); }
};

class NoConciousControl: public Modifier {
public:
    NoConciousControl()
        : Modifier("No Concious Control▲", isLimitation, isModifier)
        , v({ false, false }) { }
    NoConciousControl(const NoConciousControl& m)
        : Modifier(m)
        , v(m.v) { }
    NoConciousControl(NoConciousControl&& m)
        : Modifier(m)
        , v(m.v) { }
    NoConciousControl(QJsonObject json)
        : Modifier(json["name"].toString("No Concious Control▲"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mActivation = json["activation"].toBool(false);
                                                  v.mEffects    = json["effects"].toBool(false);
                                                }
    ~NoConciousControl() override { }

    NoConciousControl& operator=(const NoConciousControl&) = delete;
    NoConciousControl& operator=(NoConciousControl&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<NoConciousControl>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<NoConciousControl>(json); }

    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { activation = createCheckBox(p, l, "Acvtivation", std::mem_fn(&ModifierBase::checked));
                                                              effects    = createCheckBox(p, l, "Effects", std::mem_fn(&ModifierBase::checked));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              activation->setChecked(s.mActivation);
                                                              effects->setChecked(s.mEffects);
                                                              v = s;
                                                            }
    void          store() override                          { v.mActivation = activation->isChecked();
                                                              v.mEffects    = effects->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]       = name();
                                                              obj["type"]       = type();
                                                              obj["adder"]      = isAdder();
                                                              obj["activation"] = v.mActivation;
                                                              obj["effects"]    = v.mEffects;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f;
        if (v.mActivation) f += Fraction(1);
        if (v.mEffects) f += Fraction(1);
        return f;
    }

private:
    struct vars {
        bool mActivation;
        bool mEffects;
    } v {};

    QCheckBox* activation = nullptr;
    QCheckBox* effects = nullptr;

    QString optOut(bool show) {
        if (!v.mActivation && !v.mEffects) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "No Concious Control▲";
        QString sep = " (";
        if (v.mActivation) { desc += sep + "Activation"; sep = " and "; }
        if (v.mEffects) { desc += sep + "Effects"; sep = "; "; }
        return desc + ")";
    }
};

class NoGravityPenalty: public NoFormModifier {
public:
    NoGravityPenalty()
        : NoFormModifier("No Gravity Penalty", isAdvantage, Fraction(1, 2)) { }
    NoGravityPenalty(QJsonObject json)
        : NoFormModifier(json) { }
    NoGravityPenalty(const NoGravityPenalty& m)
        : NoFormModifier(m) { }
    NoGravityPenalty(NoGravityPenalty&& m)
        : NoFormModifier(m) { }
    ~NoGravityPenalty() override { }

    NoGravityPenalty& operator=(const NoGravityPenalty&) = delete;
    NoGravityPenalty& operator=(NoGravityPenalty&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<NoGravityPenalty>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<NoGravityPenalty>(json); }
};

class NoKnockback: public NoFormModifier {
public:
    NoKnockback()
        : NoFormModifier("No Knockback", isLimitation, Fraction(1, 4)) { }
    NoKnockback(QJsonObject json)
        : NoFormModifier(json) { }
    NoKnockback(const NoKnockback& m)
        : NoFormModifier(m) { }
    NoKnockback(NoKnockback&& m)
        : NoFormModifier(m) { }
    ~NoKnockback() override { }

    NoKnockback& operator=(const NoKnockback&) = delete;
    NoKnockback& operator=(NoKnockback&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<NoKnockback>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<NoKnockback>(json); }
};

class NoNoncombatMovement: public NoFormModifier {
public:
    NoNoncombatMovement()
        : NoFormModifier("No Noncombat Movement", isLimitation, Fraction(1, 4)) { }
    NoNoncombatMovement(QJsonObject json)
        : NoFormModifier(json) { }
    NoNoncombatMovement(const NoNoncombatMovement& m)
        : NoFormModifier(m) { }
    NoNoncombatMovement(NoNoncombatMovement&& m)
        : NoFormModifier(m) { }
    ~NoNoncombatMovement() override { }

    NoNoncombatMovement& operator=(const NoNoncombatMovement&) = delete;
    NoNoncombatMovement& operator=(NoNoncombatMovement&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<NoNoncombatMovement>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<NoNoncombatMovement>(json); }
};

class NoRange: public NoFormModifier {
public:
    NoRange()
        : NoFormModifier("No Range", isLimitation, Fraction(1, 2)) { }
    NoRange(QJsonObject json)
        : NoFormModifier(json) { }
    NoRange(const NoRange& m)
        : NoFormModifier(m) { }
    NoRange(NoRange&& m)
        : NoFormModifier(m) { }
    ~NoRange() override { }

    NoRange& operator=(const NoRange&) = delete;
    NoRange& operator=(NoRange&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<NoRange>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<NoRange>(json); }
};

class NoTurnMode: public NoFormModifier {
public:
    NoTurnMode()
        : NoFormModifier("No Turn Mode▲", isAdvantage, Fraction(1, 4)) { }
    NoTurnMode(QJsonObject json)
        : NoFormModifier(json) { }
    NoTurnMode(const NoTurnMode& m)
        : NoFormModifier(m) { }
    NoTurnMode(NoTurnMode&& m)
        : NoFormModifier(m) { }
    ~NoTurnMode() override { }

    NoTurnMode& operator=(const NoTurnMode&) = delete;
    NoTurnMode& operator=(NoTurnMode&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<NoTurnMode>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<NoTurnMode>(json); }
};

class NoncombatAccelerationDecelleration: public NoFormModifier {
public:
    NoncombatAccelerationDecelleration()
        : NoFormModifier("Noncombat Acceleration/Decelleration", isAdvantage, Fraction(1)) { }
    NoncombatAccelerationDecelleration(QJsonObject json)
        : NoFormModifier(json) { }
    NoncombatAccelerationDecelleration(const NoncombatAccelerationDecelleration& m)
        : NoFormModifier(m) { }
    NoncombatAccelerationDecelleration(NoncombatAccelerationDecelleration&& m)
        : NoFormModifier(m) { }
    ~NoncombatAccelerationDecelleration() override { }

    NoncombatAccelerationDecelleration& operator=(const NoncombatAccelerationDecelleration&) = delete;
    NoncombatAccelerationDecelleration& operator=(NoncombatAccelerationDecelleration&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<NoncombatAccelerationDecelleration>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<NoncombatAccelerationDecelleration>(json); }
};

class Nonpersistent: public NoFormModifier {
public:
    Nonpersistent()
        : NoFormModifier("Nonpersistent", isLimitation, Fraction(1, 4)) { }
    Nonpersistent(QJsonObject json)
        : NoFormModifier(json) { }
    Nonpersistent(const Nonpersistent& m)
        : NoFormModifier(m) { }
    Nonpersistent(Nonpersistent&& m)
        : NoFormModifier(m) { }
    ~Nonpersistent() override { }

    Nonpersistent& operator=(const Nonpersistent&) = delete;
    Nonpersistent& operator=(Nonpersistent&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<Nonpersistent>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<Nonpersistent>(json); }
};

class NonresistantDefenses: public NoFormModifier {
public:
    NonresistantDefenses()
        : NoFormModifier("Nonresistant Defenses", isLimitation, Fraction(1, 4)) { }
    NonresistantDefenses(QJsonObject json)
        : NoFormModifier(json) { }
    NonresistantDefenses(const NonresistantDefenses& m)
        : NoFormModifier(m) { }
    NonresistantDefenses(NonresistantDefenses&& m)
        : NoFormModifier(m) { }
    ~NonresistantDefenses() override { }

    NonresistantDefenses& operator=(const NonresistantDefenses&) = delete;
    NonresistantDefenses& operator=(NonresistantDefenses&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<NonresistantDefenses>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<NonresistantDefenses>(json); }
};

class OneUseAtATime: public NoFormModifier {
public:
    OneUseAtATime()
        : NoFormModifier("One Use At A Time", isLimitation, Fraction(1)) { }
    OneUseAtATime(QJsonObject json)
        : NoFormModifier(json) { }
    OneUseAtATime(const OneUseAtATime& m)
        : NoFormModifier(m) { }
    OneUseAtATime(OneUseAtATime&& m)
        : NoFormModifier(m) { }
    ~OneUseAtATime() override { }

    OneUseAtATime& operator=(const OneUseAtATime&) = delete;
    OneUseAtATime& operator=(OneUseAtATime&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<OneUseAtATime>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<OneUseAtATime>(json); }
};

class OnlyInAlternateID: public Modifier {
public:
    OnlyInAlternateID()
        : Modifier("Only In Alternate Identity", isLimitation, isModifier)
        , v({ "" }) { }
    OnlyInAlternateID(const OnlyInAlternateID& m)
        : Modifier(m)
        , v(m.v) { }
    OnlyInAlternateID(OnlyInAlternateID&& m)
        : Modifier(m)
        , v(m.v) { }
    OnlyInAlternateID(QJsonObject json)
        : Modifier(json["name"].toString("Only In Alternate Identity"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mIdentity = json["identity"].toString(0);
                                                }
    ~OnlyInAlternateID() override { }

    OnlyInAlternateID& operator=(const OnlyInAlternateID&) = delete;
    OnlyInAlternateID& operator=(OnlyInAlternateID&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<OnlyInAlternateID>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<OnlyInAlternateID>(json); }

    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { identity = createLineEdit(p, l, "Identity", std::mem_fn(&ModifierBase::changed));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              identity->setText(s.mIdentity);
                                                              v = s;
                                                            }
    void          store() override                          { v.mIdentity = identity->text();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]     = name();
                                                              obj["type"]     = type();
                                                              obj["adder"]    = isAdder();
                                                              obj["identity"] = v.mIdentity;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        return f;
    }

private:
    struct vars {
        QString mIdentity;
    } v {};

    QLineEdit* identity = nullptr;

    QString optOut(bool show) {
        if (v.mIdentity.isEmpty()) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Only In Alternate Identity (" + v.mIdentity + ")";
        return desc;
    }
};

class OnlyProtectsBarrier: public NoFormModifier {
public:
    OnlyProtectsBarrier()
        : NoFormModifier("Only Protects Baarrier", isLimitation, Fraction(1)) { }
    OnlyProtectsBarrier(QJsonObject json)
        : NoFormModifier(json) { }
    OnlyProtectsBarrier(const OnlyProtectsBarrier& m)
        : NoFormModifier(m) { }
    OnlyProtectsBarrier(OnlyProtectsBarrier&& m)
        : NoFormModifier(m) { }
    ~OnlyProtectsBarrier() override { }

    OnlyProtectsBarrier& operator=(const OnlyProtectsBarrier&) = delete;
    OnlyProtectsBarrier& operator=(OnlyProtectsBarrier&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<OnlyProtectsBarrier>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<OnlyProtectsBarrier>(json); }
};

class OnlyRestoresToStartingValues: public NoFormModifier {
public:
    OnlyRestoresToStartingValues()
        : NoFormModifier("Only Restores To Starting Values", isLimitation, Fraction(1, 2)) { }
    OnlyRestoresToStartingValues(QJsonObject json)
        : NoFormModifier(json) { }
    OnlyRestoresToStartingValues(const OnlyRestoresToStartingValues& m)
        : NoFormModifier(m) { }
    OnlyRestoresToStartingValues(OnlyRestoresToStartingValues&& m)
        : NoFormModifier(m) { }
    ~OnlyRestoresToStartingValues() override { }

    OnlyRestoresToStartingValues& operator=(const OnlyRestoresToStartingValues&) = delete;
    OnlyRestoresToStartingValues& operator=(OnlyRestoresToStartingValues&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<OnlyRestoresToStartingValues>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<OnlyRestoresToStartingValues>(json); }
};

class OnlyWorksAgainstDefined: public Modifier {
public:
    OnlyWorksAgainstDefined()
        : Modifier("Only Works Against [Defined]", isLimitation, isModifier)
        , v({ "", -1 }) { }
    OnlyWorksAgainstDefined(const OnlyWorksAgainstDefined& m)
        : Modifier(m)
        , v(m.v) { }
    OnlyWorksAgainstDefined(OnlyWorksAgainstDefined&& m)
        : Modifier(m)
        , v(m.v) { }
    OnlyWorksAgainstDefined(QJsonObject json)
        : Modifier(json["name"].toString("Only Works Against [Defined]"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mAgainst = json["against"].toString("");
                                                  v.mHowCommon = json["howCommon"].toInt(0);
                                                }
    ~OnlyWorksAgainstDefined() override { }

    OnlyWorksAgainstDefined& operator=(const OnlyWorksAgainstDefined&) = delete;
    OnlyWorksAgainstDefined& operator=(OnlyWorksAgainstDefined&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<OnlyWorksAgainstDefined>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<OnlyWorksAgainstDefined>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { against   = createLineEdit(p, l, "Only Works Against?", std::mem_fn(&ModifierBase::changed));
                                                              howCommon = createComboBox(p, l, "How Common?", { "", "Rare", "Uncommon", "Common", "Very Common" },
                                                                                         std::mem_fn(&ModifierBase::index));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              against->setText(s.mAgainst);
                                                              howCommon->setCurrentIndex(s.mHowCommon);
                                                              v = s;
                                                            }
    void          store() override                          { v.mAgainst    = against->text();
                                                              v.mHowCommon = howCommon->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]      = name();
                                                              obj["type"]      = type();
                                                              obj["adder"]     = isAdder();
                                                              obj["against"]   = v.mAgainst;
                                                              obj["howCommon"] = v.mHowCommon;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        return 1 - (v.mHowCommon - 1) * Fraction(1, 4);
    }

private:
    struct vars {
        QString mAgainst;
        int     mHowCommon;
    } v {};

    QLineEdit* against = nullptr;
    QComboBox* howCommon = nullptr;

    QString optOut(bool show) {
        if (v.mAgainst.isEmpty() || v.mHowCommon < 0) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Only Works Against " + v.mAgainst;
        return desc;
    }
};

class Opaque: public Modifier {
public:
    Opaque()
        : Modifier("Opaque", isAdvantage, isAnAdder)
        , v() { }
    Opaque(const Opaque& m)
        : Modifier(m)
        , v(m.v) { }
    Opaque(Opaque&& m)
        : Modifier(m)
        , v(m.v) { }
    Opaque(QJsonObject json)
        : Modifier(json["name"].toString("Opaque"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mWhat = toStringList(json["what"].toArray());
                                                }
    ~Opaque() override { }

    Opaque& operator=(const Opaque&) = delete;
    Opaque& operator=(Opaque&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<Opaque>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<Opaque>(json); }

    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    void          selected(int,int,bool) override           { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { what  = createTreeWidget(p, l, { { "Hearing",     { "Normal Hearing",
                                                                                                                  "Active Sonar",
                                                                                                                  "Ultrasonic Perception" } },
                                                                                               { "Mental",      { "Mental Awareness",
                                                                                                                  "Mind Scan" } },
                                                                                               { "Radio",       { "Radio Perception",
                                                                                                                  "Radar"} },
                                                                                               { "Sight",       { "Normal Sight",
                                                                                                                  "Nightvision",
                                                                                                                  "Infrared Pereception",
                                                                                                                  "Ultraviolet Perception" } },
                                                                                               { "Smell/Taste", { "Normal Smell",
                                                                                                                  "Normal Taste" } },
                                                                                               { "Touch",       { "Normal Touch" } } },
                                                                                               std::mem_fn(&ModifierBase::selected));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              setTreeWidget(what, s.mWhat);
                                                              v = s;
                                                            }
    void          store() override                          { v.mWhat = treeWidget(what);
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["what"]  = toArray(v.mWhat);
                                                              return obj;
                                                            }

    Points points(bool noStore = false) override {
        if (!noStore) store();
        Points p(0);
        QStringList groups = { "Hearing", "Mental", "Radio", "Sight", "Smell/Taste", "Touch" };
        QStringList senses = { "Normal Hearing", "Active Sonar", "Ultrasonic Perception", "Mental Awareness", "Mind Scan", "Radio Perception", "Radar", "Normal Sight",
                               "Nightvision", "Infrared Pereception", "Ultraviolet Perception", "Normal Smell", "Normal Taste", "Normal Touch" };
        for (int i = 0; i < v.mWhat.count(); ++i) {
            auto& str = v.mWhat[i];
            if (groups.contains(str)) p += 10_cp; // NOLINT
            else if (senses.contains(str)) p += 5_cp; // NOLINT
        }
        return p;
    }

private:
    struct vars {
        QStringList mWhat;
    } v {};

    QTreeWidget* what = nullptr;

    QString optOut(bool show) {
        if (v.mWhat.isEmpty()) return "<incomplete>";
        Points p(points(Modifier::NoStore));
        QString desc = (show ? QString("(+%1) ").arg(p.points) : "") + "Opaque to " + v.mWhat[0];
        auto len = v.mWhat.length();
        for (auto i = 1; i < len; ++i) {
            if (i != len - 1) desc += ", ";
            else desc += ", and ";
            desc += v.mWhat[i];
        }
        return desc;
    }

    void numeric(QString) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }
};

class Penetrating: public NoFormModifier {
public:
    Penetrating()
        : NoFormModifier("Penetrating", isAdvantage, Fraction(1, 2)) { }
    Penetrating(QJsonObject json)
        : NoFormModifier(json) { }
    Penetrating(const Penetrating& m)
        : NoFormModifier(m) { }
    Penetrating(Penetrating&& m)
        : NoFormModifier(m) { }
    ~Penetrating() override { }

    Penetrating& operator=(const Penetrating&) = delete;
    Penetrating& operator=(Penetrating&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<Penetrating>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<Penetrating>(json); }
};

class Perceivable: public Modifier {
public:
    Perceivable()
        : Modifier("Perceivable", isLimitation, isModifier)
        , v({ false }) { }
    Perceivable(const Perceivable& m)
        : Modifier(m)
        , v(m.v) { }
    Perceivable(Perceivable&& m)
        : Modifier(m)
        , v(m.v) { }
    Perceivable(QJsonObject json)
        : Modifier(json["name"].toString("Perceivable"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mImperceptable = json["imperceptable"].toBool(false);
                                                }
    ~Perceivable() override { }

    Perceivable& operator=(const Perceivable&) = delete;
    Perceivable& operator=(Perceivable&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<Perceivable>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<Perceivable>(json); }

    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { imperceptable = createCheckBox(p, l, "Imperceptable power is Obvious", std::mem_fn(&ModifierBase::checked));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              imperceptable->setChecked(s.mImperceptable);
                                                              v = s;
                                                            }
    void          store() override                          { v.mImperceptable = imperceptable->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]          = name();
                                                              obj["type"]          = type();
                                                              obj["adder"]         = isAdder();
                                                              obj["imperceptable"] = v.mImperceptable;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v.mImperceptable) f = Fraction(1, 2);
        return f;
    }

private:
    struct vars {
        bool mImperceptable;
    } v {};

    QCheckBox* imperceptable = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Perceivable";
        return desc;
    }
};

class Persistant: public NoFormModifier {
public:
    Persistant()
        : NoFormModifier("Persistant", isAdvantage, Fraction(1, 4)) { }
    Persistant(QJsonObject json)
        : NoFormModifier(json) { }
    Persistant(const Persistant& m)
        : NoFormModifier(m) { }
    Persistant(Persistant&& m)
        : NoFormModifier(m) { }
    ~Persistant() override { }

    Persistant& operator=(const Persistant&) = delete;
    Persistant& operator=(Persistant&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<Persistant>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<Persistant>(json); }
};

class PersonalImmunity: public NoFormModifier {
public:
    PersonalImmunity()
        : NoFormModifier("Personal Immunity", isAdvantage, Fraction(1, 4)) { }
    PersonalImmunity(QJsonObject json)
        : NoFormModifier(json) { }
    PersonalImmunity(const PersonalImmunity& m)
        : NoFormModifier(m) { }
    PersonalImmunity(PersonalImmunity&& m)
        : NoFormModifier(m) { }
    ~PersonalImmunity() override { }

    PersonalImmunity& operator=(const PersonalImmunity&) = delete;
    PersonalImmunity& operator=(PersonalImmunity&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<PersonalImmunity>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<PersonalImmunity>(json); }
};

class PhysicalManifestation: public NoFormModifier {
public:
    PhysicalManifestation()
        : NoFormModifier("Physical Manifestation", isLimitation, Fraction(1, 4)) { }
    PhysicalManifestation(QJsonObject json)
        : NoFormModifier(json) { }
    PhysicalManifestation(const PhysicalManifestation& m)
        : NoFormModifier(m) { }
    PhysicalManifestation(PhysicalManifestation&& m)
        : NoFormModifier(m) { }
    ~PhysicalManifestation() override { }

    PhysicalManifestation& operator=(const PhysicalManifestation&) = delete;
    PhysicalManifestation& operator=(PhysicalManifestation&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<PhysicalManifestation>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<PhysicalManifestation>(json); }
};

class PositionShift: public NoFormModifier {
public:
    PositionShift()
        : NoFormModifier("Position Shift", isLimitation, 5_cp) { } // NOLINT
    PositionShift(QJsonObject json)
        : NoFormModifier(json) { }
    PositionShift(const PositionShift& m)
        : NoFormModifier(m) { }
    PositionShift(PositionShift&& m)
        : NoFormModifier(m) { }
    ~PositionShift() override { }

    PositionShift& operator=(const PositionShift&) = delete;
    PositionShift& operator=(PositionShift&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<PositionShift>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<PositionShift>(json); }
};

class RangeBasedOnSTR: public Modifier {
public:
    RangeBasedOnSTR()
        : Modifier("Range Based On STR", isBoth, isModifier)
        , v({ false }) { }
    RangeBasedOnSTR(const RangeBasedOnSTR& m)
        : Modifier(m)
        , v(m.v) { }
    RangeBasedOnSTR(RangeBasedOnSTR&& m)
        : Modifier(m)
        , v(m.v) { }
    RangeBasedOnSTR(QJsonObject json)
        : Modifier(json["name"].toString("Range Based On STR"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mNoRange = json["noRange"].toBool(false);
                                                }
    ~RangeBasedOnSTR() override { }

    RangeBasedOnSTR& operator=(const RangeBasedOnSTR&) = delete;
    RangeBasedOnSTR& operator=(RangeBasedOnSTR&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<RangeBasedOnSTR>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<RangeBasedOnSTR>(json); }

    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { noRange = createCheckBox(p, l, "Power has no range", std::mem_fn(&ModifierBase::checked));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              noRange->setChecked(s.mNoRange);
                                                              v = s;
                                                            }
    void          store() override                          { v.mNoRange = noRange->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]    = name();
                                                              obj["type"]    = type();
                                                              obj["adder"]   = isAdder();
                                                              obj["noRange"] = v.mNoRange;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(-1, 4);
        if (v.mNoRange) f = Fraction(1, 4);
        return f;
    }

private:
    struct vars {
        bool mNoRange;
    } v {};

    QCheckBox* noRange = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Range Based On STR";
        return desc;
    }
};

class Ranged: public NoFormModifier {
public:
    Ranged()
        : NoFormModifier("Ranged", isAdvantage, Fraction(1, 2)) { }
    Ranged(QJsonObject json)
        : NoFormModifier(json) { }
    Ranged(const Ranged& m)
        : NoFormModifier(m) { }
    Ranged(Ranged&& m)
        : NoFormModifier(m) { }
    ~Ranged() override { }

    Ranged& operator=(const Ranged&) = delete;
    Ranged& operator=(Ranged&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<Ranged>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<Ranged>(json); }
};

class RapidNoncombatMovement: public NoFormModifier {
public:
    RapidNoncombatMovement()
        : NoFormModifier("Rapid Noncombat Movement", isAdvantage, Fraction(1, 2)) { }
    RapidNoncombatMovement(QJsonObject json)
        : NoFormModifier(json) { }
    RapidNoncombatMovement(const RapidNoncombatMovement& m)
        : NoFormModifier(m) { }
    RapidNoncombatMovement(RapidNoncombatMovement&& m)
        : NoFormModifier(m) { }
    ~RapidNoncombatMovement() override { }

    RapidNoncombatMovement& operator=(const RapidNoncombatMovement&) = delete;
    RapidNoncombatMovement& operator=(RapidNoncombatMovement&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<RapidNoncombatMovement>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<RapidNoncombatMovement>(json); }
};

class RealArmor: public NoFormModifier {
public:
    RealArmor()
        : NoFormModifier("Real Armor", isLimitation, Fraction(1, 4)) { }
    RealArmor(QJsonObject json)
        : NoFormModifier(json) { }
    RealArmor(const RealArmor& m)
        : NoFormModifier(m) { }
    RealArmor(RealArmor&& m)
        : NoFormModifier(m) { }
    ~RealArmor() override { }

    RealArmor& operator=(const RealArmor&) = delete;
    RealArmor& operator=(RealArmor&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<RealArmor>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<RealArmor>(json); }
};

class RealWeapon: public NoFormModifier {
public:
    RealWeapon()
        : NoFormModifier("Real Weapon", isLimitation, Fraction(1, 4)) { }
    RealWeapon(QJsonObject json)
        : NoFormModifier(json) { }
    RealWeapon(const RealWeapon& m)
        : NoFormModifier(m) { }
    RealWeapon(RealWeapon&& m)
        : NoFormModifier(m) { }
    ~RealWeapon() override { }

    RealWeapon& operator=(const RealWeapon&) = delete;
    RealWeapon& operator=(RealWeapon&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<RealWeapon>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<RealWeapon>(json); }
};

class ReducedByRange: public NoFormModifier {
public:
    ReducedByRange()
        : NoFormModifier("Reduced By Range", isLimitation, Fraction(1, 4)) { }
    ReducedByRange(QJsonObject json)
        : NoFormModifier(json) { }
    ReducedByRange(const ReducedByRange& m)
        : NoFormModifier(m) { }
    ReducedByRange(ReducedByRange&& m)
        : NoFormModifier(m) { }
    ~ReducedByRange() override { }

    ReducedByRange& operator=(const ReducedByRange&) = delete;
    ReducedByRange& operator=(ReducedByRange&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<ReducedByRange>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<ReducedByRange>(json); }
};

class ReducedByShrinking: public NoFormModifier {
public:
    ReducedByShrinking()
        : NoFormModifier("Reduced By Shrinking", isLimitation, Fraction(1, 4)) { }
    ReducedByShrinking(QJsonObject json)
        : NoFormModifier(json) { }
    ReducedByShrinking(const ReducedByShrinking& m)
        : NoFormModifier(m) { }
    ReducedByShrinking(ReducedByShrinking&& m)
        : NoFormModifier(m) { }
    ~ReducedByShrinking() override { }

    ReducedByShrinking& operator=(const ReducedByShrinking&) = delete;
    ReducedByShrinking& operator=(ReducedByShrinking&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<ReducedByShrinking>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<ReducedByShrinking>(json); }
};

class ReducedEndurance: public Modifier {
public:
    ReducedEndurance()
        : Modifier("Reduced Endurance", isAdvantage, isModifier)
        , v({ false }) { }
    ReducedEndurance(const ReducedEndurance& m)
        : Modifier(m)
        , v(m.v) { }
    ReducedEndurance(ReducedEndurance&& m)
        : Modifier(m)
        , v(m.v) { }
    ReducedEndurance(QJsonObject json)
        : Modifier(json["name"].toString("Reduced Endurance"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mNoEND = json["noEND"].toBool(false);
                                                }
    ~ReducedEndurance() override { }

    ReducedEndurance& operator=(const ReducedEndurance&) = delete;
    ReducedEndurance& operator=(ReducedEndurance&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<ReducedEndurance>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<ReducedEndurance>(json); }

    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { noEND = createCheckBox(p, l, "0 END", std::mem_fn(&ModifierBase::checked));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              noEND->setChecked(s.mNoEND);
                                                              v = s;
                                                            }
    void          store() override                          { v.mNoEND = noEND->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["noEND"] = v.mNoEND;
                                                              return obj;
                                                            }

    Fraction endChange() override { return v.mNoEND ? Fraction(0) : Fraction(1, 2); }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v.mNoEND) f = Fraction(1, 2);
        return f;
    }

private:
    struct vars {
        bool mNoEND;
    } v {};

    QCheckBox* noEND = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 (v.mNoEND ? "0 END" : (Fraction(1, 2).toString() + "END"));
        return desc;
    }
};

class ReducedNegation: public Modifier {
public:
    ReducedNegation()
        : Modifier("Reduced Negation", isAdvantage, isAnAdder)
        , v({ 0 }) { }
    ReducedNegation(const ReducedNegation& m)
        : Modifier(m)
        , v(m.v) { }
    ReducedNegation(ReducedNegation&& m)
        : Modifier(m)
        , v(m.v) { }
    ReducedNegation(QJsonObject json)
        : Modifier(json["name"].toString("Reduced Negation"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mReduced = json["reduced"].toInt(0);
                                                }
    ~ReducedNegation() override { }

    ReducedNegation& operator=(const ReducedNegation&) = delete;
    ReducedNegation& operator=(ReducedNegation&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<ReducedNegation>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<ReducedNegation>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { reduced = createLineEdit(p, l, "Points of negation?", std::mem_fn(&ModifierBase::numeric));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              reduced->setText(QString("%1").arg(s.mReduced));
                                                              v = s;
                                                            }
    void          store() override                          { v.mReduced = reduced->text().toInt();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]    = name();
                                                              obj["type"]    = type();
                                                              obj["adder"]   = isAdder();
                                                              obj["reduced"] = v.mReduced;
                                                              return obj;
                                                            }

    Points points(bool noStore = false) override {
        if (!noStore) store();
        return 2_cp * v.mReduced;
    }

private:
    struct vars {
        int mReduced;
    } v {};

    QLineEdit* reduced = nullptr;

    QString optOut(bool show) {
        if (v.mReduced < 1) return "<incomplete>";
        Points p(points(Modifier::NoStore));
        QString desc = (show ? QString("(+%1) ").arg(p.points) : "")  + QString("Reduced Negation (-%1)").arg(v.mReduced);
        return desc;
    }
};

class ReducedPenetration: public NoFormModifier {
public:
    ReducedPenetration()
        : NoFormModifier("Reduced Penetration", isLimitation, Fraction(1, 4)) { }
    ReducedPenetration(QJsonObject json)
        : NoFormModifier(json) { }
    ReducedPenetration(const ReducedPenetration& m)
        : NoFormModifier(m) { }
    ReducedPenetration(ReducedPenetration&& m)
        : NoFormModifier(m) { }
    ~ReducedPenetration() override { }

    ReducedPenetration& operator=(const ReducedPenetration&) = delete;
    ReducedPenetration& operator=(ReducedPenetration&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<ReducedPenetration>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<ReducedPenetration>(json); }
};

class ReducedRangeModifier: public Modifier {
public:
    ReducedRangeModifier()
        : Modifier("Reduced Range Modifier", isAdvantage, isModifier)
        , v({ false }) { }
    ReducedRangeModifier(const ReducedRangeModifier& m)
        : Modifier(m)
        , v(m.v) { }
    ReducedRangeModifier(ReducedRangeModifier&& m)
        : Modifier(m)
        , v(m.v) { }
    ReducedRangeModifier(QJsonObject json)
        : Modifier(json["name"].toString("Reduced Range Modifier"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mNoRangeMod = json["noRangeMod"].toBool(false);
                                                }
    ~ReducedRangeModifier() override { }

    ReducedRangeModifier& operator=(const ReducedRangeModifier&) = delete;
    ReducedRangeModifier& operator=(ReducedRangeModifier&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<ReducedRangeModifier>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<ReducedRangeModifier>(json); }

    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { noRangeMod = createCheckBox(p, l, "No Range Modifier", std::mem_fn(&ModifierBase::checked));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              noRangeMod->setChecked(s.mNoRangeMod);
                                                              v = s;
                                                            }
    void          store() override                          { v.mNoRangeMod = noRangeMod->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]       = name();
                                                              obj["type"]       = type();
                                                              obj["adder"]      = isAdder();
                                                              obj["noRangeMod"] = v.mNoRangeMod;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v.mNoRangeMod) f = Fraction(1, 2);
        return f;
    }

private:
    struct vars {
        bool mNoRangeMod;
    } v {};

    QCheckBox* noRangeMod = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 (v.mNoRangeMod ? "No Range Modifier" : (Fraction(1, 2).toString() + " Range Modifier"));
        return desc;
    }
};

class RequiredHands: public Modifier {
public:
    RequiredHands()
        : Modifier("Required HandsꚚ", isLimitation, isModifier)
        , v({ -1 }) { }
    RequiredHands(const RequiredHands& m)
        : Modifier(m)
        , v(m.v) { }
    RequiredHands(RequiredHands&& m)
        : Modifier(m)
        , v(m.v) { }
    RequiredHands(QJsonObject json)
        : Modifier(json["name"].toString("Required HandsꚚ"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mHands = json["hands"].toInt(0);
                                                }
    ~RequiredHands() override { }

    RequiredHands& operator=(const RequiredHands&) = delete;
    RequiredHands& operator=(RequiredHands&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<RequiredHands>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<RequiredHands>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { hands = createComboBox(p, l, "How Manhy Hands?", { "", "One-And-A-Half", "Two" },
                                                                                         std::mem_fn(&ModifierBase::index));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              hands->setCurrentIndex(s.mHands);
                                                              v = s;
                                                            }
    void          store() override                          { v.mHands = hands->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["hands"] = v.mHands;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        return v.mHands * Fraction(1, 4);
    }

private:
    struct vars {
        int mHands;
    } v {};

    QComboBox* hands = nullptr;

    QString optOut(bool show) {
        if (v.mHands < 1) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        static QStringList hands { "", "One-And-A-Half", "Two" };
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 hands[v.mHands] + " HandedꚚ";
        return desc;
    }
};

class RequiresARoll: public Modifier {
public:
    RequiresARoll()
        : Modifier("Requires A Roll", isLimitation, isModifier)
        , v({ false, false, -1, "", false, false, -1, -1 }) { }
    RequiresARoll(const RequiresARoll& m)
        : Modifier(m)
        , v(m.v) { }
    RequiresARoll(RequiresARoll&& m)
        : Modifier(m)
        , v(m.v) { }
    RequiresARoll(QJsonObject json)
        : Modifier(json["name"].toString("Requires A Roll"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mType  = json["rType"].toBool(false);
                                                  v.mRoll  = json["roll"].toInt(0);
                                                  v.mWhen  = json["when"].toBool(false);
                                                  v.mSkill = json["skill"].toString();
                                                  v.mIsA   = json["isa"].toBool(false);
                                                  v.mPer   = json["per"].toInt(0);
                                                  v.mTwo   = json["two"].toBool(false);
                                                  v.mFails = json["fails"].toInt(0);
                                                }
    ~RequiresARoll() override { }

    RequiresARoll& operator=(const RequiresARoll&) = delete;
    RequiresARoll& operator=(RequiresARoll&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<RequiresARoll>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<RequiresARoll>(json); }

    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { rType = createCheckBox(p, l, "Skill Roll", std::mem_fn(&ModifierBase::checked));
                                                              when  = createCheckBox(p, l, "Every phase or Use", std::mem_fn(&ModifierBase::checked));
                                                              roll  = createComboBox(p, l, "Unmodified Roll?", { "Unmodified Roll?", "7-", "8-", "9-", "10-", "11-", "12-", "13-", "14-" },
                                                                                     std::mem_fn(&ModifierBase::index));
                                                              skill = createLineEdit(p, l, "What skill?", std::mem_fn(&ModifierBase::changed));
                                                              isa   = createCheckBox(p, l, "Is a KS, PS, or SS?", std::mem_fn(&ModifierBase::checked));
                                                              two   = createCheckBox(p, l, "Choose Between Two Skills", std::mem_fn(&ModifierBase::checked));
                                                              per   = createComboBox(p, l, "-1 per?", { "20 Active points", "10 Active Points", "5 Active Points" },
                                                                                     std::mem_fn(&ModifierBase::index));
                                                              fails = createComboBox(p, l, "How fragile is it?", { "Normal", "Burnout", "Jammed" },
                                                                                     std::mem_fn(&ModifierBase::index));
                                                              roll->setEnabled(!v.mType);
                                                              skill->setEnabled(v.mType);
                                                              isa->setEnabled(v.mType);
                                                              per->setEnabled(v.mType);
                                                              two->setEnabled(v.mTwo);
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              rType->setChecked(s.mType);
                                                              when->setChecked(s.mWhen);
                                                              roll->setCurrentIndex(s.mRoll);
                                                              skill->setText(s.mSkill);
                                                              isa->setChecked(s.mIsA);
                                                              per->setCurrentIndex(s.mPer);
                                                              two->setChecked(s.mTwo);
                                                              fails->setCurrentIndex(s.mFails);
                                                              v = s;
                                                            }
    void          store() override                          { v.mType  = rType->isChecked();
                                                              v.mWhen  = when->isChecked();
                                                              v.mRoll  = roll->currentIndex();
                                                              v.mSkill = skill->text();
                                                              v.mIsA   = isa->isChecked();
                                                              v.mPer   = per->currentIndex();
                                                              v.mTwo   = two->isChecked();
                                                              v.mFails = fails->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["rType"] = v.mType;
                                                              obj["when"]  = v.mWhen;
                                                              obj["roll"]  = v.mRoll;
                                                              obj["skill"] = v.mSkill;
                                                              obj["isa"]   = v.mIsA;
                                                              obj["per"]   = v.mPer;
                                                              obj["two"]   = v.mTwo;
                                                              obj["fails"] = v.mFails;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 2);
        if (v.mType) {
            if (v.mIsA) f -= Fraction(1, 4);
            if (v.mPer == 0) f -= Fraction(1, 4);
            else if (v.mPer == 2) f += Fraction(1, 2);
            if (v.mTwo) f -= Fraction(1, 4);
        } else f -= (v.mRoll - 5) * Fraction(1, 4); // NOLINT
        if (v.mFails >= 1) f += Fraction(1, 2);
        if (f < Fraction(1, 4)) f = Fraction(1, 4);
        return f;
    }

private:
    struct vars {
        bool    mType;
        bool    mWhen;
        int     mRoll;
        QString mSkill;
        bool    mIsA;
        bool    mTwo;
        int     mPer;
        int     mFails;
    } v {};

    QCheckBox* rType = nullptr;
    QCheckBox* when = nullptr;
    QComboBox* roll = nullptr;
    QLineEdit* skill = nullptr;
    QCheckBox* isa = nullptr;
    QCheckBox* two = nullptr;
    QComboBox* per = nullptr;
    QComboBox* fails = nullptr;

    QString optOut(bool show) {
        QStringList per { "20 Active points", "10 Active Points", "5 Active Points" };
        QStringList roll { "", "7-", "8-", "9-", "10-", "11-", "12-", "13-", "14-" };
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "");
        QString sep;
        if (v.mType) {
            if (v.mSkill.isEmpty() || v.mPer < 1) return "<incomplete>";
            desc = "Skill Roll: " + v.mSkill + " (-1 per " + per[v.mPer];
            sep = "; ";
        } else {
            if (v.mRoll < 1) return "<incomplete>";
            desc = "Roll " + roll[v.mRoll];
            sep = " (";
        }
        if (v.mWhen) { desc += sep + "Every phase or Use"; sep = "; "; }
        if (v.mFails == 1) { desc += sep + "Burnout"; sep = "; "; }
        else if (v.mFails == 2) { desc += sep + "Jammed"; sep = ": "; }
        if (sep == "; ") desc += ")";
        return desc;
    }

    void checked(bool) override {
        store();
        QCheckBox* box = dynamic_cast<QCheckBox*>(sender());
        if (box == rType) {
            roll->setEnabled(!v.mType);
            skill->setEnabled(v.mType);
            isa->setEnabled(v.mType);
            per->setEnabled(v.mType);
            two->setEnabled(v.mTwo);
        }
        ModifiersDialog::ref().updateForm();
    }
};

class RequiresMultipleCharges: public Modifier {
public:
    RequiresMultipleCharges()
        : Modifier("Requires Multiple Charges", isLimitation, isAnAdder)
        , v({ 0, 0 }) { }
    RequiresMultipleCharges(const RequiresMultipleCharges& m)
        : Modifier(m)
        , v(m.v) { }
    RequiresMultipleCharges(RequiresMultipleCharges&& m)
        : Modifier(m)
        , v(m.v) { }
    RequiresMultipleCharges(QJsonObject json)
        : Modifier(json["name"].toString("Requires Multiple Charges"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mMax   = json["max"].toInt(0);
                                                  v.mNeeds = json["needs"].toInt(0);
                                                }
    ~RequiresMultipleCharges() override { }

    RequiresMultipleCharges& operator=(const RequiresMultipleCharges&) = delete;
    RequiresMultipleCharges& operator=(RequiresMultipleCharges&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<RequiresMultipleCharges>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<RequiresMultipleCharges>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { max   = createLineEdit(p, l, "Maximum charges available?", std::mem_fn(&ModifierBase::numeric));
                                                              needs = createLineEdit(p, l, "Charges needed?", std::mem_fn(&ModifierBase::numeric));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              max->setText(QString("%1").arg(s.mMax));
                                                              needs->setText(QString("%1").arg(s.mNeeds));
                                                              v = s;
                                                            }
    void          store() override                          { v.mMax   = max->text().toInt();
                                                              v.mNeeds = needs->text().toInt();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]    = name();
                                                              obj["type"]    = type();
                                                              obj["adder"]   = isAdder();
                                                              obj["max"]     = v.mMax;
                                                              obj["needs"]   = v.mNeeds;
                                                              return obj;
                                                            }

    // 1   2   4    8  16  32  64  128 256
    // 2^0 2^1 2^2 2^3 2^4 2^5 2^6 2^7 2^8
    // 2^x=C
    // log(2^x)=log(C)
    // x*log(2)=log(C)
    // x=log(C)/log(2)
    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(0);
        if (v.mNeeds == 2) f = Fraction(1, 4);
        else f = (v.mNeeds - 3) / 5 * Fraction(1, 4) + Fraction(1, 4); // NOLINT
        if (v.mMax < 65) f += (6 - (int) (log((double) v.mMax) / log(2.0))) * Fraction(1, 4); // NOLINT
        return f;
    }

private:
    struct vars {
        int mMax;
        int mNeeds;
    } v {};

    QLineEdit* max = nullptr;
    QLineEdit* needs = nullptr;

    QString optOut(bool show) {
        if (v.mMax < 1 || v.mNeeds < 2) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "");
        desc += QString("%1").arg(v.mNeeds) + " Charges per Use";
        return desc;
    }
};

class RequiresMultipleUsers: public Modifier {
public:
    RequiresMultipleUsers()
        : Modifier("Requires Multiple Users", isLimitation, isAnAdder)
        , v({ 0 }) { }
    RequiresMultipleUsers(const RequiresMultipleUsers& m)
        : Modifier(m)
        , v(m.v) { }
    RequiresMultipleUsers(RequiresMultipleUsers&& m)
        : Modifier(m)
        , v(m.v) { }
    RequiresMultipleUsers(QJsonObject json)
        : Modifier(json["name"].toString("Requires Multiple Users"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mNeeds = json["needs"].toInt(0);
                                                }
    ~RequiresMultipleUsers() override { }

    RequiresMultipleUsers& operator=(const RequiresMultipleUsers&) = delete;
    RequiresMultipleUsers& operator=(RequiresMultipleUsers&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<RequiresMultipleUsers>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<RequiresMultipleUsers>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { needs = createLineEdit(p, l, "Users needed?", std::mem_fn(&ModifierBase::numeric));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              needs->setText(QString("%1").arg(s.mNeeds));
                                                              v = s;
                                                            }
    void          store() override                          { v.mNeeds = needs->text().toInt();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]    = name();
                                                              obj["type"]    = type();
                                                              obj["adder"]   = isAdder();
                                                              obj["needs"]   = v.mNeeds;
                                                              return obj;
                                                            }

    // 1   2   4    8  16  32  64  128 256
    // 2^0 2^1 2^2 2^3 2^4 2^5 2^6 2^7 2^8
    // 2^x=C
    // log(2^x)=log(C)
    // x*log(2)=log(C)
    // x=log(C)/log(2)
    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f = ((int) (log((double) v.mNeeds) / log(2.0)) + 1) * Fraction(1, 4); // NOLINT
        return f;
    }

private:
    struct vars {
        int mNeeds;
    } v {};

    QLineEdit* needs = nullptr;

    QString optOut(bool show) {
        if (v.mNeeds < 2) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "");
        desc += QString("%1").arg(v.mNeeds) + " Users to Activate";
        return desc;
    }
};

class Resistant: public NoFormModifier {
public:
    Resistant()
        : NoFormModifier("Resistant", isAdvantage, Fraction(1, 2)) { }
    Resistant(QJsonObject json)
        : NoFormModifier(json) { }
    Resistant(const Resistant& m)
        : NoFormModifier(m) { }
    Resistant(Resistant&& m)
        : NoFormModifier(m) { }
    ~Resistant() override { }

    Resistant& operator=(const Resistant&) = delete;
    Resistant& operator=(Resistant&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<Resistant>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<Resistant>(json); }
};

class Restrainable: public Modifier {
public:
    Restrainable()
        : Modifier("Restrainable", isLimitation, isModifier)
        , v({ false, "" }) { }
    Restrainable(const Restrainable& m)
        : Modifier(m)
        , v(m.v) { }
    Restrainable(Restrainable&& m)
        : Modifier(m)
        , v(m.v) { }
    Restrainable(QJsonObject json)
        : Modifier(json["name"].toString("Restrainable"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mNonStandard = json["nonStandard"].toBool(false);
                                                  v.mRestraint   = json["restraint"].toString();
                                                }
    ~Restrainable() override { }

    Restrainable& operator=(const Restrainable&) = delete;
    Restrainable& operator=(Restrainable&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<Restrainable>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<Restrainable>(json); }

    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { nonStandard = createCheckBox(p, l, "Non-standard restraint", std::mem_fn(&ModifierBase::checked));
                                                              restraint   = createLineEdit(p, l, "How to restrain?", std::mem_fn(&ModifierBase::changed));
                                                              restraint->setEnabled(false);
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              nonStandard->setChecked(s.mNonStandard);
                                                              restraint->setText(s.mRestraint);
                                                              v = s;
                                                            }
    void          store() override                          { v.mNonStandard = nonStandard->isChecked();
                                                              v.mRestraint   = restraint->text();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]        = name();
                                                              obj["type"]        = type();
                                                              obj["adder"]       = isAdder();
                                                              obj["nonStandard"] = v.mNonStandard;
                                                              obj["restraint"]   = v.mRestraint;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 2);
        if (v.mNonStandard) f = Fraction(1, 4);
        return f;
    }

private:
    struct vars {
        bool    mNonStandard;
        QString mRestraint;
    } v {};

    QCheckBox* nonStandard = nullptr;
    QLineEdit* restraint = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "Restrainable";
        if (v.mNonStandard) desc += "(Only by " + v.mRestraint + ")";
        return desc;
    }

    void checked(bool) override {
        store();
        restraint->setEnabled(v.mNonStandard);
        ModifiersDialog::ref().updateForm();
    }
};

class Ressurection: public NoFormModifier {
public:
    Ressurection()
        : NoFormModifier("Ressurection", isAdvantage, 20_cp) { } // NOLINT
    Ressurection(QJsonObject json)
        : NoFormModifier(json) { }
    Ressurection(const Ressurection& m)
        : NoFormModifier(m) { }
    Ressurection(Ressurection&& m)
        : NoFormModifier(m) { }
    ~Ressurection() override { }

    Ressurection& operator=(const Ressurection&) = delete;
    Ressurection& operator=(Ressurection&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<Ressurection>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<Ressurection>(json); }
};

class RessurectionOnly: public Modifier {
public:
    RessurectionOnly()
        : Modifier("Ressurection Only", isLimitation, isModifier)
        , v({ false }) { }
    RessurectionOnly(const RessurectionOnly& m)
        : Modifier(m)
        , v(m.v) { }
    RessurectionOnly(RessurectionOnly&& m)
        : Modifier(m)
        , v(m.v) { }
    RessurectionOnly(QJsonObject json)
        : Modifier(json["name"].toString("Ressurection Only"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mRegen = json["regen"].toBool(false);
                                                }
    ~RessurectionOnly() override { }

    RessurectionOnly& operator=(const RessurectionOnly&) = delete;
    RessurectionOnly& operator=(RessurectionOnly&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<RessurectionOnly>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<RessurectionOnly>(json); }

    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { regen = createCheckBox(p, l, "Regeneration", std::mem_fn(&ModifierBase::checked));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              regen->setChecked(s.mRegen);
                                                              v = s;
                                                            }
    void          store() override                          { v.mRegen = regen->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["regen"] = v.mRegen;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 2);
        if (v.mRegen) f = Fraction(2);
        return f;
    }

private:
    struct vars {
        bool mRegen;
    } v {};

    QCheckBox* regen = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "-") + f.toString() + ") " : "") + "Ressurection Only";
        return desc;
    }
};

class SafeBlindTravel: public NoFormModifier {
public:
    SafeBlindTravel()
        : NoFormModifier("Safe Blind Travel", isAdvantage, Fraction(1, 4)) { }
    SafeBlindTravel(QJsonObject json)
        : NoFormModifier(json) { }
    SafeBlindTravel(const SafeBlindTravel& m)
        : NoFormModifier(m) { }
    SafeBlindTravel(SafeBlindTravel&& m)
        : NoFormModifier(m) { }
    ~SafeBlindTravel() override { }

    SafeBlindTravel& operator=(const SafeBlindTravel&) = delete;
    SafeBlindTravel& operator=(SafeBlindTravel&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<SafeBlindTravel>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<SafeBlindTravel>(json); }
};

class SideEffects: public Modifier {
public:
    SideEffects()
        : Modifier("Side Effects", isLimitation, isModifier)
        , v({ -1, "", -1, -1, false, false }) { }
    SideEffects(const SideEffects& m)
        : Modifier(m)
        , v(m.v) { }
    SideEffects(SideEffects&& m)
        : Modifier(m)
        , v(m.v) { }
    SideEffects(QJsonObject json)
        : Modifier(json["name"].toString("Side Effects"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mLevel    = json["level"].toInt(0);
                                                  v.mEffect   = json["effect"].toString();
                                                  v.mWhen     = json["when"].toInt(0);
                                                  v.mAffects  = json["affects"].toInt(0);
                                                  v.mConstant = json["constant"].toBool(false);
                                                  v.mPre      = json["pre"].toBool(false);
                                                }
    ~SideEffects() override { }

    SideEffects& operator=(const SideEffects&) = delete;
    SideEffects& operator=(SideEffects&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<SideEffects>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<SideEffects>(json); }

    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { level    = createComboBox(p, l, "Level?", { "", "Minor", "Major", "Extreme" },
                                                                                        std::mem_fn(&ModifierBase::index));
                                                              when     = createComboBox(p, l, "Side Effect Occurs When?", { "", "Required Roll Fails", "A Thing Happens", "When Power Used",
                                                                                                                            "When Power Stops Being Used" },
                                                                                       std::mem_fn(&ModifierBase::index));
                                                              affects  = createComboBox(p, l, "Affects?", { "", "Character", "Aropund Character", "Recipient", "Character And Recipient" },
                                                                                       std::mem_fn(&ModifierBase::index));
                                                              constant = createCheckBox(p, l, "Constant Power", std::mem_fn(&ModifierBase::checked));
                                                              pre      = createCheckBox(p, l, "Damage Predefined", std::mem_fn(&ModifierBase::checked));
                                                              effect   = createLineEdit(p, l, "What Effect (Power Description)?", std::mem_fn(&ModifierBase::changed));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              level->setCurrentIndex(s.mLevel);
                                                              effect->setText(s.mEffect);
                                                              when->setCurrentIndex(s.mWhen);
                                                              affects->setCurrentIndex(s.mAffects);
                                                              constant->setChecked(s.mConstant);
                                                              pre->setChecked(s.mPre);
                                                              v = s;
                                                            }
    void          store() override                          { v.mLevel    = level->currentIndex();
                                                              v.mEffect   = effect->text();
                                                              v.mWhen     = when->currentIndex();
                                                              v.mAffects  = affects->currentIndex();
                                                              v.mConstant = constant->isChecked();
                                                              v.mPre      = pre->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]     = name();
                                                              obj["type"]     = type();
                                                              obj["adder"]    = isAdder();
                                                              obj["level"]    = v.mLevel;
                                                              obj["effect"]   = v.mEffect;
                                                              obj["when"]     = v.mWhen;
                                                              obj["affects"]  = v.mAffects;
                                                              obj["constant"] = v.mConstant;
                                                              obj["pre"]      = v.mPre;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f;
        switch (v.mLevel) {
        case 1: f = Fraction(1, 4); break;
        case 2: f = Fraction(1, 2); break;
        case 3: f = Fraction(1);    break;
        }
        switch (v.mAffects) {
        case 1:                      break;
        case 2:
        case 4: f -= Fraction(1, 4); break;
        case 3:
        case 5: f += Fraction(1, 4); break; // NOLINT
        }
        if (v.mConstant) f += Fraction(1, 4);
        if (v.mPre) f -= Fraction(1, 4);
        switch (v.mWhen) {
        case 1:                      break;
        case 2:
        case 4: f -= Fraction(1, 4); break;
        case 3: f *= 2;              break;
        }
        if (f < Fraction(1, 4)) f = Fraction(1, 4);
        return f;
    }

private:
    struct vars {
        int     mLevel;
        QString mEffect;
        int     mWhen;
        int     mAffects;
        bool    mConstant;
        bool    mPre;
    } v {};

    QComboBox* level = nullptr;
    QLineEdit* effect = nullptr;
    QComboBox* when = nullptr;
    QComboBox* affects = nullptr;
    QCheckBox* constant = nullptr;
    QCheckBox* pre = nullptr;

    QString optOut(bool show) {
        QStringList affects { "", "Character", "Aropund Character", "Recipient", "Character And Recipient" };
        QStringList when { "", "Required Roll Fails", "A Thing Happens", "When Power Used",
                           "When Power Stops Being Used" };
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "-") + f.toString() + ") " : "");
        if (v.mAffects < 1 || v.mWhen < 1 || v.mLevel < 1 || v.mEffect.isEmpty()) return "<incomplete>";
        desc += "Side Effect (" + v.mEffect;
        desc += " On " + affects[v.mAffects];
        desc += " When " + when[v.mWhen];
        if (v.mConstant) desc += "; Fixed Damage";
        return desc + ")";
    }
};

class SkinContactRequired: public NoFormModifier {
public:
    SkinContactRequired()
        : NoFormModifier("Skin Contact Required", isLimitation, Fraction(1)) { }
    SkinContactRequired(QJsonObject json)
        : NoFormModifier(json) { }
    SkinContactRequired(const SkinContactRequired& m)
        : NoFormModifier(m) { }
    SkinContactRequired(SkinContactRequired&& m)
        : NoFormModifier(m) { }
    ~SkinContactRequired() override { }

    SkinContactRequired& operator=(const SkinContactRequired&) = delete;
    SkinContactRequired& operator=(SkinContactRequired&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<SkinContactRequired>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<SkinContactRequired>(json); }
};

class StandardRange: public Modifier {
public:
    StandardRange()
        : Modifier("Standard Range", isLimitation, isModifier)
        , v({ false }) { }
    StandardRange(const StandardRange& m)
        : Modifier(m)
        , v(m.v) { }
    StandardRange(StandardRange&& m)
        : Modifier(m)
        , v(m.v) { }
    StandardRange(QJsonObject json)
        : Modifier(json["name"].toString("Standard Range"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mRMod = json["rmod"].toBool(false);
                                                }
    ~StandardRange() override { }

    StandardRange& operator=(const StandardRange&) = delete;
    StandardRange& operator=(StandardRange&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<StandardRange>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<StandardRange>(json); }

    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { rmod = createCheckBox(p, l, "Subject To Range Mod", std::mem_fn(&ModifierBase::checked));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              rmod->setChecked(s.mRMod);
                                                              v = s;
                                                            }
    void          store() override                          { v.mRMod = rmod->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["rmod"]  = v.mRMod;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v.mRMod) f = Fraction(1, 2);
        return f;
    }

private:
    struct vars {
        bool mRMod;
    } v {};

    QCheckBox* rmod = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "Standard Range";
        if (v.mRMod) desc += " (Subject To Range Penallties)";
        return desc;
    }
};

class StopsWorkingIfKnockedOutStunned: public Modifier {
public:
    StopsWorkingIfKnockedOutStunned()
        : Modifier("Stops Working If Knocked Out/Stunned", isLimitation, isModifier)
        , v({ false }) { }
    StopsWorkingIfKnockedOutStunned(const StopsWorkingIfKnockedOutStunned& m)
        : Modifier(m)
        , v(m.v) { }
    StopsWorkingIfKnockedOutStunned(StopsWorkingIfKnockedOutStunned&& m)
        : Modifier(m)
        , v(m.v) { }
    StopsWorkingIfKnockedOutStunned(QJsonObject json)
        : Modifier(json["name"].toString("Stops Working If Knocked Out/Stunned"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mStunned = json["stunned"].toBool(false);
                                                }
    ~StopsWorkingIfKnockedOutStunned() override { }

    StopsWorkingIfKnockedOutStunned& operator=(const StopsWorkingIfKnockedOutStunned&) = delete;
    StopsWorkingIfKnockedOutStunned& operator=(StopsWorkingIfKnockedOutStunned&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<StopsWorkingIfKnockedOutStunned>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<StopsWorkingIfKnockedOutStunned>(json); }

    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { stunned = createCheckBox(p, l, "Or Stunned", std::mem_fn(&ModifierBase::checked));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              stunned->setChecked(s.mStunned);
                                                              v = s;
                                                            }
    void          store() override                          { v.mStunned = stunned->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]    = name();
                                                              obj["type"]    = type();
                                                              obj["adder"]   = isAdder();
                                                              obj["stunned"] = v.mStunned;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v.mStunned) f = Fraction(1, 2);
        return f;
    }

private:
    struct vars {
        bool mStunned;
    } v {};

    QCheckBox* stunned = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "Stops Working If Knocked Out";
        if (v.mStunned) desc += " Or Stunned";
        return desc;
    }
};

class Sticky: public Modifier {
public:
    Sticky()
        : Modifier("Sticky", isAdvantage, isModifier)
        , v({ false }) { }
    Sticky(QJsonObject json)
        : Modifier(json["name"].toString("Sticky"),
                   ModifierType(json["type"].toInt(isAdvantage)),
                   json["adder"].toBool(isModifier)) { v.mAll = json["all"].toBool(false); }
    Sticky(const Sticky& m)
        : Modifier(m)
        , v(m.v) { }
    Sticky(Sticky&& m)
        : Modifier(m)
        , v(m.v) { }
    ~Sticky() override { }

    Sticky& operator=(const Sticky&) = delete;
    Sticky& operator=(Sticky&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<Sticky>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<Sticky>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { all = createCheckBox(p, l, "Free one, free all??", std::mem_fn(&ModifierBase::checked));
                                                              return true; }
    void          restore() override                        { vars s = v; all->setChecked(s.mAll); v = s; }
    void          store() override                          { v.mAll = all->isChecked(); }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["all"]   = v.mAll;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        if (v.mAll) return Fraction(1, 4);
        else return Fraction(1, 2);
    }

private:
    struct vars {
        bool mAll;
    } v {};

    QCheckBox* all = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                       "Sticky";
        if (v.mAll) desc += " (Free on, free all)";
        return desc;
    }
};

class STRMinimum: public Modifier {
public:
    STRMinimum()
        : Modifier("STR MinimumꚚ", isAdvantage, isModifier)
        , v({ 1 }) { }
    STRMinimum(const STRMinimum& m)
        : Modifier(m)
        , v(m.v) { }
    STRMinimum(STRMinimum&& m)
        : Modifier(m)
        , v(m.v) { }
    STRMinimum(QJsonObject json)
        : Modifier(json["name"].toString("STR MinimumꚚ"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mMin = json["min"].toInt(0);
                                                }
    ~STRMinimum() override { }

    STRMinimum& operator=(const STRMinimum&) = delete;
    STRMinimum& operator=(STRMinimum&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<STRMinimum>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<STRMinimum>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { min = createLineEdit(p, l, "STR Minimum?", std::mem_fn(&ModifierBase::numeric));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              min->setText(QString("%1").arg(s.mMin));
                                                              v = s;
                                                            }
    void          store() override                          { v.mMin = min->text().toInt(0);
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["min"]   = v.mMin;
                                                              return obj;
                                                            }
    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        if (v.mMin >= 4 && v.mMin <= 8) return Fraction(1, 4); // NOLINT
        else if (v.mMin >= 9 && v.mMin <= 13) return Fraction(1, 2); // NOLINT
        else if (v.mMin >= 14 && v.mMin <= 18) return Fraction(3, 4); // NOLINT
        else if (v.mMin >= 19) return Fraction(1); // NOLINT
        return Fraction(0);
    }

private:
    struct vars {
        int mMin;
    } v {};

    QLineEdit* min = nullptr;

    void numeric(QString) override {
        QString txt = min->text();
        if (txt.isEmpty() || isNumber(txt)) {
            if (txt.toInt(nullptr) < 1 && !txt.isEmpty()) min->undo();
            v.mMin = txt.toInt(nullptr);
            if (v.mMin < 1) v.mMin = 1;
            return;
        }
        min->undo();
    }

    QString optOut(bool show) {
        if (v.mMin < 0) return "<incomplete>";
        Fraction f(fraction(NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                "STR MinimumꚚ (" + QString("%1)").arg(v.mMin);
        return desc;
    }
};

class STUNOnly: public NoFormModifier {
public:
    STUNOnly()
        : NoFormModifier("STUN Only", isLimitation, Fraction(1, 4)) { }
    STUNOnly(QJsonObject json)
        : NoFormModifier(json) { }
    STUNOnly(const STUNOnly& m)
        : NoFormModifier(m) { }
    STUNOnly(STUNOnly&& m)
        : NoFormModifier(m) { }
    ~STUNOnly() override { }

    STUNOnly& operator=(const STUNOnly&) = delete;
    STUNOnly& operator=(STUNOnly&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<STUNOnly>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<STUNOnly>(json); }
};

class SubjectToRangeModifier: public NoFormModifier {
public:
    SubjectToRangeModifier()
        : NoFormModifier("Subject To Range Modifier", isLimitation, Fraction(1, 4)) { }
    SubjectToRangeModifier(QJsonObject json)
        : NoFormModifier(json) { }
    SubjectToRangeModifier(const SubjectToRangeModifier& m)
        : NoFormModifier(m) { }
    SubjectToRangeModifier(SubjectToRangeModifier&& m)
        : NoFormModifier(m) { }
    ~SubjectToRangeModifier() override { }

    SubjectToRangeModifier& operator=(const SubjectToRangeModifier&) = delete;
    SubjectToRangeModifier& operator=(SubjectToRangeModifier&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<SubjectToRangeModifier>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<SubjectToRangeModifier>(json); }
};

class TimeLimit: public Modifier {
public:
    TimeLimit()
        : Modifier("Time Limit", isBoth, isModifier)
        , v({ -1, -1 }) { }
    TimeLimit(const TimeLimit& m)
        : Modifier(m)
        , v(m.v) { }
    TimeLimit(TimeLimit&& m)
        : Modifier(m)
        , v(m.v) { }
    TimeLimit(QJsonObject json)
        : Modifier(json["name"].toString("Time Limit"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mType = json["lType"].toInt(0);
                                                  v.mTime = json["time"].toInt(0);
                                                }
    ~TimeLimit() override { }

    TimeLimit& operator=(const TimeLimit&) = delete;
    TimeLimit& operator=(TimeLimit&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<TimeLimit>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<TimeLimit>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { lType = createComboBox(p, l, "Type?", { "", "Instant Power", "Constant Power", "All Others" },
                                                                                     std::mem_fn(&ModifierBase::index));
                                                              time  = createComboBox(p, l, "How long?", { "", "An Extra Phase", "1 Turn", "1 Minute", "5 Minutes", "20 Minutes",
                                                                                                          "1 Hour", "6 Hours", "1 Day", "1 Week", "1 Month", "1 Season",
                                                                                                          "1 Year", "5 Years" }, std::mem_fn(&ModifierBase::index));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              lType->setCurrentIndex(s.mType);
                                                              ModifiersDialog::ref().update();
                                                              time->setCurrentIndex(s.mTime);
                                                              v = s;
                                                            }
    void          store() override                          { v.mType = lType->currentIndex();
                                                              v.mTime = time->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["lType"] = v.mType;
                                                              obj["time"]  = v.mTime;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        switch (v.mType) {
        case 1:
        case 2: return (v.mTime) * Fraction(1, 4);
        case 3: if (v.mTime <= 5) return -4 + v.mTime * Fraction(1, 2); // NOLINT
                else return -1 + (v.mTime - 5) * Fraction(1, 4); // NOLINT
        }

        return Fraction(0);
    }

private:
    struct vars {
        int mType;
        int mTime;
    } v {};

    QComboBox* lType = nullptr;
    QComboBox* time = nullptr;

    QString optOut(bool show) {
        if (v.mType < 1 || v.mTime < 1) return "<incomplete>";
        Fraction f(fraction(NoStore));
        QStringList instant = { "", "An Extra Phase", "1 Turn", "1 Minute", "5 Minutes", "20 Minutes",
                                "1 Hour", "6 Hours", "1 Day", "1 Week", "1 Month", "1 Season",
                                "1 Year", "5 Years" };
        QStringList constant = { "", "1 Turn", "1 Minute", "5 Minutes", "20 Minutes",
                                 "1 Hour", "6 Hours", "1 Day", "1 Week", "1 Month", "1 Season",
                                 "1 Year", "5 Years" };
        QStringList other = { "", "An Extra Phase", "1 Turn", "1 Minute", "5 Minutes", "20 Minutes",
                              "1 Hour", "6 Hours", "1 Day" };
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "Time Limit (";
        switch (v.mType) {
        case 1: desc += instant[v.mTime];  break;
        case 2: desc += constant[v.mTime]; break;
        case 3: desc += other[v.mTime];    break;
        }
        return desc + ")";
    }

    void index(int) override {
        store();
        QComboBox* box = dynamic_cast<QComboBox*>(sender());
        if (box == lType) {
            QStringList instant = { "", "An Extra Phase", "1 Turn", "1 Minute", "5 Minutes", "20 Minutes",
                                    "1 Hour", "6 Hours", "1 Day", "1 Week", "1 Month", "1 Season",
                                    "1 Year", "5 Years" };
            QStringList constant = { "", "1 Turn", "1 Minute", "5 Minutes", "20 Minutes",
                                     "1 Hour", "6 Hours", "1 Day", "1 Week", "1 Month", "1 Season",
                                     "1 Year", "5 Years" };
            QStringList other = { "", "An Extra Phase", "1 Turn", "1 Minute", "5 Minutes", "20 Minutes",
                                  "1 Hour", "6 Hours", "1 Day" };
            time->clear();

            switch (v.mType) {
            case 1: time->addItems(instant);  break;
            case 2: time->addItems(constant); break;
            case 3: time->addItems(other);    break;
            }
            time->setCurrentIndex(-1);
            time->setCurrentText("How Long?");
            v.mTime = -1;
        }
        ModifiersDialog::ref().updateForm();
    }
};

class Transdimensional: public Modifier {
public:
    Transdimensional()
        : Modifier("Transdimensionalϴ", isAdvantage, isModifier)
        , v({ -1, "" }) { }
    Transdimensional(const Transdimensional& m)
        : Modifier(m)
        , v(m.v) { }
    Transdimensional(Transdimensional&& m)
        : Modifier(m)
        , v(m.v) { }
    Transdimensional(QJsonObject json)
        : Modifier(json["name"].toString("Transdimensionalϴ"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mType  = json["lType"].toInt(0);
                                                  v.mWhich = json["which"].toString();
                                                }
    ~Transdimensional() override { }

    Transdimensional& operator=(const Transdimensional&) = delete;
    Transdimensional& operator=(Transdimensional&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<Transdimensional>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<Transdimensional>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { lType = createComboBox(p, l, "Type?", { "", "Single Dimension", "Related Group of Dimensions", "Any Dimension" },
                                                                                     std::mem_fn(&ModifierBase::index));
                                                              which  = createLineEdit(p, l, "Which?", std::mem_fn(&ModifierBase::changed));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              lType->setCurrentIndex(s.mType);
                                                              which->setText(s.mWhich);
                                                              v = s;
                                                            }
    void          store() override                          { v.mType = lType->currentIndex();
                                                              v.mWhich = which->text();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["lType"] = v.mType;
                                                              obj["which"] = v.mWhich;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        switch (v.mType) {
        case 1: return Fraction(1, 2);
        case 2: return Fraction(3, 4);
        case 3: return Fraction(1);
        }

        return Fraction(0);
    }

private:
    struct vars {
        int     mType;
        QString mWhich;
    } v {};

    QComboBox* lType = nullptr;
    QLineEdit* which = nullptr;

    QString optOut(bool show) {
        if (v.mType < 1 || (v.mType != 3 && v.mWhich.isEmpty())) return "<incomplete>";
        Fraction f(fraction(NoStore));
        QStringList type = { "", "Single Dimension", "Related Group of Dimensions", "Any Dimension" };
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "Transdimensionalϴ (";
        switch (v.mType) {
        case 0:
        case 1: desc += type[v.mType] + "; " + v.mWhich; break;
        case 2: desc += type[v.mType];                  break;
        }
        return desc + ")";
    }
};

class Trigger: public Modifier {
public:
    Trigger()
        : Modifier("Trigger", isAdvantage, isModifier)
        , v({ false, "", false, -1, -1, false, false }) { }
    Trigger(const Trigger& m)
        : Modifier(m)
        , v(m.v) { }
    Trigger(Trigger&& m)
        : Modifier(m)
        , v(m.v) { }
    Trigger(QJsonObject json)
        : Modifier(json["name"].toString("Trigger"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mChange  = json["change"].toBool(false);
                                                  v.mCond    = json["cond"].toString();
                                                  v.mActive  = json["active"].toBool(false);
                                                  v.mAct     = json["act"].toInt(0);
                                                  v.mReset   = json["reset"].toInt(0);
                                                  v.mExpire  = json["expire"].toBool(false);
                                                  v.mMisfire = json["misfire"].toBool(false);
                                                }
    ~Trigger() override { }

    Trigger& operator=(const Trigger&) = delete;
    Trigger& operator=(Trigger&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<Trigger>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<Trigger>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { change  = createCheckBox(p, l, "Can Change Trigger Conditions", std::mem_fn(&ModifierBase::checked));
                                                              cond    = createLineEdit(p, l, "Conditions?", std::mem_fn(&ModifierBase::changed));
                                                              active  = createCheckBox(p, l, "Activstion is A No Time Action", std::mem_fn(&ModifierBase::checked));
                                                              act     = createComboBox(p, l, "How many conditions?", { "", "One", "Two", "Three" }, std::mem_fn(&ModifierBase::index));
                                                              reset   = createComboBox(p, l, "Reset time?", { "", "A Turn", "A Full Phase", "A Half Phase", "A Zero Phase Action",
                                                                                                              "Automatically" },
                                                                                       std::mem_fn(&ModifierBase::index));
                                                              expire  = createCheckBox(p, l, "Timer Expires", std::mem_fn(&ModifierBase::checked));
                                                              misfire = createCheckBox(p, l, "Can Misfire", std::mem_fn(&ModifierBase::checked));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              change->setChecked(s.mChange);
                                                              cond->setText(s.mCond);
                                                              active->setChecked(s.mActive);
                                                              act->setCurrentIndex(s.mAct);
                                                              reset->setCurrentIndex(s.mReset);
                                                              expire->setChecked(s.mExpire);
                                                              misfire->setChecked(s.mMisfire);
                                                            }
    void          store() override                          { v.mChange  = change->isChecked();
                                                              v.mCond    = cond->text();
                                                              v.mActive  = active->isChecked();
                                                              v.mAct     = act->currentIndex();
                                                              v.mReset   = reset->currentIndex();
                                                              v.mExpire  = expire->isChecked();
                                                              v.mMisfire = misfire->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]    = name();
                                                              obj["type"]    = type();
                                                              obj["adder"]   = isAdder();
                                                              obj["change"]  = v.mChange;
                                                              obj["cond"]    = v.mCond;
                                                              obj["active"]  = v.mActive;
                                                              obj["act"]     = v.mAct;
                                                              obj["reset"]   = v.mReset;
                                                              obj["expire"]  = v.mExpire;
                                                              obj["misfire"] = v.mMisfire;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v.mChange) f = Fraction(1, 2);
        if (v.mActive) f += Fraction(1, 4);
        f += (v.mAct - 1) * Fraction(1, 4);
        f += (v.mReset - 3) * Fraction(1, 4);
        if (v.mExpire) f -= Fraction(1, 4);
        if (v.mMisfire) f -= Fraction(1, 4);
        if (f < Fraction(1, 4)) f = Fraction(1, 4);
        return f;
    }

private:
    struct vars {
        int     mChange;
        QString mCond;
        bool    mActive;
        int     mAct;
        int     mReset;
        bool    mExpire;
        bool    mMisfire;
    } v {};

    QCheckBox* change = nullptr;
    QLineEdit* cond = nullptr;
    QCheckBox* active = nullptr;
    QComboBox* act = nullptr;
    QComboBox* reset = nullptr;
    QCheckBox* expire = nullptr;
    QCheckBox* misfire = nullptr;

    QString optOut(bool show) {
        if (v.mCond.isEmpty() || v.mAct < 1 || v.mReset < 1) return "<incomplete>";
        Fraction f(fraction(NoStore));
        QStringList reset { "", "Turn to", "Full Phase to", "Half Phase to", "0-Phase to", "Automatically" };
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "Trigger (";
        desc += v.mCond;
        if (v.mChange) desc += "; Can change conditions";
        if (v.mActive) desc += ": 0-Phase Act.";
        desc += QString("; %1 Activating Conditions").arg(v.mAct);
        desc += "; " + reset[v.mReset] + " reset";
        if (v.mExpire) desc += "; expires";
        if (v.mMisfire) desc += "; misfire";
        return desc + ")";
    }
};

class TurnMode: public NoFormModifier {
public:
    TurnMode()
        : NoFormModifier("Turn Mode", isLimitation, Fraction(1, 4)) { }
    TurnMode(QJsonObject json)
        : NoFormModifier(json) { }
    TurnMode(const TurnMode& m)
        : NoFormModifier(m) { }
    TurnMode(TurnMode&& m)
        : NoFormModifier(m) { }
    ~TurnMode() override { }

    TurnMode& operator=(const TurnMode&) = delete;
    TurnMode& operator=(TurnMode&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<TurnMode>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<TurnMode>(json); }
};

class Uncontrolled: public Modifier {
public:
    Uncontrolled()
        : Modifier("Uncontrolledϴ", isAdvantage, isModifier)
        , v({ "" }) { }
    Uncontrolled(QJsonObject json)
        : Modifier(json["name"].toString("Uncontrolledϴ"),
                   ModifierType(json["type"].toInt(0)),
                   json["addr"].toBool(isModifier)) { v.mUntil = json["until"].toString(); }
    Uncontrolled(const Uncontrolled& m)
        : Modifier(m)
        , v(m.v) { }
    Uncontrolled(Uncontrolled&& m)
        : Modifier(m)
        , v(m.v) { }
    ~Uncontrolled() override { }

    Uncontrolled& operator=(const Uncontrolled&) = delete;
    Uncontrolled& operator=(Uncontrolled&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<Uncontrolled>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<Uncontrolled>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { until = createLineEdit(p, l, "Until?", std::mem_fn(&ModifierBase::changed));
                                                              return true; }
    void          restore() override                        { vars s = v; until->setText(s.mUntil); v = s; }
    void          store() override                          { v.mUntil = until->text(); }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["until"] = v.mUntil;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        return Fraction(1, 2);
    }

private:
    struct vars {
        QString mUntil;
    } v {};

    QLineEdit* until = nullptr;

    QString optOut(bool show) {
        if (v.mUntil.isEmpty()) return "<incomplete>";
        Fraction f(fraction(NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "Uncontrolled Until " + v.mUntil;
        return desc;
    }
};

class UnifiedPower: public NoFormModifier {
public:
    UnifiedPower()
        : NoFormModifier("Unified Power▲", isLimitation, Fraction(1, 4)) { }
    UnifiedPower(QJsonObject json)
        : NoFormModifier(json) { }
    UnifiedPower(const UnifiedPower& m)
        : NoFormModifier(m) { }
    UnifiedPower(UnifiedPower&& m)
        : NoFormModifier(m) { }
    ~UnifiedPower() override { }

    UnifiedPower& operator=(const UnifiedPower&) = delete;
    UnifiedPower& operator=(UnifiedPower&&) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<UnifiedPower>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<UnifiedPower>(json); }
};

class UsableAsMovement: public Modifier {
public:
    UsableAsMovement()
        : Modifier("Usable As [Movement]", isAdvantage, isModifier)
        , v({ 0, "" }) { }
    UsableAsMovement(const UsableAsMovement& m)
        : Modifier(m)
        , v(m.v) { }
    UsableAsMovement(UsableAsMovement&& m)
        : Modifier(m)
        , v(m.v) { }
    UsableAsMovement(QJsonObject json)
        : Modifier(json["name"].toString("Usable As [Movement]"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mNum   = json["num"].toInt(0);
                                                  v.mWhich = json["which"].toString();
                                                }
    ~UsableAsMovement() override { }

    UsableAsMovement& operator=(const UsableAsMovement&) = delete;
    UsableAsMovement& operator=(UsableAsMovement&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<UsableAsMovement>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<UsableAsMovement>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { num   = createLineEdit(p, l, "Number of Movements?", std::mem_fn(&ModifierBase::numeric));
                                                              which = createLineEdit(p, l, "Which?", std::mem_fn(&ModifierBase::changed));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              num->setText(QString("%1").arg(s.mNum));
                                                              which->setText(s.mWhich);
                                                              v = s;
                                                            }
    void          store() override                          { v.mNum = num->text().toInt();
                                                              v.mWhich = which->text();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["num"]   = v.mNum;
                                                              obj["which"] = v.mWhich;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        return v.mNum * Fraction(1, 4);
    }

private:
    struct vars {
        int     mNum;
        QString mWhich;
    } v {};

    QLineEdit* num = nullptr;
    QLineEdit* which = nullptr;

    QString optOut(bool show) {
        if (v.mNum < 1 || v.mWhich.isEmpty()) return "<incomplete>";
        Fraction f(fraction(NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "Usable As " + v.mWhich;
        return desc;
    }
};

class UsableByOthers: public Modifier {
public:
    UsableByOthers()
        : Modifier("Usable On Othersϴ", isAdvantage, isModifier)
        , v({ 0, false, false, false, -1, false, -1, -1 }) { }
    UsableByOthers(const UsableByOthers& m)
        : Modifier(m)
        , v(m.v) { }
    UsableByOthers(UsableByOthers&& m)
        : Modifier(m)
        , v(m.v) { }
    UsableByOthers(QJsonObject json)
        : Modifier(json["name"].toString("Usable On Othersϴ"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mDoubles = json["double"].toInt(0);
                                                  v.mTen     = json["ten"].toBool(false);
                                                  v.mOne     = json["one"].toBool(false);
                                                  v.mForce   = json["force"].toBool(false);
                                                  v.mControl = json["control"].toInt(0);
                                                  v.mPays    = json["pays"].toBool(false);
                                                  v.mRange   = json["range"].toInt(0);
                                                  v.mStay    = json["stay"].toInt(0);
                                                }
    ~UsableByOthers() override { }

    UsableByOthers& operator=(const UsableByOthers&) = delete;
    UsableByOthers& operator=(UsableByOthers&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<UsableByOthers>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<UsableByOthers>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { doubles = createLineEdit(p, l, "How many doublings?", std::mem_fn(&ModifierBase::numeric));
                                                              ten     = createCheckBox(p, l, "Everyone with 10m can be recipient", std::mem_fn(&ModifierBase::checked));
                                                              one     = createCheckBox(p, l, "One at a time", std::mem_fn(&ModifierBase::checked));
                                                              force   = createCheckBox(p, l, "USable as Attack", std::mem_fn(&ModifierBase::checked));
                                                              control = createComboBox(p, l, "Who Controls?", { "Recipient", "Recipient, but Grantor Can Take Bacl", "Grantor" },
                                                                                       std::mem_fn(&ModifierBase::index));
                                                              pays    = createCheckBox(p, l, "Grantor pays END", std::mem_fn(&ModifierBase::checked));
                                                              range   = createComboBox(p, l, "Recipeint must be?", { "Within Reach", "Within Limited Range", "Within Standard Range" },
                                                                                       std::mem_fn(&ModifierBase::index));
                                                              stay    = createComboBox(p, l, "Recipeient?", { "Can Go Anywhere", "Must Stay Within Line-Of-Sight", "Must Remain Close" },
                                                                                       std::mem_fn(&ModifierBase::index));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              doubles->setText(QString("%1").arg(s.mDoubles));
                                                              ten->setChecked(s.mTen);
                                                              one->setChecked(s.mOne);
                                                              force->setChecked(s.mForce);
                                                              control->setCurrentIndex(s.mControl);
                                                              pays->setChecked(s.mPays);
                                                              range->setCurrentIndex(s.mRange);
                                                              stay->setCurrentIndex(s.mStay);
                                                              v = s;
                                                            }
    void          store() override                          { v.mDoubles = doubles->text().toInt(0);
                                                              v.mTen     = ten->isChecked();
                                                              v.mOne     = one->isChecked();
                                                              v.mForce   = force->isChecked();
                                                              v.mControl = control->currentIndex();
                                                              v.mPays    = pays->isChecked();
                                                              v.mRange   = range->currentIndex();
                                                              v.mStay    = stay->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]    = name();
                                                              obj["type"]    = type();
                                                              obj["adder"]   = isAdder();
                                                              obj["doubles"] = v.mDoubles;
                                                              obj["ten"]     = v.mTen;
                                                              obj["one"]     = v.mOne;
                                                              obj["force"]   = v.mForce;
                                                              obj["control"] = v.mControl;
                                                              obj["pays"]    = v.mPays;
                                                              obj["range"]   = v.mRange;
                                                              obj["stay"]    = v.mStay;
                                                              return obj;
                                                            }
    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v.mTen) f += Fraction(1);
        else f += v.mDoubles * Fraction(1, 4);
        if (v.mOne) f -= Fraction(1, 4);
        if (v.mForce) f += Fraction(1);
        f += v.mControl * Fraction(1, 4);
        if (v.mPays) f -= Fraction(1, 4);
        f += (v.mRange) * Fraction(1, 4);
        f -= (v.mStay) * Fraction(1, 4);
        if (f < Fraction(1, 4)) f = Fraction(1, 4);
        return f;
    }

private:
    struct vars {
        int  mDoubles;
        bool mTen;
        bool mOne;
        bool mForce;
        int  mControl;
        bool mPays;
        int  mRange;
        int  mStay;
    } v {};

    QLineEdit* doubles = nullptr;
    QCheckBox* ten = nullptr;
    QCheckBox* one = nullptr;
    QCheckBox* force = nullptr;
    QComboBox* control = nullptr;
    QCheckBox* pays = nullptr;
    QComboBox* range = nullptr;
    QComboBox* stay = nullptr;

    void numeric(QString) override {
        QString txt = doubles->text();
        if (txt.isEmpty() || isNumber(txt)) {
            if (txt.toInt(nullptr) < 0 && !txt.isEmpty()) doubles->undo();
            v.mDoubles = txt.toInt(nullptr);
            if (v.mDoubles < 0) v.mDoubles = 0;
            return;
        }
        doubles->undo();
    }

    QString optOut(bool show) {
        if (v.mDoubles < 0) return "<incomplete>";
        Fraction f(fraction(NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "");
        if (v.mForce) desc += "Usable Against Othersϴ";
        else desc += "Usable On Othersϴ";
        desc += "(x" + QString("%1").arg((int) pow(2.0, v.mDoubles)) + " targets"; // NOLINT
        if (v.mTen) desc += "; Anyone In 10m";
        if (v.mOne) desc += "; One At A Time";
        switch (v.mControl) {
        case 0: desc += "; Recipient Controls";                        break;
        case 1: desc += "; Recipient Controls But Grantor Can Revoke"; break;
        case 2: desc += "; Grantor Controls";                          break;
        }
        if (v.mPays) desc += "; Grantor Pays END";
        else desc += "; Receipient Pays END";
        switch (v.mRange) {
        case 0: desc += "; Must Be In Reach";         break;
        case 1: desc += "; Must Be In Limited Range"; break;
        case 2: desc += "; Must Be In Range";         break;
        }
        desc += " When Granted";
        switch (v.mStay) {
        case 1: desc += "; Must Stay In LIne-Of-Sight"; break;
        case 2: desc += "; Must stay In Range";         break;
        }

        return desc + ")";
    }
};

class VariableAdvantage: public Modifier {
public:
    VariableAdvantage()
        : Modifier("Variable Advantageϴ", isAdvantage, isModifier)
        , v({ 0, false, false, false, "" }) { }
    VariableAdvantage(const VariableAdvantage& m)
        : Modifier(m)
        , v(m.v) { }
    VariableAdvantage(VariableAdvantage&& m)
        : Modifier(m)
        , v(m.v) { }
    VariableAdvantage(QJsonObject json)
        : Modifier(json["name"].toString("Variable Advantageϴ"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mWhole   = json["whole"].toInt(0);
                                                  v.mHalf    = json["half"].toBool(false);
                                                  v.mQuarter = json["quarter"].toBool(false);
                                                  v.mLimit   = json["limit"].toBool(false);
                                                  v.mAdvs    = json["advs"].toString();
                                                }
    ~VariableAdvantage() override { }

    VariableAdvantage& operator=(const VariableAdvantage&) = delete;
    VariableAdvantage& operator=(VariableAdvantage&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<VariableAdvantage>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<VariableAdvantage>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { whole   = createLineEdit(p, l, "Whole part?", std::mem_fn(&ModifierBase::numeric));
                                                              half    = createCheckBox(p, l, "+" + Fraction(1, 2).toString(), std::mem_fn(&ModifierBase::checked));
                                                              quarter = createCheckBox(p, l, "+" + Fraction(1, 4).toString(), std::mem_fn(&ModifierBase::checked));
                                                              limit   = createCheckBox(p, l, "Only from this list:", std::mem_fn(&ModifierBase::checked));
                                                              advs    = createLineEdit(p, l, "Advantages?", std::mem_fn(&ModifierBase::changed));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              whole->setText(QString("%1").arg(s.mWhole));
                                                              half->setChecked(s.mHalf);
                                                              quarter->setChecked(s.mQuarter);
                                                              limit->setChecked(s.mLimit);
                                                              advs->setText(s.mAdvs);
                                                              v = s;
                                                            }
    void          store() override                          { v.mWhole   = whole->text().toInt(0);
                                                              v.mHalf    = half->isChecked();
                                                              v.mQuarter = quarter->isChecked();
                                                              v.mLimit   = limit->isChecked();
                                                              v.mAdvs    = advs->text();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]    = name();
                                                              obj["type"]    = type();
                                                              obj["adder"]   = isAdder();
                                                              obj["whole"]   = v.mWhole;
                                                              obj["half"]    = v.mHalf;
                                                              obj["quarter"] = v.mQuarter;
                                                              obj["limit"]   = v.mLimit;
                                                              obj["advs"]    = v.mAdvs;
                                                              return obj;
                                                            }
    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(v.mWhole);
        if (v.mHalf) f += Fraction(1, 2);
        if (v.mQuarter) f -= Fraction(1, 4);
        if (v.mLimit) f -= Fraction(1, 4);
        f = f * 2;
        if (f < Fraction(1, 4)) f = Fraction(1, 4);
        return f;
    }

private:
    struct vars {
        int     mWhole;
        bool    mHalf;
        bool    mQuarter;
        bool    mLimit;
        QString mAdvs;
    } v {};

    QLineEdit* whole = nullptr;
    QCheckBox* half = nullptr;
    QCheckBox* quarter = nullptr;
    QCheckBox* limit = nullptr;
    QLineEdit* advs = nullptr;

    QString optOut(bool show) {
        if (v.mWhole < 0 || (v.mLimit && v.mAdvs.isEmpty())) return "<incomplete>";
        Fraction f(fraction(NoStore));
        Fraction amt(v.mWhole);
        if (v.mHalf) amt += Fraction(1, 2);
        if (v.mQuarter) amt += Fraction(1, 4);
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "");
        desc += "Variable Advantageϴ";
        desc += " (+" + amt.toString() + " in Advantages";
        if (v.mLimit) desc += "; Only From: " + v.mAdvs;
        return desc + ")";
    }
};

class VariableEffect: public Modifier {
public:
    VariableEffect()
        : Modifier("Variable Effect▲", isAdvantage, isModifier)
        , v({ "" }) { }
    VariableEffect(const VariableEffect& m)
        : Modifier(m)
        , v(m.v) { }
    VariableEffect(VariableEffect&& m)
        : Modifier(m)
        , v(m.v) { }
    VariableEffect(QJsonObject json)
        : Modifier(json["name"].toString("Variable Effect▲"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mEffect = json["effect"].toString(); }
    ~VariableEffect() override { }

    VariableEffect& operator=(const VariableEffect&) = delete;
    VariableEffect& operator=(VariableEffect&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<VariableEffect>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<VariableEffect>(json); }

    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { effect = createLineEdit(p, l, "Effects?", std::mem_fn(&ModifierBase::changed));
                                                              return true; }
    void          restore() override                        { vars s = v; effect->setText(s.mEffect); v = s; }
    void          store() override                          { v.mEffect = effect->text(); }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]   = name();
                                                              obj["type"]   = type();
                                                              obj["adder"]  = isAdder();
                                                              obj["effect"] = v.mEffect;
                                                              return obj;
                                                            }
    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        return Fraction(1, 2);
    }

private:
    struct vars {
        QString mEffect;
    } v {};

    QLineEdit* effect = nullptr;

    QString optOut(bool show) {
        if (v.mEffect.isEmpty()) return "<incomplete>";
        Fraction f(fraction(NoStore));
        return (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "Variable Effect▲ (versus " + v.mEffect + ")";
    }
};

class VariableLimitations: public Modifier {
public:
    VariableLimitations()
        : Modifier("Variable Limitations▲", isAdvantage, isModifier)
        , v({ 0, false, false }) { }
    VariableLimitations(const VariableLimitations& m)
        : Modifier(m)
        , v(m.v) { }
    VariableLimitations(VariableLimitations&& m)
        : Modifier(m)
        , v(m.v) { }
    VariableLimitations(QJsonObject json)
        : Modifier(json["name"].toString("Variable Limitations▲"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mWhole   = json["whole"].toInt(0);
                                                  v.mHalf    = json["half"].toBool(false);
                                                  v.mQuarter = json["quarter"].toBool(false);
                                                }
    ~VariableLimitations() override { }

    VariableLimitations& operator=(const VariableLimitations&) = delete;
    VariableLimitations& operator=(VariableLimitations&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<VariableLimitations>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<VariableLimitations>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { whole   = createLineEdit(p, l, "Whole part?", std::mem_fn(&ModifierBase::numeric));
                                                              half    = createCheckBox(p, l, "+" + Fraction(1, 2).toString(), std::mem_fn(&ModifierBase::checked));
                                                              quarter = createCheckBox(p, l, "+" + Fraction(1, 4).toString(), std::mem_fn(&ModifierBase::checked));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              whole->setText(QString("%1").arg(s.mWhole));
                                                              half->setChecked(s.mHalf);
                                                              quarter->setChecked(s.mQuarter);
                                                              v = s;
                                                            }
    void          store() override                          { v.mWhole   = whole->text().toInt(0);
                                                              v.mHalf    = half->isChecked();
                                                              v.mQuarter = quarter->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]    = name();
                                                              obj["type"]    = type();
                                                              obj["adder"]   = isAdder();
                                                              obj["whole"]   = v.mWhole;
                                                              obj["half"]    = v.mHalf;
                                                              obj["quarter"] = v.mQuarter;
                                                              return obj;
                                                            }
    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(v.mWhole);
        if (v.mHalf) f += Fraction(1, 2);
        if (v.mQuarter) f -= Fraction(1, 4);
        f = f * 2;
        return f;
    }

private:
    struct vars {
        int     mWhole;
        bool    mHalf;
        bool    mQuarter;
    } v {};

    QLineEdit* whole = nullptr;
    QCheckBox* half = nullptr;
    QCheckBox* quarter = nullptr;

    QString optOut(bool show) {
        if (v.mWhole < 0) return "<incomplete>";
        Fraction f(fraction(NoStore));
        Fraction amt(v.mWhole);
        if (v.mHalf) amt += Fraction(1, 2);
        if (v.mQuarter) amt += Fraction(1, 4);
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "");
        desc += "Variable Limitations▲ (+" + amt.toString() + " in Limitations" + ")";
        return desc;
    }
};

class VariableSpecialEffects: public Modifier {
public:
    VariableSpecialEffects()
        : Modifier("Variable Special Effects", isAdvantage, isModifier)
        , v({ "" }) { }
    VariableSpecialEffects(const VariableSpecialEffects& m)
        : Modifier(m)
        , v(m.v) { }
    VariableSpecialEffects(VariableSpecialEffects&& m)
        : Modifier(m)
        , v(m.v) { }
    VariableSpecialEffects(QJsonObject json)
        : Modifier(json["name"].toString("Variable Special Effects"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mEffect = json["effect"].toString();
                                                }
    ~VariableSpecialEffects() override { }

    VariableSpecialEffects& operator=(const VariableSpecialEffects&) = delete;
    VariableSpecialEffects& operator=(VariableSpecialEffects&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<VariableSpecialEffects>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<VariableSpecialEffects>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { effect = createLineEdit(p, l, "Special Effects?", std::mem_fn(&ModifierBase::changed));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              effect->setText(s.mEffect);
                                                              v = s;
                                                            }
    void          store() override                          { v.mEffect = effect->text();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]   = name();
                                                              obj["type"]   = type();
                                                              obj["adder"]  = isAdder();
                                                              obj["effect"] = v.mEffect;
                                                              return obj;
                                                            }
    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        if (v.mEffect.isEmpty()) return Fraction(1, 2);
        return Fraction(1, 4);
    }

private:
    struct vars {
        QString mEffect;
    } v {};

    QLineEdit* effect = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "Variable Special Effects";
        if (!v.mEffect.isEmpty()) desc += " (from:  " + v.mEffect + ")";
        return desc;
    }
};

class WorksAgainstEGONotCharacteristic: public Modifier {
public:
    WorksAgainstEGONotCharacteristic()
        : Modifier("Works Against EGO Not [Characteristic]▲", isAdvantage, isModifier)
        , v({ -1 }) { }
    WorksAgainstEGONotCharacteristic(const WorksAgainstEGONotCharacteristic& m)
        : Modifier(m)
        , v(m.v) { }
    WorksAgainstEGONotCharacteristic(WorksAgainstEGONotCharacteristic&& m)
        : Modifier(m)
        , v(m.v) { }
    WorksAgainstEGONotCharacteristic(QJsonObject json)
        : Modifier(json["name"].toString("Works Against EGO Not [Characteristic]▲"),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false)) { v.mCharacteristic  = json["characteristic"].toInt(0);
                                                }
    ~WorksAgainstEGONotCharacteristic() override { }

    WorksAgainstEGONotCharacteristic& operator=(const WorksAgainstEGONotCharacteristic&) = delete;
    WorksAgainstEGONotCharacteristic& operator=(WorksAgainstEGONotCharacteristic&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<WorksAgainstEGONotCharacteristic>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<WorksAgainstEGONotCharacteristic>(json); }

    void          checked(bool) override                    { store(); ModifiersDialog::ref().updateForm(); }
    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    void          index(int) override                       { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { characteristic = createComboBox(p, l, "Characteristic?", { "", "STR", "DEX", "INT", "EGO", "PRE", "OCV", "DCV", "OMCV", "DMCV",
                                                                                                                         "SPD", "PD", "ED", "REC", "END", "BODY", "STUN" },
                                                                                     std::mem_fn(&ModifierBase::index));
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              characteristic->setCurrentIndex(s.mCharacteristic);
                                                              v = s;
                                                            }
    void          store() override                          { v.mCharacteristic = characteristic->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]           = name();
                                                              obj["type"]           = type();
                                                              obj["adder"]          = isAdder();
                                                              obj["characteristic"] = v.mCharacteristic;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        return Fraction(1, 4);
    }

private:
    struct vars {
        int mCharacteristic;
    } v {};

    QComboBox* characteristic = nullptr;

    QString optOut(bool show) {
        if (v.mCharacteristic < 1) return "<incomplete>";
        Fraction f(fraction(NoStore));
        QStringList characteristic = { "", "STR", "DEX", "INT", "EGO", "PRE", "OCV", "DCV", "OMCV", "DMCV",
                                       "SPD", "PD", "ED", "REC", "END", "BODY", "STUN" };
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + QString("Works Against EGO Not %1▲").arg(characteristic[v.mCharacteristic]);
        return desc;
    }
};

#endif // MODIFIER_H
