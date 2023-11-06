#ifdef __wasm__
#include <QShowEvent>

#include "filemenudialog.h"
#include "sheet.h"
#include "ui_filemenudialog.h"

FileMenuDialog::FileMenuDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileMenuDialog)
{
    ui->setupUi(this);

    connect(ui->newButton,  SIGNAL(clicked()), this, SLOT(doNew()));
    connect(ui->openButton, SIGNAL(clicked()), this, SLOT(doOpen()));
    connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(doSave()));
}

FileMenuDialog::~FileMenuDialog()
{
    delete ui;
}

void FileMenuDialog::setSave(bool enabled) {
    ui->saveButton->setEnabled(enabled);
}

void FileMenuDialog::showEvent(QShowEvent*) {
    QRect dialogPos = geometry();
    QRect buttonPos = ui->newButton->geometry();
    dialogPos.setLeft(0);
    dialogPos.setTop(buttonPos.height());
    setGeometry(dialogPos);
}

void FileMenuDialog::doNew() {
    done(QDialog::Accepted);
    emit Sheet::ref().action_New->triggered();
}

void FileMenuDialog::doOpen() {
    done(QDialog::Accepted);
    emit Sheet::ref().action_Open->triggered();
}

void FileMenuDialog::doSave() {
    done(QDialog::Accepted);
    emit Sheet::ref().action_Save->triggered();
}
#endif
