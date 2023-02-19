#include "optiondialog.h"
#include "ui_optiondialog.h"

optionDialog::optionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::optionDialog)
{
    ui->setupUi(this);

    QFont font({ QString("Segoe UIHS") });
    setFont(font);

    connect(ui->totalPointsLineEdit,   SIGNAL(textChanged(QString)), this, SLOT(numeric(QString)));
    connect(ui->complicationsLineEdit, SIGNAL(textChanged(QString)), this, SLOT(numeric(QString)));
}

optionDialog::~optionDialog()
{
    delete ui;
}

void optionDialog::numeric(QString) {
    QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
    QString txt = edit->text();
    if (txt.isEmpty() || isNumber(txt)) return;
    edit->undo();
}
