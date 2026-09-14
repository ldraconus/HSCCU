#include "viewmenudialog.h"
#include "ui_viewmenudialog.h"
#include "sheet.h"

#include <QToolButton>

void ViewMenuDialog::check(QPushButton* btn) {
    btn->setText("☑" + btn->text().mid(1));
}

ViewMenuDialog::ViewMenuDialog(QWidget *parent)
    : QDialog(parent)
    , mUi(new Ui::ViewMenuDialog) {
    mUi->setupUi(this);

    auto geom = geometry();
    geom.setSize(QSize(80, 260));
    setGeometry(geom);

    switch(Sheet::ref().zoom()) {
    case 0: check(mUi->z0_5);  break;
    case 1: check(mUi->z0_75); break;
    case 2: check(mUi->z0_9);  break;
    case 3: check(mUi->z1_0);  break;
    case 4: check(mUi->z1_25); break;
    case 5: check(mUi->z1_5);  break;
    case 6: check(mUi->z2_0);  break;
    case 7: check(mUi->z3_0);  break;
    }

    connect(mUi->z0_5,     &QPushButton::clicked, this, [this](bool) { accept(); Sheet::ref().zoom(0.5); });
    connect(mUi->z0_75,    &QPushButton::clicked, this, [this](bool) { accept(); Sheet::ref().zoom(0.75); });
    connect(mUi->z0_9,     &QPushButton::clicked, this, [this](bool) { accept(); Sheet::ref().zoom(0.9); });
    connect(mUi->z1_0,     &QPushButton::clicked, this, [this](bool) { accept(); Sheet::ref().zoom(1.0); });
    connect(mUi->z1_25,    &QPushButton::clicked, this, [this](bool) { accept(); Sheet::ref().zoom(1.25); });
    connect(mUi->z1_5,     &QPushButton::clicked, this, [this](bool) { accept(); Sheet::ref().zoom(1.5); });
    connect(mUi->z2_0,     &QPushButton::clicked, this, [this](bool) { accept(); Sheet::ref().zoom(2.0); });
    connect(mUi->z3_0,     &QPushButton::clicked, this, [this](bool) { accept(); Sheet::ref().zoom(3.0); });
    connect(mUi->Zoom_In,  &QPushButton::clicked, this, [this](bool) { accept(); Sheet::ref().zoomIn(); });
    connect(mUi->Zoom_Out, &QPushButton::clicked, this, [this](bool) { accept(); Sheet::ref().zoomOut(); });
}

ViewMenuDialog::~ViewMenuDialog() {
    delete mUi;
}

void ViewMenuDialog::showEvent(QShowEvent *) {
    auto dlg = geometry();
    QRect btn = Sheet::ref().viewButton->geometry();
    mPos = QPoint(btn.left(), btn.height());
    dlg.setTopLeft(mPos.toPoint());
    setGeometry(dlg);
}
