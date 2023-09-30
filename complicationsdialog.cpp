#include "complicationsdialog.h"
#include "ui_complicationsdialog.h"

#include <QPushButton>
#include <QSpacerItem>
#include <QVBoxLayout>

ComplicationsDialog::ComplicationsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ComplicationsDialog),
    _description(nullptr),
    _ok(nullptr),
    _points(nullptr)
{
    QFont font({ QString("Segoe UIHS") });
    setFont(font);

    ui->setupUi(this);

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(pickComplication(int)));

    QList<QString> complications = Complication::Available();
    for (const auto& complication: complications) ui->comboBox->addItem(complication);

    _ok = ui->buttonBox->button(QDialogButtonBox::Ok); // NOLINT
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
    qsizetype idx = Complication::Available().indexOf(name);
    if (idx == -1) return *this;
    _skipUpdate = true;
    ui->comboBox->setCurrentIndex(gsl::narrow<int>(idx));
    _complication = c;
    ui->comboBox->setEnabled(false);
    gsl::owner<QVBoxLayout*> layout = new QVBoxLayout(ui->form);
    try { _complication->createForm(this, layout); } catch (...) { accept(); return *this; }
    createLabel(layout, "");
    _points      = createLabel(layout, "-1 Points");
    _description = createLabel(layout, "<incomplete>", WordWrap);

    layout->addStretch(1);
    _complication->restore();
    updateForm();
    return *this;
}

gsl::owner<QLabel*> ComplicationsDialog::createLabel(QVBoxLayout* parent, QString text, bool wordWrap) {
    gsl::owner<QLabel*> label = new QLabel();
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

    gsl::owner<QVBoxLayout*> layout = dynamic_cast<gsl::owner<QVBoxLayout*>>(ui->form->layout());
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
    QCheckBox* checkBox = dynamic_cast<QCheckBox*>(sender());
    _complication->callback(checkBox);
    updateForm();
}

void ComplicationsDialog::textChanged(QString) {
    QLineEdit* text = dynamic_cast<QLineEdit*>(sender());
    _complication->callback(text);
    updateForm();
}

void ComplicationsDialog::updateForm() {
    _points->setText(QString("%1 points").arg(_complication->points().points));
    _description->setText(_complication->description());
    _ok->setEnabled(_complication->description() != "<incomplete>");
}
