#include "viewmenudialog.h"
#include "ui_viewmenudialog.h"
#include "sheet.h"

ViewMenuDialog::ViewMenuDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ViewMenuDialog) {
    ui->setupUi(this);
    connect(ui->z0_5, &QPushButton::clicked, this, [this](bool) { accepted(); emit Sheet::ref().action0_5();});
    connect(ui->z0_5, &QPushButton::clicked, this, [this](bool) { accepted(); emit Sheet::ref().action0_5();});
    connect(ui->z0_5, &QPushButton::clicked, this, [this](bool) { accepted(); emit Sheet::ref().action0_5();});
    connect(ui->z0_5, &QPushButton::clicked, this, [this](bool) { accepted(); emit Sheet::ref().action0_5();});
    connect(ui->z0_5, &QPushButton::clicked, this, [this](bool) { accepted(); emit Sheet::ref().action0_5();});
    connect(ui->z0_5, &QPushButton::clicked, this, [this](bool) { accepted(); emit Sheet::ref().action0_5();});
    connect(ui->z0_5, &QPushButton::clicked, this, [this](bool) { accepted(); emit Sheet::ref().action0_5();});
    connect(ui->z0_5, &QPushButton::clicked, this, [this](bool) { accepted(); emit Sheet::ref().action0_5();});
    connect(ui->z0_5, &QPushButton::clicked, this, [this](bool) { accepted(); emit Sheet::ref().actionZoom_In();});
    connect(ui->z0_5, &QPushButton::clicked, this, [this](bool) { accepted(); emit Sheet::ref().actionZoom_Out();});
}

ViewMenuDialog::~ViewMenuDialog() {
    delete ui;
}
