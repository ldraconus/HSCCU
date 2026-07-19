#include "complicationsdialog.h"
#include "ui_complicationsdialog.h"
#include "sheet.h"

#include <QPushButton>
#include <QSpacerItem>
#include <QVBoxLayout>

ComplicationsDialog::ComplicationsDialog(QWidget *parent) :
    Dialog(parent),
    ui(new Ui::ComplicationsDialog),
    mDescription(nullptr),
    mOk(nullptr),
    mPoints(nullptr) {
    QFont font({ QString("Segoe UIHS") });
    setFont(font);

    ui->setupUi(this);

#ifdef unix
    setStyleSheet("color: #000; background: #fff;");
#endif

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(pickComplication(int)));

    QList<QString> complications = Complication::Available();
    for (int i = 0; i < complications.count(); ++i) {
        auto& complication = complications[i];
        ui->comboBox->addItem(complication);
    }

    Sheet::ref().fixButtonBox(ui->buttonBox);
    mOk = ui->buttonBox->button(QDialogButtonBox::Ok); // NOLINT
    mOk->setEnabled(false);
}

ComplicationsDialog::~ComplicationsDialog() {
    delete ui;
}

ComplicationsDialog& ComplicationsDialog::complication(shared_ptr<Complication>& c) {
    QJsonObject obj = c->toJson();
    QString name = obj["name"].toString();
    if (name.isEmpty()) return *this;
    qsizetype idx = Complication::Available().indexOf(name);
    if (idx == -1) return *this;
    mSkipUpdate = true;
    ui->comboBox->setCurrentIndex(int(idx));
    mComplication = c;
    ui->comboBox->setEnabled(false);
    QVBoxLayout* layout = new QVBoxLayout(ui->form);
    try { mComplication->createForm(this, layout); } catch (...) { accept(); return *this; }
    createLabel(layout, "");
    mPoints      = createLabel(layout, "-1 Points");
    mDescription = createLabel(layout, "<incomplete>", WordWrap);

    layout->addStretch(1);
    mComplication->restore();
    updateForm();
    return *this;
}

QLabel* ComplicationsDialog::createLabel(QVBoxLayout* parent, QString text, bool wordWrap) {
    QLabel* label = new QLabel();
    if (label != nullptr) {
        label->setText(text);
#ifdef unix
        label->setStyleSheet("color: #000;");
#endif
        label->setWordWrap(wordWrap);
        parent->addWidget(label);
    }
    return label;
}

void ComplicationsDialog::pickComplication(int idx) {
    if (mSkipUpdate) {
        mSkipUpdate = false;
        return;
    }

    QVBoxLayout* layout = dynamic_cast<QVBoxLayout*>(ui->form->layout());
    if (layout == nullptr) {
        layout = new QVBoxLayout(ui->form);
        ui->form->setLayout(layout);
    }

    mComplication = Complication::ByIndex(idx);
    try { mComplication->createForm(this, layout); } catch (...) { accept(); }
    createLabel(layout, "");
    mPoints      = createLabel(layout, "-1 Points");
    mDescription = createLabel(layout, "<incomplete>", WordWrap);

    layout->addStretch(1);
    updateForm();
}

void ComplicationsDialog::stateChanged(int) {
    QCheckBox* checkBox = dynamic_cast<QCheckBox*>(sender());
    mComplication->callback(checkBox);
    updateForm();
}

void ComplicationsDialog::textChanged(QString) {
    QLineEdit* text = dynamic_cast<QLineEdit*>(sender());
    mComplication->callback(text);
    updateForm();
}

void ComplicationsDialog::updateForm() {
    mPoints->setText(QString("%1 points").arg(mComplication->points().points));
    mDescription->setText(mComplication->description());
    mOk->setEnabled(mComplication->description() != "<incomplete>");
}
