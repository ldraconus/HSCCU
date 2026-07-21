#ifdef __wasm__

#include <QToolButton>

#include "imgmenudialog.h"
#include "ui_imgmenudialog.h"
#include "sheet.h"

ImgMenuDialog::ImgMenuDialog(QWidget *parent) :
    QDialog(parent),
    mUi(new Ui::ImgMenuDialog)
{
    mUi->setupUi(this);

    connect(mUi->clearImageButton, SIGNAL(clicked()), this, SLOT(clearImage()));
    connect(mUi->loadImageButton,  SIGNAL(clicked()), this, SLOT(newImage()));
}

ImgMenuDialog::~ImgMenuDialog() {
    delete mUi;
}

void ImgMenuDialog::showEvent(QShowEvent*) {
    QRect dialogPos = geometry();
    QRect menuPos = Sheet::ref().imageButton->geometry();
    dialogPos.setLeft(menuPos.left());
    dialogPos.setTop(menuPos.height());
    dialogPos.setWidth(80);
    dialogPos.setHeight(48);
    setGeometry(dialogPos);
}

void ImgMenuDialog::newImage() {
    done(QDialog::Accepted);
    Sheet::ref().newImage();
}

void ImgMenuDialog::clearImage() {
    done(QDialog::Accepted);
    Sheet::ref().clearImage();
}
#endif
