#include "complicationsdialog.h"
#include "ui_complicationsdialog.h"

#include <QPushButton>
#include <QSpacerItem>
#include <QVBoxLayout>

ComplicationsDialog::ComplicationsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ComplicationsDialog)
{
    QFont font({ QString("Segoe UIHS") });
    setFont(font);

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

ComplicationsDialog& ComplicationsDialog::complication(shared_ptr<Complication>& c) {
    QJsonObject obj = c->toJson();
    QString name = obj["name"].toString();
    if (name.isEmpty()) return *this;
    int idx = Complication::Available().indexOf(name);
    if (idx == -1) return *this;
    _skipUpdate = true;
    ui->comboBox->setCurrentIndex(idx);
    _complication = c;
    ui->comboBox->setEnabled(false);
    auto* layout = new QVBoxLayout(ui->form);
    try { _complication->createForm(this, layout); } catch (...) { accept(); return *this; }
    createLabel(layout, "");
    _points      = createLabel(layout, "-1 Points");
    _description = createLabel(layout, "<incomplete>", WordWrap);

    layout->addStretch(1);
    _complication->restore();
    updateForm();
    return *this;
}

QLabel* ComplicationsDialog::createLabel(QVBoxLayout* parent, QString text, bool wordWrap) {
    QLabel* label = new QLabel();
    if (label != nullptr) {
        label->setText(text);
        label->setWordWrap(wordWrap);
        parent->addWidget(label);
    }
    return label;
}

void ComplicationsDialog::pickComplication(int idx) {
    if (_skipUpdate) {
        _skipUpdate = false;
        return;
    }

    QVBoxLayout* layout = static_cast<QVBoxLayout*>(ui->form->layout());
    if (layout == nullptr) {
        layout = new QVBoxLayout(ui->form);
        ui->form->setLayout(layout);
    }

    _complication = Complication::ByIndex(idx);
    try { _complication->createForm(this, layout); } catch (...) { accept(); }
    createLabel(layout, "");
    _points      = createLabel(layout, "-1 Points");
    _description = createLabel(layout, "<incomplete>", WordWrap);

    layout->addStretch(1);
    updateForm();
}

void ComplicationsDialog::stateChanged(int) {
    QCheckBox* checkBox = static_cast<QCheckBox*>(sender());
    _complication->callback(checkBox);
    updateForm();
}

void ComplicationsDialog::textChanged(QString) {
    QLineEdit* text = static_cast<QLineEdit*>(sender());
    _complication->callback(text);
    updateForm();
}

void ComplicationsDialog::updateForm() {
    _points->setText(QString("%1 points").arg(_complication->points().points));
    _description->setText(_complication->description());
    _ok->setEnabled(_complication->description() != "<incomplete>");
}
