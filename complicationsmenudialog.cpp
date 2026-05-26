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
    dlg.setTopLeft(mPos);
    setGeometry(dlg);

    ui->copyButton->setEnabled(mShowCopy);
    ui->cutButton->setEnabled(mShowCut);
    ui->deleteButton->setEnabled(mShowDelete);
    ui->editButton->setEnabled(mShowEdit);
    ui->moveDownButton->setEnabled(mShowMoveDown);
    ui->moveUpButton->setEnabled(mShowMoveUp);
    ui->pasteButton->setEnabled(mShowPaste);
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
