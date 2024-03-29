#include "powers.h"
#ifndef ISHSC
#include "powerdialog.h"
#endif

#include "AdjustmentPowers.h"
#include "AttackPowers.h"
#include "AutomatonPowers.h"
#include "BodyAffectingPowers.h"
#include "DefensePowers.h"
#include "Equipment.h"
#include "FrameworkPowers.h"
#include "MentalPowers.h"
#include "MovementPowers.h"
#include "SenseAffectingPowers.h"
#include "SensoryPowers.h"
#include "SpecialPowers.h"
#include "StandardPowers.h"

#include <QFile>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QScreen>
#include <QScrollBar>
#include <QWidget>

// NOLINTNEXTLINE
#define MAKE(x)    { #x, new Power::allPower<x>() }
// NOLINTNEXTLINE
#define SPCS(x,y)  { x, #y }

// NOLINTNEXTLINE
struct _statics {
    QMap<QString, Power::allBase*> powers;
} static const statics {
    .powers {
        // Adjustment Powers
        MAKE(Absorption),
        MAKE(Aid),
        MAKE(Drain),
        MAKE(Healing),

        // Attack Powers
        MAKE(Blast),
        MAKE(ChangeEnvironment),
        MAKE(Entangle),
        MAKE(Flash),
        MAKE(HandToHandAttack),
        MAKE(KillingAttack),
        MAKE(Reflection),
        MAKE(Telekinesis),
        MAKE(Transform),

        // Automaton Powers
        MAKE(CannotBeStunned),
        MAKE(DoesNotBleed),
        MAKE(NoHitLocations),
        MAKE(TakesNoSTUN),

        // Body Affecting Powers
        MAKE(DensityIncrease),
        MAKE(Desolidification),
        MAKE(Duplication),
        MAKE(ExtraLimbs),
        MAKE(Growth),
        MAKE(Multiform),
        MAKE(ShapeShift),
        MAKE(Shrinking),
        MAKE(Stretching),

        // Defense Powers
        MAKE(Barrier),
        MAKE(DamageNegation),
        MAKE(DamageResistance),
        MAKE(Deflection),
        MAKE(FlashDefense),
        MAKE(KnockbackResistance),
        MAKE(MentalDefense),
        MAKE(PowerDefense),
        MAKE(ResistantDefense),

        // Mental Powers
        MAKE(MentalBlast),
        MAKE(MentalIllusions),
        MAKE(MindControl),
        MAKE(MindLink),
        MAKE(MindScan),
        MAKE(Telepathy),

        // Movement Powers
        MAKE(ExtraDimensionalMovement),
        MAKE(FTLTravel),
        MAKE(Flight),
        MAKE(Leaping),
        MAKE(Running),
        MAKE(Swimming),
        MAKE(Swinging),
        MAKE(Teleportation),
        MAKE(Tunneling),

        // Sense Affecting Powers
        MAKE(Darkness),
        MAKE(Images),
        MAKE(Invisibility),

        // Sensory Power
        MAKE(Clairsentience),
        MAKE(EnhancedSenses),

        // Special Power
        MAKE(Blank),
        MAKE(CustomPower),
        MAKE(EnduranceReserve),
        MAKE(IndependantAdvantage),
        MAKE(Luck),
        MAKE(Regeneration),
        MAKE(Skill),
        MAKE(TeleportLocation),

        // Standard Power
        MAKE(Characteristics),
        MAKE(Clinging),
        MAKE(Dispel),
        MAKE(LifeSupport),
        MAKE(Summon),

        // Frameworks
        MAKE(Group),
        MAKE(Multipower),
        MAKE(VPP),

        // Equipment
        MAKE(Weapon),
        MAKE(Armor),
        MAKE(Equip)
    }
};

const QMap<QString, QString> Power::_adjustmentPower {
    SPCS("Absorption▲", Absorption),
    SPCS("Aid",         Aid),
    SPCS("Drain",       Drain),
    SPCS("Healing",     Healing)
};

const QMap<QString, QString> Power::_attackPower {
    SPCS("Blast",               Blast),
    SPCS("Change Environment",  ChangeEnvironment),
    SPCS("Characteristics",     Characteristics),
    SPCS("Darkness",            Darkness),
    SPCS("Dispel",              Dispel),
    SPCS("Drain",               Drain),
    SPCS("Entangle",            Entangle),
    SPCS("Flash",               Flash),
    SPCS("Hand-To-Hand Attack", HandToHandAttack),
    SPCS("Images",              Images),
    SPCS("Killing Attack",      KillingAttack),
    SPCS("Mental Blast",        MentalBlast),
    SPCS("Mental Illusions",    MentalIllusions),
    SPCS("Mind Control",        MindControl),
    SPCS("Reflection",          Reflection),
    SPCS("Telekinesis",         Telekinesis),
    SPCS("Transformϴ",          Transform)
};

const QMap<QString, QString> Power::_automatonPower {
    SPCS("Cannot Be Stunnedϴ", CannotBeStunned),
    SPCS("Does Not Bleedϴ",    DoesNotBleed),
    SPCS("No Hit Locationsϴ",  NoHitLocations),
    SPCS("Takes No STUNϴ",     TakesNoSTUN)
};

const QMap<QString, QString> Power::_bodyAffectingPower {
    SPCS("Density Increase",  DensityIncrease),
    SPCS("Desolidificationϴ", Desolidification),
    SPCS("Duplication▲",      Duplication),
    SPCS("Extra Limbs",       ExtraLimbs),
    SPCS("Growth",            Growth),
    SPCS("Multiform",         Multiform),
    SPCS("Shape Shift",       ShapeShift),
    SPCS("Shrinking",         Shrinking),
    SPCS("Stretching",        Stretching)
};

const QMap<QString, QString> Power::_defensePower {
    SPCS("Barrier",              Barrier),
    SPCS("Characteristics",      Characteristics),
    SPCS("Damage Negation▲",     DamageNegation),
    SPCS("Damage Resistance▲",   DamageResistance),
    SPCS("Deflection▲",          Deflection),
    SPCS("Flash Defense",        FlashDefense),
    SPCS("Knockback Resistance", KnockbackResistance),
    SPCS("Mental Defense",       MentalDefense),
    SPCS("Power Defense",        PowerDefense),
    SPCS("Resistant Defense",    ResistantDefense)
};

const QMap<QString, QString> Power::_frameworkPower {
    SPCS("Group",               Group),
    SPCS("Multipower",          Multipower),
    SPCS("Variable Power Pool", VPP),
    SPCS("Blank Line",          Blank)
};

const QMap<QString, QString> Power::_mentalPower {
    SPCS("Mental Blast",     MentalBlast),
    SPCS("Mental Illusions", MentalIllusions),
    SPCS("Mind Control",     MindControl),
    SPCS("Mind Link",        MindLink),
    SPCS("Mind Scan",        MindScan),
    SPCS("Telepathy",        Telepathy)
};

const QMap<QString, QString> Power::_movementPower {
    SPCS("Extra-Dimensional Movementϴ", ExtraDimensionalMovement),
    SPCS("FTL Travel",                  FTLTravel),
    SPCS("Flight",                      Flight),
    SPCS("Leaping",                     Leaping),
    SPCS("Running",                     Running),
    SPCS("Swimming",                    Swimming),
    SPCS("Swinging",                    Swinging),
    SPCS("Teleportation",               Teleportation),
    SPCS("Tunneling",                   Tunneling)
};

const QMap<QString, QString> Power::_senseAffectingPower {
    SPCS("Darkness",     Darkness),
    SPCS("Flash",        Flash),
    SPCS("Images",       Images),
    SPCS("Invisibility", Invisibility)
};

const QMap<QString, QString> Power::_sensoryPower {
    SPCS("Clairsentienceϴ", Clairsentience),
    SPCS("Enhanced Senses", EnhancedSenses),
    SPCS("Mind Scan",       MindScan)
};

const QMap<QString, QString> Power::_specialPower {
    SPCS("Custom Power",          CustomPower),
    SPCS("Duplication",           Duplication),
    SPCS("Endurance Reserve",     EnduranceReserve),
    SPCS("Enhanced Senses",       EnhancedSenses),
    SPCS("Extra Limbs",           ExtraLimbs),
    SPCS("Flash Defense",         FlashDefense),
    SPCS("Independant Advantage", IndependantAdvantage),
    SPCS("Knockback Resistance",  KnockbackResistance),
    SPCS("Luck",                  Luck),
    SPCS("Mental Defense",        MentalDefense),
    SPCS("Power Defense",         PowerDefense),
    SPCS("Regeneration",          Regeneration),
    SPCS("Skill",                 Skill),
    SPCS("Teleport Location",     TeleportLocation)
};

const QMap<QString, QString> Power::_standardPower {
    SPCS("Barrier",             Barrier),
    SPCS("Blast",               Blast),
    SPCS("Change Environment",  ChangeEnvironment),
    SPCS("Characteristics",     Characteristics),
    SPCS("Clairsentienceϴ",     Clairsentience),
    SPCS("Clinging",            Clinging),
    SPCS("Damage Negation▲",    DamageNegation),
    SPCS("Damage Resistance▲",  DamageResistance),
    SPCS("Darkness",            Darkness),
    SPCS("Deflection▲",         Deflection),
    SPCS("Density Increase",    DensityIncrease),
    SPCS("Desolidificationϴ",   Desolidification),
    SPCS("Dispel",              Dispel),
    SPCS("Entangle",            Entangle),
    SPCS("Flash",               Flash),
    SPCS("Hand-To-Hand Attack", HandToHandAttack),
    SPCS("Images",              Images),
    SPCS("Killing Attack",      KillingAttack),
    SPCS("Life Support",        LifeSupport),
    SPCS("Multiform",           Multiform),
    SPCS("Reflection",          Reflection),
    SPCS("Resistant Defense",   ResistantDefense),
    SPCS("Shape Shift",         ShapeShift),
    SPCS("Stretching",          Stretching),
    SPCS("Summonϴ",             Summon),
    SPCS("Telekinesis",         Telekinesis),
    SPCS("Transformϴ",          Transform)
};

const QMap<QString, QString> Power::_equipment {
    SPCS("Weapon",    Weapon),
    SPCS("Armor",     Armor),
    SPCS("Equipment", Equip)
};

#ifndef ISHSC
QTableWidget* Power::createAdvantages(QWidget* parent, QVBoxLayout* layout) {
    return PowerDialog::ref().createAdvantages(parent, layout);
#else
QTableWidget* Power::createAdvantages(QWidget*, QVBoxLayout*) {
    return nullptr;
#endif
}

#ifndef ISHSC
QTableWidget* Power::createLimitations(QWidget* parent, QVBoxLayout* layout) {
    return PowerDialog::ref().createLimitations(parent, layout);
#else
QTableWidget* Power::createLimitations(QWidget*, QVBoxLayout*) {
    return nullptr;
#endif
}

Power::Power() {
}

int Power::doubling() {
    int dbl = 1;
    for (const auto& mod: _modifiers) {
        if (!mod->isAdder() || mod->name() != "Improved Noncombat Movement") continue;
        dbl += mod->doubling();
    }
    return (int) pow(2, dbl);
}

bool Power::isNumber(QString txt) {
    bool ok = true;
    txt.toInt(&ok, 10);
    return ok;
}

void Power::callback(QCheckBox* checkBox) {
    _sender = checkBox;
    auto f = _callbacksCB.find(checkBox);
    if (f == _callbacksCB.end()) return;
    auto function = f->second;
    function(this, checkBox->isChecked());
}

void Power::callback(QLineEdit* edit) {
    _sender = edit;
    auto f = _callbacksEdit.find(edit);
    if (f == _callbacksEdit.end()) return;
    auto function = f->second;
    function(this, 0);
}

void Power::callback(QComboBox* combo) {
    _sender = combo;
    auto f = _callbacksCBox.find(combo);
    if (f == _callbacksCBox.end()) return;
    auto function = f->second;
    function(this, combo->currentIndex());
}

void Power::callback(QComboBox* combo, bool) {
    _sender = combo;
    auto f = _callbacksActivatedCBox.find(combo);
    if (f == _callbacksActivatedCBox.end()) return;
    auto function = f->second;
    function(this, combo->currentIndex(), 0);
}

void Power::callback(QPushButton* btn) {
    _sender = btn;
    auto f = _callbacksBtn.find(btn);
    if (f == _callbacksBtn.end()) return;
    auto function = f->second;
    function(this);
}

void Power::callback(QTreeWidget* tree) {
    _sender = tree;
    auto f = _callbacksTree.find(tree);
    if (f == _callbacksTree.end()) return;
    auto function = f->second;
    function(this, 0, 0, false);
}

QCheckBox* Power::createCheckBox(QWidget* parent, QVBoxLayout* layout, QString prompt, int before) {
    QCheckBox* checkBox = new QCheckBox(layout->parentWidget());
    checkBox->setText(prompt);
    checkBox->setChecked(false);
#ifdef unix
    QString style = "color: #000; background: #fff;";
    checkBox->setStyleSheet(style);
#endif
    if (before == -1) layout->addWidget(checkBox);
    else layout->insertWidget(before, checkBox);
    parent->connect(checkBox, SIGNAL(stateChanged(int)), parent, SLOT(stateChanged(int)));
    return checkBox;
}

QCheckBox* Power::createCheckBox(QWidget* parent, QVBoxLayout* layout, QString prompt, checkBoxCallback callback, int before) {
    QCheckBox* checkBox = createCheckBox(parent, layout, prompt, before);
    if (checkBox) _callbacksCB.emplace(checkBox, callback);
    return checkBox;
}

QComboBox* Power::createComboBox(QWidget* parent, QVBoxLayout* layout, QString prompt, QList<QString> options, comboBoxCallback callback,
                                 int before) {
    QComboBox* comboBox = createComboBox(parent, layout, prompt, options, before);
    if (comboBox) _callbacksCBox.emplace(comboBox, callback);
    return comboBox;
}

QComboBox* Power::createComboBox(QWidget* parent, QVBoxLayout* layout, QString prompt, QList<QString> options, activatedCallback callback,
                                 int before) {
    QComboBox* comboBox = createComboBox(parent, layout, prompt, options, before);
    if (comboBox) _callbacksActivatedCBox.emplace(comboBox, callback);
    return comboBox;
}

QComboBox* Power::createComboBox(QWidget* parent, QVBoxLayout* layout, QString prompt, QList<QString> options, int before) {
    QComboBox* comboBox = new QComboBox(layout->parentWidget());
    comboBox->addItems(options);
    comboBox->setPlaceholderText(prompt);
    comboBox->setToolTip(prompt);
    comboBox->setCurrentIndex(-1);
#ifdef unix
    QString style = "QComboBox { color: #000; background: #fff; } QComboBox QAbstractItemView { color: #000; background-color: #fff; }";
    comboBox->setStyleSheet(style);
#endif
    if (before == -1) layout->addWidget(comboBox);
    else layout->insertWidget(before, comboBox);
    parent->connect(comboBox, SIGNAL(currentIndexChanged(int)), parent, SLOT(currentIndexChanged(int)));
    parent->connect(comboBox, SIGNAL(activated(int)),           parent, SLOT(activated(int)));
    return comboBox;
}

QLabel* Power::createLabel(QWidget*, QVBoxLayout* layout, QString text, int before) {
    QLabel* label = new QLabel(layout->parentWidget());
    label->setText(text);
#ifdef unix
    QString style = "color: #000; background: #fff;";
    label->setStyleSheet(style);
#endif
    if (before == -1) layout->addWidget(label);
    else layout->insertWidget(before, label);
    return label;
}

QLineEdit* Power::createLineEdit(QWidget* parent, QVBoxLayout* layout, QString prompt, lineEditCallback callback, int before) {
    QLineEdit* lineEdit = createLineEdit(parent, layout, prompt, before);
    if (lineEdit) _callbacksEdit.emplace(lineEdit, callback);
    return lineEdit;
}

QLineEdit* Power::createLineEdit(QWidget* parent, QVBoxLayout* layout, QString prompt, int before) {
    QLineEdit* lineEdit = new QLineEdit(layout->parentWidget());
    lineEdit->setPlaceholderText(prompt);
    lineEdit->setToolTip(prompt);

    QString style = "color: #000; background: cyan;";
    lineEdit->setStyleSheet(style);

    if (before == -1) layout->addWidget(lineEdit);
    else layout->insertWidget(before, lineEdit);
    parent->connect(lineEdit, SIGNAL(textChanged(QString)), parent, SLOT(textChanged(QString)));
    return lineEdit;
}

QPushButton* Power::createPushButton(QWidget* parent, QVBoxLayout* layout, QString prompt, pushButtonCallback callback, int before) {
    QPushButton* btn = createPushButton(parent, layout, prompt, before);
    if (btn) _callbacksBtn.emplace(btn, callback);
    return btn;
}

QPushButton* Power::createPushButton(QWidget* parent, QVBoxLayout* layout, QString prompt, int before) {
    QPushButton* btn = new QPushButton(layout->parentWidget());
    btn->setText(prompt);
    btn->setToolTip(prompt);
#ifdef unix
    btn->setStyleSheet("color: #000; background: #fff");
#endif
    if (before == -1) layout->addWidget(btn);
    else layout->insertWidget(before, btn);
    parent->connect(btn, SIGNAL(clicked(bool)), parent, SLOT(buttonPressed(bool)));
    return btn;
}

static QTreeWidgetItem* createTWItem(QString str) {
    QTreeWidgetItem* item = new QTreeWidgetItem({ str });
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    item->setCheckState(0, Qt::CheckState::Unchecked);
    return item;
}

QTreeWidget* Power::createTreeWidget(QWidget* parent, QVBoxLayout* layout, QMap<QString, QStringList> options, treeCallback callback, int before) {
    QTreeWidget* tree = createTreeWidget(parent, layout, options, before);
    if (tree) _callbacksTree.emplace(tree, callback);
    return tree;
}

QTreeWidget* Power::createTreeWidget(QWidget* parent, QVBoxLayout* layout, QMap<QString, QStringList> options, int before) {
    QTreeWidget* tree = new QTreeWidget(layout->parentWidget());
    QStringList keys = options.keys();
    double height = 0.0;
    QFont font;
    for (const auto& key: keys) {
        auto* twitem = createTWItem(key);
        font = twitem->font(0);
        height += font.pointSizeF() + 4.0;
        for (const auto& child: options[key]) {
            auto* twchild = createTWItem(child);
            twitem->addChild(twchild);
            font = twchild->font(0);
            height += font.pointSizeF();
        }
        tree->addTopLevelItem(twitem);
        twitem->setExpanded(true);
    }
    QScreen* screen = parent->screen();
    int hgt = height * screen->physicalDotsPerInchY() / 72.0;
    tree->setMinimumHeight(hgt);
    tree->setStyleSheet("border-style: none;"
#ifdef unix
                        "background: #fff; color: #000;"
#endif
                        );
    if (before == -1) layout->addWidget(tree);
    else layout->insertWidget(before, tree);
    auto* hdr = tree->header();
    hdr->setVisible(false);
    parent->connect(tree, SIGNAL(itemSelectionChanged()),            parent, SLOT(itemSelectionChanged()));
    parent->connect(tree, SIGNAL(itemChanged(QTreeWidgetItem*,int)), parent, SLOT(itemChanged(QTreeWidgetItem*,int)));
    return tree;
}

std::map<QWidget*, QLineEdit*> Power::_labeledEdits;

QWidget* Power::createLabeledEdit(QWidget* parent, QVBoxLayout* layout, QString label, lineEditCallback callback, int before) {
    QWidget* wdgt = createLabeledEdit(parent, layout, label, before);
    if (wdgt) _callbacksEdit.emplace(labeledEdit(wdgt), callback);
    return wdgt;
}

QWidget* Power::createLabeledEdit(QWidget* parent, QVBoxLayout* layout, QString label, int before) {
    QFrame* labledEdit = new QFrame(layout->parentWidget());
    labledEdit->setFrameShape(QFrame::NoFrame);
    labledEdit->setFrameShadow(QFrame::Plain);
    labledEdit->setLineWidth(0);
#ifdef unix
    labledEdit->setStyleSheet("color: #000; background: #fff");
#endif

    QHBoxLayout* horizontalLayout = new QHBoxLayout(labledEdit);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);
    labledEdit->setLayout(horizontalLayout);

    QLabel* lbl = new QLabel(labledEdit);
    lbl->setText(label);
#ifdef unix
    lbl->setStyleSheet("color: #000; background: #fff");
#endif
    horizontalLayout->addWidget(lbl);

    QLineEdit* lineEdit = new QLineEdit(labledEdit);
    horizontalLayout->addWidget(lineEdit);

    QWidget* sp = static_cast<QWidget*>(labledEdit);
    _labeledEdits[sp] = lineEdit;

    if (before == -1) layout->addWidget(labledEdit);
    else layout->insertWidget(before, labledEdit);

    parent->connect(lineEdit, SIGNAL(textChanged(QString)), parent, SLOT(textChanged(QString)));

    return sp;
}

void Power::ClearForm(QVBoxLayout* layout) {
    QLayoutItem* item {};
    while ((item = layout->takeAt(0)) != NULL) {
        delete item->widget();
        delete item;
    }
}

void Power::createForm(QWidget* parent, QVBoxLayout* layout) {
    ClearForm(layout);
    form(parent, layout);
}

QList<QString> Power::Available() {
    return AdjustmentPowers() +
           AttackPowers() +
           AutomatonPowers() +
           BodyAffectingPowers() +
           DefensePowers() +
           FrameworkPowers() +
           MentalPowers() +
           MovementPowers() +
           SenseAffectingPowers() +
           SensoryPowers() +
           SpecialPowers() +
           StandardPowers() +
           Equipment();
}

QList<QString> Power::AdjustmentPowers() {
    return _adjustmentPower.keys();
}

QList<QString> Power::AttackPowers() {
    return _attackPower.keys();
}

QList<QString> Power::AutomatonPowers() {
    return _automatonPower.keys();
}

QList<QString> Power::BodyAffectingPowers() {
    return _bodyAffectingPower.keys();
}

QList<QString> Power::DefensePowers() {
    return _defensePower.keys();
}

QList<QString> Power::FrameworkPowers() {
    return _frameworkPower.keys();
}

QList<QString> Power::MentalPowers() {
    return _mentalPower.keys();
}

QList<QString> Power::MovementPowers() {
    return _movementPower.keys();
}

QList<QString> Power::SenseAffectingPowers() {
    return _senseAffectingPower.keys();
}

QList<QString> Power::SensoryPowers() {
    return _sensoryPower.keys();
}

QList<QString> Power::SpecialPowers() {
    return _specialPower.keys();
}

QList<QString> Power::StandardPowers() {
    return _standardPower.keys();
}

QList<QString> Power::Equipment() {
    static bool loaded = false;

    if (!loaded) loaded = LoadEquipment();
    return _equipment.keys();
}

bool Power::LoadEquipment() {
    QString data;
    QString fileName(":/data/equipment.json");

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) return false;
    data = file.readAll();
    file.close();

    QJsonDocument json = QJsonDocument::fromJson(data.toUtf8());
    if (json.isEmpty()) return false;
    QJsonObject top = json.object();
    QJsonArray arr = top["equipment"].toArray();
    auto count = arr.count();
    for (auto i = 0; i < count; ++i) {
        QJsonArray equip = arr[i].toArray();
        QString type = equip[0].toString();
        QJsonObject obj = equip[1].toObject();
        QString name = obj["powerName"].toString();

        if (type == "weapon") Weapon::catalog()[name] = obj;
        else if (type == "armor") Armor::catalog()[name] = obj;
        else Equip::catalog()[name] = obj;
    }
    return true;
}

shared_ptr<Power> Power::ByName(QString name) {
    if (_adjustmentPower.contains(name))     return statics.powers[_adjustmentPower[name]]->create();
    if (_attackPower.contains(name))         return statics.powers[_attackPower[name]]->create();
    if (_automatonPower.contains(name))      return statics.powers[_automatonPower[name]]->create();
    if (_bodyAffectingPower.contains(name))  return statics.powers[_bodyAffectingPower[name]]->create();
    if (_defensePower.contains(name))        return statics.powers[_defensePower[name]]->create();
    if (_mentalPower.contains(name))         return statics.powers[_mentalPower[name]]->create();
    if (_movementPower.contains(name))       return statics.powers[_movementPower[name]]->create();
    if (_senseAffectingPower.contains(name)) return statics.powers[_senseAffectingPower[name]]->create();
    if (_sensoryPower.contains(name))        return statics.powers[_sensoryPower[name]]->create();
    if (_specialPower.contains(name))        return statics.powers[_specialPower[name]]->create();
    if (_standardPower.contains(name))       return statics.powers[_standardPower[name]]->create();
    if (_frameworkPower.contains(name))      return statics.powers[_frameworkPower[name]]->create();
    if (_equipment.contains(name))           return statics.powers[_equipment[name]]->create();
    return nullptr;
}

shared_ptr<Power> Power::FromJson(QString name, const QJsonObject& json) {
    shared_ptr<Power> power;
    if (_adjustmentPower.contains(name))     power = statics.powers[_adjustmentPower[name]]->create(json);
    if (_attackPower.contains(name))         power = statics.powers[_attackPower[name]]->create(json);
    if (_automatonPower.contains(name))      power = statics.powers[_automatonPower[name]]->create(json);
    if (_bodyAffectingPower.contains(name))  power = statics.powers[_bodyAffectingPower[name]]->create(json);
    if (_defensePower.contains(name))        power = statics.powers[_defensePower[name]]->create(json);
    if (_mentalPower.contains(name))         power = statics.powers[_mentalPower[name]]->create(json);
    if (_movementPower.contains(name))       power = statics.powers[_movementPower[name]]->create(json);
    if (_senseAffectingPower.contains(name)) power = statics.powers[_senseAffectingPower[name]]->create(json);
    if (_sensoryPower.contains(name))        power = statics.powers[_sensoryPower[name]]->create(json);
    if (_specialPower.contains(name))        power = statics.powers[_specialPower[name]]->create(json);
    if (_standardPower.contains(name))       power = statics.powers[_standardPower[name]]->create(json);
    if (_frameworkPower.contains(name))      power = statics.powers[_frameworkPower[name]]->create(json);
    if (_equipment.contains(name))           power = statics.powers[_equipment[name]]->create(json);
    if (power == nullptr) return nullptr;

    QJsonObject obj = json["modifiers"].toObject();
    QStringList keys = obj.keys();
    for (const auto& key: keys) {
        const auto& base = Modifiers::ByName(key);
        shared_ptr<Modifier> mod = base->create(obj[key].toObject());
        power->_modifiers.append(mod);
        if (mod->type() == ModifierBase::isAdvantage ||
            (mod->type() == ModifierBase::isBoth && mod->fraction(Modifier::NoStore) >= 0)) power->advantagesList().append(mod);
        else power->limitationsList().append(mod);
    }
    return power;
}

Fraction Power::endLessActing() {
    Fraction advantages(1);
    advantages += adv();
    for (const auto& mod: advantagesList()) {
        if (mod->name() == "Reduced Endurance") continue;
        advantages += mod->fraction(NoStore);
    }
    Power* parent = _parent;
    if (parent != nullptr) {
        for (const auto& mod: parent->advantagesList())
            if (!mod->isAdder()) {
                if (mod->name() == "Reduced Endurance") continue;
                advantages += mod->fraction(Power::NoStore);
            }
    }
    return advantages;
}

QString Power::end() {
    Points active(points(NoStore));
    for (const auto& mod: advantagesList()) {
        if (mod->name() == "Reduced Endurance") continue;
        else if (mod->isAdder()) active += mod->points(Modifier::NoStore);
    }
    Fraction reduced(1);
#ifndef ISHSC
    auto opt = Sheet::ref().getOption();
    int per = opt.activePerEND().points;
#else
    int per = 10;
#endif
    auto adv = this->endLessActing();
    active = Points((active.points * adv).toInt());
    if (findModifier("Charges") != _modifiers.end()) per = 0;
    auto mod = findModifier("Costs Endurance");
    if (mod != _modifiers.end()) {
#ifndef ISHSC
        per = opt.activePerEND().points;
#endif
        if ((*mod)->endChange() == Fraction(1, 2)) active = active / 2_cp;
    }
    mod = findModifier("Reduced Endurance");
    if (mod != _modifiers.end() && (*mod)->endChange() != Fraction(1, 2)) per = 0;
    mod = findModifier("Increased END Cost");
    if (mod != _modifiers.end()) {
#ifndef ISHSC
        if (per == 0) per = opt.activePerEND().points;
#endif
        active = Points((active.points * (*mod)->endChange()).toInt());
    }
#ifndef ISHSC
    if (findModifier("Costs END Only To Activate") != _modifiers.end() && per == 0) per = opt.activePerEND().points;
#endif
    mod = findModifier("Costs Endurance To Maintain");
    if (mod != _modifiers.end()) {
#ifndef ISHSC
        if (per == 0) per = opt.activePerEND().points;
#endif
        active = Points((active.points * (*mod)->endChange()).toInt());
    }

    int end = 0;
    if (per != 0) {
        int acting = active.points;
        if (per % 2 == 0) end = (acting + per / 2 - 1) / per;
        else end = (acting + per / 2) / per;
        mod = findModifier("Reduced Endurance");
        if (mod != _modifiers.end() && (*mod)->endChange() == Fraction(1, 2)) end = end / 2;
        if (end < 1) end = 1;
    }
    return (end == 0) ? "-" : QString("%1").arg(end);
}

QString Power::noEnd() {
#ifndef ISHSC
    auto opt = Sheet::ref().getOption();
#endif
    Points active(points(NoStore));
    for (const auto& mod: advantagesList()) {
        if (mod->name() == "Reduced Endurance") continue;
        else if (mod->isAdder()) active += mod->points(Modifier::NoStore);
    }
    auto adv = this->endLessActing();
    active = Points((active.points * adv).toInt());
    int end = 0;
    int per = 0;
    if (findModifier("Charges") != _modifiers.end()) per = 0;
    auto mod = findModifier("Costs Endurance");
    if (mod != _modifiers.end()) {
#ifndef ISHSC
        per = opt.activePerEND().points;
#endif
        if ((*mod)->endChange() == Fraction(1, 2)) active = active / 2_cp;
    }
    mod = findModifier("Increased END Cost");
    if (mod != _modifiers.end()) {
#ifndef ISHSC
        if (per == 0) per = opt.activePerEND().points;
#endif
        active = Points((active.points * (*mod)->endChange()).toInt());
    }
#ifndef ISHSC
    if (findModifier("Costs END Only To Activate") != _modifiers.end() && per == 0) per = opt.activePerEND().points;
#endif
    mod = findModifier("Costs Endurance To Maintain");
    if (mod != _modifiers.end()) {
#ifndef ISHSC
        if (per == 0) per = opt.activePerEND().points;
#endif
        active = Points((active.points * (*mod)->endChange()).toInt());
    }
    mod = findModifier("Reduced Endurance");
    if (mod != _modifiers.end() && per != 0) {
        if ((*mod)->endChange() == Fraction(1, 2)) active = active / 2_cp;
    }

    if (per != 0) end = (active.points + per / 2) / per;
    return (end == 0) ? "-" : QString("%1").arg(end);
}

QList<shared_ptr<Modifier>>::iterator Power::findModifier(QString name) {
    for (auto mod = _modifiers.begin(); mod != _modifiers.end(); ++mod)
        if ((*mod)->name() == name) return mod;
    if (_parent != nullptr) {
        for (auto mod = _parent->_modifiers.begin(); mod != _parent->_modifiers.end(); ++mod)
            if ((*mod)->name() == name) return mod;
    }
    return _modifiers.end();
}

bool Power::hasModifier(QString name) {
    return findModifier(name) != _modifiers.end();
}

Points Power::real(Fraction add, Points mod, Fraction sub) {
    Fraction pnts(acting(add, mod).points);
    Fraction limits(0);
    if (sub == 0) limits = Fraction(1);
    limits += lim() + sub;
    for (const auto& mod: limitationsList()) limits += mod->fraction(NoStore).abs();
    pnts = pnts / limits;
    return Points(pnts.toInt(true));
}

Points Power::acting(Fraction add, Points mod) {
    int base = points(NoStore).points;
    Fraction pnts(base);
    Fraction advantages(0);
    if (add == 0) advantages = Fraction(1);
    advantages += adv() + add;
    pnts += Fraction(mod.points);
    for (const auto& mod: advantagesList())
        if (mod->isAdder()) pnts += mod->points(NoStore).points;
        else advantages += mod->fraction(NoStore);
    pnts = pnts * advantages;
    return Points(pnts.toInt(true));
}

Points Power::active() {
    return 0_cp;
}
