#include "optiondialog.h"
#include "ui_optiondialog.h"
#include "sheet.h"
#ifdef __wasm__
#include "ui_wasm.h"
#else
#include "ui_sheet.h"
#endif

optionDialog::optionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::optionDialog)
{
    ui->setupUi(this);

    QFont font({ QString("Segoe UIHS") });
    setFont(font);

    connect(ui->totalPointsLineEdit,   SIGNAL(textChanged(QString)),     this, SLOT(numeric(QString)));
    connect(ui->complicationsLineEdit, SIGNAL(textChanged(QString)),     this, SLOT(numeric(QString)));
    connect(ui->activePointsPerEND,    SIGNAL(textChanged(QString)),     this, SLOT(numeric(QString)));
    connect(ui->comboBox,              SIGNAL(currentIndexChanged(int)), this, SLOT(pickSomething(int)));
    connect(this,                      SIGNAL(accepted()),               this, SLOT(accept()));
}

optionDialog::~optionDialog()
{
    delete ui;
}

QMap<int, int> table {
    {  25,  15 },
    {  50,  25 },
    { 100,  30 },
    { 175,  50 },
    { 225,  50 },
    { 275,  50 },
    { 300,  60 },
    { 400,  75 },
    { 500,  75 },
    { 650, 100 },
    { 750, 100 }
};

void optionDialog::accept() {
    Sheet::ref().option().complications(Points(complications()));
    Sheet::ref().option().equipmentFree(equipmentFree());
    Sheet::ref().option().showFrequencyRolls(showFrequencyRolls());
    Sheet::ref().option().showNotesPage(showNotesPage());
    Sheet::ref().option().activePerEND(Points(activePointsPerEND()));
    Sheet::ref().option().totalPoints(Points(totalPoints()));
    Sheet::ref().option().normalHumanMaxima(normalHumanMaxima());
    Sheet::ref().option().store();
    Sheet::ref().UI()->optLabel->setVisible(Sheet::ref().option().showNotesPage());
    Sheet::ref().updateDisplay();
    Sheet::ref().changed();
    close();
}

void optionDialog::numeric(QString) {
    QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
    QString txt = edit->text();
    if (txt.isEmpty() || isNumber(txt)) {
        if (txt.isEmpty()) ui->comboBox->setCurrentIndex(table.count());
        else {
            int cp = ui->totalPointsLineEdit->text().toInt();
            auto keys = table.keys();
            if (!keys.contains(cp)) ui->comboBox->setCurrentIndex(table.count());
            else if (table[cp] != ui->complicationsLineEdit->text().toInt()) ui->comboBox->setCurrentIndex(table.count());
            else ui->comboBox->setCurrentIndex(keys.indexOf(cp));
        }
        return;
    }
    edit->undo();
}

void optionDialog::pickSomething(int something) {
    if (something >= table.count()) return;
    auto keys = table.keys();
    ui->totalPointsLineEdit->setText(QString("%1").arg(keys[something]));
    ui->complicationsLineEdit->setText(QString("%1").arg(table[keys[something]]));
}
