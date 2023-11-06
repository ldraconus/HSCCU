#include "complicationsmenudialog.h"
#include "ui_complicationsmenudialog.h"
#include "sheet.h"
#include "sheet_ui.h"

ComplicationsMenuDialog::ComplicationsMenuDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ComplicationsMenuDialog)
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

ComplicationsMenuDialog::~ComplicationsMenuDialog()
{
    delete ui;
}

void ComplicationsMenuDialog::showEvent(QShowEvent*) {
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

void ComplicationsMenuDialog::newButton() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->newComplication->triggered();
}

void ComplicationsMenuDialog::editButton() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->editComplication->triggered();
}

void ComplicationsMenuDialog::deleteButton() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->deleteComplication->triggered();
}

void ComplicationsMenuDialog::cutButton() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->cutComplication->triggered();
}

void ComplicationsMenuDialog::copyButton() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->copyComplication->triggered();
}

void ComplicationsMenuDialog::pasteButton() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->pasteComplication->triggered();
}

void ComplicationsMenuDialog::moveUpButton() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->moveComplicationUp->triggered();
}

void ComplicationsMenuDialog::moveDownButtons() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->moveComplicationDown->triggered();
}
