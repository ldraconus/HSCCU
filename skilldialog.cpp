#include "skilldialog.h"
#include "ui_skilldialog.h"

#include <QVBoxLayout>

SkillDialog::SkillDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SkillDialog)
{
    ui->setupUi(this);

    connect(ui->skillTalentOrPerkComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(pickType(int)));
    connect(ui->availableComboBox,         SIGNAL(currentIndexChanged(int)), this, SLOT(pickOne(int)));

    _ok = ui->buttonBox->button(QDialogButtonBox::Ok);
    _ok->setEnabled(false);
}

SkillDialog::~SkillDialog()
{
    delete ui;
}

QLabel* SkillDialog::createLabel(QVBoxLayout* parent, QString text, bool wordWrap = false) {
    QLabel* label = new QLabel();
    if (label != nullptr) {
        label->setText(text);
        label->setWordWrap(wordWrap);
        parent->addWidget(label);
    }
    return label;
}

void SkillDialog::pickOne(int) {
    if (_skipUpdate) {
        _skipUpdate = false;
        return;
    }

    QVBoxLayout* layout = static_cast<QVBoxLayout*>(ui->form->layout());
    if (layout == nullptr) {
        layout = new QVBoxLayout(ui->form);
        ui->form->setLayout(layout);
    }

    if (_skilltalentorperk) delete _skilltalentorperk;

    _skilltalentorperk = SkillTalentOrPerk::ByName(ui->availableComboBox->currentText());
    try { _skilltalentorperk->createForm(this, layout); } catch (...) { accept(); }

    createLabel(layout, "");
    _points      = createLabel(layout, "-1 Points");
    _description = createLabel(layout, "<incomplete>", WordWrap);

    layout->addStretch(1);
    updateForm();
}

void SkillDialog::pickType(int type) {
    QList<QString> available;
    switch (type) {
    case 0:  available = SkillTalentOrPerk::SkillsAvailable();  break;
    case 1:  available = SkillTalentOrPerk::TalentsAvailable(); break;
    default: available = SkillTalentOrPerk::PerksAvailable();   break;
    }

    for (const auto& skilltalentorperk: available) ui->availableComboBox->addItem(skilltalentorperk);
    ui->availableComboBox->setEnabled(true);

    QVBoxLayout* layout = static_cast<QVBoxLayout*>(ui->form->layout());
    if (layout == nullptr) {
        layout = new QVBoxLayout(ui->form);
        ui->form->setLayout(layout);
    }

    if (_skilltalentorperk) delete _skilltalentorperk;
    _skilltalentorperk = nullptr;

    SkillTalentOrPerk::ClearForm(layout);
}

void SkillDialog::stateChanged(int) {
    QCheckBox* checkBox = static_cast<QCheckBox*>(sender());
    _skilltalentorperk->callback(checkBox);
    updateForm();
}

void SkillDialog::textChanged(QString) {
    QLineEdit* text = static_cast<QLineEdit*>(sender());
    _skilltalentorperk->callback(text);
    updateForm();
}

void SkillDialog::updateForm() {
    _points->setText(QString("%1 points").arg(_skilltalentorperk->points()));
    _description->setText(_skilltalentorperk->description(SkillTalentOrPerk::ShowRoll));
    _ok->setEnabled(_skilltalentorperk->description() != "<incomplete>");
}
