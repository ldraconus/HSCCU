#include "viewmenudialog.h"
#include "ui_viewmenudialog.h"
#include "sheet.h"

ViewMenuDialog::ViewMenuDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ViewMenuDialog) {
    ui->setupUi(this);
    connect(ui->z0_5,     &QPushButton::clicked, this, [this](bool) { accepted(); emit Sheet::ref().action0_5; });
    connect(ui->z0_75,    &QPushButton::clicked, this, [this](bool) { accepted(); emit Sheet::ref().action0_5(); });
    connect(ui->z0_9,     &QPushButton::clicked, this, [this](bool) { accepted(); emit Sheet::ref().action0_5(); });
    connect(ui->z1_0,     &QPushButton::clicked, this, [this](bool) { accepted(); emit Sheet::ref().action0_5(); });
    connect(ui->z1_25,    &QPushButton::clicked, this, [this](bool) { accepted(); emit Sheet::ref().action0_5(); });
    connect(ui->z1_5,     &QPushButton::clicked, this, [this](bool) { accepted(); emit Sheet::ref().action0_5(); });
    connect(ui->z2_0,     &QPushButton::clicked, this, [this](bool) { accepted(); emit Sheet::ref().action0_5(); });
    connect(ui->z3_0,     &QPushButton::clicked, this, [this](bool) { accepted(); emit Sheet::ref().action0_5(); });
    connect(ui->Zoom_In,  &QPushButton::clicked, this, [this](bool) { accepted(); emit Sheet::ref().actionZoom_In();});
    connect(ui->Zoom_Out, &QPushButton::clicked, this, [this](bool) { accepted(); emit Sheet::ref().actionZoom_Out();});
}

ViewMenuDialog::~ViewMenuDialog() {
    delete ui;
}
