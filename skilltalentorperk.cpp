#include "skilltalentorperk.h"

#include "AgilitySkills.h"
#include "BackgroundSkills.h"
#include "CombatSkills.h"
#include "CustomSkill.h"
#include "SkillEnhancers.h"
#include "IntellectSkills.h"
#include "InteractionSkills.h"
#include "MiscSkill.h"
#include "Perk.h"
#include "Talent.h"

#define MAKE(x)          SkillTalentOrPerk::skill<x> _##x
#define MAKE_PERK(x)     SkillTalentOrPerk::perk<x> _##x
#define MAKE_TALENT(x)   SkillTalentOrPerk::talent<x> _##x
#define MAKE_ENHANCER(x) SkillTalentOrPerk::enhancer<x> _##x
#define LINK(x)          #x, &statics::_##x
#define SPCS(x,y)        x, &statics::_##y

namespace statics {
    // SKILLS
    MAKE(Acrobatics);
    MAKE(Acting);
    MAKE(Analyze);
    MAKE(AutofireSkills);
    MAKE(Breakfall);
    MAKE(Bribery);
    MAKE(Bugging);
    MAKE(Bureaucratics);
    MAKE(Charm);
    MAKE(Climbing);
    MAKE(CombatDriving);
    MAKE(CombatPiloting);
    MAKE(CSL);
    MAKE(ComputerProgramming);
    MAKE(Concealment);
    MAKE(Contortionist);
    MAKE(Conversation);
    MAKE(Cramming);
    MAKE(Criminology);
    MAKE(Cryptography);
    MAKE(CustomSkill);
    MAKE(Deduction);
    MAKE(DefenseManeuver);
    MAKE(Demolitions);
    MAKE(Disguise);
    MAKE(Electronics);
    MAKE(FastDraw);
    MAKE(ForensicMedicine);
    MAKE(Forgery);
    MAKE(Gambling);
    MAKE(HighSociety);
    MAKE(Interrogation);
    MAKE(Inventor);
    MAKE(KS);
    MAKE(Language);
    MAKE(Lipreading);
    MAKE(Lockpicking);
    MAKE(Mechanics);
    MAKE(Mimicry);
    MAKE(MartialArts);
    MAKE(MCSL);
    MAKE(MSL);
    MAKE(Navigation);
    MAKE(Oratory);
    MAKE(Paramedics);
    MAKE(PenaltySkillLevels);
    MAKE(Persuasion);
    MAKE(PowerSkill);
    MAKE(PS);
    MAKE(RapidAttack);
    MAKE(Riding);
    MAKE(SS);
    MAKE(Shadowing);
    MAKE(SkillLevels);
    MAKE(SleightOfHand);
    MAKE(Stealth);
    MAKE(Streetwise);
    MAKE(SecuritySystems);
    MAKE(Survival);
    MAKE(SystemsOperation);
    MAKE(Tactics);
    MAKE(Teamwork);
    MAKE(Tracking);
    MAKE(Trading);
    MAKE(TF);
    MAKE(TwoWeaponFighting);
    MAKE(Ventriloquism);
    MAKE(WeaponFamiliarity);
    MAKE(Weaponsmith);

    // PERKS
    MAKE_PERK(Access);
    MAKE_PERK(Anonymity);
    MAKE_PERK(ComputerLink);
    MAKE_PERK(Contact);
    MAKE_PERK(DeepCover);
    MAKE_PERK(Favor);
    MAKE_PERK(Follower);
    MAKE_PERK(FringeBenefit);
    MAKE_PERK(Money);
    MAKE_PERK(PositiveReputation);
    MAKE_PERK(VehiclesAndBases);

    // TALENT
    MAKE_TALENT(AbsoluteRangeSense);
    MAKE_TALENT(AbsoluteTimeSense);
    MAKE_TALENT(Ambidexterity);
    MAKE_TALENT(AnimalFriendship);
    MAKE_TALENT(BumpOfDirection);
    MAKE_TALENT(CombatLuck);
    MAKE_TALENT(CombatSense);
    MAKE_TALENT(DangerSense);
    MAKE_TALENT(DeadlyBlow);
    MAKE_TALENT(DoubleJointed);
    MAKE_TALENT(EideticMemory);
    MAKE_TALENT(EnvironmentalMovement);
    MAKE_TALENT(LightningCalculator);
    MAKE_TALENT(LightningReflexes);
    MAKE_TALENT(Lightsleep);
    MAKE_TALENT(OffHandDefense);
    MAKE_TALENT(PerfectPitch);
    MAKE_TALENT(Resistance);
    MAKE_TALENT(SimulateDeath);
    MAKE_TALENT(SpeedReading);
    MAKE_TALENT(StrikingAppearence);
    MAKE_TALENT(UniversalTranslator);
    MAKE_TALENT(Weaponmaster);

    // ENHANCERS
    MAKE_ENHANCER(BlankSkill);
    MAKE_ENHANCER(JackOfAllTrades);
    MAKE_ENHANCER(Linguist);
    MAKE_ENHANCER(Scholar);
    MAKE_ENHANCER(Scientist);
    MAKE_ENHANCER(Traveler);
    MAKE_ENHANCER(WellConnected);
}

QMap<QString, SkillTalentOrPerk::skillBase*> SkillTalentOrPerk::_skills {
    { LINK(Acrobatics) },
    { LINK(Acting) },
    { LINK(Analyze) },
    { SPCS("Autofire Skills", AutofireSkills) },
    { LINK(Breakfall) },
    { LINK(Bribery) },
    { LINK(Bugging) },
    { LINK(Bureaucratics) },
    { LINK(Charm) },
    { LINK(Climbing) },
    { SPCS("Combat Driving", CombatDriving) },
    { SPCS("Combat Piloting", CombatPiloting) },
    { SPCS("Combat Skill Levels", CSL) },
    { SPCS("Computer Programming", ComputerProgramming) },
    { LINK(Concealment) },
    { LINK(Contortionist) },
    { LINK(Conversation) },
    { LINK(Cramming) },
    { LINK(Criminology) },
    { LINK(Cryptography) },
    { SPCS("Custom Skill", CustomSkill) },
    { LINK(Deduction) },
    { SPCS("Defense Maneuver", DefenseManeuver) },
    { LINK(Disguise) },
    { LINK(Electronics) },
    { SPCS("Fast Draw", FastDraw) },
    { SPCS("Forensice Medicine", ForensicMedicine) },
    { LINK(Forgery) },
    { LINK(Gambling) },
    { SPCS("High Society", HighSociety) },
    { LINK(Interrogation) },
    { LINK(Inventor) },
    { SPCS("Knowledge Skill", KS) },
    { LINK(Language) },
    { LINK(Lipreading) },
    { LINK(Lockpicking) },
    { SPCS("Martial Arts", MartialArts) },
    { LINK(Mimicry) },
    { LINK(Mechanics) },
    { SPCS("Mental Combat Skill Levels", MCSL) },
    { SPCS("Movement Skill Levels", MSL) },
    { LINK(Navigation) },
    { LINK(Oratory) },
    { LINK(Paramedics) },
    { SPCS("Penalty Skill Levels", PenaltySkillLevels) },
    { LINK(Persuasion) },
    { SPCS("Power Skill", PowerSkill) },
    { SPCS("Professional Skill", PS) },
    { SPCS("Rapid Attack", RapidAttack) },
    { LINK(Riding) },
    { SPCS("Science Skill", SS) },
    { SPCS("Security Systems", SecuritySystems) },
    { LINK(Shadowing) },
    { SPCS("Skill Levels", SkillLevels ) },
    { SPCS("Sleight Of Hand", SleightOfHand) },
    { LINK(Stealth) },
    { LINK(Streetwise) },
    { SPCS("Systems Operation", SystemsOperation) },
    { LINK(Survival) },
    { LINK(Tactics) },
    { LINK(Teamwork) },
    { LINK(Tracking) },
    { LINK(Trading) },
    { SPCS("Transport Familiarity", TF) },
    { SPCS("Two-Weapon Fighting", TwoWeaponFighting) },
    { LINK(Ventriloquism) },
    { SPCS("Weapon Familiarity", WeaponFamiliarity) },
    { LINK(Weaponsmith) }
};

QMap<QString, SkillTalentOrPerk::talentBase*> SkillTalentOrPerk::_talents {
    { SPCS("Absolute Range Sense", AbsoluteRangeSense) },
    { SPCS("Absolute Time Sense", AbsoluteTimeSense) },
    { LINK(Ambidexterity) },
    { SPCS("Animal Friendship", AnimalFriendship) },
    { SPCS("Bump Of Direction", BumpOfDirection) },
    { SPCS("Combat Luck", CombatLuck) },
    { SPCS("Combat Sense", CombatSense) },
    { SPCS("Danger Senseϴ", DangerSense) },
    { SPCS("Deadly Blow▲", DeadlyBlow) },
    { SPCS("Double Jointed", DoubleJointed) },
    { SPCS("Eidetic Memory", EideticMemory) },
    { SPCS("Environmental Movement", EnvironmentalMovement) },
    { SPCS("Lightning Calculator", LightningCalculator) },
    { SPCS("Lightning Reflexes▲", LightningReflexes) },
    { LINK(Lightsleep) },
    { SPCS("Off-Hand Defense", OffHandDefense) },
    { SPCS("Perfect Pitch", PerfectPitch) },
    { LINK(Resistance) },
    { SPCS("Simulate Death", SimulateDeath) },
    { SPCS("Speed Reading", SpeedReading) },
    { SPCS("Strining Appearence", StrikingAppearence) },
    { SPCS("Universal Translatorϴ", UniversalTranslator) },
    { SPCS("Weaponmaster▲", Weaponmaster) }
};

QMap<QString, SkillTalentOrPerk::perkBase*> SkillTalentOrPerk::_perks {
    { LINK(Access) },
    { LINK(Anonymity) },
    { SPCS("Computer Link", ComputerLink) },
    { LINK(Contact) },
    { SPCS("Deep Cover", DeepCover) },
    { LINK(Favor) },
    { LINK(Follower) },
    { SPCS("Fringe Benefit", FringeBenefit) },
    { LINK(Money) },
    { SPCS("Positive Reputation", PositiveReputation) },
    { SPCS("Vehicles And Bases", VehiclesAndBases) }
};

QMap<QString, SkillTalentOrPerk::enhancerBase*> SkillTalentOrPerk::_enhancers {
    { SPCS("Blank Line", BlankSkill) },
    { SPCS("Jack Of All Trades", JackOfAllTrades) },
    { LINK(Linguist) },
    { LINK(Scholar) },
    { LINK(Scientist) },
    { LINK(Traveler) },
    { SPCS("Well-Connected", WellConnected) }
};

SkillTalentOrPerk::SkillTalentOrPerk() { }

bool SkillTalentOrPerk::isNumber(QString txt) {
    bool ok;
    txt.toInt(&ok, 10);
    return ok;
}

void SkillTalentOrPerk::callback(QCheckBox* checkBox) {
    _sender = checkBox;
    auto f = _callbacksCB.find(checkBox);
    if (f == _callbacksCB.end()) return;
    auto function = f.value();
    function(this, checkBox->isChecked());
}

void SkillTalentOrPerk::callback(QLineEdit* edit) {
    _sender = edit;
    auto f = _callbacksEdit.find(edit);
    if (f == _callbacksEdit.end()) return;
    auto function = f.value();
    function(this, edit->text());
}

QCheckBox* SkillTalentOrPerk::createCheckBox(QWidget* parent, QVBoxLayout* layout, QString prompt) {
    QCheckBox* checkBox = new QCheckBox(layout->parentWidget());
    checkBox->setText(prompt);
    checkBox->setChecked(false);
    checkBox->setStyleSheet("QCheckBox { color: #000; background: #fff; }");
    layout->addWidget(checkBox);
    parent->connect(checkBox, SIGNAL(stateChanged(int)), parent, SLOT(stateChanged(int)));
    return checkBox;
}

QCheckBox* SkillTalentOrPerk::createCheckBox(QWidget* parent, QVBoxLayout* layout, QString prompt, std::_Mem_fn<void (SkillTalentOrPerk::*)(bool)> callback) {
    QCheckBox* checkBox = new QCheckBox(layout->parentWidget());
    checkBox->setText(prompt);
    checkBox->setChecked(false);
    checkBox->setStyleSheet("QCheckBox { color: #000; background: #fff; }");
    layout->addWidget(checkBox);
    parent->connect(checkBox, SIGNAL(stateChanged(int)), parent, SLOT(stateChanged(int)));
    _callbacksCB.insert(_callbacksCB.cend(), checkBox, callback);
    return checkBox;
}

QComboBox* SkillTalentOrPerk::createComboBox(QWidget* parent, QVBoxLayout* layout, QString prompt, QList<QString> options, std::_Mem_fn<void (SkillTalentOrPerk::*)(int)> callback) {
    QComboBox* comboBox = new QComboBox(layout->parentWidget());
    comboBox->addItems(options);
    comboBox->setStyleSheet("QComboBox { color: #000; background: #fff; }");
    comboBox->setPlaceholderText(prompt);
    comboBox->setToolTip(prompt);
    comboBox->setCurrentIndex(-1);
    layout->addWidget(comboBox);
    parent->connect(comboBox, SIGNAL(currentIndexChanged(int)), parent, SLOT(currentIndexChanged(int)));
    _callbacksCBox.insert(_callbacksCBox.cend(), comboBox, callback);
    return comboBox;
}

QComboBox* SkillTalentOrPerk::createComboBox(QWidget* parent, QVBoxLayout* layout, QString prompt, QList<QString> options) {
    QComboBox* comboBox = new QComboBox(layout->parentWidget());
    comboBox->addItems(options);
    comboBox->setStyleSheet("QComboBox { color: #000; background: #fff; }");
    comboBox->setPlaceholderText(prompt);
    comboBox->setToolTip(prompt);
    comboBox->setCurrentIndex(-1);
    layout->addWidget(comboBox);
    parent->connect(comboBox, SIGNAL(currentIndexChanged(int)), parent, SLOT(currentIndexChanged(int)));
    return comboBox;
}

QLabel* SkillTalentOrPerk::createLabel(QWidget*, QVBoxLayout* layout, QString text) {
    QLabel* label = new QLabel(layout->parentWidget());
    label->setText(text);
    label->setStyleSheet("QLabel { color: #000; background: #fff; }");
    layout->addWidget(label);
    return label;
}

QLineEdit* SkillTalentOrPerk::createLineEdit(QWidget* parent, QVBoxLayout* layout, QString prompt, std::_Mem_fn<void (SkillTalentOrPerk::*)(QString)> callback) {
    QLineEdit* lineEdit = new QLineEdit(layout->parentWidget());
    lineEdit->setPlaceholderText(prompt);
    lineEdit->setToolTip(prompt);
    lineEdit->setText("");
    lineEdit->setStyleSheet("QLineEdit { color: #000; background: #fff; }");
    layout->addWidget(lineEdit);
    parent->connect(lineEdit, SIGNAL(textChanged(QString)), parent, SLOT(textChanged(QString)));
    _callbacksEdit.insert(_callbacksEdit.cend(), lineEdit, callback);
    return lineEdit;
}

QLineEdit* SkillTalentOrPerk::createLineEdit(QWidget* parent, QVBoxLayout* layout, QString prompt) {
    QLineEdit* lineEdit = new QLineEdit(layout->parentWidget());
    lineEdit->setPlaceholderText(prompt);
    lineEdit->setToolTip(prompt);
    lineEdit->setStyleSheet("QLineEdit { color: #000; background: #fff; }");
    layout->addWidget(lineEdit);
    parent->connect(lineEdit, SIGNAL(textChanged(QString)), parent, SLOT(textChanged(QString)));
    return lineEdit;
}

void SkillTalentOrPerk::ClearForm(QVBoxLayout* layout) {
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != NULL) {
        delete item->widget();
        delete item;
    }
}

bool SkillTalentOrPerk::createForm(QWidget* parent, QVBoxLayout* layout) {
    ClearForm(layout);
    return form(parent, layout);
}

QList<QString> SkillTalentOrPerk::Available() {
    return SkillsAvailable() + TalentsAvailable() + PerksAvailable();
}

QList<QString> SkillTalentOrPerk::SkillsAvailable() {
    return _skills.keys();
}

QList<QString> SkillTalentOrPerk::TalentsAvailable() {
    return _talents.keys();
}

QList<QString> SkillTalentOrPerk::PerksAvailable() {
    return _perks.keys();
}


shared_ptr<SkillTalentOrPerk> SkillTalentOrPerk::ByName(QString name) {
    if (_skills.find(name) != _skills.end()) return _skills[name]->create();
    else if (_talents.find(name) != _talents.end()) return _talents[name]->create();
    else if (_perks.find(name) != _perks.end()) return _perks[name]->create();
    else if (_enhancers.find(name) != _enhancers.end()) return _enhancers[name]->create();
    else return nullptr;
}

shared_ptr<SkillTalentOrPerk> SkillTalentOrPerk::FromJson(QString name, const QJsonObject& json) {
    if (_skills.find(name) != _skills.end()) return _skills[name]->create(json);
    else if (_talents.find(name) != _talents.end()) return _talents[name]->create(json);
    else if (_perks.find(name) != _perks.end()) return _perks[name]->create(json);
    else if (_enhancers.find(name) != _enhancers.end()) return _enhancers[name]->create(json);
    else return nullptr;
}
