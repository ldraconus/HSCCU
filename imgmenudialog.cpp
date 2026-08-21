#include <QToolButton>

#include "imgmenudialog.h"
#include "ui_imgmenudialog.h"
#include "sheet.h"

ImgMenuDialog::ImgMenuDialog(QWidget *parent) :
    QDialog(parent),
    mUi(new Ui::ImgMenuDialog) {
    mUi->setupUi(this);

    connect(mUi->clearImageButton, SIGNAL(clicked()), this, SLOT(clearImage()));
    connect(mUi->loadImageButton,  SIGNAL(clicked()), this, SLOT(newImage()));
}

ImgMenuDialog::~ImgMenuDialog() {
    delete mUi;
}

void ImgMenuDialog::showEvent(QShowEvent*) {
    QRect dlg = geometry();
#ifdef __wasm__
    QRect btn = Sheet::ref().imageButton->geometry();
    if (mPos == QPoint()) mPos = mapToGlobal(QPoint(btn.left(), btn.height()));
#endif
    dlg.moveTopLeft(mPos.toPoint());
    dlg.setWidth(80);
    dlg.setHeight(48);
    setGeometry(dlg);
}

void ImgMenuDialog::newImage() {
    done(QDialog::Accepted);
    Sheet::ref().newImage();
}

void ImgMenuDialog::clearImage() {
    done(QDialog::Accepted);
    Sheet::ref().clearImage();
}
