#ifndef POWERS_H
#define POWERS_H

#include "powerdialog.h"
#include "fraction.h"
#include "modifier.h"
#include "skilltalentorperk.h"

#include <functional>
#include <map>

#include <QCheckBox>
#include <QComboBox>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QObject>
#include <QTableWidget>
#include <QTreeWidget>
#include <QVBoxLayout>

class Power {
protected:
    typedef std::_Mem_fn<void (Power::*)(int)>            lineEditCallback;
    typedef std::_Mem_fn<void (Power::*)(bool)>           checkBoxCallback;
    typedef std::_Mem_fn<void (Power::*)(int)>            comboBoxCallback;
    typedef std::_Mem_fn<void (Power::*)(void)>           pushButtonCallback;
    typedef std::_Mem_fn<void (Power::*)(int, int, bool)> treeCallback;

    QCheckBox*   createCheckBox(QWidget*, QVBoxLayout*, QString, checkBoxCallback, int before = -1);
    QCheckBox*   createCheckBox(QWidget*, QVBoxLayout*, QString, int before = -1);
    QComboBox*   createComboBox(QWidget*, QVBoxLayout*, QString, QList<QString>, comboBoxCallback, int before = -1);
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

    static std::map<QWidget*, QLineEdit*> _labeledEdits;

    QTableWidget* createAdvantages(QWidget* parent, QVBoxLayout* layout) {
        return PowerDialog::ref().createAdvantages(parent, layout);
    }
    QTableWidget* createLimitations(QWidget* parent, QVBoxLayout* layout) {
        return PowerDialog::ref().createLimitations(parent, layout);
    }

    QList<shared_ptr<Modifier>>::iterator findModifier(QString);

    std::map<QCheckBox*,   checkBoxCallback>   _callbacksCB;
    std::map<QComboBox*,   comboBoxCallback>   _callbacksCBox;
    std::map<QLineEdit*,   lineEditCallback>   _callbacksEdit;
    std::map<QTreeWidget*, treeCallback>       _callbacksTree;
    std::map<QPushButton*, pushButtonCallback> _callbacksBtn;
    void empty(bool) { }

    QString add(QString n, int p) {
        QList<QString> vals = n.split("/");
        QString res;
        bool first = true;
        for (const auto& val: vals) {
            if (first) first = false;
            else res += "/";
            QList<QString> num = val.split("-");
            res += QString("%1-").arg(num[0].toInt(0) + p);
        }
        return res;
    }

    QList<shared_ptr<Modifier>> _advantagesList;
    QList<shared_ptr<Modifier>> _limitationsList;

    bool     _inMultipower = false;
    QWidget* _sender;

    Power* _parent = nullptr;
    int    _row    = -1;

public:
    Ui::PowerDialog* ui;

    QLineEdit* labeledEdit(QWidget* w)       { return _labeledEdits[w]; }
    bool       labeledEditExists(QWidget* w) { return _labeledEdits.find(w) != _labeledEdits.end(); }

    class allBase {
    public:
        allBase() { }
        allBase(const allBase&) { }
        allBase(allBase*) { }

        virtual shared_ptr<Power> create()                        = 0;
        virtual shared_ptr<Power> create(const QJsonObject& json) = 0;
    };

    template <typename T>
    class allPower: public allBase {
    public:
        allPower(): allBase()                          { }
        allPower(const allPower& b): allBase(b) { }
        allPower(allPower* b): allBase(b)       { }

        shared_ptr<Power> create() override                        { return make_shared<T>(); }
        shared_ptr<Power> create(const QJsonObject& json) override { return make_shared<T>(json); }
    };


    Power();
    Power(const Power& p)
        : _advantagesList(p._advantagesList)
        , _limitationsList(p._limitationsList)
        , _inMultipower(p._inMultipower)
        , _sender(p._sender)
        , _parent(p._parent)
        , _row(p._row)
        , _modifiers(p._modifiers) { Power(); }
    Power(Power&& p)
        : _advantagesList(p._advantagesList)
        , _limitationsList(p._limitationsList)
        , _inMultipower(p._inMultipower)
        , _sender(p._sender)
        , _parent(p._parent)
        , _row(p._row)
        , _modifiers(p._modifiers) { Power(); }
    virtual ~Power() { }

    virtual Power& operator=(const Power& s) {
        if (this != &s) {
            _advantagesList = s._advantagesList;
            _limitationsList = s._limitationsList;
            _inMultipower = s._inMultipower;
            _sender = s._sender;
            _parent = s._parent;
            _row = s._row;
            _modifiers = s._modifiers;
            ui = s.ui;
        }
        return *this;
    }
    virtual Power& operator=(Power&& s) {
        _advantagesList = s._advantagesList;
        _limitationsList = s._limitationsList;
        _inMultipower = s._inMultipower;
        _sender = s._sender;
        _parent = s._parent;
        _row = s._row;
        _modifiers = s._modifiers;
        ui = s.ui;
        return *this;
    }

    static const bool NoStore = true;
    static const bool ShowEND = true;

    virtual QString     description(bool roll = false) { return roll ? "" : ""; };
    virtual void        form(QWidget*, QVBoxLayout*)   { }
    virtual QString     name()                         { return ""; }
    virtual Points<>    points(bool noStore = false)   { return noStore ? 0_cp : 0_cp; }
    virtual QString     nickname()                     { return ""; }
    virtual void        restore()                      { }
    virtual void        store()                        { }
    virtual bool        varying()                      { return false; }

    virtual QJsonObject toJson()                 { QJsonObject obj;
                                                   QJsonObject mods;
                                                   for (const auto& mod: _advantagesList)  mods[mod->name()] = mod->toJson();
                                                   for (const auto& mod: _limitationsList) mods[mod->name()] = mod->toJson();
                                                   obj["modifiers"] = mods;
                                                   return obj;
                                                 }
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

    Power* parent()         { return _parent; }
    Power* parent(Power* p) { _parent = p; return _parent; }
    int    row()            { return _row; }
    int    row(int r)       { _row = r; return _row; }

    struct sizeMods {
        int _STR;
        int _CON;
        int _PRE;
        int _PD;
        int _ED;
        int _BODY;
        int _STUN;
        int _reach;
        int _running;
        int _KBRes;
        int _aoeRad;
        QString _comp;
    };

    virtual int characteristic(int) { return 0; }

    virtual bool                      isFramework()                  { return false; }
    virtual bool                      isMultipower()                 { return false; }
    virtual bool                      isVPP()                        { return false; }
    virtual bool                      isValid(shared_ptr<Power>)     { return true; }
    virtual void                      inMultipower()                 { _inMultipower = true; }
    virtual Points<>                  pool()                         { return 0_cp; }
    virtual int                       count()                        { return -1; }
    virtual void                      append(shared_ptr<Power>)      { }
    virtual void                      insert(int, shared_ptr<Power>) { }
    virtual Points<>                  display(int&, QTableWidget*)   { return 0_cp; }
    virtual Points<>                  display(QString&)              { return 0_cp; }
    virtual void                      remove(int)                    { }
    virtual void                      remove(shared_ptr<Power>)      { }
    virtual QList<shared_ptr<Power>>& list()                         { static QList<shared_ptr<Power>> l; return l; }

    virtual int                           FD()          { return 0; }
    virtual int                           MD()          { return 0; }
    virtual int                           PowD()        { return 0; }
    virtual int                           rED()         { return 0; }
    virtual int                           rPD()         { return 0; }
    virtual shared_ptr<SkillTalentOrPerk> skill()       { return nullptr; }
    virtual int                           str()         { return 0; }
    virtual sizeMods&                     growthStats() { static sizeMods sm; return sm; }
    virtual int                           move()        { return 0; }
    virtual QString                       units()       { return "m"; }

    int doubling();

    Points<> real(Fraction a = Fraction(0), Points<> mod = 0_cp, Fraction s = Fraction(0));
    Points<> acting(Fraction a = Fraction(0), Points<> mod = 0_cp);
    Points<> active();

    QList<shared_ptr<Modifier>>& advantagesList()  { return _advantagesList; }
    QList<shared_ptr<Modifier>>& limitationsList() { return _limitationsList; }
    QList<shared_ptr<Modifier>>& modifiers()       { return _modifiers; }

    QWidget* sender() { return _sender; }

    void callback(QCheckBox*);
    void callback(QComboBox*);
    void callback(QLineEdit*);
    void callback(QTreeWidget* tree);
    void callback(QPushButton* btn);

    void createForm(QWidget*, QVBoxLayout*);

    static QList<QString>    Available();
    static void              ClearForm(QVBoxLayout*);
    static void              ClearLabeledEdits() { _labeledEdits.clear(); }
    static QList<QString>    AdjustmentPowers();
    static QList<QString>    AttackPowers();
    static QList<QString>    AutomatonPowers();
    static QList<QString>    BodyAffectingPowers();
    static QList<QString>    DefensePowers();
    static QList<QString>    FrameworkPowers();
    static QList<QString>    MentalPowers();
    static QList<QString>    MovementPowers();
    static QList<QString>    SenseAffectingPowers();
    static QList<QString>    SensoryPowers();
    static QList<QString>    SpecialPowers();
    static QList<QString>    StandardPowers();
    static shared_ptr<Power> ByName(QString);
    static shared_ptr<Power> FromJson(QString, const QJsonObject&);

    bool isNumber(QString);

private:
    QList<shared_ptr<Modifier>> _modifiers;

    static QMap<QString, allBase*> _adjustmentPower;
    static QMap<QString, allBase*> _attackPower;
    static QMap<QString, allBase*> _automatonPower;
    static QMap<QString, allBase*> _bodyAffectingPower;
    static QMap<QString, allBase*> _defensePower;
    static QMap<QString, allBase*> _frameworkPower;
    static QMap<QString, allBase*> _mentalPower;
    static QMap<QString, allBase*> _movementPower;
    static QMap<QString, allBase*> _senseAffectingPower;
    static QMap<QString, allBase*> _sensoryPower;
    static QMap<QString, allBase*> _specialPower;
    static QMap<QString, allBase*> _standardPower;
};

class AllPowers: public Power {
protected:
    int countCommas(QString x) {
        if (x.isEmpty()) return 0;
        QStringList words = x.split(",");
        return words.count();
    }

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
    AllPowers(): Power() { }
    AllPowers(QString name)
        : Power()
        , v { name, "" } { }
    AllPowers(const AllPowers& s)
        : Power(s)
        , v(s.v) { }
    AllPowers(AllPowers&& s)
        : Power(s)
        , v(s.v) { }
    AllPowers(const QJsonObject& json)
        : Power()
        , v { json["name"].toString(), json["powerName"].toString(""), json["varies"].toBool(false) } { }

    virtual AllPowers& operator=(const AllPowers& s) {
        if (this != &s) {
            Power::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual AllPowers& operator=(AllPowers&& s) {
        Power::operator=(s);
        v = s.v;
        return *this;
    }

    QString     description(bool roll = false) override = 0;
    Points<>    points(bool noStore = false) override   = 0;

    Fraction adv() override         { return Fraction(0); }
    void     checked(bool) override { }
    Fraction lim() override         { return Fraction(0); }
    void     numeric(int) override  { }

    void        form(QWidget* widget, QVBoxLayout* layout) override { powerName = createLineEdit(widget, layout, "Nickname of power");
                                                                      if (_parent != nullptr && _parent->isMultipower()) varies = createCheckBox(widget, layout, "Varies");
                                                                      else if (_inMultipower) varies = createCheckBox(widget, layout, "Varies");
                                                                      else varies = nullptr;
                                                                    }
    QString     name() override                                     { return v._name;
                                                                    }
    QString     nickname() override                                 { return v._powerName;
                                                                    }
    void        restore() override                                  { vars s = v;
                                                                      powerName->setText(s._powerName);
                                                                      if (varies != nullptr) varies->setChecked(s._varies);
                                                                      v = s;
                                                                    }
    void        store() override                                    { v._powerName = powerName->text();
                                                                      if (varies != nullptr) v._varies = varies->isChecked();
                                                                    }
    QJsonObject toJson() override                                   { QJsonObject obj = Power::toJson();
                                                                      obj["name"]      = v._name;
                                                                      obj["powerName"] = v._powerName;
                                                                      obj["varies"]    = v._varies;
                                                                      return obj;
                                                                    }
    bool        varying() override                                  { return v._varies; }

private:
    struct vars {
        QString _name      = "";
        QString _powerName = "";
        bool    _varies    = false;
    } v;

    QLineEdit* powerName;
    QCheckBox* varies;
};


#endif // POWERS_H
