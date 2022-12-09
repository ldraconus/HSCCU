#include "powerdialog.h"
#include "ui_powerdialog.h"
#include "powers.h"

#include <QHeaderView>
#include <QMenu>
#include <QPushButton>
#include <QScrollBar>

PowerDialog* PowerDialog::_ptr;

PowerDialog::PowerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PowerDialog)
{
    ui->setupUi(this);
    _ptr = this;

    connect(ui->powerTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(pickType(int)));
    connect(ui->availableComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(pickOne(int)));

    _ok = ui->buttonBox->button(QDialogButtonBox::Ok);
    _ok->setEnabled(false);
}

PowerDialog::~PowerDialog()
{
    delete ui;
}

QMenu* PowerDialog::createMenu(QWidget* parent, const QFont& font, QList<menuItems> items) {
    QMenu* menu = new QMenu(parent);
    menu->setFont(font);
    for (auto& item: items) {
        if (item.action == nullptr) {
            menu->addSeparator();
            continue;
        }
        QAction*& action = *item.action;
        action = new QAction(item.text);
        action->setFont(font);
        menu->addAction(action);
    }
    return menu;
}

QTableWidget* PowerDialog::createTableWidget(QWidget* parent, QVBoxLayout* layout, QList<int> columns, QString w, int h) {
    QTableWidget* tablewidget = new QTableWidget(parent);
    tablewidget->setContextMenuPolicy(Qt::CustomContextMenu);
    tablewidget->setWordWrap(true);
    auto verticalHeader = tablewidget->verticalHeader();
    QFont font = tablewidget->font();
    int pnt = font.pointSize();
    int sz = pnt * 2;
    int l = tablewidget->width();
    verticalHeader->setVisible(false);
    verticalHeader->setMinimumSectionSize(1);
    verticalHeader->setMaximumSectionSize(pnt * 5);
    verticalHeader->setDefaultSectionSize(1);
    verticalHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
    auto horizontalHeader = tablewidget->horizontalHeader();
    horizontalHeader->setVisible(false);
    horizontalHeader->setStretchLastSection(true);
    horizontalHeader->setMaximumSectionSize(l);
    horizontalHeader->setDefaultSectionSize(10);
    horizontalHeader->setDefaultAlignment(Qt::AlignLeft);
    horizontalHeader->setMaximumSize(l, sz);
    tablewidget->setSelectionMode(QAbstractItemView::SingleSelection);
    tablewidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tablewidget->setFont(font);
    QString family = font.family();
    tablewidget->setStyleSheet("QTableWidget { selection-color: black;"
                                           "   selection-background-color: cyan;"
                                           "   gridline-color: white;"
                                           "   border-style: none;"
                                 + QString("   font: %2pt \"%1\";").arg(family).arg((pnt * 8 + 5) / 10) +
                                           "   color: black;"
                                           " } "
                               "QHeaderView::section { background-color: white;"
                                                   "   border-style: none;"
                                                   "   color: black;" +
                                           QString("   font: bold %2pt \"%1\";").arg(family).arg((pnt * 8 + 5) / 10) +
                                                   " }");
    tablewidget->setColumnCount(columns.count());
    tablewidget->setRowCount(0);
    tablewidget->setToolTip(w);
    QRect r = tablewidget->geometry();
    if (h != -1) r.setHeight(h);
    tablewidget->setGeometry(r);
    int total = 0;
    for (int i = 0; i < tablewidget->rowCount(); ++i) tablewidget->resizeRowToContents(i);
    for (int i = 1; i < tablewidget->columnCount(); ++i) {
        tablewidget->setColumnWidth(i - 1, columns[i]);
        total += tablewidget->columnWidth(i - 1);
    }
    tablewidget->setColumnWidth(columns.count() - 1, l - total);
    layout->addWidget(tablewidget);

    return tablewidget;
}

QTableWidget* PowerDialog::createAdvantages(QWidget* parent, QVBoxLayout* layout) {
    QTableWidget* tablewidget = createTableWidget(parent, layout, { 15, 100 }, "Power Advantages", 75);
    _advantagesMenu = createMenu(tablewidget, tablewidget->font(), { { "New",       &_newAdvantage },
                                                                     { "Edit",      &_editAdvantage },
                                                                     { "Delete",    &_deleteAdvantage },
                                                                     { "-",         },
                                                                     { "Cut",       &_cutAdvantage },
                                                                     { "Copy",      &_copyAdvantage },
                                                                     { "Paste",     &_pasteAdvantage },
                                                                     { "-",         },
                                                                     { "Move Up",   &_moveAdvantageUp },
                                                                     { "Move Down", &_moveAdvantageDown } } );
    connect(tablewidget,        SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(advantageDoubleClicked(QTableWidgetItem*)));
    connect(tablewidget,        SIGNAL(customContextMenuRequested(QPoint)),   this, SLOT(advantagesMenu(QPoint)));
    connect(_advantagesMenu,    SIGNAL(aboutToShow()),                        this, SLOT(aboutToShowAdvantagesMenu()));
    connect(_newAdvantage,      SIGNAL(triggered()),                          this, SLOT(newAdvantage()));
    connect(_editAdvantage,     SIGNAL(triggered()),                          this, SLOT(editAdvantage()));
    connect(_deleteAdvantage,   SIGNAL(triggered()),                          this, SLOT(deleteAdvantage()));
    connect(_cutAdvantage,      SIGNAL(triggered()),                          this, SLOT(cutAdvantage()));
    connect(_copyAdvantage,     SIGNAL(triggered()),                          this, SLOT(copyAdvantage()));
    connect(_pasteAdvantage,    SIGNAL(triggered()),                          this, SLOT(pasteAdvantage()));
    connect(_moveAdvantageUp,   SIGNAL(triggered()),                          this, SLOT(moveAdvantageUp()));
    connect(_moveAdvantageDown, SIGNAL(triggered()),                          this, SLOT(moveAdvantageDown()));
    _advantages = tablewidget;
    return tablewidget;
}

QLabel* PowerDialog::createLabel(QVBoxLayout* parent, QString text, bool wordWrap) {
    QLabel* label = new QLabel();
    if (label != nullptr) {
        label->setText(text);
        label->setWordWrap(wordWrap);
        parent->addWidget(label);
    }
    return label;
}

QTableWidget* PowerDialog::createLimitations(QWidget* parent, QVBoxLayout* layout) {
    QTableWidget* tablewidget = createTableWidget(parent, layout, { 15, 100 }, "Power Limitations", 75);
    _limitationsMenu    = createMenu(tablewidget, tablewidget->font(), { { "New",       &_newLimitation },
                                                                         { "Edit",      &_editLimitation },
                                                                         { "Delete",    &_deleteLimitation },
                                                                         { "-",         },
                                                                         { "Cut",       &_cutLimitation },
                                                                         { "Copy",      &_copyLimitation },
                                                                         { "Paste",     &_pasteLimitation },
                                                                         { "-",         },
                                                                         { "Move Up",   &_moveLimitationUp },
                                                                         { "Move Down", &_moveLimitationDown } } );
    connect(tablewidget,         SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(limitationDoubleClicked(QTableWidgetItem*)));
    connect(tablewidget,         SIGNAL(customContextMenuRequested(QPoint)),   this, SLOT(limitationsMenu(QPoint)));
    connect(_limitationsMenu,    SIGNAL(aboutToShow()),                        this, SLOT(aboutToShowLimitationsMenu()));
    connect(_newLimitation,      SIGNAL(triggered()),                          this, SLOT(newLimitation()));
    connect(_editLimitation,     SIGNAL(triggered()),                          this, SLOT(editLimitation()));
    connect(_deleteLimitation,   SIGNAL(triggered()),                          this, SLOT(deleteLimitation()));
    connect(_cutLimitation,      SIGNAL(triggered()),                          this, SLOT(cutLimitation()));
    connect(_copyLimitation,     SIGNAL(triggered()),                          this, SLOT(copyLimitation()));
    connect(_pasteLimitation,    SIGNAL(triggered()),                          this, SLOT(pasteLimitation()));
    connect(_moveLimitationUp,   SIGNAL(triggered()),                          this, SLOT(moveLimitationUp()));
    connect(_moveLimitationDown, SIGNAL(triggered()),                          this, SLOT(moveLimitationDown()));
    _limitations = tablewidget;
    return tablewidget;
}

void PowerDialog::aboutToShowAdvantagesMenu() {

}

void PowerDialog::aboutToShowLimitationsMenu() {

}

void PowerDialog::advantagesMenu(QPoint pos) {
    _advantagesMenu->exec(mapToGlobal(pos
                                      + _advantages->pos()
                                      - QPoint(0, ui->scrollArea->verticalScrollBar()->value())));

}

void PowerDialog::copyAdvantage() {

}

void PowerDialog::copyLimitation() {

}

void PowerDialog::cutAdvantage() {

}

void PowerDialog::cutLimitation() {

}

void PowerDialog::deleteAdvantage() {

}

void PowerDialog::deleteLimitation() {

}

void PowerDialog::editAdvantage() {

}

void PowerDialog::editLimitation() {

}

void PowerDialog::limitationsMenu(QPoint pos) {
    _limitationsMenu->exec(mapToGlobal(pos
                                       + _limitations->pos()
                                       - QPoint(0, ui->scrollArea->verticalScrollBar()->value())));

}

void PowerDialog::moveAdvantageDown() {

}

void PowerDialog::moveLimitationDown() {

}

void PowerDialog::moveAdvantageUp() {

}

void PowerDialog::moveLimitationUp() {

}

void PowerDialog::newLimitation() {

}

void PowerDialog::newAdvantage() {

}

void PowerDialog::pasteAdvantage() {

}

void PowerDialog::pasteLimitation() {

}

void PowerDialog::pickOne(int) {
    if (_skipUpdate) {
        _skipUpdate = false;
        return;
    }

    QVBoxLayout* layout = static_cast<QVBoxLayout*>(ui->form->layout());
    if (layout == nullptr) {
        layout = new QVBoxLayout(ui->form);
        ui->form->setLayout(layout);
    }

    if (_power) delete _power;

    _power = Power::ByName(ui->availableComboBox->currentText());
    try { _power->createForm(this, layout); } catch (...) { accept(); }


    createLabel(layout, "");
    createLabel(layout, "Advantages");
    _advantages = createAdvantages(this, layout);
    createLabel(layout, "");
    createLabel(layout, "Limitations");
    _limitations = createLimitations(this, layout);
    createLabel(layout, "");
    _points      = createLabel(layout, "-1 Points");
    _description = createLabel(layout, "<incomplete>", WordWrap);

    layout->addStretch(1);
    updateForm();
}

void PowerDialog::pickType(int type) {
    QList<QString> available;
    switch (type) {
    case 0: available = Power::AdjustmentPowers();  break;
    }

    for (const auto& power: available) ui->availableComboBox->addItem(power);
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

    if (_power) delete _power;
    _power = nullptr;

    Power::ClearForm(layout);
}

PowerDialog& PowerDialog::powerorequipment(Power* s) {
    QJsonObject obj = s->toJson();
    QString name = obj["name"].toString();
    if (name.isEmpty()) return *this;
    int idx = Power::Available().indexOf(name);
    if (idx == -1) return *this;

    int type = -1;
    if ((idx = Power::AdjustmentPowers().indexOf(name)) != -1) type = 0;
//    else if ((idx = SkillTalentOrPerk::TalentsAvailable().indexOf(name)) != -1) type = 1;
//    else if ((idx = SkillTalentOrPerk::PerksAvailable().indexOf(name)) != -1) type = 2;
    else return *this;

    _skipUpdate = true;
    ui->powerTypeComboBox->setCurrentIndex(type);
    ui->powerTypeComboBox->setEnabled(false);
    _skipUpdate = true;
    ui->availableComboBox->setCurrentIndex(idx);
    ui->availableComboBox->setEnabled(false);

    QVBoxLayout* layout = static_cast<QVBoxLayout*>(ui->form->layout());
    if (layout == nullptr) {
        layout = new QVBoxLayout(ui->form);
        ui->form->setLayout(layout);
    }

    _power = s;
    _power->createForm(this, layout);
    createLabel(layout, "");
    createLabel(layout, "Advantages");
    _advantages = createAdvantages(this, layout);
    createLabel(layout, "");
    createLabel(layout, "Limitations");
    _limitations = createLimitations(this, layout);
    createLabel(layout, "");
    _points      = createLabel(layout, "-1 Points");
    _description = createLabel(layout, "<incomplete>", WordWrap);

    layout->addStretch(1);
    _power = s;
    _power->restore();
    updateForm();
    return *this;
}

void PowerDialog::stateChanged(int) {
    QCheckBox* checkBox = static_cast<QCheckBox*>(sender());
    _power->callback(checkBox);
    updateForm();
}

void PowerDialog::textChanged(QString) {
    QLineEdit* text = static_cast<QLineEdit*>(sender());
    _power->callback(text);
    updateForm();
}

void PowerDialog::updateForm() {
    _points->setText(QString("%1 points").arg(_power->points()));
    _description->setText(_power->description(Power::ShowEND)); // [TODO] add advantages and limitations
    _ok->setEnabled(_power->description() != "<incomplete>");
}
