#include "skilldialog.h"
#include "ui_skilldialog.h"

#include <QVBoxLayout>

SkillDialog::SkillDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SkillDialog)
{
    QFont font({ QString("Segoe UIHS") });
    setFont(font);

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
    case 0: available = SkillTalentOrPerk::SkillsAvailable();  break;
    case 1: available = SkillTalentOrPerk::TalentsAvailable(); break;
    case 2: available = SkillTalentOrPerk::PerksAvailable();   break;
    case 3: available = { "Blank Line",
                          "Jack of All Trades",
                          "Linguist",
                          "Scholar",
                          "Scientist",
                          "Traveller",
                          "Well-Connected"
                        };                                     break;
    }

    ui->availableComboBox->clear();
    for (const auto& skilltalentorperk: available) ui->availableComboBox->addItem(skilltalentorperk);
    ui->availableComboBox->setEnabled(true);

    if (_skipUpdate) {
        _skipUpdate = false;
        return;
    }

    QVBoxLayout* layout = static_cast<QVBoxLayout*>(ui->form->layout());
    if (layout == nullptr) {
        layout = new QVBoxLayout(ui->form);
        ui->form->setLayout(layout);
    }

    _skilltalentorperk = nullptr;

    SkillTalentOrPerk::ClearForm(layout);
}

SkillDialog& SkillDialog::skilltalentorperk(shared_ptr<SkillTalentOrPerk> s) {
    QJsonObject obj = s->toJson();
    QString name = obj["name"].toString();
    if (name.isEmpty()) return *this;
    int idx = SkillTalentOrPerk::Available().indexOf(name);
    if (idx == -1) return *this;

    int type = -1;
    if ((idx = SkillTalentOrPerk::SkillsAvailable().indexOf(name)) != -1) type = 0;
    else if ((idx = SkillTalentOrPerk::TalentsAvailable().indexOf(name)) != -1) type = 1;
    else if ((idx = SkillTalentOrPerk::PerksAvailable().indexOf(name)) != -1) type = 2;
    else if ((idx = QStringList({ "Blank Line", "Jack of All Trades", "Linguist", "Scholar", "Scientist", "Traveller", "Well-Connected" }).indexOf(name)) == -1) type = 3;
    else return *this;

    _skipUpdate = true;
    ui->skillTalentOrPerkComboBox->setCurrentIndex(type);
    ui->skillTalentOrPerkComboBox->setEnabled(false);
    _skipUpdate = true;
    ui->availableComboBox->setCurrentIndex(idx);
    ui->availableComboBox->setEnabled(false);

    QVBoxLayout* layout = static_cast<QVBoxLayout*>(ui->form->layout());
    if (layout == nullptr) {
        layout = new QVBoxLayout(ui->form);
        ui->form->setLayout(layout);
    }

    _skilltalentorperk = s;
    try { _skilltalentorperk->createForm(this, layout); } catch (...) { accept(); return *this; }

    createLabel(layout, "");
    _points      = createLabel(layout, "-1 Points");
    _description = createLabel(layout, "<incomplete>", WordWrap);

    layout->addStretch(1);
    _skilltalentorperk = s;
    _skilltalentorperk->restore();
    updateForm();
    return *this;
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
    _points->setText(QString("%1 points").arg(_skilltalentorperk->points().points));
    _description->setText(_skilltalentorperk->description(SkillTalentOrPerk::ShowRoll));
    _ok->setEnabled(_skilltalentorperk->description() != "<incomplete>");
}
