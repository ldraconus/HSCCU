#include "sheet.h"
#include "ui_sheet.h"

Sheet::Sheet(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Sheet)
{
    ui->setupUi(this);
}

Sheet::~Sheet()
{
    delete ui;
}

