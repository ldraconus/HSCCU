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
        : _name("")
        , _type(isLimitation)
        , _adder(isModifier)
        , _base(nullptr) { }
    ModifierBase(QString nm, ModifierType type, bool adder, base* base = nullptr)
        : _name(nm)
        , _type(type)
        , _adder(adder)
        , _base(base) { }
    ModifierBase(const ModifierBase& m)
        : _name(m._name)
        , _type(m._type)
        , _adder(m._adder)
        , _base(m._base) { }
    ModifierBase(const ModifierBase* m)
        : _name(m->_name)
        , _type(m->_type)
        , _adder(m->_adder)
        , _base(m->_base) { }
    ModifierBase(ModifierBase&& m)
        : _name(m._name)
        , _type(m._type)
        , _adder(m._adder)
        , _base(m._base) { }

    ModifierBase& operator=(const ModifierBase& m) = delete;
    ModifierBase& operator=(ModifierBase&&) = delete;

    virtual ~ModifierBase() { }

    QWidget* _sender = nullptr; // NOLINT

    bool             isAdder() const  { return _adder; }
    base             modifier() const { return _base; }
    ModifierType     type() const     { return _type; }
    QString          name() const     { return _name; }

    void callback(QCheckBox*);
    void callback(QLineEdit*);
    void callback(QTreeWidget*);
    void callback(QComboBox*);

    virtual shared_ptr<class Modifier> create();
    virtual shared_ptr<class Modifier> create(const QJsonObject&);

    virtual bool          createForm(QWidget*, QVBoxLayout*) { return false; }
    virtual QString       description(bool show = false)     { return show ? "<incomplete>" : "<incomplete>"; }

private:
    QString       _name = "";
    ModifierType  _type = isLimitation;
    bool          _adder = false;
    base*         _base = nullptr;

 protected:
    QMap<QCheckBox*,   std::_Mem_fn<void (ModifierBase::*)(bool)>>            _callbacksCB; // NNOLINT
    QMap<QComboBox*,   std::_Mem_fn<void (ModifierBase::*)(int)>>             _callbacksCBox; // NOLINT
    QMap<QLineEdit*,   std::_Mem_fn<void (ModifierBase::*)(QString)>>         _callbacksEdit; // NOLINT
    QMap<QTreeWidget*, std::_Mem_fn<void (ModifierBase::*)(int, int, bool)>>  _callbacksTree; // NOLINT
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
    gsl::owner<QCheckBox*>   createCheckBox(QWidget*, QVBoxLayout*, QString, std::_Mem_fn<void (ModifierBase::*)(bool)>);
    gsl::owner<QCheckBox*>   createCheckBox(QWidget*, QVBoxLayout*, QString);
    gsl::owner<QComboBox*>   createComboBox(QWidget*, QVBoxLayout*, QString, QList<QString>, std::_Mem_fn<void (ModifierBase::*)(int)>);
    gsl::owner<QComboBox*>   createComboBox(QWidget*, QVBoxLayout*, QString, QList<QString>);
    gsl::owner<QLabel*>      createLabel(QWidget*, QVBoxLayout*, QString);
    gsl::owner<QLineEdit*>   createLineEdit(QWidget*, QVBoxLayout*, QString, std::_Mem_fn<void (ModifierBase::*)(QString)>);
    gsl::owner<QLineEdit*>   createLineEdit(QWidget*, QVBoxLayout*, QString);
    gsl::owner<QTreeWidget*> createTreeWidget(QWidget*, QVBoxLayout*, QMap<QString, QStringList>, std::_Mem_fn<void (ModifierBase::*)(int, int, bool)>);
    gsl::owner<QTreeWidget*> createTreeWidget(QWidget*, QVBoxLayout*, QMap<QString, QStringList>);

    QJsonArray toArray(QStringList list) {
        QJsonArray array;
        for (const auto& str: list) array.append(str);
        return array;
    }
    QStringList toStringList(QJsonArray array) {
        QStringList list;
        for (const auto& str: array) list.append(str.toString());
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

    QWidget* sender() const { return _sender; }

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

    QString  description(bool show = false) override { return ModifierBase::description(show); }
    virtual Fraction fraction(bool noStore = false)          { return noStore ? Fraction(0) : Fraction(0); }
    virtual Points   points(bool noStore = false)            { return noStore ? 0_cp : 0_cp; }
    virtual Fraction endChange()                             { return Fraction(1); }

    static void ClearForm(QVBoxLayout* layout);

    bool isNumber(QString);
};

class Modifiers {
private:
    static QMap<QString, shared_ptr<Modifier>> _modifiers; // NOLINT

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
        , _value(v)
        , _points(0_cp) { }
    NoFormModifier(QString n, ModifierType m, Points p)
        : Modifier(n, m, isAnAdder)
        , _value(Fraction(0))
        , _points(p) { }
    NoFormModifier(const NoFormModifier& m)
        : Modifier(m)
        , _value(m._value)
        , _points(m._points) { }
    NoFormModifier(NoFormModifier&& m)
        : Modifier(m)
        , _value(m._value)
        , _points(m._points) { }
    NoFormModifier(QJsonObject json)
        : Modifier(json["name"].toString(""),
                   ModifierType(json["type"].toInt(0)),
                   json["adder"].toBool(false))
        , _value(Fraction(json["value"].toArray()[0].toInt(0), json["value"].toArray()[1].toInt(1)))
        , _points(json["points"].toInt()) { }
    ~NoFormModifier() override { }

    NoFormModifier& operator=(const NoFormModifier& n) = delete;
    NoFormModifier& operator=(NoFormModifier&& n) = delete;

    shared_ptr<Modifier> create() override                        { return make_shared<NoFormModifier>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override { return make_shared<NoFormModifier>(json); }

    QString       description(bool show = false) override  { return QString(show ? fraction(Modifier::NoStore).toString() + " ": "") + name(); }
    Fraction      fraction(bool noStore = false) override  { return noStore ? _value : _value; }
    Points        points(bool noStore = false) override    { return noStore ? _points : _points; }
    void          restore() override                       { }
    void          store() override                         { }
    QJsonObject   toJson() override                        { QJsonObject obj;
                                                             obj["name"]  = name();
                                                             obj["type"]  = type();
                                                             obj["adder"] = isAdder();
                                                             QJsonArray arr;
                                                             arr.append((int) _value.numerator());
                                                             arr.append((int) _value.denominator());
                                                             obj["value"] = arr;
                                                             obj["points"] = (int) _points.points;
                                                             return obj;
                                                           }
private:
    Fraction _value;
    Points _points;
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

    gsl::owner<QCheckBox*> aquiresRoll = nullptr;

    QString optOut(bool show) {
        Fraction half(1, 2);
        if (v._aquiresRoll) return QString(show ? "(-1) " : "") + "Ablative, Aquires a Skill Roll When Exceeded";
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
                   json["adder"].toBool(false)) { v._sense = json["sense"].toInt(0); }
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
                                                              sense->setCurrentIndex(s._sense);
                                                              v = s; }
    void          store() override                          { v._sense = sense->currentIndex(); }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"] = name();
                                                              obj["type"] = type();
                                                              obj["adder"] = isAdder();
                                                              obj["sense"] = v._sense;
                                                              return obj; }

private:
    struct vars {
        int _sense = 0;
    } v;

    gsl::owner<QComboBox*> sense = nullptr;

    QString optOut(bool show) {
        Fraction quarter(1, 4);
        if (v._sense < 1) return "<incomplete>";
        QStringList sense { "", "Hearing", "Mental", "Radio", "Sight", "Smell/Taste", "Touch" };
        return (show ? "(-" + quarter.toString() + ") " : "") + "Affected As Another Sense (" + sense[v._sense] + ")";
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
                   json["adder"].toBool(false)) { v._veryUncommon = json["veryUncommon"].toBool();
                                                  v._senses       = json["senses"].toString(""); }
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
                                                              return v._veryUncommon ? Fraction(1, 2) : Fraction(1, 4); }
    void          restore() override                        { vars s = v;
                                                              veryUncommon->setChecked(s._veryUncommon);
                                                              senses->setText(s._senses);
                                                              v = s; }
    void          store() override                          { v._senses       = senses->text();
                                                              v._veryUncommon = veryUncommon->isChecked(); }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]         = name();
                                                              obj["type"]         = type();
                                                              obj["adder"]        = isAdder();
                                                              obj["senses"]       = v._senses;
                                                              obj["veryUncommon"] = v._veryUncommon;
                                                              return obj; }

private:
    struct vars {
        bool    _veryUncommon = false;
        QString _senses = "";
    } v;

    gsl::owner<QCheckBox*> veryUncommon = nullptr;
    gsl::owner<QLineEdit*> senses = nullptr;

    QString optOut(bool show) {
        Fraction quarter(1, 4);
        Fraction half(1, 2);
        if (v._senses.isEmpty()) return "<incomplete>";
        return (show ? "(-" + (v._veryUncommon ? half.toString() : quarter.toString()) + ") " : "") + "Affected As More Than One Sense: " + v._senses +
                (v._veryUncommon ? " (Senses are very uncommon)" : "");
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
                   json["adder"].toBool(false)) { v._limited = json["limited"].toBool();
                                                  v._what    = json["what"].toString(""); }
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
                                                              return v._limited ? Fraction(1, 4) : Fraction(1, 2); }
    void          restore() override                        { vars s = v;
                                                              limited->setChecked(s._limited);
                                                              what->setText(s._what);
                                                              v = s; }
    void          store() override                          { v._what    = what->text();
                                                              v._limited = limited->isChecked(); }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]    = name();
                                                              obj["type"]    = type();
                                                              obj["adder"]   = isAdder();
                                                              obj["limited"] = v._limited;
                                                              obj["what"]    = v._what;
                                                              return obj; }

private:
    struct vars {
        bool    _limited = false;
        QString _what = "";
    } v;

    gsl::owner<QCheckBox*> limited = nullptr;
    gsl::owner<QLineEdit*> what = nullptr;

    QString optOut(bool show) {
        Fraction quarter(1, 4);
        Fraction half(1, 2);
        if (v._what.isEmpty() && v._limited) return "<incomplete>";
        return (show ? "(+" + (v._limited ? quarter.toString() : half.toString()) + ") " : "") + "Affects Desolid" +
                (v._limited ? " (only versus " + v._what + ")" : "");
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
                   json["adder"].toBool(false)) { v._is = json["is"].toInt(0); }
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
                                                              switch (v._is) {
                                                              case 0:  return Fraction(1, 4);
                                                              case 1:  return Fraction(-1, 4);
                                                              case 2:  return Fraction(0, 1);
                                                              case 3:  return Fraction(1, 4);
                                                              default: return Fraction(0, 1);
                                                              }
                                                            }
    void          restore() override                        { vars s = v;
                                                              is->setCurrentIndex(s._is);
                                                              v = s; }
    void          store() override                          { v._is = is->currentIndex(); }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["is"]    = v._is;
                                                              return obj; }

private:
    struct vars {
        int    _is = 0;
    } v;

    gsl::owner<QComboBox*> is = nullptr;

    QString optOut(bool show) {
        QStringList is = {
            "",
            "Mental Power uses OCV instead of OMCV",
            "Mental Power attacks against DCV instead of DMCV",
            "Non-Mental Power uses OMCV instead of OCV",
            "Non-Mental Power attacks against DMCV instead of DCV"
        };
        if (v._is < 1) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        return (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "ACV▲: " + is[v._is];
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
                   json["adder"].toBool(false)) { v._kind = json["kind"].toInt(0);
                                                  v._explosion = json["explosion"].toBool(false);
                                                  v._fixedShape = json["fixedShape"].toBool(false);
                                                  v._shape = json["shape"].toString("");
                                                  v._nonselective = json["nonselective"].toBool(false);
                                                  v._selective = json["selective"].toBool(false);
                                                  v._mobile = json["mobile"].toBool(false);
                                                  v._accurate = json["accurate"].toBool(false);
                                                  v._thinCone = json["thinCone"].toBool(false);
                                                  v._damageShield = json["damageShield"].toBool(false);
                                                  v._multiplier = json["multiplier"].toInt(0);
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
                                                              kind->setCurrentIndex(s._kind);
                                                              fixedShape->setChecked(s._fixedShape);
                                                              shape->setText(s._shape);
                                                              explosion->setChecked(s._explosion);
                                                              nonselective->setChecked(s._nonselective);
                                                              selective->setChecked(s._selective);
                                                              mobile->setChecked(s._mobile);
                                                              accurate->setChecked(s._accurate);
                                                              thinCone->setChecked(s._thinCone);
                                                              damageShield->setChecked(s._damageShield);
                                                              multiplier->setText(QString("%1").arg(s._multiplier));
                                                              v = s;
                                                            }
    void          store() override                          { v._kind = kind->currentIndex();
                                                              v._fixedShape = fixedShape->isChecked();
                                                              v._shape = shape->text();
                                                              v._explosion = explosion->isChecked();
                                                              v._nonselective = nonselective->isChecked();
                                                              v._selective = selective->isChecked();
                                                              v._mobile = mobile->isChecked();
                                                              v._accurate = mobile->isChecked();
                                                              v._thinCone = thinCone->isChecked();
                                                              v._damageShield = damageShield->isChecked();
                                                              v._multiplier = multiplier->text().toInt(0);
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["kind"]  = v._kind;
                                                              obj["fixedShape"] = v._fixedShape;
                                                              obj["shape"] = v._shape;
                                                              obj["explosion"] = v._explosion;
                                                              obj["nonselective"] = v._nonselective;
                                                              obj["selective"] = v._selective;
                                                              obj["mobile"] = v._mobile;
                                                              obj["accurate"] = v._accurate;
                                                              obj["thinCone"] = v._thinCone;
                                                              obj["damageShield"] = v._damageShield;
                                                              obj["multiplier"] = v._multiplier;
                                                              return obj;
                                                            }
    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        int steps = 0;
        if (v._multiplier < 1) v._multiplier = 1;
        switch (v._kind) {
        case 0: steps = (int) (log((double) (v._multiplier - 1) / 4 + 1)  / log(2.0)); break; // NOLINT
        case 1: steps = (int) (log((double) (v._multiplier - 1) / 8 + 1)  / log(2.0)); break; // NOLINT
        case 2: steps = (int) (log((double) (v._multiplier - 1) / 16 + 1) / log(2.0)); break; // NOLINT
        case 3: steps = (int) (log((double) (v._multiplier - 1) / 2 + 1)  / log(2.0)); break; // NOLINT
        case 4: steps = (int) (log((double) (v._multiplier - 1) / 2 + 1)  / log(2.0)); break; // NOLINT
        }
        f *= steps + 1;
        if (v._explosion) f -= Fraction(1, 2);
        if (v._nonselective) f -= Fraction(1, 4);
        if (v._selective) f += Fraction(1, 4);
        if (v._mobile) f += Fraction(1, 2);
        if (v._accurate) f += Fraction(1, 4);
        if (v._thinCone) f -= Fraction(1, 4);
        if (v._fixedShape) f -= Fraction(1, 4);
        if (f < Fraction(1, 4)) f = Fraction(1, 4);
        return f;
    }

private:
    struct vars {
        int     _kind = 0;
        bool    _fixedShape = false;
        QString _shape = "";
        bool    _explosion = false;
        bool    _nonselective = false;
        bool    _selective = false;
        bool    _mobile = false;
        bool    _accurate = false;
        bool    _thinCone = false;
        bool    _damageShield = false;
        int     _multiplier = 0;
    } v;

    gsl::owner<QComboBox*> kind = nullptr;
    gsl::owner<QCheckBox*> fixedShape = nullptr;
    gsl::owner<QLineEdit*> shape = nullptr;
    gsl::owner<QCheckBox*> explosion = nullptr;
    gsl::owner<QCheckBox*> nonselective = nullptr;
    gsl::owner<QCheckBox*> selective = nullptr;
    gsl::owner<QCheckBox*> mobile = nullptr;
    gsl::owner<QCheckBox*> accurate = nullptr;
    gsl::owner<QCheckBox*> thinCone = nullptr;
    gsl::owner<QCheckBox*> damageShield = nullptr;
    gsl::owner<QLineEdit*> multiplier = nullptr;

    void changed(QString) override {
        QString txt = multiplier->text();
        if (txt.isEmpty() || isNumber(txt)) {
            if (txt.toInt(nullptr) < 1 && !txt.isEmpty()) multiplier->undo();
            v._multiplier = txt.toInt(nullptr);
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
            multiplier->setText(QString("%1").arg(v._multiplier));
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
        if (v._kind < 0 || (v._fixedShape && v._shape.isEmpty())) return "<incomplete>";
        if (v._multiplier < 1) v._multiplier = 1;
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "Area Of Effect: " + kind[v._kind];
        if (v._kind == 4) desc += QString(" (%1x%1%2)").arg(v._multiplier).arg(size[v._kind]);
        else desc += QString(" (%1%2)").arg(v._multiplier).arg(size[v._kind]);
        if (v._fixedShape) desc += "; Fixed Shape(" + v._shape + ")";
        if (v._accurate) desc += "; Accurate";
        if (v._explosion) desc += "; Explosion";
        if (v._mobile) desc += "; Mobile";
        if (v._nonselective) desc += "; Non-selective Targeting";
        if (v._selective) desc += "; Selective Targeting";
        if (v._thinCone) desc += "; Thin Cone";
        if (v._damageShield) desc += "; Damage Shield";
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
                   json["adder"].toBool(false)) { v._times = json["times"].toInt(0);
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
                                                              times->setText(QString("%1").arg(s._times));
                                                              v = s;
                                                            }
    void          store() override                          { v._times = times->text().toInt(0);
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["times"] = v._times;
                                                              return obj;
                                                            }
    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        f *= v._times;
        return f;
    }

private:
    struct vars {
        int _times = 0;
    } v;

    QLineEdit* times = nullptr;

    void changed(QString) override {
        QString txt = times->text();
        if (txt.isEmpty() || isNumber(txt)) {
            if (txt.toInt(nullptr) < 1 && !txt.isEmpty()) times->undo();
            v._times = txt.toInt(nullptr);
            if (v._times < 1) v._times = 1;
            return;
        }
        times->undo();
    }

    QString optOut(bool show) {
        if (v._times < 0) return "<incomplete>";
        Fraction f(fraction(NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                ((v._times > 1) ? QString("%1x").arg(v._times) : "") +"Armor Piercing";
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
                                                  v._newone = json["newone"].toInt(0);
                                                  v._versus = json["versus"].toString();
                                                  v._nnd = json["nnd"].toBool(false);
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
                                                              newone->setCurrentIndex(s._newone);
                                                              versus->setText(s._versus);
                                                              nnd->setChecked(s._nnd);
                                                              v = s;
                                                            }
    void          store() override                          { v._original = original->currentIndex();
                                                              v._newone = newone->currentIndex();
                                                              v._versus = versus->text();
                                                              v._nnd = nnd->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["original"] = v._original;
                                                              obj["newone"] = v._newone;
                                                              obj["versus"] = v._versus;
                                                              obj["nnd"] = v._nnd;
                                                              return obj;
                                                            }
    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 2);
        int diff = v._newone - v._original;
        f *= diff;
        if (v._nnd) f -= Fraction(1, 2);
        return f;
    }

private:
    struct vars {
        int _original = 0;
        int _newone = 0;
        QString _versus = "";
        bool _nnd = false;
    } v;

    gsl::owner<QComboBox*> original = nullptr;
    gsl::owner<QComboBox*> newone = nullptr;
    gsl::owner<QLineEdit*> versus = nullptr;
    gsl::owner<QCheckBox*> nnd = nullptr;

    QString optOut(bool show) {
        if (v._original < 1 || v._newone < 1 || v._versus.isEmpty()) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 + (v._nnd ? "NND: Defense is " : "Attack Versus Alternate Defense: ") + v._versus;
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
                   json["adder"].toBool(false)) { v._five = json["five"].toBool(false);
                                                  v._doubling = json["doubling"].toInt(0);
                                                  v._bypass = json["bypass"].toBool(false);
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
                                                              five->setChecked(s._five);
                                                              doubling->setText(QString("%1").arg(s._doubling));
                                                              bypass->setChecked(s._bypass);
                                                              v = s;
                                                            }
    void          store() override                          { v._five = five->isChecked();
                                                              v._doubling = doubling->text().toInt(nullptr);
                                                              v._bypass = bypass->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["five"] = v._five;
                                                              obj["doubling"] = v._doubling;
                                                              obj["bypass"] = v._bypass;
                                                              return obj;
                                                            }
    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v._five) f = Fraction(1, 2);
        if (v._doubling > 0) f += Fraction(1, 2) * v._doubling;
        if (v._bypass) f += 1;
        return f;
    }

private:
    struct vars {
        bool _five = false;
        int _doubling = 0;
        bool _bypass = false;
    } v;

    gsl::owner<QCheckBox*> five = nullptr;
    gsl::owner<QLineEdit*> doubling = nullptr;
    gsl::owner<QCheckBox*> bypass = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 QString("Autofire: %1 shots").arg((3 + 5) * std::pow(2, v._doubling)); // NOLINT
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
                   json["adder"].toBool(false)) { v._value = json["value"].toInt(0);
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
                                                              value->setCurrentIndex(s._value);
                                                              v = s;
                                                            }
    void          store() override                          { v._value = value->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["value"] = v._value;
                                                              return obj;
                                                            }
    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        static QList<Fraction> fractions { { 0, 1 }, { 1, 4 }, { 1, 2 }, { 3, 4 }, { 1, 1 }, { 1, { 1, 4 } }, { 1, { 1, 2 } }, { 1, { 3, 4 } }, { 2, 1 } };
        return fractions[v._value];
    }

private:
    struct vars {
        int _value = 0;
    } v;

    gsl::owner<QComboBox*> value = nullptr;

    QString optOut(bool show) {
        static QStringList values { "", "¹⁄₄⁄", "¹⁄₂⁄", "³⁄₄", "1", "1¹⁄₄⁄", "1¹⁄₂⁄", "1³⁄₄", "2" };

        if (v._value < 1) return "<incomplete>";
        return (show ? QString("(%1").arg(values[v._value]) + ") " : "") + "Can Only Be Used Through Mind Link";
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
                   json["adder"].toBool(false)) { v._veryEffective = json["veryEffective"].toBool();
                                                  v._maneuver = json["maneuver"].toString();
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
                                                              return v._veryEffective ? Fraction(1, 2) : Fraction(1, 4); }
    void          restore() override                        { vars s = v;
                                                              veryEffective->setChecked(s._veryEffective);
                                                              maneuver->setText(s._maneuver);
                                                              v = s; }
    void          store() override                          { v._veryEffective = veryEffective->isChecked();
                                                              v._maneuver = maneuver->text();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]          = name();
                                                              obj["type"]          = type();
                                                              obj["adder"]         = isAdder();
                                                              obj["maneuver"]      = v._maneuver;
                                                              obj["veryEffective"] = v._veryEffective;
                                                              return obj; }

private:
    struct vars {
        bool    _veryEffective = false;
        QString _maneuver = "";
    } v;

    gsl::owner<QCheckBox*> veryEffective = nullptr;
    gsl::owner<QLineEdit*> maneuver = nullptr;

    QString optOut(bool show) {
        Fraction quarter(1, 4);
        Fraction half(1, 2);
        return (show ? "(-" + (v._veryEffective ? half.toString() : quarter.toString()) + ") " : "") + "Cannot Be used With " + v._maneuver +
                (v._veryEffective ? " (very effective)" : "");
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
        if (v._charges > 0) {
            for (lim = 0; lim < max; ++lim) if (_charges[lim] >= v._charges) break; // NOLINT
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
                   json["adder"].toBool(false)) { v._charges      = json["charges"].toInt(0);
                                                  v._clips        = json["clips"].toInt(0);
                                                  v._boostable    = json["boostable"].toBool(false);
                                                  v._continuing   = json["continuing"].toInt(0);
                                                  v._fuelCharge   = json["fuelCharge"].toBool(false);
                                                  v._recoverable  = json["recoverable"].toBool(false);
                                                  v._recoveryTime = json["recoveryTime"].toInt(0);
                                                  v._expensive    = json["expensive"].toBool(false);
                                                  v._reload       = json["reload"].toInt(0);
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
                                                              if (v._boostable) res += Fraction(1, 4);
                                                              if (v._clips) {
                                                                  int steps = (int) (log((double) v._clips) / log(2.0)); // NOLINT
                                                                  res += steps * Fraction(1, 4);
                                                              }
                                                              res += v._continuing * Fraction(1, 4);
                                                              if (v._fuelCharge) res += Fraction(1, 4);
                                                              if (v._recoverable) res += Fraction(1, 2);
                                                              if (v._recoveryTime >= 0) res -= v._recoveryTime * Fraction(1, 4);
                                                              if (v._expensive) res -= Fraction(1, 4);
                                                              if (v._reload > -1) res -= v._reload * Fraction(1, 4);
                                                              return res;
                                                            }
    void          restore() override                        { vars s = v;
                                                              charges->setText(QString("%1").arg(s._charges));
                                                              clips->setText(QString("%1").arg(s._clips));
                                                              boostable->setEnabled(s._clips > 1);
                                                              boostable->setChecked(s._boostable);
                                                              continuing->setCurrentIndex(s._continuing);
                                                              fuelCharge->setChecked(s._fuelCharge);
                                                              recoverable->setChecked(s._recoverable);
                                                              recoveryTime->setCurrentIndex(s._recoveryTime);
                                                              expensive->setChecked(s._expensive);
                                                              reload->setCurrentIndex(s._reload);
                                                              v = s;
                                                            }
    void          store() override                          { v._charges      = charges->text().toInt();
                                                              v._clips        = clips->text().toInt();
                                                              v._boostable    = boostable->isChecked();
                                                              v._continuing   = continuing->currentIndex();
                                                              v._fuelCharge   = fuelCharge->isChecked();
                                                              v._recoverable  = recoverable->isChecked();
                                                              v._recoveryTime = recoveryTime->currentIndex();
                                                              v._expensive    = expensive->isChecked();
                                                              v._reload       = reload->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]         = name();
                                                              obj["type"]         = type();
                                                              obj["charges"]      = v._charges;
                                                              obj["clips"]        = v._clips;
                                                              obj["boostable"]    = v._boostable;
                                                              obj["continuing"]   = v._continuing;
                                                              obj["fuelCharge"]   = v._fuelCharge;
                                                              obj["recoverable"]  = v._recoverable;
                                                              obj["recoveryTime"] = v._recoveryTime;
                                                              obj["expensive"]    = v._expensive;
                                                              obj["reload"]       = v._reload;
                                                              return obj;
                                                            }

private:
    struct vars {
        int  _charges = 0;
        int  _clips = 0;
        bool _boostable = false;
        int  _continuing = 0;
        bool _fuelCharge = false;
        bool _recoverable = false;
        int  _recoveryTime = 0;
        bool _expensive = false;
        int  _reload = 0;
    } v;

    gsl::owner<QLineEdit*> charges = nullptr;
    gsl::owner<QLineEdit*> clips = nullptr;
    gsl::owner<QCheckBox*> boostable = nullptr;
    gsl::owner<QComboBox*> continuing = nullptr;
    gsl::owner<QCheckBox*> fuelCharge = nullptr;
    gsl::owner<QCheckBox*> recoverable = nullptr;
    gsl::owner<QComboBox*> recoveryTime = nullptr;
    gsl::owner<QCheckBox*> expensive = nullptr;
    gsl::owner<QComboBox*> reload = nullptr;

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
                    v._reload = -1;
                    boostable->setEnabled(false);
                    boostable->setChecked(false);
                    v._boostable = false;
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
        if (v._charges < 1) return "<incomplete>";
        Fraction mod = fraction(NoStore);
        QString res = (show ? QString("(%1").arg((mod > 0) ? "+" : "") + fraction(NoStore).toString() + ") " : "") + "Charges (" +
                QString("%1").arg(v._charges) + QString(" charge%1").arg((v._charges > 1) ? "s" : "");
        if (v._fuelCharge) res += "; Fuel Charge";
        if (v._recoverable) res += "; Recoverable";
        if (v._recoveryTime > 0) res += "; Recovery Time: " + recTime[v._recoveryTime];
        if (v._continuing > 0) res += "Continuing: " + conTime[v._continuing];
        if (v._clips > 1) res += QString("; %1 clips").arg(v._clips);
        if (v._reload > 0) res += "; Extra Time to Reload - " + extraTime[v._reload];
        if (v._expensive) res += "; Charges are Expensive or Difficult to Obtain";
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
                   json["adder"].toBool(false)) { v._zeroDCV  = json["zeroDCV"].toBool(false);
                                                  v._unaware  = json["unaware"].toBool(false);
                                                  v._constant = json["constant"].toBool(false);
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
                                                              if (v._zeroDCV) res += Fraction(1, 4);
                                                              if (v._unaware) res += Fraction(1, 4);
                                                              return res;
                                                            }
    void          restore() override                        { vars s = v;
                                                              zeroDCV->setChecked(s._zeroDCV);
                                                              unaware->setChecked(s._unaware);
                                                              constant->setChecked(s._constant);
                                                              v = s;
                                                            }
    void          store() override                          { v._zeroDCV  = zeroDCV->isChecked();
                                                              v._unaware  = unaware->isChecked();
                                                              v._constant = constant->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]     = name();
                                                              obj["type"]     = type();
                                                              obj["zeroDCV"]  = v._zeroDCV;
                                                              obj["unaware"]  = v._unaware;
                                                              obj["constant"] = v._constant;
                                                              return obj;
                                                            }

private:
    struct vars {
        bool _zeroDCV = false;
        bool _unaware = false;
        bool _constant = false;
    } v;

    gsl::owner<QCheckBox*> zeroDCV = nullptr;
    gsl::owner<QCheckBox*> unaware = nullptr;
    gsl::owner<QCheckBox*> constant = nullptr;

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
                   json["adder"].toBool(false)) { v._howMuch  = json["howMuch"].toInt(0);
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
                                                              if (v._howMuch > 0) res += Fraction(1, 4);
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

    Fraction endChange() override { if (v._howMuch == 0) return Fraction(1, 2); return Fraction(1); }

private:
    struct vars {
        int  _howMuch = 0;
    } v;

    gsl::owner<QComboBox*> howMuch = nullptr;

    QString optOut(bool show) {
        if (v._howMuch < 1) return "<incomplete>";
        Fraction mod = fraction(Modifier::NoStore);
        QString res = (show ? QString("(%1").arg((mod > 0) ? "+" : "") + fraction(Modifier::NoStore).toString() + ") " : "") + "Costs Endurance (";
        if (v._howMuch == 1) res += Fraction(1, 2).toString();
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
                   json["adder"].toBool(false)) { v._howMuch  = json["howMuch"].toInt(0);
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
                                                              if (v._howMuch > 0) res += Fraction(1, 4);
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

    Fraction endChange() override { return v._howMuch == 0 ? Fraction(1, 2) : Fraction(1); }

    struct vars {
        int  _howMuch = 0;
    } v;

    gsl::owner<QComboBox*> howMuch = nullptr;

    QString optOut(bool show) {
        if (v._howMuch < 1) return "<incomplete>";
        Fraction mod = fraction(Modifier::NoStore);
        QString res = (show ? QString("(%1").arg((mod > 0) ? "+" : "") + fraction(Modifier::NoStore).toString() + ") " : "") + "Costs Endurance To Maintain (";
        if (v._howMuch == 1) res += Fraction(1, 2).toString();
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

    gsl::owner<QLineEdit*> times = nullptr;
    gsl::owner<QComboBox*> duration = nullptr;
    gsl::owner<QCheckBox*> once = nullptr;
    gsl::owner<QCheckBox*> oneUse = nullptr;

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

    gsl::owner<QComboBox*> howMuch = nullptr;

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
                   json["adder"].toBool(false)) { v._doubling = json["doubling"].toInt(0);
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
                                                              doubling->setText(QString("%1").arg(s._doubling));
                                                              v = s;
                                                            }
    void          store() override                          { v._doubling = doubling->text().toInt(nullptr);
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["doubling"] = v._doubling;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(0, 1);
        if (v._doubling > 0) f = Fraction(1, 4) * v._doubling;
        return f;
    }

private:
    struct vars {
        int _doubling = 0;
    } v;

    gsl::owner<QLineEdit*> doubling = nullptr;

    QString optOut(bool show) {
        if (v._doubling == 0) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 QString("Delayed EffectꚚ: x%1").arg((int) pow(2.0, (double) v._doubling)); // NOLINT
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

    gsl::owner<QComboBox*> duration = nullptr;

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
                   json["adder"].toBool(false)) { v._doubling = json["doubling"].toInt(0);
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
                                                              doubling->setText(QString("%1").arg(s._doubling));
                                                              v = s;
                                                            }
    void          store() override                          { v._doubling = doubling->text().toInt(nullptr);
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]     = name();
                                                              obj["type"]     = type();
                                                              obj["adder"]    = isAdder();
                                                              obj["doubling"] = v._doubling;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(0, 1);
        if (v._doubling > 0) f = Fraction(1, 4) * v._doubling;
        return f;
    }

private:
    struct vars {
        int _doubling = 0;
    } v;

    gsl::owner<QLineEdit*> doubling = nullptr;

    QString optOut(bool show) {
        if (v._doubling == 0) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 QString("Difficult To Dispel: x%1 Active Points").arg((int) pow(2.0, (double) v._doubling)); // NOLINT
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
                   json["adder"].toBool(false)) { v._doesnt = json["doesnt"].toString("");
                                                  v._howCommon = json["howCommon"].toInt(0);
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
                                                              doesnt->setText(s._doesnt);
                                                              howCommon->setCurrentIndex(s._howCommon);
                                                              v = s;
                                                            }
    void          store() override                          { v._doesnt    = doesnt->text();
                                                              v._howCommon = howCommon->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]      = name();
                                                              obj["type"]      = type();
                                                              obj["adder"]     = isAdder();
                                                              obj["doesnt"]    = v._doesnt;
                                                              obj["howCommon"] = v._howCommon;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        return v._howCommon * Fraction(1, 4);
    }

private:
    struct vars {
        QString _doesnt = "";
        int     _howCommon = 0;
    } v;

    gsl::owner<QLineEdit*> doesnt = nullptr;
    gsl::owner<QComboBox*> howCommon = nullptr;

    QString optOut(bool show) {
        if (v._doesnt.isEmpty() || v._howCommon < 1) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Doesn't Work On " + v._doesnt;
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
                   json["adder"].toBool(false)) { v._effects = json["effects"].toString("");
                                                  v._howMany = json["howMany"].toInt(0);
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
                                                              effects->setText(s._effects);
                                                              howMany->setCurrentIndex(s._howMany);
                                                              v = s;
                                                            }
    void          store() override                          { v._effects = effects->text();
                                                              v._howMany = howMany->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]    = name();
                                                              obj["type"]    = type();
                                                              obj["adder"]   = isAdder();
                                                              obj["effects"] = v._effects;
                                                              obj["howMany"] = v._howMany;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        return (v._howMany) * Fraction(1, 2);
    }

private:
    struct vars {
        QString _effects = "";
        int     _howMany = 0;
    } v;

    gsl::owner<QLineEdit*> effects = nullptr;
    gsl::owner<QComboBox*> howMany = nullptr;

    QString optOut(bool show) {
        if (v._effects.isEmpty() || v._howMany < 1) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Expanded Effect▲: " + v._effects;
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
                   json["adder"].toBool(false)) { v._time     = json["time"].toInt(-1);
                                                  v._lockout  = json["lockout"].toBool(false);
                                                  v._activate = json["activate"].toBool(false);
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
                                                              time->setCurrentIndex(s._time);
                                                              lockout->setChecked(s._lockout);
                                                              activate->setChecked(s._activate);
                                                              v = s;
                                                            }
    void          store() override                          { v._time     = time->currentIndex();
                                                              v._lockout  = lockout->isChecked();
                                                              v._activate = activate->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]     = name();
                                                              obj["type"]     = type();
                                                              obj["adder"]    = isAdder();
                                                              obj["time"]     = v._time;
                                                              obj["lockout"]  = v._lockout;
                                                              obj["activate"] = v._activate;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v._time <= 5) switch (v._time) { // NOLINT
        case 2:
        case 3: f = Fraction(1, 2);    break;
        case 4: f = Fraction(3, 4);    break;
        case 5: f = Fraction(1, Fraction(1, 4)); break; // NOLINT
        } else f = Fraction(1) + (v._time - 5) * Fraction(1, 2); // NOLINT
        if (v._lockout) f += Fraction(1, 4);
        if (v._activate) f = f / 2;
        return f;
    }

private:
    struct vars {
        int _time = 0;
        int _lockout = 0;
        int _activate = 0;
    } v;

    gsl::owner<QComboBox*> time = nullptr;
    gsl::owner<QCheckBox*> lockout = nullptr;
    gsl::owner<QCheckBox*> activate = nullptr;

    QString optOut(bool show) {
        static QStringList extra { "", "Delayed Phase", "Extra Segment", "Full Phase", "Extra Phase",
                                   "Turn", "Minute", "5 Minutes", "20 Minutes", "Hour", "6 Hours",
                                   "Day", "Week", "Month", "Season", "Year", "5 Years" };
        if (v._time < 1) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Extra Time: " + extra[v._time];
        QString sep = " (";
        if (v._lockout) { desc += sep + "Cannot Activate Other Powers"; sep = "; "; }
        if (v._activate) { desc += sep + "Only to Activate"; sep = "; "; }
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
                   json["adder"].toBool(false)) { v._throughout  = json["throughout"].toBool(false);
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
                                                              throughout->setChecked(s._throughout);
                                                              v = s;
                                                            }
    void          store() override                          { v._throughout = throughout->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]     = name();
                                                              obj["type"]     = type();
                                                              obj["adder"]    = isAdder();
                                                              obj["throughout"] = v._throughout;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 2);
        if (v._throughout) f = Fraction(1, 1);
        return f;
    }

private:
    struct vars {
        bool _throughout = false;
    } v;

    gsl::owner<QCheckBox*> throughout = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Eye Contact Required";
        if (v._throughout) desc += "(Throughout)";
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
                   json["adder"].toBool(false)) { v._what          = json["what"].toString();
                                                  v._type          = json["Type"].toInt(-1);
                                                  v._mobility      = json["mobility"].toInt(0);
                                                  v._expendability = json["expendability"].toInt(0);
                                                  v._durability    = json["durability"].toInt(0);
                                                  v._universal     = json["universal"].toBool(false);
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
                                                              what->setText(s._what);
                                                              ntype->setCurrentIndex(s._type);
                                                              mobility->setCurrentIndex(s._mobility);
                                                              expendability->setCurrentIndex(s._expendability);
                                                              durability->setCurrentIndex(s._durability);
                                                              universal->setChecked(s._universal);
                                                              v = s;
                                                            }
    void          store() override                          { v._what          = what->text();
                                                              v._type          = ntype->currentIndex();
                                                              v._mobility      = mobility->currentIndex();
                                                              v._expendability = expendability->currentIndex();
                                                              v._durability    = durability->currentIndex();
                                                              v._universal     = universal->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]          = name();
                                                              obj["type"]          = type();
                                                              obj["what"]          = v._what;
                                                              obj["adder"]         = isAdder();
                                                              obj["Type"]          = v._type;
                                                              obj["mobiliity"]     = v._mobility;
                                                              obj["expendability"] = v._expendability;
                                                              obj["durability"]    = v._durability;
                                                              obj["universal"]     = v._universal;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        static QList<Fraction> Type          { { 0, 1 }, { 1, 4 }, { 1, 2 }, { 1, 2 }, { 1, 1 } };
        static QList<Fraction> Mobility      { { 0, 1 }, { 0, 1 }, { 1, 2 }, { 1, 1 }, { 1, 4 } };
        static QList<Fraction> Expendability { { 0, 1 }, { 0, 1 }, { 1, 4 }, { 1, 2 }, { 1, 1 } };
        static QList<Fraction> Durability    { { 0, 1 }, { 0, 1 }, { 1, 4 }, { 0, 1 }, { 0, 1 } };
        try { return Type[v._type + 1] + Mobility[v._mobility + 1] + Expendability[v._expendability + 1] + Durability[v._durability + 1]; }
        catch (...) { return Fraction(0); }
    }

private:
    struct vars {
        QString _what;
        int     _type;
        int     _mobility;
        int     _expendability;
        int     _durability;
        bool    _universal;
    } v {};

    gsl::owner<QLineEdit*> what = nullptr;
    gsl::owner<QComboBox*> ntype = nullptr;
    gsl::owner<QComboBox*> mobility = nullptr;
    gsl::owner<QComboBox*> expendability = nullptr;
    gsl::owner<QComboBox*> durability = nullptr;
    gsl::owner<QCheckBox*> universal = nullptr;

    QString optOut(bool show) {
        if (v._what.isEmpty() || v._type < 0) return "<incomplete>";
        QStringList Type { "IIF", "IAF", "OIF", "OAF" };
        QStringList Mobiillity { "", "Bulky", "Immobile", "Arrangement" };
        QStringList Expendability { "", "Difficult To Obtain", "Very Difficult To Obtain", "Extremely Difficult To Obtain" };
        QStringList Durability { "", "Fragile", "Durable", "Unbreakable" };
        Fraction f = fraction(Modifier::NoStore);
        try {
            QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + Type[v._type] + " (" + v._what;
            QString sep = "; ";
            if (v._mobility > 0) desc += sep + Mobiillity[v._mobility];
            if (v._expendability > 0) desc += sep + Expendability[v._expendability];
            if (v._durability > 0) desc += sep + Durability[v._durability];
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
                   json["adder"].toBool(false)) { v._throughout  = json["throughout"].toBool(false);
                                                  v._bothHands   = json["bothHands"].toBool(false);
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
                                                              bothHands->setChecked(s._bothHands);
                                                              throughout->setChecked(s._throughout);
                                                              v = s;
                                                            }
    void          store() override                          { v._throughout = throughout->isChecked();
                                                              v._bothHands = bothHands->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]       = name();
                                                              obj["type"]       = type();
                                                              obj["adder"]      = isAdder();
                                                              obj["throughout"] = v._throughout;
                                                              obj["bothHands"]  = v._bothHands;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v._bothHands) f = Fraction(1, 2);
        if (v._throughout) f *= 2;
        return f;
    }

private:
    struct vars {
        bool _bothHands;
        bool _throughout;
    } v {};

    gsl::owner<QCheckBox*> bothHands = nullptr;
    gsl::owner<QCheckBox*> throughout = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Gesstures";
        QString sep = " (";
        if (v._bothHands) { desc += sep + "Both Hands"; sep = "; "; }
        if (v._throughout) { desc += sep + "Throughout"; sep = "; "; }
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
                   json["adder"].toBool(false)) { v._changeable = json["changeable"].toBool(false);
                                                  v._shape      = json["shape"].toString();
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
                                                              changeable->setChecked(s._changeable);
                                                              shape->setText(s._shape);
                                                              v = s;
                                                            }
    void          store() override                          { v._changeable = changeable->isChecked();
                                                              v._shape      = shape->text();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]       = name();
                                                              obj["type"]       = type();
                                                              obj["adder"]      = isAdder();
                                                              obj["changeable"] = v._changeable;
                                                              obj["shape"]      = v._shape;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v._changeable) f = Fraction(1, 2);
        return f;
    }

private:
    struct vars {
        bool    _changeable;
        QString _shape;
    } v {};

    gsl::owner<QCheckBox*> changeable = nullptr;
    gsl::owner<QLineEdit*> shape = nullptr;

    QString optOut(bool show) {
        if (!v._changeable && v._shape.isEmpty()) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Hole In The Middle";
        if (v._changeable) desc += " (Any Shape)";
        else desc += " (" + v._shape + ")";
        return desc;
    }

    void checked(bool) override {
        store();
        shape->setEnabled(!v._changeable);
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
                   json["adder"].toBool(true)) { v._doubling  = json["doubling"].toInt(0);
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
                                                              dbling->setText(QString("%1").arg(s._doubling));
                                                              v = s;
                                                            }
    void          store() override                          { v._doubling = dbling->text().toInt();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]     = name();
                                                              obj["type"]     = type();
                                                              obj["adder"]    = isAdder();
                                                              obj["doubling"] = v._doubling;
                                                              return obj;
                                                            }

    Points points(bool noStore = false) override {
        if (!noStore) store();
        return 5_cp * v._doubling; // NOLINT
    }

    int doubling() override { return v._doubling; }

private:
    struct vars {
        int _doubling;
    } v {};

    gsl::owner<QLineEdit*> dbling = nullptr;

    QString optOut(bool show) {
        Points p(points(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((p < 0) ? "" : "+") + QString("%1").arg(p.points) + ") " : "") +
                 "Improved Noncombat Movement (" + QString("x%1").arg(2 * (int) pow(2, v._doubling)) + ")";
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
                   json["adder"].toBool(false)) { v._zero  = json["zero"].toBool(false);
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
                                                              zero->setChecked(s._zero);
                                                              v = s;
                                                            }
    void          store() override                          { v._zero = zero->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["zero"]  = v._zero;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v._zero) f = Fraction(1, 2);
        return f;
    }

private:
    struct vars {
        bool _zero;
    } v {};

    gsl::owner<QCheckBox*> zero = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Inaccurate";
        if (v._zero) desc += " (0 OCV, Range Mods start at 6m)";
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
                   json["adder"].toBool(false)) { v._throughout = json["throughout"].toBool(false);
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
                                                              throughout->setChecked(s._throughout);
                                                              v = s;
                                                            }
    void          store() override                          { v._throughout = throughout->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]       = name();
                                                              obj["type"]       = type();
                                                              obj["adder"]      = isAdder();
                                                              obj["throughout"] = v._throughout;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v._throughout) f *= 2;
        return f;
    }

private:
    struct vars {
        bool _throughout;
    } v {};

    gsl::owner<QCheckBox*> throughout = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Incantations";
        QString sep = " (";
        if (v._throughout) { desc += sep + "Throughout"; sep = "; "; }
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
                   json["adder"].toBool(false)) { v._amount        = json["amount"].toInt(-1);
                                                  v._circumstances = json["circumstances"].toInt(0);
                                                  v._what          = json["what"].toString();
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
                                                              amount->setCurrentIndex(s._amount);
                                                              circumstances->setCurrentIndex(s._circumstances);
                                                              v = s;
                                                            }
    void          store() override                          { v._amount        = amount->currentIndex();
                                                              v._circumstances = circumstances->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]          = name();
                                                              obj["type"]          = type();
                                                              obj["adder"]         = isAdder();
                                                              obj["amount"]        = v._amount;
                                                              obj["circumstances"] = v._circumstances;
                                                              obj["what"]          = v._what;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f;
        if (v._circumstances < 0) return f;
        if (v._amount < 8) f = Fraction(1, 2) * v._amount; // NOLINT
        else if (v._amount == 8) f = Fraction(3, Fraction(1, 2)); // NOLINT
        else f = Fraction(4);
        f -= v._circumstances * Fraction(1, 4);
        if (f < Fraction(0)) f = Fraction(0);
        return f;
    }

    Fraction endChange() override { return Fraction(v._amount + 1); }

private:
    struct vars {
        int     _amount;
        int     _circumstances;
        QString _what;
    } v {};

    gsl::owner<QComboBox*> amount = nullptr;
    gsl::owner<QComboBox*> circumstances = nullptr;
    gsl::owner<QLineEdit*> what = nullptr;

    QString optOut(bool show) {
        if (v._amount < 1) return "<incomplete>";
        QStringList Amount { "", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10" };
        QStringList Circumstances { "", "Very Common", "Common", "Uncommon" };
        Fraction f = fraction(Modifier::NoStore);
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "Increased END Cost (" + Amount[v._amount];
        if (Circumstances[v._circumstances] != "") desc += "; " + Circumstances[v._circumstances] + ((v._circumstances != 0) ? ": " + v._what : "");
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
                   json["adder"].toBool(false)) { v._doubling  = json["doubling"].toInt(0);
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
                                                              doubling->setText(QString("%1").arg(s._doubling));
                                                              v = s;
                                                            }
    void          store() override                          { v._doubling = doubling->text().toInt();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]     = name();
                                                              obj["type"]     = type();
                                                              obj["adder"]    = isAdder();
                                                              obj["doubling"] = v._doubling;
                                                              return obj;
                                                            }

    Points points(bool noStore = false) override {
        if (!noStore) store();
        return 5_cp * v._doubling; // NOLINT
    }

private:
    struct vars {
        int _doubling;
    } v {};

    gsl::owner<QLineEdit*> doubling = nullptr;

    QString optOut(bool show) {
        Points p(points(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((p < 0) ? "" : "+") + QString("%1").arg(p.points) + ") " : "") +
                 "Increased Mass (" + QString("x%1, %2 kg").arg((int) pow(2, v._doubling)).arg(200 * (int) pow(2, v._doubling)) + ")"; // NOLINT
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
                   json["adder"].toBool(false)) { v._doubling  = json["doubling"].toInt(0);
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
                                                              doubling->setText(QString("%1").arg(s._doubling));
                                                              v = s;
                                                            }
    void          store() override                          { v._doubling = doubling->text().toInt();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]     = name();
                                                              obj["type"]     = type();
                                                              obj["adder"]    = isAdder();
                                                              obj["doubling"] = v._doubling;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        return Fraction(1, 4) * v._doubling;
    }

private:
    struct vars {
        int _doubling;
    } v {};

    gsl::owner<QLineEdit*> doubling = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Increased Maximum Effect (" + QString("x%1").arg((int) pow(2, v._doubling)) + ")";
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
                   json["adder"].toBool(false)) { v._doubling  = json["doubling"].toInt(0);
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
                                                              doubling->setText(QString("%1").arg(s._doubling));
                                                              v = s;
                                                            }
    void          store() override                          { v._doubling = doubling->text().toInt();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]     = name();
                                                              obj["type"]     = type();
                                                              obj["adder"]    = isAdder();
                                                              obj["doubling"] = v._doubling;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        return Fraction(1, 4) * v._doubling;
    }

private:
    struct vars {
        int _doubling;
    } v {};

    gsl::owner<QLineEdit*> doubling = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Increased Maximum Range (" + QString("x%1").arg((int) pow(2, v._doubling)) + ")";
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
                   json["adder"].toBool(false)) { v._location  = json["location"].toInt(0);
                                                  v._locAndDir = json["locAndDir"].toString();
                                                  v._direction = json["mobility"].toInt(0);
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
                                                              location->setCurrentIndex(s._location);
                                                              locAndDir->setText(s._locAndDir);
                                                              direction->setCurrentIndex(s._direction);
                                                              v = s;
                                                            }
    void          store() override                          { v._location  = location->currentIndex();
                                                              v._locAndDir = locAndDir->text();
                                                              v._direction = direction->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]      = name();
                                                              obj["type"]      = type();
                                                              obj["adder"]     = isAdder();
                                                              obj["location"]  = v._location;
                                                              obj["locAndDir"] = v._locAndDir;
                                                              obj["direction"] = v._direction;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f;
        if (v._location < 0 || v._direction < 0) return f;
        f = Fraction(1, 4) * (v._location + v._direction);
        return f;
    }

private:
    struct vars {
        int     _location;
        QString _locAndDir;
        int     _direction;
    } v {};

    gsl::owner<QComboBox*> location = nullptr;
    gsl::owner<QLineEdit*> locAndDir = nullptr;
    gsl::owner<QComboBox*> direction = nullptr;

    QString optOut(bool show) {
        if (v._location < 0 || v._direction < 0 || ((v._location == 2 || v._direction == 2) && v._locAndDir.isEmpty())) return "<incomplete>";
        QStringList Location { "Always The Character", "Always the Same", "Variable" };
        QStringList Direction { "Directly from Source to Target", "Always the Same", "Variable" };
        Fraction f = fraction(Modifier::NoStore);
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "Indirect (" + Location[v._location];
        if (v._location == 1 && v._direction != 2) desc += ", " + v._locAndDir;
        desc += "; " + Direction[v._direction];
        if (v._location == 2 || v._direction == 2) desc += ", " + v._locAndDir;
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
                   json["adder"].toBool(false)) { v._inobvious = json["inobvious"].toBool(false);
                                                  v._how       = json["how"].toInt(0);
                                                  v._sense     = json["sense"].toString();
                                                  v._effect    = json["Effect"].toInt(0);
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
                                                              inobvious->setChecked(s._inobvious);
                                                              how->setCurrentIndex(s._how);
                                                              sense->setText(s._sense);
                                                              effect->setCurrentIndex(s._effect);
                                                              v = s;
                                                            }
    void          store() override                          { v._inobvious = inobvious->isChecked();
                                                              v._how       = how->currentIndex();
                                                              v._sense     = sense->text();
                                                              v._effect    = effect->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]      = name();
                                                              obj["type"]      = type();
                                                              obj["adder"]     = isAdder();
                                                              obj["inobvious"] = v._inobvious;
                                                              obj["how"]       = v._how;
                                                              obj["sense"]     = v._sense;
                                                              obj["effect"]    = v._effect;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(0, 1);
        if (v._effect > 0) f = (v._effect % 2 != 0) ? Fraction(1, 4) : Fraction(1, 2);
        if (v._inobvious) f += Fraction(1, 4) * (v._how);
        else if (v._how > 2) f += Fraction(1, 2) * (v._how - 3);
        else f += Fraction(1, 4) * v._how;
        return f;
    }

private:
    struct vars {
        bool    _inobvious;
        int     _how;
        QString _sense;
        int     _effect;
    } v {};

    gsl::owner<QCheckBox*> inobvious = nullptr;
    gsl::owner<QComboBox*> how = nullptr;
    gsl::owner<QLineEdit*> sense = nullptr;
    gsl::owner<QComboBox*> effect = nullptr;

    QString optOut(bool show) {
        if (v._how < 1 || v._effect < 1) return "<incomplete>";
        if (v._inobvious && v._how != 1 && v._sense.isEmpty()) return "<incomplete>";
        if (!v._inobvious && v._how != 3 && v._sense.isEmpty()) return "<incomplete>";
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
        if (v._inobvious) desc += How[v._how + 2];
        else desc += How[v._how];
        desc += (v._sense.isEmpty() ? "" : ": " + v._sense);
        if (v._effect > 0) desc += "; " + Effect[v._effect];
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
        if (v._inobvious) start = 2;
        v._how = -1;
        how->setCurrentIndex(-1);
        how->clear();
        for (int i = start; i < data.count(); ++i) how->addItem(data[i]);
        ModifiersDialog::ref().updateForm();
    }

    void index(int) override {
        store();
        sense->setEnabled((v._inobvious && v._how != 1) || (!v._inobvious && v._how != 3));
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
                   json["adder"].toBool(false)) { v._sense = json["sense"].toString();
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
                                                              sense->setText(s._sense);
                                                              v = s;
                                                            }
    void          store() override                          { v._sense     = sense->text();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["sense"] = v._sense;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        return Fraction(1, 4);
    }

private:
    struct vars {
        QString _sense;
    } v {};

    gsl::owner<QLineEdit*> sense = nullptr;

    QString optOut(bool show) {
        if (v._sense.isEmpty()) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Limited Effect (" + v._sense + ")";
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
                   json["adder"].toBool(false)) { v._conditional = json["conditional"].toBool(false);
                                                  v._how         = json["how"].toInt(0);
                                                  v._what        = json["what"].toString();
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
                                                              conditional->setChecked(s._conditional);
                                                              how->setCurrentIndex(s._how);
                                                              what->setText(s._what);
                                                              v = s;
                                                            }
    void          store() override                          { v._conditional = conditional->isChecked();
                                                              v._how       = how->currentIndex();
                                                              v._what     = what->text();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]        = name();
                                                              obj["type"]        = type();
                                                              obj["adder"]       = isAdder();
                                                              obj["conditional"] = v._conditional;
                                                              obj["how"]         = v._how;
                                                              obj["what"]        = v._what;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        switch (v._how + (v._conditional ? 1 : 0)) {
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
        bool    _conditional;
        int     _how;
        QString _what;
    } v {};

    gsl::owner<QCheckBox*> conditional = nullptr;
    gsl::owner<QComboBox*> how = nullptr;
    gsl::owner<QLineEdit*> what = nullptr;

    QString optOut(bool show) {
        if (v._how < 1 || v._what.isEmpty()) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "-") + f.toString() + ") " : "") +
                 "Limited Power (";
        desc += v._what;
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
        v._how = -1;
        how->setCurrentIndex(-1);
        how->clear();
        how->setCurrentText(v._conditional ? "What Condition?" : "How Limited?");
        for (int i = 0; i < (v._conditional ? conditional.count() : limited.count()); ++i) how->addItem(v._conditional ? conditional[i] : limited[i]);
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
                   json["adder"].toBool(false)) { v._noRange = json["noRange"].toBool(false);
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
                                                              noRange->setChecked(s._noRange);
                                                              v = s;
                                                            }
    void          store() override                          { v._noRange = noRange->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]    = name();
                                                              obj["type"]    = type();
                                                              obj["adder"]   = isAdder();
                                                              obj["noRange"] = v._noRange;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v._noRange) f = Fraction(1, 4);
        return f;
    }

private:
    struct vars {
        bool _noRange;
    } v {};

    gsl::owner<QCheckBox*> noRange = nullptr;

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
                   json["adder"].toBool(false)) { v._how         = json["how"].toInt(0);
                                                  v._what        = json["what"].toString();
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
                                                              how->setCurrentIndex(s._how);
                                                              what->setText(s._what);
                                                              v = s;
                                                            }
    void          store() override                          { v._how       = how->currentIndex();
                                                              v._what     = what->text();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]        = name();
                                                              obj["type"]        = type();
                                                              obj["adder"]       = isAdder();
                                                              obj["how"]         = v._how;
                                                              obj["what"]        = v._what;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        switch (v._how) {
        case 1: return Fraction(1, 4);
        case 2: return Fraction(1, 2);
        case 3: return Fraction(1, 1);
        }
        Fraction f(0, 1);
        return f;
    }

private:
    struct vars {
        int     _how;
        QString _what;
    } v {};

    gsl::owner<QComboBox*> how = nullptr;
    gsl::owner<QLineEdit*> what = nullptr;

    QString optOut(bool show) {
        if (v._how < 1 || v._what.isEmpty()) return "<incomplete>";
        static QStringList Limited { "", "Very Common", "Common", "Uncommon" };
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Limited Special Effect (" + Limited[v._how] + ": " + v._what + ")";
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
                   json["adder"].toBool(false)) { v._greater  = json["greater"].toBool(false);
                                                  v._both     = json["both"].toBool(false);
                                                  v._prop     = json["prop"].toBool(false);
                                                  v._full     = json["full"].toBool(false);
                                                  v._constant = json["constant"].toBool(false);
                                                  v._instant  = json["instant"].toBool(false);
                                                  v._target   = json["target"].toString();
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
                                                              greater->setChecked(s._greater);
                                                              both->setChecked(s._both);
                                                              prop->setChecked(s._prop);
                                                              full->setChecked(s._full);
                                                              constant->setChecked(s._constant);
                                                              instant->setChecked(s._instant);
                                                              target->setText(s._target);
                                                              v = s;
                                                            }
    void          store() override                          { v._greater  = greater->isChecked();
                                                              v._both     = both->isChecked();
                                                              v._prop     = prop->isChecked();
                                                              v._full     = full->isChecked();
                                                              v._constant = constant->isChecked();
                                                              v._instant  = instant->isChecked();
                                                              v._target   = target->text();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]     = name();
                                                              obj["type"]     = type();
                                                              obj["adder"]    = isAdder();
                                                              obj["greater"]  = v._greater;
                                                              obj["both"]     = v._both;
                                                              obj["prop"]     = v._prop;
                                                              obj["full"]     = v._full;
                                                              obj["constant"] = v._constant;
                                                              obj["instant"]  = v._instant;
                                                              obj["target"]   = v._target;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(-1, 2);
        if (v._greater) f = Fraction(-1, 4);
        if (v._both) f -= Fraction(1, 4);
        if (v._prop) f += Fraction(1, 4);
        if (v._full) f -= Fraction(1, 4);
        if (v._constant) f += Fraction(1, 4);
        if (v._instant) f += Fraction(1, 4);
        return f;
    }

private:
    struct vars {
        bool    _greater;
        bool    _both;
        bool    _prop;
        bool    _full;
        bool    _constant;
        bool    _instant;
        QString _target;
    } v {};

    gsl::owner<QCheckBox*> greater = nullptr;
    gsl::owner<QCheckBox*> both = nullptr;
    gsl::owner<QCheckBox*> prop = nullptr;
    gsl::owner<QCheckBox*> full = nullptr;
    gsl::owner<QCheckBox*> constant = nullptr;
    gsl::owner<QCheckBox*> instant = nullptr;
    gsl::owner<QLineEdit*> target = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Linked to " + v._target;
        QString sep = " (";
        if (v._greater) { desc += sep + "Greater Power Linked to Lesser▲"; sep = "; "; }
        if (v._both) { desc += sep + "Must Use Together and Lesser is Inconvenient"; sep = "; "; }
        if (v._prop) { desc += sep + "Need Not Use Powers Proportionally"; sep = "; "; }
        if (v._full) { desc += sep + "Greater must be at Full Power to use Lesser"; sep = "; "; }
        if (v._constant) { desc += sep + "Greater Power is Constant"; sep = "; "; }
        if (v._instant) { desc += sep + "Instant Power can be Used any Time with Constant"; sep = ": "; }
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
                   json["adder"].toBool(false)) { v._effect = json["effect"].toInt(0);
                                                  v._other  = json["other"].toString();
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
                                                              effect->setCurrentIndex(s._effect);
                                                              other->setText(s._other);
                                                              v = s;
                                                            }
    void          store() override                          { v._effect = effect->currentIndex();
                                                              v._other  = other->text();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]   = name();
                                                              obj["type"]   = type();
                                                              obj["adder"]  = isAdder();
                                                              obj["effect"] = v._effect;
                                                              obj["other"]  = v._other;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f;
        if (!v._other.isEmpty()) f = Fraction(1, 4);
        switch (v._effect) {
        case 1: return f + Fraction(1, 4);
        case 2: return f + Fraction(1, 2);
        case 3: return f + Fraction(3, 4);
        }
        return Fraction(0, 1);
    }

private:
    struct vars {
        int     _effect;
        QString _other;
    } v {};

    gsl::owner<QComboBox*> effect = nullptr;
    gsl::owner<QLineEdit*> other = nullptr;

    QString optOut(bool show) {
        if (v._effect < 1) return "<incomplete>";
        static QStringList Limited { "", "EGO+10", "EGO+20", "EGO+30" };
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Mandatory Effect (" + Limited[v._effect];
        if (!v._other.isEmpty()) desc += " and " + v._other;
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
                   json["adder"].toBool(false)) { v._mass = json["mass"].toInt(0);
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
                                                              mass->setCurrentIndex(s._mass);
                                                              v = s;
                                                            }
    void          store() override                          { v._mass = mass->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["mass"]  = v._mass;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        switch (v._mass) {
        case 1: return Fraction(1, 2);
        case 2: return Fraction(1, 1);
        case 3: return Fraction(1, Fraction(1, 2));
        }
        return Fraction(0, 1);
    }

private:
    struct vars {
        int     _mass;
    } v {};

    gsl::owner<QComboBox*> mass = nullptr;

    QString optOut(bool show) {
        if (v._mass < 1) return "<incomplete>";
        static QStringList Limited { "", Fraction(1, 2).toString() + " Mass", "Normal Mass", "2x Mass" };
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "MassꚚ (" + Limited[v._mass] + ")";
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
                   json["adder"].toBool(false)) { v._scale     = json["scale"].toInt(0);
                                                  v._invariant = json["invariant"].toBool(false);
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
                                                              scale->setCurrentIndex(s._scale);
                                                              invariant->setChecked(s._invariant);
                                                              v = s;
                                                            }
    void          store() override                          { v._scale     = scale->currentIndex();
                                                              v._invariant = invariant->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]      = name();
                                                              obj["type"]      = type();
                                                              obj["adder"]     = isAdder();
                                                              obj["scale"]     = v._scale;
                                                              obj["invariant"] = v._invariant;
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
        if (v._scale < 1) return Fraction(0);
        return cost[v._scale] + (v._invariant ? Fraction(1, 4) : Fraction(0));
    }

private:
    struct vars {
        int  _scale;
        bool _invariant;
    } v {};

    gsl::owner<QComboBox*> scale = nullptr;
    gsl::owner<QCheckBox*> invariant = nullptr;

    QString optOut(bool show) {
        if (v._scale == -1) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QStringList scale { "", "1 km", "10 km", "100 km", "1,000 km", "10,000 km", "100,000 km",
                            "1 million km", "100 million km", "1 billion km", "10 billion km",
                            "100 billion km", "1 trillion km", "1 light year", "10 light years",
                            "100,000 light-years", "100 billion light-years" };
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Megascaleϴ (" + scale[v._scale];
        if (v._invariant) desc += "; Can't change scale";
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
                   json["adder"].toBool(false)) { v._activation = json["activation"].toBool(false);
                                                  v._effects    = json["effects"].toBool(false);
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
                                                              activation->setChecked(s._activation);
                                                              effects->setChecked(s._effects);
                                                              v = s;
                                                            }
    void          store() override                          { v._activation = activation->isChecked();
                                                              v._effects    = effects->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]       = name();
                                                              obj["type"]       = type();
                                                              obj["adder"]      = isAdder();
                                                              obj["activation"] = v._activation;
                                                              obj["effects"]    = v._effects;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f;
        if (v._activation) f += Fraction(1);
        if (v._effects) f += Fraction(1);
        return f;
    }

private:
    struct vars {
        bool _activation;
        bool _effects;
    } v {};

    gsl::owner<QCheckBox*> activation = nullptr;
    gsl::owner<QCheckBox*> effects = nullptr;

    QString optOut(bool show) {
        if (!v._activation && !v._effects) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "No Concious Control▲";
        QString sep = " (";
        if (v._activation) { desc += sep + "Activation"; sep = " and "; }
        if (v._effects) { desc += sep + "Effects"; sep = "; "; }
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
                   json["adder"].toBool(false)) { v._identity = json["identity"].toString(0);
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
                                                              identity->setText(s._identity);
                                                              v = s;
                                                            }
    void          store() override                          { v._identity = identity->text();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]     = name();
                                                              obj["type"]     = type();
                                                              obj["adder"]    = isAdder();
                                                              obj["identity"] = v._identity;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        return f;
    }

private:
    struct vars {
        QString _identity;
    } v {};

    gsl::owner<QLineEdit*> identity = nullptr;

    QString optOut(bool show) {
        if (v._identity.isEmpty()) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Only In Alternate Identity (" + v._identity + ")";
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
                   json["adder"].toBool(false)) { v._against = json["against"].toString("");
                                                  v._howCommon = json["howCommon"].toInt(0);
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
                                                              against->setText(s._against);
                                                              howCommon->setCurrentIndex(s._howCommon);
                                                              v = s;
                                                            }
    void          store() override                          { v._against    = against->text();
                                                              v._howCommon = howCommon->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]      = name();
                                                              obj["type"]      = type();
                                                              obj["adder"]     = isAdder();
                                                              obj["against"]   = v._against;
                                                              obj["howCommon"] = v._howCommon;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        return 1 - (v._howCommon - 1) * Fraction(1, 4);
    }

private:
    struct vars {
        QString _against;
        int     _howCommon;
    } v {};

    gsl::owner<QLineEdit*> against = nullptr;
    gsl::owner<QComboBox*> howCommon = nullptr;

    QString optOut(bool show) {
        if (v._against.isEmpty() || v._howCommon < 0) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 "Only Works Against " + v._against;
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
                   json["adder"].toBool(false)) { v._what = toStringList(json["what"].toArray());
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
                                                              setTreeWidget(what, s._what);
                                                              v = s;
                                                            }
    void          store() override                          { v._what = treeWidget(what);
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["what"]  = toArray(v._what);
                                                              return obj;
                                                            }

    Points points(bool noStore = false) override {
        if (!noStore) store();
        Points p(0);
        QStringList groups = { "Hearing", "Mental", "Radio", "Sight", "Smell/Taste", "Touch" };
        QStringList senses = { "Normal Hearing", "Active Sonar", "Ultrasonic Perception", "Mental Awareness", "Mind Scan", "Radio Perception", "Radar", "Normal Sight",
                               "Nightvision", "Infrared Pereception", "Ultraviolet Perception", "Normal Smell", "Normal Taste", "Normal Touch" };
        for (const auto& str: v._what) {
            if (groups.contains(str)) p += 10_cp; // NOLINT
            else if (senses.contains(str)) p += 5_cp; // NOLINT
        }
        return p;
    }

private:
    struct vars {
        QStringList _what;
    } v {};

    gsl::owner<QTreeWidget*> what = nullptr;

    QString optOut(bool show) {
        if (v._what.isEmpty()) return "<incomplete>";
        Points p(points(Modifier::NoStore));
        QString desc = (show ? QString("(+%1) ").arg(p.points) : "") + "Opaque to " + v._what[0];
        auto len = v._what.length();
        for (auto i = 1; i < len; ++i) {
            if (i != len - 1) desc += ", ";
            else desc += ", and ";
            desc += v._what[i];
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
                   json["adder"].toBool(false)) { v._imperceptable = json["imperceptable"].toBool(false);
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
                                                              imperceptable->setChecked(s._imperceptable);
                                                              v = s;
                                                            }
    void          store() override                          { v._imperceptable = imperceptable->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]          = name();
                                                              obj["type"]          = type();
                                                              obj["adder"]         = isAdder();
                                                              obj["imperceptable"] = v._imperceptable;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v._imperceptable) f = Fraction(1, 2);
        return f;
    }

private:
    struct vars {
        bool _imperceptable;
    } v {};

    gsl::owner<QCheckBox*> imperceptable = nullptr;

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
                   json["adder"].toBool(false)) { v._noRange = json["noRange"].toBool(false);
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
                                                              noRange->setChecked(s._noRange);
                                                              v = s;
                                                            }
    void          store() override                          { v._noRange = noRange->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]    = name();
                                                              obj["type"]    = type();
                                                              obj["adder"]   = isAdder();
                                                              obj["noRange"] = v._noRange;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(-1, 4);
        if (v._noRange) f = Fraction(1, 4);
        return f;
    }

private:
    struct vars {
        bool _noRange;
    } v {};

    gsl::owner<QCheckBox*> noRange = nullptr;

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
                   json["adder"].toBool(false)) { v._noEND = json["noEND"].toBool(false);
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
                                                              noEND->setChecked(s._noEND);
                                                              v = s;
                                                            }
    void          store() override                          { v._noEND = noEND->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["noEND"] = v._noEND;
                                                              return obj;
                                                            }

    Fraction endChange() override { return v._noEND ? Fraction(0) : Fraction(1, 2); }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v._noEND) f = Fraction(1, 2);
        return f;
    }

private:
    struct vars {
        bool _noEND;
    } v {};

    gsl::owner<QCheckBox*> noEND = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 (v._noEND ? "0 END" : (Fraction(1, 2).toString() + "END"));
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
                   json["adder"].toBool(false)) { v._reduced = json["reduced"].toInt(0);
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
                                                              reduced->setText(QString("%1").arg(s._reduced));
                                                              v = s;
                                                            }
    void          store() override                          { v._reduced = reduced->text().toInt();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]    = name();
                                                              obj["type"]    = type();
                                                              obj["adder"]   = isAdder();
                                                              obj["reduced"] = v._reduced;
                                                              return obj;
                                                            }

    Points points(bool noStore = false) override {
        if (!noStore) store();
        return 2_cp * v._reduced;
    }

private:
    struct vars {
        int _reduced;
    } v {};

    gsl::owner<QLineEdit*> reduced = nullptr;

    QString optOut(bool show) {
        if (v._reduced < 1) return "<incomplete>";
        Points p(points(Modifier::NoStore));
        QString desc = (show ? QString("(+%1) ").arg(p.points) : "")  + QString("Reduced Negation (-%1)").arg(v._reduced);
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
                   json["adder"].toBool(false)) { v._noRangeMod = json["noRangeMod"].toBool(false);
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
                                                              noRangeMod->setChecked(s._noRangeMod);
                                                              v = s;
                                                            }
    void          store() override                          { v._noRangeMod = noRangeMod->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]       = name();
                                                              obj["type"]       = type();
                                                              obj["adder"]      = isAdder();
                                                              obj["noRangeMod"] = v._noRangeMod;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v._noRangeMod) f = Fraction(1, 2);
        return f;
    }

private:
    struct vars {
        bool _noRangeMod;
    } v {};

    gsl::owner<QCheckBox*> noRangeMod = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 (v._noRangeMod ? "No Range Modifier" : (Fraction(1, 2).toString() + " Range Modifier"));
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
                   json["adder"].toBool(false)) { v._hands = json["hands"].toInt(0);
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
                                                              hands->setCurrentIndex(s._hands);
                                                              v = s;
                                                            }
    void          store() override                          { v._hands = hands->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["hands"] = v._hands;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        return v._hands * Fraction(1, 4);
    }

private:
    struct vars {
        int _hands;
    } v {};

    gsl::owner<QComboBox*> hands = nullptr;

    QString optOut(bool show) {
        if (v._hands < 1) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        static QStringList hands { "", "One-And-A-Half", "Two" };
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                 hands[v._hands] + " HandedꚚ";
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
                   json["adder"].toBool(false)) { v._type  = json["rType"].toBool(false);
                                                  v._roll  = json["roll"].toInt(0);
                                                  v._when  = json["when"].toBool(false);
                                                  v._skill = json["skill"].toString();
                                                  v._isa   = json["isa"].toBool(false);
                                                  v._per   = json["per"].toInt(0);
                                                  v._two   = json["two"].toBool(false);
                                                  v._fails = json["fails"].toInt(0);
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
                                                              roll->setEnabled(!v._type);
                                                              skill->setEnabled(v._type);
                                                              isa->setEnabled(v._type);
                                                              per->setEnabled(v._type);
                                                              two->setEnabled(v._two);
                                                              return true; }
    void          restore() override                        { vars s = v;
                                                              rType->setChecked(s._type);
                                                              when->setChecked(s._when);
                                                              roll->setCurrentIndex(s._roll);
                                                              skill->setText(s._skill);
                                                              isa->setChecked(s._isa);
                                                              per->setCurrentIndex(s._per);
                                                              two->setChecked(s._two);
                                                              fails->setCurrentIndex(s._fails);
                                                              v = s;
                                                            }
    void          store() override                          { v._type  = rType->isChecked();
                                                              v._when  = when->isChecked();
                                                              v._roll  = roll->currentIndex();
                                                              v._skill = skill->text();
                                                              v._isa   = isa->isChecked();
                                                              v._per   = per->currentIndex();
                                                              v._two   = two->isChecked();
                                                              v._fails = fails->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["rType"] = v._type;
                                                              obj["when"]  = v._when;
                                                              obj["roll"]  = v._roll;
                                                              obj["skill"] = v._skill;
                                                              obj["isa"]   = v._isa;
                                                              obj["per"]   = v._per;
                                                              obj["two"]   = v._two;
                                                              obj["fails"] = v._fails;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 2);
        if (v._type) {
            if (v._isa) f -= Fraction(1, 4);
            if (v._per == 0) f -= Fraction(1, 4);
            else if (v._per == 2) f += Fraction(1, 2);
            if (v._two) f -= Fraction(1, 4);
        } else f -= (v._roll - 5) * Fraction(1, 4); // NOLINT
        if (v._fails >= 1) f += Fraction(1, 2);
        if (f < Fraction(1, 4)) f = Fraction(1, 4);
        return f;
    }

private:
    struct vars {
        bool    _type;
        bool    _when;
        int     _roll;
        QString _skill;
        bool    _isa;
        bool    _two;
        int     _per;
        int     _fails;
    } v {};

    gsl::owner<QCheckBox*> rType = nullptr;
    gsl::owner<QCheckBox*> when = nullptr;
    gsl::owner<QComboBox*> roll = nullptr;
    gsl::owner<QLineEdit*> skill = nullptr;
    gsl::owner<QCheckBox*> isa = nullptr;
    gsl::owner<QCheckBox*> two = nullptr;
    gsl::owner<QComboBox*> per = nullptr;
    gsl::owner<QComboBox*> fails = nullptr;

    QString optOut(bool show) {
        QStringList per { "20 Active points", "10 Active Points", "5 Active Points" };
        QStringList roll { "", "7-", "8-", "9-", "10-", "11-", "12-", "13-", "14-" };
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "");
        QString sep;
        if (v._type) {
            if (v._skill.isEmpty() || v._per < 1) return "<incomplete>";
            desc = "Skill Roll: " + v._skill + " (-1 per " + per[v._per];
            sep = "; ";
        } else {
            if (v._roll < 1) return "<incomplete>";
            desc = "Roll " + roll[v._roll];
            sep = " (";
        }
        if (v._when) { desc += sep + "Every phase or Use"; sep = "; "; }
        if (v._fails == 1) { desc += sep + "Burnout"; sep = "; "; }
        else if (v._fails == 2) { desc += sep + "Jammed"; sep = ": "; }
        if (sep == "; ") desc += ")";
        return desc;
    }

    void checked(bool) override {
        store();
        QCheckBox* box = dynamic_cast<QCheckBox*>(sender());
        if (box == rType) {
            roll->setEnabled(!v._type);
            skill->setEnabled(v._type);
            isa->setEnabled(v._type);
            per->setEnabled(v._type);
            two->setEnabled(v._two);
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
                   json["adder"].toBool(false)) { v._max   = json["max"].toInt(0);
                                                  v._needs = json["needs"].toInt(0);
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
                                                              max->setText(QString("%1").arg(s._max));
                                                              needs->setText(QString("%1").arg(s._needs));
                                                              v = s;
                                                            }
    void          store() override                          { v._max   = max->text().toInt();
                                                              v._needs = needs->text().toInt();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]    = name();
                                                              obj["type"]    = type();
                                                              obj["adder"]   = isAdder();
                                                              obj["max"]     = v._max;
                                                              obj["needs"]   = v._needs;
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
        if (v._needs == 2) f = Fraction(1, 4);
        else f = (v._needs - 3) / 5 * Fraction(1, 4) + Fraction(1, 4); // NOLINT
        if (v._max < 65) f += (6 - (int) (log((double) v._max) / log(2.0))) * Fraction(1, 4); // NOLINT
        return f;
    }

private:
    struct vars {
        int _max;
        int _needs;
    } v {};

    gsl::owner<QLineEdit*> max = nullptr;
    gsl::owner<QLineEdit*> needs = nullptr;

    QString optOut(bool show) {
        if (v._max < 1 || v._needs < 2) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "");
        desc += QString("%1").arg(v._needs) + " Charges per Use";
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
                   json["adder"].toBool(false)) { v._needs = json["needs"].toInt(0);
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
                                                              needs->setText(QString("%1").arg(s._needs));
                                                              v = s;
                                                            }
    void          store() override                          { v._needs = needs->text().toInt();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]    = name();
                                                              obj["type"]    = type();
                                                              obj["adder"]   = isAdder();
                                                              obj["needs"]   = v._needs;
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
        Fraction f = ((int) (log((double) v._needs) / log(2.0)) + 1) * Fraction(1, 4); // NOLINT
        return f;
    }

private:
    struct vars {
        int _needs;
    } v {};

    gsl::owner<QLineEdit*> needs = nullptr;

    QString optOut(bool show) {
        if (v._needs < 2) return "<incomplete>";
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "");
        desc += QString("%1").arg(v._needs) + " Users to Activate";
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
                   json["adder"].toBool(false)) { v._nonStandard = json["nonStandard"].toBool(false);
                                                  v._restraint   = json["restraint"].toString();
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
                                                              nonStandard->setChecked(s._nonStandard);
                                                              restraint->setText(s._restraint);
                                                              v = s;
                                                            }
    void          store() override                          { v._nonStandard = nonStandard->isChecked();
                                                              v._restraint   = restraint->text();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]        = name();
                                                              obj["type"]        = type();
                                                              obj["adder"]       = isAdder();
                                                              obj["nonStandard"] = v._nonStandard;
                                                              obj["restraint"]   = v._restraint;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 2);
        if (v._nonStandard) f = Fraction(1, 4);
        return f;
    }

private:
    struct vars {
        bool    _nonStandard;
        QString _restraint;
    } v {};

    gsl::owner<QCheckBox*> nonStandard = nullptr;
    gsl::owner<QLineEdit*> restraint = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "Restrainable";
        if (v._nonStandard) desc += "(Only by " + v._restraint + ")";
        return desc;
    }

    void checked(bool) override {
        store();
        restraint->setEnabled(v._nonStandard);
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
                   json["adder"].toBool(false)) { v._regen = json["regen"].toBool(false);
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
                                                              regen->setChecked(s._regen);
                                                              v = s;
                                                            }
    void          store() override                          { v._regen = regen->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["regen"] = v._regen;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 2);
        if (v._regen) f = Fraction(2);
        return f;
    }

private:
    struct vars {
        bool _regen;
    } v {};

    gsl::owner<QCheckBox*> regen = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "Ressurection Only";
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
                   json["adder"].toBool(false)) { v._level    = json["level"].toInt(0);
                                                  v._effect   = json["effect"].toString();
                                                  v._when     = json["when"].toInt(0);
                                                  v._affects  = json["affects"].toInt(0);
                                                  v._constant = json["constant"].toBool(false);
                                                  v._pre      = json["pre"].toBool(false);
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
                                                              level->setCurrentIndex(s._level);
                                                              effect->setText(s._effect);
                                                              when->setCurrentIndex(s._when);
                                                              affects->setCurrentIndex(s._affects);
                                                              constant->setChecked(s._constant);
                                                              pre->setChecked(s._pre);
                                                              v = s;
                                                            }
    void          store() override                          { v._level    = level->currentIndex();
                                                              v._effect   = effect->text();
                                                              v._when     = when->currentIndex();
                                                              v._affects  = affects->currentIndex();
                                                              v._constant = constant->isChecked();
                                                              v._pre      = pre->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]     = name();
                                                              obj["type"]     = type();
                                                              obj["adder"]    = isAdder();
                                                              obj["level"]    = v._level;
                                                              obj["effect"]   = v._effect;
                                                              obj["when"]     = v._when;
                                                              obj["affects"]  = v._affects;
                                                              obj["constant"] = v._constant;
                                                              obj["pre"]      = v._pre;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f;
        switch (v._level) {
        case 1: f = Fraction(1, 4); break;
        case 2: f = Fraction(1, 2); break;
        case 3: f = Fraction(1);    break;
        }
        switch (v._affects) {
        case 1:                      break;
        case 2:
        case 4: f -= Fraction(1, 4); break;
        case 3:
        case 5: f += Fraction(1, 4); break; // NOLINT
        }
        if (v._constant) f += Fraction(1, 4);
        if (v._pre) f -= Fraction(1, 4);
        switch (v._when) {
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
        int     _level;
        QString _effect;
        int     _when;
        int     _affects;
        bool    _constant;
        bool    _pre;
    } v {};

    gsl::owner<QComboBox*> level = nullptr;
    gsl::owner<QLineEdit*> effect = nullptr;
    gsl::owner<QComboBox*> when = nullptr;
    gsl::owner<QComboBox*> affects = nullptr;
    gsl::owner<QCheckBox*> constant = nullptr;
    gsl::owner<QCheckBox*> pre = nullptr;

    QString optOut(bool show) {
        QStringList affects { "", "Character", "Aropund Character", "Recipient", "Character And Recipient" };
        QStringList when { "", "Required Roll Fails", "A Thing Happens", "When Power Used",
                           "When Power Stops Being Used" };
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "");
        if (v._affects < 1 || v._when < 1 || v._level < 1 || v._effect.isEmpty()) return "<incomplete>";
        desc += "Side Effect (" + v._effect;
        desc += " On " + affects[v._affects];
        desc += " When " + when[v._when];
        if (v._constant) desc += "; Fixed Damage";
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
                   json["adder"].toBool(false)) { v._rmod = json["rmod"].toBool(false);
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
                                                              rmod->setChecked(s._rmod);
                                                              v = s;
                                                            }
    void          store() override                          { v._rmod = rmod->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["rmod"]  = v._rmod;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v._rmod) f = Fraction(1, 2);
        return f;
    }

private:
    struct vars {
        bool _rmod;
    } v {};

    gsl::owner<QCheckBox*> rmod = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(Modifier::NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "Standard Range";
        if (v._rmod) desc += " (Subject To Range Penallties)";
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
                   json["adder"].toBool(false)) { v._stunned = json["stunned"].toBool(false);
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
                                                              stunned->setChecked(s._stunned);
                                                              v = s;
                                                            }
    void          store() override                          { v._stunned = stunned->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]    = name();
                                                              obj["type"]    = type();
                                                              obj["adder"]   = isAdder();
                                                              obj["stunned"] = v._stunned;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v._stunned) f = Fraction(1, 2);
        return f;
    }

private:
    struct vars {
        bool _stunned;
    } v {};

    gsl::owner<QCheckBox*> stunned = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "Stops Working If Knocked Out";
        if (v._stunned) desc += " Or Stunned";
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
                   json["adder"].toBool(isModifier)) { v._all = json["all"].toBool(false); }
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
    void          restore() override                        { vars s = v; all->setChecked(s._all); v = s; }
    void          store() override                          { v._all = all->isChecked(); }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["all"]   = v._all;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        if (v._all) return Fraction(1, 4);
        else return Fraction(1, 2);
    }

private:
    struct vars {
        bool _all;
    } v {};

    gsl::owner<QCheckBox*> all = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                       "Sticky";
        if (v._all) desc += " (Free on, free all)";
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
                   json["adder"].toBool(false)) { v._min = json["min"].toInt(0);
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
                                                              min->setText(QString("%1").arg(s._min));
                                                              v = s;
                                                            }
    void          store() override                          { v._min = min->text().toInt(0);
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["min"]   = v._min;
                                                              return obj;
                                                            }
    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        if (v._min >= 4 && v._min <= 8) return Fraction(1, 4); // NOLINT
        else if (v._min >= 9 && v._min <= 13) return Fraction(1, 2); // NOLINT
        else if (v._min >= 14 && v._min <= 18) return Fraction(3, 4); // NOLINT
        else if (v._min >= 19) return Fraction(1); // NOLINT
        return Fraction(0);
    }

private:
    struct vars {
        int _min;
    } v {};

    gsl::owner<QLineEdit*> min = nullptr;

    void numeric(QString) override {
        QString txt = min->text();
        if (txt.isEmpty() || isNumber(txt)) {
            if (txt.toInt(nullptr) < 1 && !txt.isEmpty()) min->undo();
            v._min = txt.toInt(nullptr);
            if (v._min < 1) v._min = 1;
            return;
        }
        min->undo();
    }

    QString optOut(bool show) {
        if (v._min < 0) return "<incomplete>";
        Fraction f(fraction(NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") +
                "STR MinimumꚚ (" + QString("%1)").arg(v._min);
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
                   json["adder"].toBool(false)) { v._type = json["lType"].toInt(0);
                                                  v._time = json["time"].toInt(0);
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
                                                              lType->setCurrentIndex(s._type);
                                                              ModifiersDialog::ref().update();
                                                              time->setCurrentIndex(s._time);
                                                              v = s;
                                                            }
    void          store() override                          { v._type = lType->currentIndex();
                                                              v._time = time->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["lType"] = v._type;
                                                              obj["time"]  = v._time;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        switch (v._type) {
        case 1:
        case 2: return (v._time) * Fraction(1, 4);
        case 3: if (v._time <= 5) return -4 + v._time * Fraction(1, 2); // NOLINT
                else return -1 + (v._time - 5) * Fraction(1, 4); // NOLINT
        }

        return Fraction(0);
    }

private:
    struct vars {
        int _type;
        int _time;
    } v {};

    gsl::owner<QComboBox*> lType = nullptr;
    gsl::owner<QComboBox*> time = nullptr;

    QString optOut(bool show) {
        if (v._type < 1 || v._time < 1) return "<incomplete>";
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
        switch (v._type) {
        case 1: desc += instant[v._time];  break;
        case 2: desc += constant[v._time]; break;
        case 3: desc += other[v._time];    break;
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

            switch (v._type) {
            case 1: time->addItems(instant);  break;
            case 2: time->addItems(constant); break;
            case 3: time->addItems(other);    break;
            }
            time->setCurrentIndex(-1);
            time->setCurrentText("How Long?");
            v._time = -1;
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
                   json["adder"].toBool(false)) { v._type  = json["lType"].toInt(0);
                                                  v._which = json["which"].toString();
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
                                                              lType->setCurrentIndex(s._type);
                                                              which->setText(s._which);
                                                              v = s;
                                                            }
    void          store() override                          { v._type = lType->currentIndex();
                                                              v._which = which->text();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["lType"] = v._type;
                                                              obj["which"] = v._which;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        switch (v._type) {
        case 1: return Fraction(1, 2);
        case 2: return Fraction(3, 4);
        case 3: return Fraction(1);
        }

        return Fraction(0);
    }

private:
    struct vars {
        int     _type;
        QString _which;
    } v {};

    gsl::owner<QComboBox*> lType = nullptr;
    gsl::owner<QLineEdit*> which = nullptr;

    QString optOut(bool show) {
        if (v._type < 1 || (v._type != 3 && v._which.isEmpty())) return "<incomplete>";
        Fraction f(fraction(NoStore));
        QStringList type = { "", "Single Dimension", "Related Group of Dimensions", "Any Dimension" };
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "Transdimensionalϴ (";
        switch (v._type) {
        case 0:
        case 1: desc += type[v._type] + "; " + v._which; break;
        case 2: desc += type[v._type];                  break;
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
                   json["adder"].toBool(false)) { v._change  = json["change"].toBool(false);
                                                  v._cond    = json["cond"].toString();
                                                  v._active  = json["active"].toBool(false);
                                                  v._act     = json["act"].toInt(0);
                                                  v._reset   = json["reset"].toInt(0);
                                                  v._expire  = json["expire"].toBool(false);
                                                  v._misfire = json["misfire"].toBool(false);
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
                                                              change->setChecked(s._change);
                                                              cond->setText(s._cond);
                                                              active->setChecked(s._active);
                                                              act->setCurrentIndex(s._act);
                                                              reset->setCurrentIndex(s._reset);
                                                              expire->setChecked(s._expire);
                                                              misfire->setChecked(s._misfire);
                                                            }
    void          store() override                          { v._change  = change->isChecked();
                                                              v._cond    = cond->text();
                                                              v._active  = active->isChecked();
                                                              v._act     = act->currentIndex();
                                                              v._reset   = reset->currentIndex();
                                                              v._expire  = expire->isChecked();
                                                              v._misfire = misfire->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]    = name();
                                                              obj["type"]    = type();
                                                              obj["adder"]   = isAdder();
                                                              obj["change"]  = v._change;
                                                              obj["cond"]    = v._cond;
                                                              obj["active"]  = v._active;
                                                              obj["act"]     = v._act;
                                                              obj["reset"]   = v._reset;
                                                              obj["expire"]  = v._expire;
                                                              obj["misfire"] = v._misfire;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v._change) f = Fraction(1, 2);
        if (v._active) f += Fraction(1, 4);
        f += (v._act - 1) * Fraction(1, 4);
        f += (v._reset - 3) * Fraction(1, 4);
        if (v._expire) f -= Fraction(1, 4);
        if (v._misfire) f -= Fraction(1, 4);
        if (f < Fraction(1, 4)) f = Fraction(1, 4);
        return f;
    }

private:
    struct vars {
        int     _change;
        QString _cond;
        bool    _active;
        int     _act;
        int     _reset;
        bool    _expire;
        bool    _misfire;
    } v {};

    gsl::owner<QCheckBox*> change = nullptr;
    gsl::owner<QLineEdit*> cond = nullptr;
    gsl::owner<QCheckBox*> active = nullptr;
    gsl::owner<QComboBox*> act = nullptr;
    gsl::owner<QComboBox*> reset = nullptr;
    gsl::owner<QCheckBox*> expire = nullptr;
    gsl::owner<QCheckBox*> misfire = nullptr;

    QString optOut(bool show) {
        if (v._cond.isEmpty() || v._act < 1 || v._reset < 1) return "<incomplete>";
        Fraction f(fraction(NoStore));
        QStringList reset { "", "Turn to", "Full Phase to", "Half Phase to", "0-Phase to", "Automatically" };
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "Trigger (";
        desc += v._cond;
        if (v._change) desc += "; Can change conditions";
        if (v._active) desc += ": 0-Phase Act.";
        desc += QString("; %1 Activating Conditions").arg(v._act);
        desc += "; " + reset[v._reset] + " reset";
        if (v._expire) desc += "; expires";
        if (v._misfire) desc += "; misfire";
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
                   json["addr"].toBool(isModifier)) { v._until = json["until"].toString(); }
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
    void          restore() override                        { vars s = v; until->setText(s._until); v = s; }
    void          store() override                          { v._until = until->text(); }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["until"] = v._until;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        return Fraction(1, 2);
    }

private:
    struct vars {
        QString _until;
    } v {};

    gsl::owner<QLineEdit*> until = nullptr;

    QString optOut(bool show) {
        if (v._until.isEmpty()) return "<incomplete>";
        Fraction f(fraction(NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "Uncontrolled Until " + v._until;
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
                   json["adder"].toBool(false)) { v._num   = json["num"].toInt(0);
                                                  v._which = json["which"].toString();
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
                                                              num->setText(QString("%1").arg(s._num));
                                                              which->setText(s._which);
                                                              v = s;
                                                            }
    void          store() override                          { v._num = num->text().toInt();
                                                              v._which = which->text();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]  = name();
                                                              obj["type"]  = type();
                                                              obj["adder"] = isAdder();
                                                              obj["num"]   = v._num;
                                                              obj["which"] = v._which;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        return v._num * Fraction(1, 4);
    }

private:
    struct vars {
        int     _num;
        QString _which;
    } v {};

    gsl::owner<QLineEdit*> num = nullptr;
    gsl::owner<QLineEdit*> which = nullptr;

    QString optOut(bool show) {
        if (v._num < 1 || v._which.isEmpty()) return "<incomplete>";
        Fraction f(fraction(NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "Usable As " + v._which;
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
                   json["adder"].toBool(false)) { v._doubles = json["double"].toInt(0);
                                                  v._ten     = json["ten"].toBool(false);
                                                  v._one     = json["one"].toBool(false);
                                                  v._force   = json["force"].toBool(false);
                                                  v._control = json["control"].toInt(0);
                                                  v._pays    = json["pays"].toBool(false);
                                                  v._range   = json["range"].toInt(0);
                                                  v._stay    = json["stay"].toInt(0);
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
                                                              doubles->setText(QString("%1").arg(s._doubles));
                                                              ten->setChecked(s._ten);
                                                              one->setChecked(s._one);
                                                              force->setChecked(s._force);
                                                              control->setCurrentIndex(s._control);
                                                              pays->setChecked(s._pays);
                                                              range->setCurrentIndex(s._range);
                                                              stay->setCurrentIndex(s._stay);
                                                              v = s;
                                                            }
    void          store() override                          { v._doubles = doubles->text().toInt(0);
                                                              v._ten     = ten->isChecked();
                                                              v._one     = one->isChecked();
                                                              v._force   = force->isChecked();
                                                              v._control = control->currentIndex();
                                                              v._pays    = pays->isChecked();
                                                              v._range   = range->currentIndex();
                                                              v._stay    = stay->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]    = name();
                                                              obj["type"]    = type();
                                                              obj["adder"]   = isAdder();
                                                              obj["doubles"] = v._doubles;
                                                              obj["ten"]     = v._ten;
                                                              obj["one"]     = v._one;
                                                              obj["force"]   = v._force;
                                                              obj["control"] = v._control;
                                                              obj["pays"]    = v._pays;
                                                              obj["range"]   = v._range;
                                                              obj["stay"]    = v._stay;
                                                              return obj;
                                                            }
    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(1, 4);
        if (v._ten) f += Fraction(1);
        else f += v._doubles * Fraction(1, 4);
        if (v._one) f -= Fraction(1, 4);
        if (v._force) f += Fraction(1);
        f += v._control * Fraction(1, 4);
        if (v._pays) f -= Fraction(1, 4);
        f += (v._range) * Fraction(1, 4);
        f -= (v._stay) * Fraction(1, 4);
        if (f < Fraction(1, 4)) f = Fraction(1, 4);
        return f;
    }

private:
    struct vars {
        int  _doubles;
        bool _ten;
        bool _one;
        bool _force;
        int  _control;
        bool _pays;
        int  _range;
        int  _stay;
    } v {};

    gsl::owner<QLineEdit*> doubles = nullptr;
    gsl::owner<QCheckBox*> ten = nullptr;
    gsl::owner<QCheckBox*> one = nullptr;
    gsl::owner<QCheckBox*> force = nullptr;
    gsl::owner<QComboBox*> control = nullptr;
    gsl::owner<QCheckBox*> pays = nullptr;
    gsl::owner<QComboBox*> range = nullptr;
    gsl::owner<QComboBox*> stay = nullptr;

    void numeric(QString) override {
        QString txt = doubles->text();
        if (txt.isEmpty() || isNumber(txt)) {
            if (txt.toInt(nullptr) < 0 && !txt.isEmpty()) doubles->undo();
            v._doubles = txt.toInt(nullptr);
            if (v._doubles < 0) v._doubles = 0;
            return;
        }
        doubles->undo();
    }

    QString optOut(bool show) {
        if (v._doubles < 0) return "<incomplete>";
        Fraction f(fraction(NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "");
        if (v._force) desc += "Usable Against Othersϴ";
        else desc += "Usable On Othersϴ";
        desc += "(x" + QString("%1").arg((int) pow(2.0, v._doubles)) + " targets"; // NOLINT
        if (v._ten) desc += "; Anyone In 10m";
        if (v._one) desc += "; One At A Time";
        switch (v._control) {
        case 0: desc += "; Recipient Controls";                        break;
        case 1: desc += "; Recipient Controls But Grantor Can Revoke"; break;
        case 2: desc += "; Grantor Controls";                          break;
        }
        if (v._pays) desc += "; Grantor Pays END";
        else desc += "; Receipient Pays END";
        switch (v._range) {
        case 0: desc += "; Must Be In Reach";         break;
        case 1: desc += "; Must Be In Limited Range"; break;
        case 2: desc += "; Must Be In Range";         break;
        }
        desc += " When Granted";
        switch (v._stay) {
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
                   json["adder"].toBool(false)) { v._whole   = json["whole"].toInt(0);
                                                  v._half    = json["half"].toBool(false);
                                                  v._quarter = json["quarter"].toBool(false);
                                                  v._limit   = json["limit"].toBool(false);
                                                  v._advs    = json["advs"].toString();
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
                                                              whole->setText(QString("%1").arg(s._whole));
                                                              half->setChecked(s._half);
                                                              quarter->setChecked(s._quarter);
                                                              limit->setChecked(s._limit);
                                                              advs->setText(s._advs);
                                                              v = s;
                                                            }
    void          store() override                          { v._whole   = whole->text().toInt(0);
                                                              v._half    = half->isChecked();
                                                              v._quarter = quarter->isChecked();
                                                              v._limit   = limit->isChecked();
                                                              v._advs    = advs->text();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]    = name();
                                                              obj["type"]    = type();
                                                              obj["adder"]   = isAdder();
                                                              obj["whole"]   = v._whole;
                                                              obj["half"]    = v._half;
                                                              obj["quarter"] = v._quarter;
                                                              obj["limit"]   = v._limit;
                                                              obj["advs"]    = v._advs;
                                                              return obj;
                                                            }
    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(v._whole);
        if (v._half) f += Fraction(1, 2);
        if (v._quarter) f -= Fraction(1, 4);
        if (v._limit) f -= Fraction(1, 4);
        f = f * 2;
        if (f < Fraction(1, 4)) f = Fraction(1, 4);
        return f;
    }

private:
    struct vars {
        int     _whole;
        bool    _half;
        bool    _quarter;
        bool    _limit;
        QString _advs;
    } v {};

    gsl::owner<QLineEdit*> whole = nullptr;
    gsl::owner<QCheckBox*> half = nullptr;
    gsl::owner<QCheckBox*> quarter = nullptr;
    gsl::owner<QCheckBox*> limit = nullptr;
    gsl::owner<QLineEdit*> advs = nullptr;

    QString optOut(bool show) {
        if (v._whole < 0 || (v._limit && v._advs.isEmpty())) return "<incomplete>";
        Fraction f(fraction(NoStore));
        Fraction amt(v._whole);
        if (v._half) amt += Fraction(1, 2);
        if (v._quarter) amt += Fraction(1, 4);
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "");
        desc += "Variable Advantageϴ";
        desc += " (+" + amt.toString() + " in Advantages";
        if (v._limit) desc += "; Only From: " + v._advs;
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
                   json["adder"].toBool(false)) { v._effect = json["effect"].toString(); }
    ~VariableEffect() override { }

    VariableEffect& operator=(const VariableEffect&) = delete;
    VariableEffect& operator=(VariableEffect&&) = delete;

    shared_ptr<Modifier> create() override                         { return make_shared<VariableEffect>(*this); }
    shared_ptr<Modifier> create(const QJsonObject& json) override  { return make_shared<VariableEffect>(json); }

    void          changed(QString) override                 { store(); ModifiersDialog::ref().updateForm(); }
    QString       description(bool show = false) override   { return optOut(show); }
    bool          form(QWidget* p, QVBoxLayout* l) override { effect = createLineEdit(p, l, "Effects?", std::mem_fn(&ModifierBase::changed));
                                                              return true; }
    void          restore() override                        { vars s = v; effect->setText(s._effect); v = s; }
    void          store() override                          { v._effect = effect->text(); }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]   = name();
                                                              obj["type"]   = type();
                                                              obj["adder"]  = isAdder();
                                                              obj["effect"] = v._effect;
                                                              return obj;
                                                            }
    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        return Fraction(1, 2);
    }

private:
    struct vars {
        QString _effect;
    } v {};

    gsl::owner<QLineEdit*> effect = nullptr;

    QString optOut(bool show) {
        if (v._effect.isEmpty()) return "<incomplete>";
        Fraction f(fraction(NoStore));
        return (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "Variable Effect▲ (versus " + v._effect + ")";
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
                   json["adder"].toBool(false)) { v._whole   = json["whole"].toInt(0);
                                                  v._half    = json["half"].toBool(false);
                                                  v._quarter = json["quarter"].toBool(false);
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
                                                              whole->setText(QString("%1").arg(s._whole));
                                                              half->setChecked(s._half);
                                                              quarter->setChecked(s._quarter);
                                                              v = s;
                                                            }
    void          store() override                          { v._whole   = whole->text().toInt(0);
                                                              v._half    = half->isChecked();
                                                              v._quarter = quarter->isChecked();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]    = name();
                                                              obj["type"]    = type();
                                                              obj["adder"]   = isAdder();
                                                              obj["whole"]   = v._whole;
                                                              obj["half"]    = v._half;
                                                              obj["quarter"] = v._quarter;
                                                              return obj;
                                                            }
    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        Fraction f(v._whole);
        if (v._half) f += Fraction(1, 2);
        if (v._quarter) f -= Fraction(1, 4);
        f = f * 2;
        return f;
    }

private:
    struct vars {
        int     _whole;
        bool    _half;
        bool    _quarter;
    } v {};

    gsl::owner<QLineEdit*> whole = nullptr;
    gsl::owner<QCheckBox*> half = nullptr;
    gsl::owner<QCheckBox*> quarter = nullptr;

    QString optOut(bool show) {
        if (v._whole < 0) return "<incomplete>";
        Fraction f(fraction(NoStore));
        Fraction amt(v._whole);
        if (v._half) amt += Fraction(1, 2);
        if (v._quarter) amt += Fraction(1, 4);
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
                   json["adder"].toBool(false)) { v._effect = json["effect"].toString();
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
                                                              effect->setText(s._effect);
                                                              v = s;
                                                            }
    void          store() override                          { v._effect = effect->text();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]   = name();
                                                              obj["type"]   = type();
                                                              obj["adder"]  = isAdder();
                                                              obj["effect"] = v._effect;
                                                              return obj;
                                                            }
    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        if (v._effect.isEmpty()) return Fraction(1, 2);
        return Fraction(1, 4);
    }

private:
    struct vars {
        QString _effect;
    } v {};

    gsl::owner<QLineEdit*> effect = nullptr;

    QString optOut(bool show) {
        Fraction f(fraction(NoStore));
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + "Variable Special Effects";
        if (!v._effect.isEmpty()) desc += " (from:  " + v._effect + ")";
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
                   json["adder"].toBool(false)) { v._characteristic  = json["characteristic"].toInt(0);
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
                                                              characteristic->setCurrentIndex(s._characteristic);
                                                              v = s;
                                                            }
    void          store() override                          { v._characteristic = characteristic->currentIndex();
                                                            }
    QJsonObject   toJson() override                         { QJsonObject obj;
                                                              obj["name"]           = name();
                                                              obj["type"]           = type();
                                                              obj["adder"]          = isAdder();
                                                              obj["characteristic"] = v._characteristic;
                                                              return obj;
                                                            }

    Fraction fraction(bool noStore = false) override {
        if (!noStore) store();
        return Fraction(1, 4);
    }

private:
    struct vars {
        int _characteristic;
    } v {};

    gsl::owner<QComboBox*> characteristic = nullptr;

    QString optOut(bool show) {
        if (v._characteristic < 1) return "<incomplete>";
        Fraction f(fraction(NoStore));
        QStringList characteristic = { "", "STR", "DEX", "INT", "EGO", "PRE", "OCV", "DCV", "OMCV", "DMCV",
                                       "SPD", "PD", "ED", "REC", "END", "BODY", "STUN" };
        QString desc = (show ? QString("(%1").arg((f < 0) ? "" : "+") + f.toString() + ") " : "") + QString("Works Against EGO Not %1▲").arg(characteristic[v._characteristic]);
        return desc;
    }
};

#endif // MODIFIER_H
