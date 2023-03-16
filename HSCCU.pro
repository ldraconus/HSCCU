QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RC_ICONS = HeroSystem.ico

SOURCES += \
    character.cpp \
    characteristic.cpp \
    complication.cpp \
    complicationsdialog.cpp \
    complicationsmenudialog.cpp \
    editmenudialog.cpp \
    filemenudialog.cpp \
    fraction.cpp \
    imgmenudialog.cpp \
    main.cpp \
    modifier.cpp \
    modifiersdialog.cpp \
    option.cpp \
    optiondialog.cpp \
    powerdialog.cpp \
    powers.cpp \
    printwindow.cpp \
    sheet.cpp \
    skilldialog.cpp \
    skillmenudialog.cpp \
    skilltalentorperk.cpp

HEADERS += \
    AccidentalChange.h \
    AdjustmentPowers.h \
    AgilitySkills.h \
    AttackPowers.h \
    AutomatonPowers.h \
    BackgroundSkills.h \
    BodyAffectingPowers.h \
    CombatSkills.h \
    DefensePowers.h \
    Dependence.h \
    Dependent.h \
    DistinctiveFeature.h \
    Enraged.h \
    FrameworkPowers.h \
    Hunted.h \
    IntellectSkills.h \
    InteractionSkills.h \
    MentalPowers.h \
    MiscSkill.h \
    Money.h \
    MovementPowers.h \
    NegativeReputation.h \
    Perk.h \
    PhysicalComplication.h \
    PsychologicalComplication.h \
    Rivalry.h \
    SenseAffectingPowers.h \
    SensoryPowers.h \
    SkillEnhancers.h \
    SocialComplication.h \
    SpecialPowers.h \
    StandardPowers.h \
    Susceptibility.h \
    Talent.h \
    Unluck.h \
    Vulnerability.h \
    character.h \
    characteristic.h \
    complication.h \
    complicationsdialog.h \
    complicationsmenudialog.h \
    editmenudialog.h \
    filemenudialog.h \
    fraction.h \
    imgmenudialog.h \
    modifier.h \
    modifiersdialog.h \
    option.h \
    optiondialog.h \
    powerdialog.h \
    powers.h \
    printwindow.h \
    shared.h \
    sheet.h \
    sheet_ui.h \
    skilldialog.h \
    skillmenudialog.h \
    skilltalentorperk.h \
    wasm.h

FORMS += \
    complicationsdialog.ui \
    complicationsmenudialog.ui \
    editmenudialog.ui \
    filemenudialog.ui \
    imgmenudialog.ui \
    modifiersdialog.ui \
    optiondialog.ui \
    powerdialog.ui \
    printwindow.ui \
    sheet.ui \
    skilldialog.ui \
    skillmenudialog.ui \
    wasm.ui

wasm {
    QMAKE_LFLAGS += -sASYNCIFY -Os
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

DISTFILES += \
    TODO
