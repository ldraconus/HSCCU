#include "skillmenudialog.h"
#include "ui_skillmenudialog.h"
#include "sheet.h"
#include "sheet_ui.h"

SkillMenuDialog::SkillMenuDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SkillMenuDialog)
{
    ui->setupUi(this);

    connect(ui->newButton,      SIGNAL(clicked()), this, SLOT(newButton()));
    connect(ui->editButton,     SIGNAL(clicked()), this, SLOT(editButton()));
    connect(ui->deleteButton,   SIGNAL(clicked()), this, SLOT(deleteButton()));
    connect(ui->cutButton,      SIGNAL(clicked()), this, SLOT(cutButton()));
    connect(ui->copyButton,     SIGNAL(clicked()), this, SLOT(copyButton()));
    connect(ui->pasteButton,    SIGNAL(clicked()), this, SLOT(pasteButton()));
    connect(ui->muveUpButton,   SIGNAL(clicked()), this, SLOT(moveUpButton()));
    connect(ui->moveDownButton, SIGNAL(clicked()), this, SLOT(moveDownButton()));
}

SkillMenuDialog::~SkillMenuDialog()
{
    delete ui;
}

void SkillMenuDialog::showEvent(QShowEvent*) {
    QRect dlg = geometry();
    dlg.setTopLeft(_pos);
    setGeometry(dlg);

    ui->copyButton->setEnabled(_showCopy);
    ui->cutButton->setEnabled(_showCut);
    ui->deleteButton->setEnabled(_showDelete);
    ui->editButton->setEnabled(_showEdit);
    ui->moveDownButton->setEnabled(_showMoveDown);
    ui->muveUpButton->setEnabled(_showMoveUp);
    ui->pasteButton->setEnabled(_showPaste);
}

void SkillMenuDialog::newButton() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->newSkillTalentOrPerk->triggered();
}

void SkillMenuDialog::editButton() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->editSkillTalentOrPerk->triggered();
}

void SkillMenuDialog::deleteButton() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->deleteSkillTalentOrPerk->triggered();
}

void SkillMenuDialog::cutButton() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->cutSkillTalentOrPerk->triggered();
}

void SkillMenuDialog::copyButton() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->copySkillTalentOrPerk->triggered();
}

void SkillMenuDialog::pasteButton() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->pasteSkillTalentOrPerk->triggered();
}

void SkillMenuDialog::moveUpButton() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->moveSkillTalentOrPerkUp->triggered();
}

void SkillMenuDialog::moveDownButtons() {
    done(QDialog::Accepted);
    emit Sheet::ref().getUi()->moveSkillTalentOrPerkUp->triggered();
}
