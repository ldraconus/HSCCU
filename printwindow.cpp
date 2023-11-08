#include "printwindow.h"
#include "ui_printwindow.h"

PrintWindow::PrintWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PrintWindow)
{
    ui->setupUi(this);
}

PrintWindow::~PrintWindow()
{
    delete ui;
}
