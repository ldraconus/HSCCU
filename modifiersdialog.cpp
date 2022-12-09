#include "modifiersdialog.h"
#include "ui_modifiersdialog.h"

ModifiersDialog::ModifiersDialog(bool advantage, QString type, QString which, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ModifiersDialog)
{
    ui->setupUi(this);
}

ModifiersDialog::~ModifiersDialog()
{
    delete ui;
}
