#ifdef __wasm__
#include <QClipboard>
#include <QMimeData>

#include "editmenudialog.h"
#include "sheet.h"
#include "ui_editmenudialog.h"

EditMenuDialog::EditMenuDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditMenuDialog)
{
    ui->setupUi(this);
    connect(ui->CopyButton,   SIGNAL(clicked()), this, SLOT(doCopy()));
    connect(ui->CutButton,    SIGNAL(clicked()), this, SLOT(doCut()));
    connect(ui->OptionButton, SIGNAL(clicked()), this, SLOT(doOption()));
    connect(ui->PasteButton,  SIGNAL(clicked()), this, SLOT(doPaste()));
}

void EditMenuDialog::mousePressEvent(QMouseEvent* me) {
    QRect valid = geometry();
    if (!valid.contains(me->pos())) done(QDialog::Rejected);
}

void EditMenuDialog::showEvent(QShowEvent*) {
    QRect dialogPos = geometry();
    dialogPos.setLeft(ui->CopyButton->geometry().width() / 2 + 10);
    dialogPos.setTop(ui->CopyButton->geometry().height());
    setGeometry(dialogPos);
}

EditMenuDialog::~EditMenuDialog()
{
    delete ui;
}

void EditMenuDialog::doCopy() {
    done(QDialog::Accepted);
    emit Sheet::ref().actionC_opy->triggered();
//    Sheet::ref().copyCharacter();
}

void EditMenuDialog::doCut() {
    done(QDialog::Accepted);
    emit Sheet::ref().action_Cut->triggered();
//    Sheet::ref().cutCharacter();
}

void EditMenuDialog::doOption() {
    done(QDialog::Accepted);
    emit Sheet::ref().actionOptions->triggered();
//    Sheet::ref().options();
}

void EditMenuDialog::doPaste() {
    done(QDialog::Accepted);
    emit Sheet::ref().action_Paste->triggered();
//    Sheet::ref().pasteCharacter();
}
#endif
