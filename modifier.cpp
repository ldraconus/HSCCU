#include "modifier.h"
#include "qcheckbox.h"

#include <QHeaderView>
#include <QScreen>

#define MAKE(x)           Modifier::mod<x> _##x
#define LINK(w, x, y, z)  _modifiers[w] = make_shared<Modifier>(w, x, y, &statics::_##z);

namespace statics {
    MAKE(Ablative);
    MAKE(AffectedAsAnotherSense);
    MAKE(AffectedAsMoreThanOneSense);
    MAKE(AffectsDesolid);
    MAKE(AffectsPhysicalWorld);
    MAKE(Allocatable);
    MAKE(AlterableOriginPoint);
    MAKE(AlterableSize);
    MAKE(AlternateCombatValue);
    MAKE(AlwaysOn);
    MAKE(AreaOfEffect);
    MAKE(ArmorPiercing);
    MAKE(AVAD);
    MAKE(Autofire);
    MAKE(Backlash);
    MAKE(BasedOnCON);
    MAKE(Beam);
    MAKE(BodyOnly);
    MAKE(CanApplyRemoveAdders);
    MAKE(CanBeDeflectedReflected);
    MAKE(CanBeDispelled);
    MAKE(CanHealLimbs);
    MAKE(CanOnlyBeUsedThroughMindLink);
    MAKE(CannotBeEscapedWithTeleportation);
    MAKE(CannotBeUsedThroughMindLink);
    MAKE(CannotBeUsedWith);
    MAKE(CannotUseTargeting);
    MAKE(Charges);
    MAKE(CombatAccelerationDecelleration);
    MAKE(Concentration);
    MAKE(Constant);
    MAKE(CostsEndurance);
    MAKE(CostsEnduranceOnlyToActivate);
    MAKE(CostsEnduranceToMaintain);
    MAKE(Cumulative);
    MAKE(DamageOverTime);
    MAKE(DecreasedAccelerationDeceleration);
    MAKE(DelayedEffect);
    MAKE(DelayedFadeReturnRate);
    MAKE(DifficultToDispel);
    MAKE(Dismissible);
    MAKE(DividedEffect);
    MAKE(DoesBODY);
    MAKE(DoesKnockback);
    MAKE(DoesntWorkOnDefinedDamage);
    MAKE(DoesntWorkWhileDuplicatExists);
    MAKE(DoubleKnockback);
    MAKE(ExpandedEffect);
    MAKE(ExtraTime);
    MAKE(EyeContactRequired);
    MAKE(Focus);
    MAKE(Gestures);
    MAKE(Hardened);
    MAKE(HoleInTheMiddle);
    MAKE(Impenetrable);
    MAKE(ImprovedNoncombatMovement);
    MAKE(Inaccurate);
    MAKE(Incantations);
    MAKE(IncreasedENDCost);
    MAKE(IncreasedMass);
    MAKE(IncreasedMaximumEffect);
    MAKE(IncreasedMaximumRange);
    MAKE(Indirect);
    MAKE(Inherent);
    MAKE(Instant);
    MAKE(InvisiblePowerEffects);
    MAKE(LimitedEffect);
    MAKE(LimitedManeuverability);
    MAKE(LimitedPower);
    MAKE(LimitedRange);
    MAKE(LimitedSpecialEffect);
    MAKE(LineOfSight);
    MAKE(Linked);
    MAKE(Lockout);
    MAKE(MandatoryEffect);
    MAKE(Mass);
    MAKE(Megascale);
    MAKE(MentalDefenseAddsToEGO);
    MAKE(NoConciousControl);
    MAKE(NoGravityPenalty);
    MAKE(NoKnockback);
    MAKE(NoNoncombatMovement);
    MAKE(NoRange);
    MAKE(NoTurnMode);
    MAKE(NoncombatAccelerationDecelleration);
    MAKE(Nonpersistent);
    MAKE(NonresistentDefenses);
    MAKE(OneUseAtATime);
    MAKE(OnlyInAlternateID);
    MAKE(OnlyProtectsBarrier);
    MAKE(OnlyRestoresToStartingValues);
    MAKE(OnlyWorksAgainstDefined);
    MAKE(Opaque);
    MAKE(Penetrating);
    MAKE(Perceivable);
    MAKE(Persistant);
    MAKE(PersonalImmunity);
    MAKE(PhysicalManifestation);
    MAKE(PositionShift);
    MAKE(RangeBasedOnSTR);
    MAKE(Ranged);
    MAKE(RapidNoncombatMovement);
    MAKE(RealArmor);
    MAKE(RealWeapon);
    MAKE(ReducedByRange);
    MAKE(ReducedByShrinking);
    MAKE(ReducedEndurance);
    MAKE(ReducedNegation);
    MAKE(ReducedPenetration);
    MAKE(ReducedRangeModifier);
    MAKE(RequiredHands);
    MAKE(RequiresARoll);
    MAKE(RequiresMultipleCharges);
    MAKE(RequiresMultipleUsers);
    MAKE(Resistant);
    MAKE(Restrainable);
    MAKE(Ressurection);
    MAKE(RessurectionOnly);
    MAKE(SafeBlindTravel);
    MAKE(SideEffects);
    MAKE(SkinContactRequired);
    MAKE(StandardRange);
    MAKE(Sticky);
    MAKE(StopsWorkingIfKnockedOutStunned);
    MAKE(STRMinimum);
    MAKE(STUNOnly);
    MAKE(SubjectToRangeModifier);
    MAKE(TimeLimit);
    MAKE(Transdimensional);
    MAKE(Trigger);
    MAKE(TurnMode);
    MAKE(Uncontrolled);
    MAKE(UnifiedPower);
    MAKE(UsableAsMovement);
    MAKE(UsableByOthers);
    MAKE(VariableAdvantage);
    MAKE(VariableEffect);
    MAKE(VariableLimitations);
    MAKE(VariableSpecialEffects);
    MAKE(WorksAgainstEGONotCharacteristic);
}

QMap<QString, shared_ptr<Modifier>> Modifiers::_modifiers;

Modifiers::Modifiers()
{
    if (!_modifiers.isEmpty()) return;
    LINK("Ablative",                                Modifier::isLimitation, Modifier::isModifier, Ablative);
    LINK("Affected As Another Sense",               Modifier::isLimitation, Modifier::isModifier, AffectedAsAnotherSense);
    LINK("Affected As More Than One Sense",         Modifier::isLimitation, Modifier::isModifier, AffectedAsMoreThanOneSense);
    LINK("Affects Desolid",                         Modifier::isAdvantage,  Modifier::isModifier, AffectsDesolid);
    LINK("Affects Physical World",                  Modifier::isAdvantage,  Modifier::isModifier, AffectsPhysicalWorld);
    LINK("Allocatable▲",                            Modifier::isAdvantage,  Modifier::isModifier, Allocatable);
    LINK("Alterable Origin Pointt",                 Modifier::isAdvantage,  Modifier::isAnAdder,  AlterableOriginPoint);
    LINK("Alterable Size",                          Modifier::isAdvantage,  Modifier::isAnAdder,  AlterableSize);
    LINK("Alternate Combat Value▲",                 Modifier::isBoth,       Modifier::isModifier, AlternateCombatValue);
    LINK("Always On",                               Modifier::isLimitation, Modifier::isModifier, AlwaysOn);
    LINK("Area Of Effect",                          Modifier::isAdvantage,  Modifier::isModifier, AreaOfEffect);
    LINK("Armor Piercing",                          Modifier::isAdvantage,  Modifier::isModifier, ArmorPiercing);
    LINK("Attack Versus Alternate Defense▲",        Modifier::isBoth,       Modifier::isModifier, AVAD);
    LINK("Autofire",                                Modifier::isAdvantage,  Modifier::isModifier, Autofire);
    LINK("Backlash",                                Modifier::isAdvantage,  Modifier::isModifier, Backlash);
    LINK("Based on CON",                            Modifier::isLimitation, Modifier::isModifier, BasedOnCON);
    LINK("Beam",                                    Modifier::isLimitation, Modifier::isModifier, Beam);
    LINK("BODY Only",                               Modifier::isLimitation, Modifier::isModifier, BodyOnly);
    LINK("Can Apply/Remove Adders",                 Modifier::isAdvantage,  Modifier::isModifier, CanApplyRemoveAdders);
    LINK("Can be Deflected/Reflected",              Modifier::isLimitation, Modifier::isModifier, CanBeDeflectedReflected);
    LINK("Can Be Dispelled",                        Modifier::isLimitation, Modifier::isModifier, CanBeDispelled);
    LINK("Can Heal Limbs",                          Modifier::isAdvantage,  Modifier::isAnAdder,  CanHealLimbs);
    LINK("Can Only Be Used Through Mind Link",      Modifier::isLimitation, Modifier::isModifier, CanOnlyBeUsedThroughMindLink);
    LINK("Cannot Be Escaped With Teleportation",    Modifier::isAdvantage,  Modifier::isModifier, CannotBeEscapedWithTeleportation);
    LINK("Cannot Be Used Through Mind Link",        Modifier::isLimitation, Modifier::isModifier, CannotBeUsedThroughMindLink);
    LINK("Cannot Be Used With ...",                 Modifier::isLimitation, Modifier::isModifier, CannotBeUsedWith);
    LINK("Cannot Use Targeting",                    Modifier::isLimitation, Modifier::isModifier, CannotUseTargeting);
    LINK("Charges",                                 Modifier::isBoth,       Modifier::isModifier, Charges);
    LINK("Combat Acceleration/Decelleration",       Modifier::isAdvantage,  Modifier::isModifier, CombatAccelerationDecelleration);
    LINK("Concentration",                           Modifier::isLimitation, Modifier::isModifier, Concentration);
    LINK("Constant",                                Modifier::isAdvantage,  Modifier::isModifier, Constant);
    LINK("Costs Endurance",                         Modifier::isLimitation, Modifier::isModifier, CostsEndurance);
    LINK("Costs Endurance Only To Activate▲",       Modifier::isAdvantage,  Modifier::isModifier, CostsEnduranceOnlyToActivate);
    LINK("Costs Endurance To Maintain",             Modifier::isLimitation, Modifier::isModifier, CostsEnduranceToMaintain);
    LINK("Cumulative▲",                             Modifier::isAdvantage,  Modifier::isModifier, Cumulative);
    LINK("Damage Over Time▲",                       Modifier::isAdvantage,  Modifier::isModifier, DamageOverTime);
    LINK("Decreased Acceleration/Decelleration",    Modifier::isLimitation, Modifier::isModifier, DecreasedAccelerationDeceleration);
    LINK("Delayed EffectꚚ",                         Modifier::isAdvantage,  Modifier::isModifier, DelayedEffect);
    LINK("Delayed Fade/Return Rate▲",               Modifier::isAdvantage,  Modifier::isModifier, DelayedFadeReturnRate);
    LINK("Difficult To Dispel",                     Modifier::isAdvantage,  Modifier::isModifier, DifficultToDispel);
    LINK("Dimissible▲",                             Modifier::isAdvantage,  Modifier::isAnAdder,  Dismissible);
    LINK("Divided Effect",                          Modifier::isLimitation, Modifier::isModifier, DividedEffect);
    LINK("Does BODYϴ",                              Modifier::isAdvantage,  Modifier::isModifier, DoesBODY);
    LINK("Does Knockback",                          Modifier::isAdvantage,  Modifier::isModifier, DoesKnockback);
    LINK("Doesn't Work On [Defined] Damage",        Modifier::isLimitation, Modifier::isModifier, DoesntWorkOnDefinedDamage);
    LINK("Doesn't Work While Duplicate Exists",     Modifier::isLimitation, Modifier::isModifier, DoesntWorkWhileDuplicatExists);
    LINK("Double Knockback",                        Modifier::isAdvantage,  Modifier::isModifier, DoubleKnockback);
    LINK("Expanded Effect▲",                        Modifier::isAdvantage,  Modifier::isModifier, ExpandedEffect);
    LINK("Extra Time",                              Modifier::isLimitation, Modifier::isModifier, ExtraTime);
    LINK("Eye Contact Required",                    Modifier::isLimitation, Modifier::isModifier, EyeContactRequired);
    LINK("Focus",                                   Modifier::isLimitation, Modifier::isModifier, Focus);
    LINK("Gestures",                                Modifier::isLimitation, Modifier::isModifier, Gestures);
    LINK("Hardened",                                Modifier::isAdvantage,  Modifier::isModifier, Hardened);
    LINK("Hole In The Middle",                      Modifier::isAdvantage,  Modifier::isModifier, HoleInTheMiddle);
    LINK("Impenetrable",                            Modifier::isAdvantage,  Modifier::isModifier, Impenetrable);
    LINK("Improved Noncombat Movement",             Modifier::isAdvantage,  Modifier::isAnAdder,  ImprovedNoncombatMovement);
    LINK("Inaccurate",                              Modifier::isLimitation, Modifier::isModifier, Inaccurate);
    LINK("Incantations",                            Modifier::isLimitation, Modifier::isModifier, Incantations);
    LINK("Increased END Cost",                      Modifier::isLimitation, Modifier::isModifier, IncreasedENDCost);
    LINK("Increased Mass",                          Modifier::isAdvantage,  Modifier::isAnAdder,  IncreasedMass);
    LINK("Increased Maximum Effect",                Modifier::isAdvantage,  Modifier::isModifier, IncreasedMaximumEffect);
    LINK("Increased Maximum Range",                 Modifier::isAdvantage,  Modifier::isModifier, IncreasedMaximumRange);
    LINK("Indirect",                                Modifier::isAdvantage,  Modifier::isModifier, Indirect);
    LINK("Inherent",                                Modifier::isAdvantage,  Modifier::isModifier, Inherent);
    LINK("Instant",                                 Modifier::isLimitation, Modifier::isModifier, Instant);
    LINK("Invisible Power Effects",                 Modifier::isAdvantage,  Modifier::isModifier, InvisiblePowerEffects);
    LINK("Limited Effect",                          Modifier::isLimitation, Modifier::isModifier, LimitedEffect);
    LINK("Limited Maneuverability",                 Modifier::isLimitation, Modifier::isModifier, LimitedManeuverability);
    LINK("Limited Power",                           Modifier::isLimitation, Modifier::isModifier, LimitedPower);
    LINK("Limited Range",                           Modifier::isBoth,       Modifier::isModifier, LimitedRange);
    LINK("Limited Special Effect",                  Modifier::isLimitation, Modifier::isModifier, LimitedSpecialEffect);
    LINK("Line Of Sight▲",                          Modifier::isAdvantage,  Modifier::isModifier, LineOfSight);
    LINK("Linked",                                  Modifier::isBoth,       Modifier::isModifier, Linked);
    LINK("Lockout",                                 Modifier::isLimitation, Modifier::isModifier, Lockout);
    LINK("Mandatory Effect",                        Modifier::isLimitation, Modifier::isModifier, MandatoryEffect);
    LINK("MassꚚ",                                   Modifier::isLimitation, Modifier::isModifier, Mass);
    LINK("Megascaleϴ",                              Modifier::isAdvantage,  Modifier::isModifier, Megascale);
    LINK("Mental Defense Adds To EGO",              Modifier::isLimitation, Modifier::isModifier, MentalDefenseAddsToEGO);
    LINK("No Concious Control▲",                    Modifier::isLimitation, Modifier::isModifier, NoConciousControl);
    LINK("No Gravity Penalty",                      Modifier::isAdvantage,  Modifier::isModifier, NoGravityPenalty);
    LINK("No Knockback",                            Modifier::isLimitation, Modifier::isModifier, NoKnockback);
    LINK("No Noncombat Movement",                   Modifier::isLimitation, Modifier::isModifier, NoNoncombatMovement);
    LINK("No Range",                                Modifier::isLimitation, Modifier::isModifier, NoRange);
    LINK("No Turn Mode▲",                           Modifier::isAdvantage,  Modifier::isModifier, NoTurnMode);
    LINK("Noncombat Acceleration/Decelleration",    Modifier::isAdvantage,  Modifier::isModifier, NoncombatAccelerationDecelleration);
    LINK("Nonpersistant",                           Modifier::isLimitation, Modifier::isModifier, Nonpersistent);
    LINK("Nonresistant Defenses",                   Modifier::isLimitation, Modifier::isModifier, NonresistentDefenses);
    LINK("One Use At A Time",                       Modifier::isLimitation, Modifier::isModifier, OneUseAtATime);
    LINK("Only In Alternate Identity",              Modifier::isLimitation, Modifier::isModifier, OnlyInAlternateID);
    LINK("Only Protects Barrier",                   Modifier::isLimitation, Modifier::isModifier, OnlyProtectsBarrier);
    LINK("Only Restores To Starting Values",        Modifier::isLimitation, Modifier::isModifier, OnlyRestoresToStartingValues);
    LINK("Only Works Against [Defined]",            Modifier::isLimitation, Modifier::isModifier, OnlyWorksAgainstDefined);
    LINK("Opaque",                                  Modifier::isAdvantage,  Modifier::isAnAdder,  Opaque);
    LINK("Penetrating",                             Modifier::isAdvantage,  Modifier::isModifier, Penetrating);
    LINK("Perceivable",                             Modifier::isLimitation, Modifier::isModifier, Perceivable);
    LINK("Persistent",                              Modifier::isAdvantage,  Modifier::isModifier, Persistant);
    LINK("Personal Immunity",                       Modifier::isAdvantage,  Modifier::isModifier, PersonalImmunity);
    LINK("Physical Manifestation",                  Modifier::isLimitation, Modifier::isModifier, PhysicalManifestation);
    LINK("Position Shift",                          Modifier::isAdvantage,  Modifier::isAnAdder,  PositionShift);
    LINK("Range Based On STR",                      Modifier::isBoth,       Modifier::isModifier, RangeBasedOnSTR);
    LINK("Ranged",                                  Modifier::isAdvantage,  Modifier::isModifier, Ranged);
    LINK("Rapid Noncombat Movement",                Modifier::isAdvantage,  Modifier::isModifier, RapidNoncombatMovement);
    LINK("Real Armor",                              Modifier::isLimitation, Modifier::isModifier, RealArmor);
    LINK("Real Weapon",                             Modifier::isLimitation, Modifier::isModifier, RealWeapon);
    LINK("Reduced By Range",                        Modifier::isLimitation, Modifier::isModifier, ReducedByRange);
    LINK("Reduced By Shrinking",                    Modifier::isLimitation, Modifier::isModifier, ReducedByShrinking);
    LINK("Reduced Endurance",                       Modifier::isAdvantage,  Modifier::isModifier, ReducedEndurance);
    LINK("Reduced Negation",                        Modifier::isAdvantage,  Modifier::isAnAdder,  ReducedNegation);
    LINK("Reduced Penetration",                     Modifier::isLimitation, Modifier::isModifier, ReducedPenetration);
    LINK("Reduced Range Modifier",                  Modifier::isAdvantage,  Modifier::isModifier, ReducedRangeModifier);
    LINK("Required HandsꚚ",                         Modifier::isLimitation, Modifier::isModifier, RequiredHands);
    LINK("Requires A Roll",                         Modifier::isLimitation, Modifier::isModifier, RequiresARoll);
    LINK("Requires Multiple Charges",               Modifier::isLimitation, Modifier::isModifier, RequiresMultipleCharges);
    LINK("Requires Multiple Users",                 Modifier::isLimitation, Modifier::isModifier, RequiresMultipleUsers);
    LINK("Resistant",                               Modifier::isAdvantage,  Modifier::isModifier, Resistant);
    LINK("Restrainable",                            Modifier::isLimitation, Modifier::isModifier, Restrainable);
    LINK("Resurrectionϴ",                           Modifier::isAdvantage,  Modifier::isAnAdder,  Ressurection);
    LINK("Resurrection Only",                       Modifier::isLimitation, Modifier::isModifier, RessurectionOnly);
    LINK("Safe Blind Travel",                       Modifier::isAdvantage,  Modifier::isModifier, SafeBlindTravel);
    LINK("Side Effects",                            Modifier::isLimitation, Modifier::isModifier, SideEffects);
    LINK("Skin Contact Required",                   Modifier::isLimitation, Modifier::isModifier, SkinContactRequired);
    LINK("Standard Range",                          Modifier::isLimitation, Modifier::isModifier, StandardRange);
    LINK("Sticky",                                  Modifier::isAdvantage,  Modifier::isModifier, Sticky);
    LINK("Stops Working If Knocked Out/Stunned",    Modifier::isLimitation, Modifier::isModifier, StopsWorkingIfKnockedOutStunned);
    LINK("STR MinimumꚚ",                            Modifier::isLimitation, Modifier::isModifier, STRMinimum);
    LINK("STUN Only",                               Modifier::isLimitation, Modifier::isModifier, STUNOnly);
    LINK("Subject To Range Modifier",               Modifier::isLimitation, Modifier::isModifier, SubjectToRangeModifier);
    LINK("Time Limit",                              Modifier::isBoth,       Modifier::isModifier, TimeLimit);
    LINK("Transdimensionalϴ",                       Modifier::isAdvantage,  Modifier::isModifier, Transdimensional);
    LINK("Trigger",                                 Modifier::isAdvantage,  Modifier::isModifier, Trigger);
    LINK("Turn Mode",                               Modifier::isLimitation, Modifier::isModifier, TurnMode);
    LINK("Uncontrolledϴ",                           Modifier::isAdvantage,  Modifier::isModifier, Uncontrolled);
    LINK("Unified Power▲",                          Modifier::isLimitation, Modifier::isModifier, UnifiedPower);
    LINK("Usable As [Movement]",                    Modifier::isAdvantage,  Modifier::isModifier, UsableAsMovement);
    LINK("Usable On Othersϴ",                       Modifier::isBoth,       Modifier::isModifier, UsableByOthers);
    LINK("Variable Advantageϴ",                     Modifier::isAdvantage,  Modifier::isModifier, VariableAdvantage);
    LINK("Variable Effect▲",                        Modifier::isAdvantage,  Modifier::isModifier, VariableEffect);
    LINK("Variable Limitations▲",                   Modifier::isLimitation, Modifier::isModifier, VariableLimitations);
    LINK("Variable Special Effects",                Modifier::isAdvantage,  Modifier::isModifier, VariableSpecialEffects);
    LINK("Works Against EGO not [Characteristic]▲", Modifier::isAdvantage,  Modifier::isModifier, WorksAgainstEGONotCharacteristic);
}

shared_ptr<Modifier> ModifierBase::create() {
    if (_base) return _base->create();
    return nullptr;
}

shared_ptr<Modifier> ModifierBase::create(const QJsonObject& json) {
    if (_base) return _base->create(json);
    return nullptr;
}

QList<shared_ptr<Modifier>> Modifiers::operator()(Modifier::ModifierType type) const {
    QList<shared_ptr<Modifier>> modifiers;
    QStringList keys = _modifiers.keys();
    for (const auto& modifier: keys) if (type & _modifiers[modifier]->type()) modifiers.push_back(_modifiers[modifier]);
    return modifiers;
}

bool Modifier::isNumber(QString txt) {
    bool ok;
    txt.toInt(&ok, 10);
    return ok;
}

void ModifierBase::callback(QCheckBox* checkBox) {
    _sender = checkBox;
    auto f = _callbacksCB.find(checkBox);
    if (f == _callbacksCB.end()) return;
    auto function = f.value();
    function(this, checkBox->isChecked());
}

void ModifierBase::callback(QComboBox* combo) {
    _sender = combo;
    auto f = _callbacksCBox.find(combo);
    if (f == _callbacksCBox.end()) return;
    auto function = f.value();
    function(this, combo->currentIndex());
}

void ModifierBase::callback(QLineEdit* edit) {
    _sender = edit;
    auto f = _callbacksEdit.find(edit);
    if (f == _callbacksEdit.end()) return;
    auto function = f.value();
    function(this, edit->text());
}

void ModifierBase::callback(QTreeWidget* tree) {
    _sender = tree;
    auto f = _callbacksTree.find(tree);
    if (f == _callbacksTree.end()) return;
    auto function = f.value();
    function(this, 0, 0, false);
}

void Modifier::createForm(QWidget* parent, QVBoxLayout* layout) {
    ClearForm(layout);
    form(parent, layout);
}

QCheckBox* Modifier::createCheckBox(QWidget* parent, QVBoxLayout* layout, QString prompt) {
    QCheckBox* checkBox = new QCheckBox(layout->parentWidget());
    checkBox->setText(prompt);
    checkBox->setChecked(false);
    layout->addWidget(checkBox);
    parent->connect(checkBox, SIGNAL(stateChanged(int)), parent, SLOT(stateChanged(int)));
    return checkBox;
}

QCheckBox* Modifier::createCheckBox(QWidget* parent, QVBoxLayout* layout, QString prompt, std::_Mem_fn<void (ModifierBase::*)(bool)> callback) {
    QCheckBox* checkBox = new QCheckBox(layout->parentWidget());
    checkBox->setText(prompt);
    checkBox->setChecked(false);
    layout->addWidget(checkBox);
    parent->connect(checkBox, SIGNAL(stateChanged(int)), parent, SLOT(stateChanged(int)));
    _callbacksCB.insert(_callbacksCB.cend(), checkBox, callback);
    return checkBox;
}

QComboBox* Modifier::createComboBox(QWidget* parent, QVBoxLayout* layout, QString prompt, QList<QString> options, std::_Mem_fn<void (ModifierBase::*)(int)> callback) {
    QComboBox* comboBox = new QComboBox(layout->parentWidget());
    comboBox->addItems(options);
    comboBox->setPlaceholderText(prompt);
    comboBox->setToolTip(prompt);
    comboBox->setCurrentIndex(-1);
    layout->addWidget(comboBox);
    parent->connect(comboBox, SIGNAL(currentIndexChanged(int)), parent, SLOT(currentIndexChanged(int)));
    _callbacksCBox.insert(_callbacksCBox.cend(), comboBox, callback);
    return comboBox;
}

QComboBox* Modifier::createComboBox(QWidget* parent, QVBoxLayout* layout, QString prompt, QList<QString> options) {
    QComboBox* comboBox = new QComboBox(layout->parentWidget());
    comboBox->addItems(options);
    comboBox->setPlaceholderText(prompt);
    comboBox->setToolTip(prompt);
    comboBox->setCurrentIndex(-1);
    layout->addWidget(comboBox);
    parent->connect(comboBox, SIGNAL(currentIndexChanged(int)), parent, SLOT(currentIndexChanged(int)));
    return comboBox;
}

QLabel* Modifier::createLabel(QWidget*, QVBoxLayout* layout, QString text) {
    QLabel* label = new QLabel(layout->parentWidget());
    label->setText(text);
    layout->addWidget(label);
    return label;
}

QLineEdit* Modifier::createLineEdit(QWidget* parent, QVBoxLayout* layout, QString prompt, std::_Mem_fn<void (ModifierBase::*)(QString)> callback) {
    QLineEdit* lineEdit = new QLineEdit(layout->parentWidget());
    lineEdit->setPlaceholderText(prompt);
    lineEdit->setToolTip(prompt);
    lineEdit->setText("");
    layout->addWidget(lineEdit);
    parent->connect(lineEdit, SIGNAL(textChanged(QString)), parent, SLOT(textChanged(QString)));
    _callbacksEdit.insert(_callbacksEdit.cend(), lineEdit, callback);
    return lineEdit;
}

QLineEdit* Modifier::createLineEdit(QWidget* parent, QVBoxLayout* layout, QString prompt) {
    QLineEdit* lineEdit = new QLineEdit(layout->parentWidget());
    lineEdit->setPlaceholderText(prompt);
    lineEdit->setToolTip(prompt);
    layout->addWidget(lineEdit);
    parent->connect(lineEdit, SIGNAL(textChanged(QString)), parent, SLOT(textChanged(QString)));
    return lineEdit;
}

static QTreeWidgetItem* createTWItem(QString str) {
    QTreeWidgetItem* item = new QTreeWidgetItem({ str });
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    item->setCheckState(0, Qt::CheckState::Unchecked);
    return item;
}

QTreeWidget* Modifier::createTreeWidget(QWidget* parent, QVBoxLayout* layout, QMap<QString, QStringList> options, std::_Mem_fn<void (ModifierBase::*)(int, int, bool)> callback) {
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
    tree->setStyleSheet("QTreeWidget { border-style: none; }");
    layout->addWidget(tree);
    auto* hdr = tree->header();
    hdr->setVisible(false);
    parent->connect(tree, SIGNAL(itemSelectionChanged()), parent, SLOT(itemSelectionChanged()));
    parent->connect(tree, SIGNAL(itemChanged(QTreeWidgetItem*,int)), parent, SLOT(itemChanged(QTreeWidgetItem*,int)));
    _callbacksTree.insert(_callbacksTree.cend(), tree, callback);
    return tree;
}

QTreeWidget* Modifier::createTreeWidget(QWidget* parent, QVBoxLayout* layout, QMap<QString, QStringList> options) {
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
    tree->setStyleSheet("QTreeWidget { border-style: none; }");
    layout->addWidget(tree);
    auto* hdr = tree->header();
    hdr->setVisible(false);
    parent->connect(tree, SIGNAL(itemSelectionChanged()), parent, SLOT(itemSelectionChanged()));
    parent->connect(tree, SIGNAL(itemChanged(QTreeWidgetItem*,int)), parent, SLOT(itemChanged(QTreeWidgetItem*,int)));
    return tree;
}

shared_ptr<Modifier> Modifiers::ByName(QString name) {
    if (_modifiers.find(name) != _modifiers.end())
        return _modifiers[name]->create();
    return nullptr;
}

void Modifier::ClearForm(QVBoxLayout* layout) {
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != NULL) {
        delete item->widget();
        delete item;
    }
}

