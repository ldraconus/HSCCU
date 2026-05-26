#include "modifiersdialog.h"
#include "ui_modifiersdialog.h"
#include "modifier.h"
#include "powerdialog.h"
#include "shared.h"
#include "sheet.h"

ModifiersDialog* ModifiersDialog::mModifiersDialog = nullptr; // NOLINT

ModifiersDialog::ModifiersDialog(bool advantage, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ModifiersDialog) {
    QFont font({ QString("Segoe UIHS") });
    setFont(font);

    ui->setupUi(this);

#ifdef unix
    setStyleSheet("color: #000; background: #fff;");
#endif

    mModifiersDialog = this;

    mModifiers = make_shared<Modifiers>();

    setModifiers(advantage);
    ui->form->setWindowTitle(advantage ? "Pick Advantage" : "Pick Limitation");
    ui->comboBox->setPlaceholderText(advantage ? "Pick an Advantage" : "Pick a Limitation");

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(pickOne(int)));
    connect(this,         SIGNAL(accepted()),               this, SLOT(doAccepted()));

    Sheet::ref().fixButtonBox(ui->buttonBox);
    mOk = ui->buttonBox->button(QDialogButtonBox::Ok); // NOLINT
    mOk->setEnabled(false);

}

ModifiersDialog::~ModifiersDialog() {
    delete ui;
}

gsl::owner<QLabel*> ModifiersDialog::createLabel(QVBoxLayout* parent, QString text, bool wordWrap) {
    gsl::owner<QLabel*> label = new QLabel();
    if (label != nullptr) {
        label->setText(text);
        label->setWordWrap(wordWrap);
        parent->addWidget(label);
    }
    return label;
}

void ModifiersDialog::doAccepted() {
    if (mAdd) {
        if (mModifier->type() == ModifierBase::isAdvantage ||
            (mModifier->type() == ModifierBase::isBoth && mModifier->fraction(Modifier::NoStore) > 0)) {
            PowerDialog::ref().powerorequipment()->advantagesList().append(mModifier);
        } else PowerDialog::ref().powerorequipment()->limitationsList().append(mModifier);
    }
    PowerDialog::ref().updateForm();
}

bool ModifiersDialog::modifier(shared_ptr<Modifier>& mod) {
    QJsonObject obj = mod->toJson();

    bool advantage = false;
    if (mod->type() == ModifierBase::isAdvantage) advantage = true;
    else if (mod->type() == ModifierBase::isLimitation) advantage = false;
    else if (mod->fraction(Modifier::NoStore) < 0) advantage = false;
    else advantage = true;

    mSkipUpdate = true;
    setModifiers(advantage);
    ui->form->setWindowTitle(advantage ? "Edit Advantage" : "Edit Limitation");
    int idx = ui->comboBox->findText(obj["name"].toString());
    if (idx == -1) return false;

    ui->comboBox->setCurrentIndex(idx);
    ui->comboBox->setEnabled(false);

    QVBoxLayout* layout = static_cast<QVBoxLayout*>(ui->form->layout()); // NOLINT
    if (layout == nullptr) {
        layout = new QVBoxLayout(ui->form); // NOLINT
        ui->form->setLayout(layout);
    }

    mAdd = false;
    mModifier = mod;
    /* This would be the simple way to handle this, but it does not work because the dialog is not up at this time.
     * Going to try and put it in an event or something, try something that will be hit when the dialog is up
    try { _modifier->createForm(this, layout); } catch (...) { accept(); }
    * timers don't work with WASM, so try the show event handler instead ... */
    if (mModifier->createForm(this, layout) == false) return false;

    createLabel(layout, "");
    mDescription = createLabel(layout, "<incomplete>", true);

    layout->addStretch(1);
    mModifier->restore();
    updateForm();
    return true;
}

void ModifiersDialog::setModifiers(bool advantage) {
    QList<shared_ptr<Modifier>> modifiers = (*mModifiers)(advantage ? Modifier::isAdvantage : Modifier::isLimitation);
    ui->comboBox->clear();
    for (const auto& modifier: modifiers) ui->comboBox->addItem(modifier->name());
}

void ModifiersDialog::pickOne(int) {
    if (mSkipUpdate) {
        mSkipUpdate = false;
        return;
    }

    QVBoxLayout* layout = static_cast<QVBoxLayout*>(ui->form->layout());
    if (layout == nullptr) {
        layout = new QVBoxLayout(ui->form);
        ui->form->setLayout(layout);
    }

    mAdd = true;
    mModifier = Modifiers::ByName(ui->comboBox->currentText());
    if ((mJustAccept = !mModifier->createForm(this, layout)) == true) {
        accept();
        return;
    }

    createLabel(layout, "");
    mDescription = createLabel(layout, "<incomplete>", true);

    layout->addStretch(1);
    updateForm();
}

void ModifiersDialog::updateForm() {
    mDescription->setText(mModifier->description(true));
    mOk->setEnabled(mModifier->description() != "<incomplete>");
}

void ModifiersDialog::stateChanged(int) {
    QCheckBox* checkBox = static_cast<QCheckBox*>(sender());
    mModifier->callback(checkBox);
    updateForm();
}

void ModifiersDialog::textChanged(QString) {
    QLineEdit* edit = static_cast<QLineEdit*>(sender());
    mModifier->callback(edit);
    updateForm();
}

void ModifiersDialog::currentIndexChanged(int) {
    QComboBox* combo = static_cast<QComboBox*>(sender());
    mModifier->callback(combo);
    updateForm();
}

void ModifiersDialog::itemSelectionChanged() {
    QTreeWidget* tree = static_cast<QTreeWidget*>(sender());
    mModifier->callback(tree);
    updateForm();
}

void ModifiersDialog::itemChanged(QTreeWidgetItem* item, int) {
    static bool Ignoring = false;
    if (!Ignoring) {
        Ignoring = true;
        if (item->parent() == nullptr) {
            int children = item->childCount();
            for (int i = 0; i < children; ++i) item->child(i)->setCheckState(0, item->checkState(0));
        }
        else item->parent()->setCheckState(0, Qt::Unchecked);
        update();
        Ignoring = false;
    } else return;
    itemSelectionChanged();
}
