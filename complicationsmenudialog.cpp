#include "complicationsmenudialog.h"
#include "ui_complicationsmenudialog.h"
#include "sheet.h"
#include "sheet_ui.h"

#include <QToolButton>

ComplicationsMenuDialog::ComplicationsMenuDialog(QWidget *parent) :
    QDialog(parent),
    mUi(new Ui::ComplicationsMenuDialog) {
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

ComplicationsMenuDialog::~ComplicationsMenuDialog() {
    delete mUi;
}

void ComplicationsMenuDialog::showEvent(QShowEvent*) {
    QRect dlg = geometry();
    QRect btn = Sheet::ref().complicationsButton->geometry();
    QSize sz = dlg.size();
    dlg.setTopLeft(QPoint(btn.left(), btn.height()));
    dlg.setSize(sz);
    setGeometry(dlg);

    mUi->copyButton->setEnabled(mShowCopy);
    mUi->cutButton->setEnabled(mShowCut);
    mUi->deleteButton->setEnabled(mShowDelete);
    mUi->editButton->setEnabled(mShowEdit);
    mUi->moveDownButton->setEnabled(mShowMoveDown);
    mUi->moveUpButton->setEnabled(mShowMoveUp);
    mUi->pasteButton->setEnabled(mShowPaste);
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
