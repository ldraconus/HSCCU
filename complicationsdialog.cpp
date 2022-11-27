#include "complicationsdialog.h"
#include "ui_complicationsdialog.h"

#include <QPushButton>
#include <QSpacerItem>
#include <QVBoxLayout>

ComplicationsDialog::ComplicationsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ComplicationsDialog)
{
    ui->setupUi(this);

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(pickComplication(int)));

    QList<QString> complications = Complication::Available();
    for (const auto& complication: complications) ui->comboBox->addItem(complication);

    _ok = ui->buttonBox->button(QDialogButtonBox::Ok);
    _ok->setEnabled(false);
}

ComplicationsDialog::~ComplicationsDialog()
{
    delete ui;
}

QLabel* ComplicationsDialog::createLabel(QVBoxLayout* parent, QString text, bool wordWrap) {
    QLabel* label = new QLabel();
    label->setText(text);
    label->setWordWrap(wordWrap);
    parent->addWidget(label);
    return label;
}

void ComplicationsDialog::pickComplication(int idx) {
    auto* layout = new QVBoxLayout(ui->form);
    ui->form->setLayout(layout);

    _complication = Complication::ByIndex(idx);
    _complication->createForm(this, layout);
    createLabel(layout, "");
    _points      = createLabel(layout, "-1 Points");
    _description = createLabel(layout, "<incomplete>", WordWrap);

    layout->addStretch(1);
    updateForm();
}

void ComplicationsDialog::stateChanged(int) {
    QCheckBox* checkBox = static_cast<QCheckBox*>(sender());
    _complication->callback(checkBox);
}

void ComplicationsDialog::updateForm() {
    _points->setText(QString("%1 points").arg(_complication->points()));
    _description->setText(_complication->description());
    _ok->setEnabled(_complication->description() != "<incomplete>");
}
