QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    character.cpp \
    characteristic.cpp \
    complication.cpp \
    complicationsdialog.cpp \
    fraction.cpp \
    main.cpp \
    modifier.cpp \
    modifiersdialog.cpp \
    option.cpp \
    powerdialog.cpp \
    powers.cpp \
    sheet.cpp \
    skilldialog.cpp \
    skilltalentorperk.cpp

HEADERS += \
    AccidentalChange.h \
    AdjustmentPowers.h \
    AgilitySkills.h \
    BackgroundSkills.h \
    CombatSkills.h \
    Dependence.h \
    Dependent.h \
    DistinctiveFeature.h \
    Enraged.h \
    Hunted.h \
    IntellectSkills.h \
    InteractionSkills.h \
    MiscSkill.h \
    Money.h \
    NegativeReputation.h \
    Perk.h \
    PhysicalComplication.h \
    PsychologicalComplication.h \
    Rivalry.h \
    SocialComplication.h \
    Susceptibility.h \
    Talent.h \
    Unluck.h \
    Vulnerability.h \
    character.h \
    characteristic.h \
    complication.h \
    complicationsdialog.h \
    fraction.h \
    modifier.h \
    modifiersdialog.h \
    option.h \
    powerdialog.h \
    powers.h \
    sheet.h \
    sheet_ui.h \
    skilldialog.h \
    skilltalentorperk.h

FORMS += \
    complicationsdialog.ui \
    modifiersdialog.ui \
    powerdialog.ui \
    sheet.ui \
    skilldialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
