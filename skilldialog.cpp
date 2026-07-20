#include "skilldialog.h"
#include "ui_skilldialog.h"

#include "sheet.h"

#include <QVBoxLayout>

SkillDialog::SkillDialog(QWidget *parent) :
    Dialog(parent),
    ui(new Ui::SkillDialog) {
    QFont font({ QString("Segoe UIHS") });
    setFont(font);

    ui->setupUi(this);

#ifdef unix
    setStyleSheet("color: #000; background: #fff;");
#endif

    connect(ui->skillTalentOrPerkComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(pickType(int)));
    connect(ui->availableComboBox,         SIGNAL(currentIndexChanged(int)), this, SLOT(pickOne(int)));

    Sheet::ref().fixButtonBox(ui->buttonBox);

    mOk = ui->buttonBox->button(QDialogButtonBox::Ok);
    mOk->setEnabled(false);
}

SkillDialog::~SkillDialog() {
    delete ui;
}

QLabel* SkillDialog::createLabel(QVBoxLayout* parent, QString text, bool wordWrap = false) {
    QLabel* label = new QLabel();
    if (label != nullptr) {
        label->setText(text);
        label->setStyleSheet("QLabel { color: #000; background: #fff }");
        label->setWordWrap(wordWrap);
        parent->addWidget(label);
    }
    return label;
}

void SkillDialog::pickOne(int) {
    if (mSkipUpdate) {
        mSkipUpdate = false;
        return;
    }

    QVBoxLayout* layout = static_cast<QVBoxLayout*>(ui->form->layout());
    if (layout == nullptr) {
        layout = new QVBoxLayout(ui->form);
        ui->form->setLayout(layout);
    }

    mSkillTalentOrPerk = SkillTalentOrPerk::ByName(ui->availableComboBox->currentText());
    if (!mSkillTalentOrPerk->createForm(this, layout)) accept();

    createLabel(layout, "");
    mPoints      = createLabel(layout, "-1 Points");
    mDescription = createLabel(layout, "<incomplete>", WordWrap);

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
    for (const auto& skilltalentorperk: std::as_const(available)) ui->availableComboBox->addItem(skilltalentorperk);
    ui->availableComboBox->setEnabled(true);

    if (mSkipUpdate) {
        mSkipUpdate = false;
        return;
    }

    QVBoxLayout* layout = static_cast<QVBoxLayout*>(ui->form->layout());
    if (layout == nullptr) {
        layout = new QVBoxLayout(ui->form);
        ui->form->setLayout(layout);
    }

    mSkillTalentOrPerk = nullptr;

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

    mSkipUpdate = true;
    ui->skillTalentOrPerkComboBox->setCurrentIndex(type);
    ui->skillTalentOrPerkComboBox->setEnabled(false);
    mSkipUpdate = true;
    ui->availableComboBox->setCurrentIndex(idx);
    ui->availableComboBox->setEnabled(false);

    QVBoxLayout* layout = static_cast<QVBoxLayout*>(ui->form->layout());
    if (layout == nullptr) {
        layout = new QVBoxLayout(ui->form);
        ui->form->setLayout(layout);
    }

    mSkillTalentOrPerk = s;
    try { mSkillTalentOrPerk->createForm(this, layout); } catch (...) { accept(); return *this; }

    createLabel(layout, "");
    mPoints      = createLabel(layout, "-1 Points");
    mDescription = createLabel(layout, "<incomplete>", WordWrap);

    layout->addStretch(1);
    mSkillTalentOrPerk = s;
    mSkillTalentOrPerk->restore();
    updateForm();
    return *this;
}

void SkillDialog::stateChanged(bool) {
    QCheckBox* checkBox = static_cast<QCheckBox*>(sender());
    mSkillTalentOrPerk->callback(checkBox);
    updateForm();
}

void SkillDialog::textChanged(QString) {
    QLineEdit* text = static_cast<QLineEdit*>(sender());
    mSkillTalentOrPerk->callback(text);
    updateForm();
}

void SkillDialog::updateForm() {
    mPoints->setText(QString("%1 points").arg(mSkillTalentOrPerk->points().points));
    mDescription->setText(mSkillTalentOrPerk->description(SkillTalentOrPerk::ShowRoll));
    mOk->setEnabled(mSkillTalentOrPerk->description() != "<incomplete>");
}
