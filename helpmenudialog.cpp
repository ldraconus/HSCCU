#include "helpmenudialog.h"
#include "ui_helpmenudialog.h"

#include "sheet.h"

#include <QToolBUtton>

HelpMenuDialog::HelpMenuDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::HelpMenuDialog) {
    ui->setupUi(this);

    auto geom = geometry();
    geom.setSize(QSize(98, 24));
    setGeometry(geom);

    connect(ui->aboutPushButton,   SIGNAL(clicked()), this, SLOT(doAbout()));
}

HelpMenuDialog::~HelpMenuDialog() {
    delete ui;
}

void HelpMenuDialog::mousePressEvent(QMouseEvent* me) {
    QRect valid = geometry();
    if (!valid.contains(me->pos())) done(QDialog::Rejected);
}

void HelpMenuDialog::showEvent(QShowEvent*) {
    QRect dialogPos = geometry();
    QRect menuPos = Sheet::ref().helpButton->geometry();
    dialogPos.setLeft(menuPos.left());
    dialogPos.setTop(menuPos.height());
    setGeometry(dialogPos);
}

void HelpMenuDialog::doAbout() {
    done(QDialog::Accepted);
    emit Sheet::ref().actionAbout->triggered();
}
