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
    QFont font({ QString("Segoe UIHS") });
    setFont(font);

    ui->setupUi(this);

#ifdef unix
    setStyleSheet("color: #000; background: #fff;");
#endif

    connect(ui->totalPointsLineEdit,   SIGNAL(textChanged(QString)),     this, SLOT(numeric(QString)));
    connect(ui->complicationsLineEdit, SIGNAL(textChanged(QString)),     this, SLOT(numeric(QString)));
    connect(ui->activePointsPerEND,    SIGNAL(textChanged(QString)),     this, SLOT(numeric(QString)));
    connect(ui->comboBox,              SIGNAL(currentIndexChanged(int)), this, SLOT(pickSomething(int)));
    connect(this,                      SIGNAL(accepted()),               this, SLOT(accept()));

    auto rect = ui->banner->geometry();
    rect.setSize({ 293, 109 }); // NOLINT
    ui->banner->setGeometry(rect);
    ui->banner->setAlignment(Qt::AlignCenter);

    if (Sheet::ref().option().banner().isEmpty()) Sheet::ref().option().banner() = ":/gfx/HeroSystem-Banner.png";
    setBanner(Sheet::ref().option().banner());

    Sheet::ref().fixButtonBox(ui->buttonBox);
}

optionDialog::~optionDialog()
{
    delete ui;
}

QMap<int, int> table { // NOLINT
    {  25,  15 }, // NOLINT
    {  50,  25 }, // NOLINT
    { 100,  30 }, // NOLINT
    { 175,  50 }, // NOLINT
    { 225,  50 }, // NOLINT
    { 275,  50 }, // NOLINT
    { 300,  60 }, // NOLINT
    { 400,  75 }, // NOLINT
    { 500,  75 }, // NOLINT
    { 650, 100 }, // NOLINT
    { 750, 100 } // NOLINT
};

void optionDialog::accept() {
    Sheet::ref().option().banner(banner());
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

void optionDialog::mouseReleaseEvent(QMouseEvent* evt) {
    QRect rect = ui->banner->geometry();
    QPoint pos = evt->position().toPoint();
    if (!rect.contains(pos)) return;
    QStringList banners = Sheet::ref().getBanners();
    auto which = banners.indexOf(_banner) + 1;
    if (which == banners.length()) which = 0;
    setBanner(banners[which]);
}

void optionDialog::numeric(QString) {
    QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
    QString txt = edit->text();
    if (txt.isEmpty() || isNumber(txt)) {
        if (txt.isEmpty()) ui->comboBox->setCurrentIndex(table.count()); // NOLINT
        else {
            int cp = ui->totalPointsLineEdit->text().toInt();
            auto keys = table.keys();
            if (!keys.contains(cp)) ui->comboBox->setCurrentIndex(table.count()); // NOLINT
            else if (table[cp] != ui->complicationsLineEdit->text().toInt()) ui->comboBox->setCurrentIndex(table.count()); // NOLINT
            else ui->comboBox->setCurrentIndex(keys.indexOf(cp)); // NOLINT
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
