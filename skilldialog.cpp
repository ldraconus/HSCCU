#include "skilldialog.h"
#include "ui_skilldialog.h"

SkillDialog::SkillDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SkillDialog)
{
    ui->setupUi(this);
}

SkillDialog::~SkillDialog()
{
    delete ui;
}
