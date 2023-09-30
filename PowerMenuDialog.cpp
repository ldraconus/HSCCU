#include "PowerMenuDialog.h"
#include "ui_PowerMenuDialog.h"
#include "sheet.h"
#include "sheet_ui.h"

PowerMenuDialog::PowerMenuDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PowerMenuDialog)
{
    ui->setupUi(this);

    connect(ui->newButton,      SIGNAL(clicked()), this, SLOT(newButton()));
    connect(ui->editButton,     SIGNAL(clicked()), this, SLOT(editButton()));
    connect(ui->deleteButton,   SIGNAL(clicked()), this, SLOT(deleteButton()));
    connect(ui->cutButton,      SIGNAL(clicked()), this, SLOT(cutButton()));
    connect(ui->copyButton,     SIGNAL(clicked()), this, SLOT(copyButton()));
    connect(ui->pasteButton,    SIGNAL(clicked()), this, SLOT(pasteButton()));
    connect(ui->moveUpButton,   SIGNAL(clicked()), this, SLOT(moveUpButton()));
    connect(ui->moveDownButton, SIGNAL(clicked()), this, SLOT(moveDownButton()));
}

PowerMenuDialog::~PowerMenuDialog()
{
    delete ui;
}

void PowerMenuDialog::showEvent(QShowEvent*) {
    QRect dlg = geometry();
    dlg.setTopLeft(_pos);
    setGeometry(dlg);

    ui->copyButton->setEnabled(_showCopy);
    ui->cutButton->setEnabled(_showCut);
    ui->deleteButton->setEnabled(_showDelete);
    ui->editButton->setEnabled(_showEdit);
    ui->moveDownButton->setEnabled(_showMoveDown);
    ui->moveUpButton->setEnabled(_showMoveUp);
    ui->pasteButton->setEnabled(_showPaste);
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
