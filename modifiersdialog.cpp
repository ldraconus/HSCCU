#include "modifiersdialog.h"
#include "ui_modifiersdialog.h"
#include "modifier.h"
#include "powerdialog.h"
#include "shared.h"
#include "sheet.h"

ModifiersDialog* ModifiersDialog::_modifiersDialog = nullptr; // NOLINT

ModifiersDialog::ModifiersDialog(bool advantage, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ModifiersDialog)
{
    QFont font({ QString("Segoe UIHS") });
    setFont(font);

    ui->setupUi(this);

#ifdef unix
    setStyleSheet("color: #000; background: #fff;");
#endif

    _modifiersDialog = this;

    _modifiers = make_shared<Modifiers>();

    setModifiers(advantage);
    ui->form->setWindowTitle(advantage ? "Pick Advantage" : "Pick Limitation");
    ui->comboBox->setPlaceholderText(advantage ? "Pick an Advantage" : "Pick a Limitation");

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(pickOne(int)));
    connect(this,         SIGNAL(accepted()),               this, SLOT(doAccepted()));

    Sheet::ref().fixButtonBox(ui->buttonBox);
    _ok = ui->buttonBox->button(QDialogButtonBox::Ok); // NOLINT
    _ok->setEnabled(false);

}

ModifiersDialog::~ModifiersDialog()
{
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
    if (_add) {
        if (_modifier->type() == ModifierBase::isAdvantage ||
            (_modifier->type() == ModifierBase::isBoth && _modifier->fraction(Modifier::NoStore) > 0)) {
            PowerDialog::ref().powerorequipment()->advantagesList().append(_modifier);
        } else PowerDialog::ref().powerorequipment()->limitationsList().append(_modifier);
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

    _skipUpdate = true;
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

    _add = false;
    _modifier = mod;
    /* This would be the simple way to handle this, but it does not work because the dialog is not up at this time.
     * Going to try and put it in an event or something, try something that will be hit when the dialog is up
    try { _modifier->createForm(this, layout); } catch (...) { accept(); }
    * timers don't work with WASM, so try the show event handler instead ... */
    if (_modifier->createForm(this, layout) == false) return false;

    createLabel(layout, "");
    _description = createLabel(layout, "<incomplete>", true);

    layout->addStretch(1);
    _modifier->restore();
    updateForm();
    return true;
}

void ModifiersDialog::setModifiers(bool advantage) {
    QList<shared_ptr<Modifier>> modifiers = (*_modifiers)(advantage ? Modifier::isAdvantage : Modifier::isLimitation);
    ui->comboBox->clear();
    for (const auto& modifier: modifiers) ui->comboBox->addItem(modifier->name());
}

void ModifiersDialog::pickOne(int) {
    if (_skipUpdate) {
        _skipUpdate = false;
        return;
    }

    QVBoxLayout* layout = static_cast<QVBoxLayout*>(ui->form->layout());
    if (layout == nullptr) {
        layout = new QVBoxLayout(ui->form);
        ui->form->setLayout(layout);
    }

    _add = true;
    _modifier = Modifiers::ByName(ui->comboBox->currentText());
    if ((_justAccept = !_modifier->createForm(this, layout)) == true) {
        accept();
        return;
    }

    createLabel(layout, "");
    _description = createLabel(layout, "<incomplete>", true);

    layout->addStretch(1);
    updateForm();
}

void ModifiersDialog::updateForm() {
    _description->setText(_modifier->description(true));
    _ok->setEnabled(_modifier->description() != "<incomplete>");
}

void ModifiersDialog::stateChanged(int) {
    QCheckBox* checkBox = static_cast<QCheckBox*>(sender());
    _modifier->callback(checkBox);
    updateForm();
}

void ModifiersDialog::textChanged(QString) {
    QLineEdit* edit = static_cast<QLineEdit*>(sender());
    _modifier->callback(edit);
    updateForm();
}

void ModifiersDialog::currentIndexChanged(int) {
    QComboBox* combo = static_cast<QComboBox*>(sender());
    _modifier->callback(combo);
    updateForm();
}

void ModifiersDialog::itemSelectionChanged() {
    QTreeWidget* tree = static_cast<QTreeWidget*>(sender());
    _modifier->callback(tree);
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
