QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RC_ICONS = HeroSystem.ico

win32: INCLUDEPATH += C:/Users/chris/src/include
wasm: INCLUDEPATH += C:/users/chris/src/include
unix: INCLUDEPATH += /home/chris/src/include

SOURCES += \
    Equipment.cpp \
    character.cpp \
    characteristic.cpp \
    complication.cpp \
    complicationsdialog.cpp \
    fraction.cpp \
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
    Debug.h \
    DefensePowers.h \
    Dependence.h \
    Dependent.h \
    DistinctiveFeature.h \
    Enraged.h \
    Equipment.h \
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
    fraction.h \
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
    skilltalentorperk.h \
    wasm.h

FORMS += \
    complicationsdialog.ui \
    modifiersdialog.ui \
    optiondialog.ui \
    powerdialog.ui \
    printwindow.ui \
    sheet.ui \
    skilldialog.ui

wasm {
SOURCES += \
    PowerMenuDialog.cpp \
    complicationsmenudialog.cpp \
    editmenudialog.cpp \
    filemenudialog.cpp \
    imgmenudialog.cpp

HEADERS += \
    PowerMenuDialog.h \
    complicationsmenudialog.h \
    editmenudialog.h \
    filemenudialog.h \
    imgmenudialog.h

FORMS += \
    PowerMenuDialog.ui \
    complicationsmenudialog.ui \
    editmenudialog.ui \
    filemenudialog.ui \
    imgmenudialog.ui \
    skillmenudialog.ui \
    wasm.ui

QMAKE_LFLAGS += -sASYNCIFY -Os
}

unix {
    QMAKE_CFLAGS += -DANDROID
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

DISTFILES += \
    ../build-HSCCU-WebAssembly_Qt_6_6_0_single_threaded-Release/HSCCU.html \
    ../build-HSCCU-WebAssembly_Qt_6_6_0_single_threaded-Release/HSCCU.js \
    ../build-HSCCU-WebAssembly_Qt_6_6_0_single_threaded-Release/qtloader.js \
    Installer/config/Style.qss \
    Installer/config/background.png \
    Installer/config/config.xml \
    Installer/packages/com.vendor.product/meta/license.txt \
    Installer/packages/com.vendor.product/meta/package.xml \
    TODO \
    TODO.txt \
    equipment.json
