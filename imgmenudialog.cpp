#ifdef __wasm__
#include "imgmenudialog.h"
#include "ui_imgmenudialog.h"
#include "sheet.h"

ImgMenuDialog::ImgMenuDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImgMenuDialog)
{
    ui->setupUi(this);

    connect(ui->clearImageButton, SIGNAL(clicked()), this, SLOT(clearImage()));
    connect(ui->loadImageButton,  SIGNAL(clicked()), this, SLOT(newImage()));
}

ImgMenuDialog::~ImgMenuDialog()
{
    delete ui;
}

void ImgMenuDialog::showEvent(QShowEvent*) {
    QRect dlg = geometry();
    dlg.setTopLeft(_pos);
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
#endif
