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
} const statics {
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

const QMap<QString, QString> Power::mAdjustmentPower {
    SPCS("Absorption▲", Absorption),
    SPCS("Aid",         Aid),
    SPCS("Drain",       Drain),
    SPCS("Healing",     Healing)
};

const QMap<QString, QString> Power::mAttackPower {
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

const QMap<QString, QString> Power::mAutomatonPower {
    SPCS("Cannot Be Stunnedϴ", CannotBeStunned),
    SPCS("Does Not Bleedϴ",    DoesNotBleed),
    SPCS("No Hit Locationsϴ",  NoHitLocations),
    SPCS("Takes No STUNϴ",     TakesNoSTUN)
};

const QMap<QString, QString> Power::mBodyAffectingPower {
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

const QMap<QString, QString> Power::mDefensePower {
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

const QMap<QString, QString> Power::mFrameworkPower {
    SPCS("Group",               Group),
    SPCS("Multipower",          Multipower),
    SPCS("Variable Power Pool", VPP),
    SPCS("Blank Line",          Blank)
};

const QMap<QString, QString> Power::mMentalPower {
    SPCS("Mental Blast",     MentalBlast),
    SPCS("Mental Illusions", MentalIllusions),
    SPCS("Mind Control",     MindControl),
    SPCS("Mind Link",        MindLink),
    SPCS("Mind Scan",        MindScan),
    SPCS("Telepathy",        Telepathy)
};

const QMap<QString, QString> Power::mMovementPower {
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

const QMap<QString, QString> Power::mSenseAffectingPower {
    SPCS("Darkness",     Darkness),
    SPCS("Flash",        Flash),
    SPCS("Images",       Images),
    SPCS("Invisibility", Invisibility)
};

const QMap<QString, QString> Power::mSensoryPower {
    SPCS("Clairsentienceϴ", Clairsentience),
    SPCS("Enhanced Senses", EnhancedSenses),
    SPCS("Mind Scan",       MindScan)
};

const QMap<QString, QString> Power::mSpecialPower {
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

const QMap<QString, QString> Power::mStandardPower {
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

const QMap<QString, QString> Power::mEquipment {
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
    for (const auto& mod: std::as_const(mModifiers)) {
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
    mSender = checkBox;
    auto f = mCallbacksCB.find(checkBox);
    if (f == mCallbacksCB.end()) return;
    auto function = f->second;
    function(this, checkBox->isChecked());
}

void Power::callback(QLineEdit* edit) {
    mSender = edit;
    auto f = mCallbacksEdit.find(edit);
    if (f == mCallbacksEdit.end()) return;
    auto function = f->second;
    function(this, 0);
}

void Power::callback(QComboBox* combo) {
    mSender = combo;
    auto f = mCallbacksCBox.find(combo);
    if (f == mCallbacksCBox.end()) return;
    auto function = f->second;
    function(this, combo->currentIndex());
}

void Power::callback(QComboBox* combo, bool) {
    mSender = combo;
    auto f = mCallbacksActivatedCBox.find(combo);
    if (f == mCallbacksActivatedCBox.end()) return;
    auto function = f->second;
    function(this, combo->currentIndex(), 0);
}

void Power::callback(QPushButton* btn) {
    mSender = btn;
    auto f = mCallbacksBtn.find(btn);
    if (f == mCallbacksBtn.end()) return;
    auto function = f->second;
    function(this);
}

void Power::callback(QTreeWidget* tree) {
    mSender = tree;
    auto f = mCallbacksTree.find(tree);
    if (f == mCallbacksTree.end()) return;
    auto function = f->second;
    function(this, 0, 0, false);
}

QCheckBox* Power::createCheckBox(QWidget* parent, QVBoxLayout* layout, QString prompt, int before) {
    QCheckBox* checkBox = new QCheckBox(layout->parentWidget());
    checkBox->setText(prompt);
    checkBox->setChecked(false);
    QString style = "QCheckBox {"
                    "  color: black; "
                    "  background: white; "
                    "} "
                    "QCheckBox::indicator {"
                    "  width: 13px;"
                    "  height: 13px;"
                    "  border: 1px solid gray; "
                    "  color: black;"
                    "  background: cyan;"
                    "} "
                    "QCheckBox::indicator:checked {"
                    "  border: 1px solid gray; "
                    "  image: url(:/icons/Check.png); "
                    "  background: cyan;"
                    "}";
    checkBox->setStyleSheet(style);
    if (before == -1) layout->addWidget(checkBox);
    else layout->insertWidget(before, checkBox);
    parent->connect(checkBox, SIGNAL(clicked(bool)), parent, SLOT(stateChanged(bool)));
    return checkBox;
}

QCheckBox* Power::createCheckBox(QWidget* parent, QVBoxLayout* layout, QString prompt, checkBoxCallback callback, int before) {
    QCheckBox* checkBox = createCheckBox(parent, layout, prompt, before);
    if (checkBox) mCallbacksCB.emplace(checkBox, callback);
    return checkBox;
}

QComboBox* Power::createComboBox(QWidget* parent, QVBoxLayout* layout, QString prompt, QList<QString> options, comboBoxCallback callback,
                                 int before) {
    QComboBox* comboBox = createComboBox(parent, layout, prompt, options, before);
    if (comboBox) mCallbacksCBox.emplace(comboBox, callback);
    return comboBox;
}

QComboBox* Power::createComboBox(QWidget* parent, QVBoxLayout* layout, QString prompt, QList<QString> options, activatedCallback callback,
                                 int before) {
    QComboBox* comboBox = createComboBox(parent, layout, prompt, options, before);
    if (comboBox) mCallbacksActivatedCBox.emplace(comboBox, callback);
    return comboBox;
}

QComboBox* Power::createComboBox(QWidget* parent, QVBoxLayout* layout, QString prompt, QList<QString> options, int before) {
    QComboBox* comboBox = new QComboBox(layout->parentWidget());
    comboBox->addItems(options);
    comboBox->setPlaceholderText(prompt);
    comboBox->setToolTip(prompt);
    comboBox->setCurrentIndex(-1);
    QString style = "QComboBox {"
                    "  border: 1px solid gray;"
                    "  color: #000;"
                    "  background: cyan; "
                    "}"
                    "QComboBox QAbstractItemView {"
                    "  border: 1px solid gray"
                    "  color: #000;"
                    "  background-color: lightgray; "
                    "}"
                    "QComboBox::indicator {"
                    "  color: black;"
                    "  background: white;"
                    "  border: 1px solid gray;"
                    "}"
                    "QComboBox::down-arrow {"
                    "  color: black;"
                    "  background: white;"
                    "}";
    comboBox->setStyleSheet(style);
    if (before == -1) layout->addWidget(comboBox);
    else layout->insertWidget(before, comboBox);
    parent->connect(comboBox, SIGNAL(currentIndexChanged(int)), parent, SLOT(currentIndexChanged(int)));
    parent->connect(comboBox, SIGNAL(activated(int)),           parent, SLOT(activated(int)));
    return comboBox;
}

QLabel* Power::createLabel(QWidget*, QVBoxLayout* layout, QString text, int before) {
    QLabel* label = new QLabel(layout->parentWidget());
    label->setText(text);
    QString style = "color: #000; background: #fff;";
    label->setStyleSheet(style);
    if (before == -1) layout->addWidget(label);
    else layout->insertWidget(before, label);
    return label;
}

QLineEdit* Power::createLineEdit(QWidget* parent, QVBoxLayout* layout, QString prompt, lineEditCallback callback, int before) {
    QLineEdit* lineEdit = createLineEdit(parent, layout, prompt, before);
    if (lineEdit) mCallbacksEdit.emplace(lineEdit, callback);
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
    if (btn) mCallbacksBtn.emplace(btn, callback);
    return btn;
}

QPushButton* Power::createPushButton(QWidget* parent, QVBoxLayout* layout, QString prompt, int before) {
    QPushButton* btn = new QPushButton(layout->parentWidget());
    btn->setText(prompt);
    btn->setToolTip(prompt);
    btn->setStyleSheet("color: #000; background: #fff");
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
    if (tree) mCallbacksTree.emplace(tree, callback);
    return tree;
}

QTreeWidget* Power::createTreeWidget(QWidget* parent, QVBoxLayout* layout, QMap<QString, QStringList> options, int before) {
    QTreeWidget* tree = new QTreeWidget(layout->parentWidget());
    QStringList keys = options.keys();
    double height = 0.0;
    QFont font;
    for (const auto& key: std::as_const(keys)) {
        auto* twitem = createTWItem(key);
        font = twitem->font(0);
        height += font.pointSizeF() + 4.0;
        for (const auto& child: std::as_const(options[key])) {
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
                        "background: #fff; color: #000;"
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
    if (wdgt) mCallbacksEdit.emplace(labeledEdit(wdgt), callback);
    return wdgt;
}

QWidget* Power::createLabeledEdit(QWidget* parent, QVBoxLayout* layout, QString label, int before) {
    QFrame* labledEdit = new QFrame(layout->parentWidget());
    labledEdit->setFrameShape(QFrame::NoFrame);
    labledEdit->setFrameShadow(QFrame::Plain);
    labledEdit->setLineWidth(0);
    labledEdit->setStyleSheet("color: #000; background: #fff");

    QHBoxLayout* horizontalLayout = new QHBoxLayout(labledEdit);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);
    labledEdit->setLayout(horizontalLayout);

    QLabel* lbl = new QLabel(labledEdit);
    lbl->setText(label);
    lbl->setStyleSheet("color: #000; background: #fff");
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
    return mAdjustmentPower.keys();
}

QList<QString> Power::AttackPowers() {
    return mAttackPower.keys();
}

QList<QString> Power::AutomatonPowers() {
    return mAutomatonPower.keys();
}

QList<QString> Power::BodyAffectingPowers() {
    return mBodyAffectingPower.keys();
}

QList<QString> Power::DefensePowers() {
    return mDefensePower.keys();
}

QList<QString> Power::FrameworkPowers() {
    return mFrameworkPower.keys();
}

QList<QString> Power::MentalPowers() {
    return mMentalPower.keys();
}

QList<QString> Power::MovementPowers() {
    return mMovementPower.keys();
}

QList<QString> Power::SenseAffectingPowers() {
    return mSenseAffectingPower.keys();
}

QList<QString> Power::SensoryPowers() {
    return mSensoryPower.keys();
}

QList<QString> Power::SpecialPowers() {
    return mSpecialPower.keys();
}

QList<QString> Power::StandardPowers() {
    return mStandardPower.keys();
}

QList<QString> Power::Equipment() {
    static bool loaded = false;

    if (!loaded) loaded = LoadEquipment();
    return mEquipment.keys();
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
    if (mAdjustmentPower.contains(name))     return statics.powers[mAdjustmentPower[name]]->create();
    if (mAttackPower.contains(name))         return statics.powers[mAttackPower[name]]->create();
    if (mAutomatonPower.contains(name))      return statics.powers[mAutomatonPower[name]]->create();
    if (mBodyAffectingPower.contains(name))  return statics.powers[mBodyAffectingPower[name]]->create();
    if (mDefensePower.contains(name))        return statics.powers[mDefensePower[name]]->create();
    if (mMentalPower.contains(name))         return statics.powers[mMentalPower[name]]->create();
    if (mMovementPower.contains(name))       return statics.powers[mMovementPower[name]]->create();
    if (mSenseAffectingPower.contains(name)) return statics.powers[mSenseAffectingPower[name]]->create();
    if (mSensoryPower.contains(name))        return statics.powers[mSensoryPower[name]]->create();
    if (mSpecialPower.contains(name))        return statics.powers[mSpecialPower[name]]->create();
    if (mStandardPower.contains(name))       return statics.powers[mStandardPower[name]]->create();
    if (mFrameworkPower.contains(name))      return statics.powers[mFrameworkPower[name]]->create();
    if (mEquipment.contains(name))           return statics.powers[mEquipment[name]]->create();
    return nullptr;
}

shared_ptr<Power> Power::FromJson(QString name, const QJsonObject& json) {
    shared_ptr<Power> power;
    if (mAdjustmentPower.contains(name))     power = statics.powers[mAdjustmentPower[name]]->create(json);
    if (mAttackPower.contains(name))         power = statics.powers[mAttackPower[name]]->create(json);
    if (mAutomatonPower.contains(name))      power = statics.powers[mAutomatonPower[name]]->create(json);
    if (mBodyAffectingPower.contains(name))  power = statics.powers[mBodyAffectingPower[name]]->create(json);
    if (mDefensePower.contains(name))        power = statics.powers[mDefensePower[name]]->create(json);
    if (mMentalPower.contains(name))         power = statics.powers[mMentalPower[name]]->create(json);
    if (mMovementPower.contains(name))       power = statics.powers[mMovementPower[name]]->create(json);
    if (mSenseAffectingPower.contains(name)) power = statics.powers[mSenseAffectingPower[name]]->create(json);
    if (mSensoryPower.contains(name))        power = statics.powers[mSensoryPower[name]]->create(json);
    if (mSpecialPower.contains(name))        power = statics.powers[mSpecialPower[name]]->create(json);
    if (mStandardPower.contains(name))       power = statics.powers[mStandardPower[name]]->create(json);
    if (mFrameworkPower.contains(name))      power = statics.powers[mFrameworkPower[name]]->create(json);
    if (mEquipment.contains(name))           power = statics.powers[mEquipment[name]]->create(json);
    if (power == nullptr) return nullptr;

    QJsonObject obj = json["modifiers"].toObject();
    QStringList keys = obj.keys();
    for (const auto& key: std::as_const(keys)) {
        const auto& base = Modifiers::ByName(key);
        shared_ptr<Modifier> mod = base->create(obj[key].toObject());
        power->mModifiers.append(mod);
        if (mod->type() == ModifierBase::isAdvantage ||
            (mod->type() == ModifierBase::isBoth && mod->fraction(Modifier::NoStore) >= 0)) power->advantagesList().append(mod);
        else power->limitationsList().append(mod);
    }
    return power;
}

Fraction Power::endLessActing() {
    Fraction advantages(1);
    advantages += adv();
    for (const auto& mod: std::as_const(advantagesList())) {
        if (mod->name() == "Reduced Endurance") continue;
        advantages += mod->fraction(NoStore);
    }
    Power* parent = mParent;
    if (parent != nullptr) {
        for (const auto& mod: std::as_const(parent->advantagesList()))
            if (!mod->isAdder()) {
                if (mod->name() == "Reduced Endurance") continue;
                advantages += mod->fraction(Power::NoStore);
            }
    }
    return advantages;
}

QString Power::end() {
    Points active(points(NoStore));
    for (const auto& mod: std::as_const(advantagesList())) {
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
    if (findModifier("Charges") != mModifiers.end()) per = 0;
    auto mod = findModifier("Costs Endurance");
    if (mod != mModifiers.end()) {
#ifndef ISHSC
        per = opt.activePerEND().points;
#endif
        if ((*mod)->endChange() == Fraction(1, 2)) active = active / 2_cp;
    }
    mod = findModifier("Reduced Endurance");
    if (mod != mModifiers.end() && (*mod)->endChange() != Fraction(1, 2)) per = 0;
    mod = findModifier("Increased END Cost");
    if (mod != mModifiers.end()) {
#ifndef ISHSC
        if (per == 0) per = opt.activePerEND().points;
#endif
        active = Points((active.points * (*mod)->endChange()).toInt());
    }
#ifndef ISHSC
    if (findModifier("Costs END Only To Activate") != mModifiers.end() && per == 0) per = opt.activePerEND().points;
#endif
    mod = findModifier("Costs Endurance To Maintain");
    if (mod != mModifiers.end()) {
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
        if (mod != mModifiers.end() && (*mod)->endChange() == Fraction(1, 2)) end = end / 2;
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
    if (findModifier("Charges") != mModifiers.end()) per = 0;
    auto mod = findModifier("Costs Endurance");
    if (mod != mModifiers.end()) {
#ifndef ISHSC
        per = opt.activePerEND().points;
#endif
        if ((*mod)->endChange() == Fraction(1, 2)) active = active / 2_cp;
    }
    mod = findModifier("Increased END Cost");
    if (mod != mModifiers.end()) {
#ifndef ISHSC
        if (per == 0) per = opt.activePerEND().points;
#endif
        active = Points((active.points * (*mod)->endChange()).toInt());
    }
#ifndef ISHSC
    if (findModifier("Costs END Only To Activate") != mModifiers.end() && per == 0) per = opt.activePerEND().points;
#endif
    mod = findModifier("Costs Endurance To Maintain");
    if (mod != mModifiers.end()) {
#ifndef ISHSC
        if (per == 0) per = opt.activePerEND().points;
#endif
        active = Points((active.points * (*mod)->endChange()).toInt());
    }
    mod = findModifier("Reduced Endurance");
    if (mod != mModifiers.end() && per != 0) {
        if ((*mod)->endChange() == Fraction(1, 2)) active = active / 2_cp;
    }

    if (per != 0) end = (active.points + per / 2) / per;
    return (end == 0) ? "-" : QString("%1").arg(end);
}

QList<shared_ptr<Modifier>>::iterator Power::findModifier(QString name) {
    for (auto mod = mModifiers.begin(); mod != mModifiers.end(); ++mod)
        if ((*mod)->name() == name) return mod;
    if (mParent != nullptr) {
        for (auto mod = mParent->mModifiers.begin(); mod != mParent->mModifiers.end(); ++mod)
            if ((*mod)->name() == name) return mod;
    }
    return mModifiers.end();
}

bool Power::hasModifier(QString name) {
    return findModifier(name) != mModifiers.end();
}

Points Power::real(Fraction add, Points mod, Fraction sub) {
    Fraction pnts(acting(add, mod).points);
    Fraction limits(0);
    if (sub == 0) limits = Fraction(1);
    limits += lim() + sub;
    for (const auto& mod: std::as_const(limitationsList())) limits += mod->fraction(NoStore).abs();
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
    for (const auto& mod: std::as_const(advantagesList()))
        if (mod->isAdder()) pnts += mod->points(NoStore).points;
        else advantages += mod->fraction(NoStore);
    pnts = pnts * advantages;
    return Points(pnts.toInt(true));
}

Points Power::active() {
    return 0_cp;
}
