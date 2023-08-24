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
    emit Sheet::ref().getUi()->newComplication->triggered();
}

void PowerMenuDialog::editButton() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->editComplication->triggered();
}

void PowerMenuDialog::deleteButton() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->deleteComplication->triggered();
}

void PowerMenuDialog::cutButton() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->cutComplication->triggered();
}

void PowerMenuDialog::copyButton() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->copyComplication->triggered();
}

void PowerMenuDialog::pasteButton() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->pasteComplication->triggered();
}

void PowerMenuDialog::moveUpButton() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->moveComplicationUp->triggered();
}

void PowerMenuDialog::moveDownButtons() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->moveComplicationDown->triggered();
}
