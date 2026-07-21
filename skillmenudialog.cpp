#include "skillmenudialog.h"
#include "ui_skillmenudialog.h"
#include "sheet.h"
#include "sheet_ui.h"

#include <QToolButton>

SkillMenuDialog::SkillMenuDialog(QWidget *parent) :
    QDialog(parent),
    mUi(new Ui::SkillMenuDialog) {
    mUi->setupUi(this);

    connect(mUi->newButton,      SIGNAL(clicked()), this, SLOT(newButton()));
    connect(mUi->editButton,     SIGNAL(clicked()), this, SLOT(editButton()));
    connect(mUi->deleteButton,   SIGNAL(clicked()), this, SLOT(deleteButton()));
    connect(mUi->cutButton,      SIGNAL(clicked()), this, SLOT(cutButton()));
    connect(mUi->copyButton,     SIGNAL(clicked()), this, SLOT(copyButton()));
    connect(mUi->pasteButton,    SIGNAL(clicked()), this, SLOT(pasteButton()));
    connect(mUi->muveUpButton,   SIGNAL(clicked()), this, SLOT(moveUpButton()));
    connect(mUi->moveDownButton, SIGNAL(clicked()), this, SLOT(moveDownButton()));
}

SkillMenuDialog::~SkillMenuDialog() {
    delete mUi;
}

void SkillMenuDialog::showEvent(QShowEvent*) {
    QRect dialogPos = geometry();
    QRect menuPos = Sheet::ref().skillsTalentsAndPerksButton->geometry();
    dialogPos.setLeft(menuPos.left());
    dialogPos.setTop(menuPos.height());
    setGeometry(dialogPos);

    mUi->copyButton->setEnabled(mShowCopy);
    mUi->cutButton->setEnabled(mShowCut);
    mUi->deleteButton->setEnabled(mShowDelete);
    mUi->editButton->setEnabled(mShowEdit);
    mUi->moveDownButton->setEnabled(mShowMoveDown);
    mUi->muveUpButton->setEnabled(mShowMoveUp);
    mUi->pasteButton->setEnabled(mShowPaste);
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
