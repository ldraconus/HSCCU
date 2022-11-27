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
    main.cpp \
    option.cpp \
    sheet.cpp

HEADERS += \
    AccidentalChange.h \
    Dependence.h \
    character.h \
    characteristic.h \
    complication.h \
    complicationsdialog.h \
    option.h \
    sheet.h \
    sheet_ui.h

FORMS += \
    complicationsdialog.ui \
    sheet.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
