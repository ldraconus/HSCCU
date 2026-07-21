#include "powerMenuDialog.h"
#include "ui_powerMenuDialog.h"
#include "sheet.h"
#include "sheet_ui.h"

#include <QToolButton>

PowerMenuDialog::PowerMenuDialog(QWidget *parent) :
    QDialog(parent),
    mUi(new Ui::powerMenuDialog) {
    mUi->setupUi(this);

    connect(mUi->newButton,      SIGNAL(clicked()), this, SLOT(newButton()));
    connect(mUi->editButton,     SIGNAL(clicked()), this, SLOT(editButton()));
    connect(mUi->deleteButton,   SIGNAL(clicked()), this, SLOT(deleteButton()));
    connect(mUi->cutButton,      SIGNAL(clicked()), this, SLOT(cutButton()));
    connect(mUi->copyButton,     SIGNAL(clicked()), this, SLOT(copyButton()));
    connect(mUi->pasteButton,    SIGNAL(clicked()), this, SLOT(pasteButton()));
    connect(mUi->moveUpButton,   SIGNAL(clicked()), this, SLOT(moveUpButton()));
    connect(mUi->moveDownButton, SIGNAL(clicked()), this, SLOT(moveDownButton()));
}

PowerMenuDialog::~PowerMenuDialog() {
    delete mUi;
}

void PowerMenuDialog::showEvent(QShowEvent*) {
    QRect dialogPos = geometry();
    QRect menuPos = Sheet::ref().powersAndEquipmentButton->geometry();
    dialogPos.setLeft(menuPos.left());
    dialogPos.setTop(menuPos.height());
    dialogPos.setWidth(82);
    dialogPos.setHeight(194);
    setGeometry(dialogPos);

    mUi->copyButton->setEnabled(mShowCopy);
    mUi->cutButton->setEnabled(mShowCut);
    mUi->deleteButton->setEnabled(mShowDelete);
    mUi->editButton->setEnabled(mShowEdit);
    mUi->moveDownButton->setEnabled(mShowMoveDown);
    mUi->moveUpButton->setEnabled(mShowMoveUp);
    mUi->pasteButton->setEnabled(mShowPaste);
}

void PowerMenuDialog::newButton() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->newPowerOrEquipment->triggered();
}

void PowerMenuDialog::editButton() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->editPowerOrEquipment->triggered();
}

void PowerMenuDialog::deleteButton() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->deletePowerOrEquipment->triggered();
}

void PowerMenuDialog::cutButton() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->cutPowerOrEquipment->triggered();
}

void PowerMenuDialog::copyButton() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->copyPowerOrEquipment->triggered();
}

void PowerMenuDialog::pasteButton() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->pastePowerOrEquipment->triggered();
}

void PowerMenuDialog::moveUpButton() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->movePowerOrEquipmentUp->triggered();
}

void PowerMenuDialog::moveDownButtons() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->movePowerOrEquipmentDown->triggered();
}
