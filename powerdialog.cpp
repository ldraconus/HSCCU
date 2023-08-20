#include "powerdialog.h"
#include "ui_powerdialog.h"
#include "sheet.h"

#include "modifiersdialog.h"

#include <QClipboard>
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>
#include <QMimeData>
#include <QPushButton>
#include <QScreen>
#include <QScrollBar>
#include <QTimer>

PowerDialog*      PowerDialog::_ptr;
shared_ptr<Power> PowerDialog::_dummy = nullptr;

PowerDialog::PowerDialog(QWidget *parent, shared_ptr<Power>& save)
    : QDialog(parent)
    , _saved(save)
{
    QFont font({ QString("Segoe UIHS") });
    setFont(font);

    ui = new Ui::PowerDialog;
    ui->setupUi(this);

    _ptr = this;
    _inMultipower = false;

    connect(ui->powerTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(pickType(int)));
    connect(ui->availableComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(pickOne(int)));

    _ok = ui->okButton;
    _ok->setEnabled(false);
    _cancel = ui->cancelButton;
    _cancel->setEnabled(true);
    _accepted = false;

    connect(_ok,     SIGNAL(clicked()), this, SLOT(ok()));
    connect(_cancel, SIGNAL(clicked()), this, SLOT(cancel()));

    QTimer::singleShot(100, this, &PowerDialog::doUpdate);
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
    QRect r = tablewidget->geometry();
    QFont font = tablewidget->font();
    int pnt = font.pointSize();
    verticalHeader->setVisible(false);
    verticalHeader->setMinimumSectionSize(1);
    verticalHeader->setMaximumSectionSize(h != -1 ? h : r.height());
    verticalHeader->setDefaultSectionSize(1);
    verticalHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
    auto horizontalHeader = tablewidget->horizontalHeader();
    horizontalHeader->setVisible(false);
    horizontalHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
    horizontalHeader->setStretchLastSection(true);
    horizontalHeader->setMaximumSectionSize(r.width());
    horizontalHeader->setDefaultSectionSize(10);
    horizontalHeader->setDefaultAlignment(Qt::AlignLeft);
    horizontalHeader->setMaximumSize(r.width(), h != -1 ? h : r.height());
    tablewidget->setSelectionMode(QAbstractItemView::SingleSelection);
    tablewidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tablewidget->setFont(font);
    QString family = font.family();
    tablewidget->setStyleSheet("QTableWidget { selection-color: black;"
                                           "   selection-background-color: cyan;"
                                           "   gridline-color: white;"
                                           "   border-style: none;"
                                 + QString("   font: %2pt \"%1\";").arg(family).arg(pnt) +
                                           "   color: black;"
                                           " } "
                               "QHeaderView::section { background-color: white;"
                                                   "   border-style: none;"
                                                   "   color: black;" +
                                           QString("   font: bold %2pt \"%1\";").arg(family).arg(pnt) +
                                                   " }");
    tablewidget->setColumnCount(columns.count());
    tablewidget->setRowCount(0);

    tablewidget->setToolTip(w);

    if (h != -1) r.setHeight(h);

    int total = 0;
    for (int i = 1; i < tablewidget->columnCount(); ++i) total += tablewidget->columnWidth(i - 1);
    tablewidget->setColumnWidth(columns.count() - 1, r.width() - total);

    tablewidget->setGeometry(r);
    layout->addWidget(tablewidget);

    return tablewidget;
}

QTableWidget* PowerDialog::createAdvantages(QWidget* parent, QVBoxLayout* layout) {
    _advantages = createTableWidget(parent, layout, { 15, 200 }, "Power Advantages", 75);
#ifndef __wasm__
    _advantagesMenu = createMenu(_advantages, _advantages->font(), { { "New",       &_newAdvantage },
                                                                     { "Edit",      &_editAdvantage },
                                                                     { "Delete",    &_deleteAdvantage },
                                                                     { "-",         },
                                                                     { "Cut",       &_cutAdvantage },
                                                                     { "Copy",      &_copyAdvantage },
                                                                     { "Paste",     &_pasteAdvantage },
                                                                     { "-",         },
                                                                     { "Move Up",   &_moveAdvantageUp },
                                                                     { "Move Down", &_moveAdvantageDown } } );
    connect(_advantages,        SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(advantageDoubleClicked(QTableWidgetItem*)));
    connect(_advantages,        SIGNAL(customContextMenuRequested(QPoint)),   this, SLOT(advantagesMenu(QPoint)));
    connect(_advantagesMenu,    SIGNAL(aboutToShow()),                        this, SLOT(aboutToShowAdvantagesMenu()));
    connect(_newAdvantage,      SIGNAL(triggered()),                          this, SLOT(newAdvantage()));
    connect(_editAdvantage,     SIGNAL(triggered()),                          this, SLOT(editAdvantage()));
    connect(_deleteAdvantage,   SIGNAL(triggered()),                          this, SLOT(deleteAdvantage()));
    connect(_cutAdvantage,      SIGNAL(triggered()),                          this, SLOT(cutAdvantage()));
    connect(_copyAdvantage,     SIGNAL(triggered()),                          this, SLOT(copyAdvantage()));
    connect(_pasteAdvantage,    SIGNAL(triggered()),                          this, SLOT(pasteAdvantage()));
    connect(_moveAdvantageUp,   SIGNAL(triggered()),                          this, SLOT(moveAdvantageUp()));
    connect(_moveAdvantageDown, SIGNAL(triggered()),                          this, SLOT(moveAdvantageDown()));
#else
    connect(_advantages,        SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(advantageDoubleClicked(QTableWidgetItem*)));
#endif

    return _advantages;
}

#ifdef __wasm__
QWidget* PowerDialog::createEditButton(QWidget* parent, QVBoxLayout* layout, const QList<const char*>& functions) {
    QFrame* frame = nullptr;
    QHBoxLayout* horizontalLayout;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QPushButton *pushButton_5;
    QPushButton *pushButton_6;
    QPushButton *pushButton_7;
    QPushButton *pushButton_8;

    frame = new QFrame(parent);
    frame->setObjectName("frame");
    frame->setFrameShape(QFrame::NoFrame);
    frame->setFrameShadow(QFrame::Plain);
    frame->setLineWidth(0);
    frame->resize(309, 30);
    horizontalLayout = new QHBoxLayout(frame);
    horizontalLayout->setSpacing(3);
    horizontalLayout->setObjectName("horizontalLayout");
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    pushButton = new QPushButton(frame);
    pushButton->setObjectName("pushButton");
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/gfx/New.ico"), QSize(), QIcon::Normal, QIcon::Off);
    pushButton->setIcon(icon);
    pushButton->setIconSize(QSize(24, 24));
    pushButton->setStatusTip("New ...");
    horizontalLayout->addWidget(pushButton);

    pushButton_2 = new QPushButton(frame);
    pushButton_2->setObjectName("pushButton_2");
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/gfx/Edit.ico"), QSize(), QIcon::Normal, QIcon::Off);
    pushButton_2->setIcon(icon1);
    pushButton_2->setIconSize(QSize(24, 24));
    pushButton_2->setStatusTip("Edit Selected");
    horizontalLayout->addWidget(pushButton_2);

    pushButton_3 = new QPushButton(frame);
    pushButton_3->setObjectName("pushButton_3");
    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/gfx/Delete.ico"), QSize(), QIcon::Normal, QIcon::Off);
    pushButton_3->setIcon(icon2);
    pushButton_3->setIconSize(QSize(24, 24));
    pushButton_3->setStatusTip("Delete Selected");
    horizontalLayout->addWidget(pushButton_3);

    pushButton_4 = new QPushButton(frame);
    pushButton_4->setObjectName("pushButton_4");
    QIcon icon3;
    icon3.addFile(QString::fromUtf8(":/gfx/Cut.ico"), QSize(), QIcon::Normal, QIcon::Off);
    pushButton_4->setIcon(icon3);
    pushButton_4->setIconSize(QSize(24, 24));
    pushButton_4->setStatusTip("Cut Selected");
    horizontalLayout->addWidget(pushButton_4);

    pushButton_5 = new QPushButton(frame);
    pushButton_5->setObjectName("pushButton_5");
    QIcon icon4;
    icon4.addFile(QString::fromUtf8(":/gfx/Copy.ico"), QSize(), QIcon::Normal, QIcon::Off);
    pushButton_5->setIcon(icon4);
    pushButton_5->setIconSize(QSize(24, 24));
    pushButton_5->setStatusTip("Copy Selected");
    horizontalLayout->addWidget(pushButton_5);

    pushButton_6 = new QPushButton(frame);
    pushButton_6->setObjectName("pushButton_6");
    QIcon icon5;
    icon5.addFile(QString::fromUtf8(":/gfx/Paste.ico"), QSize(), QIcon::Normal, QIcon::Off);
    pushButton_6->setIcon(icon5);
    pushButton_6->setIconSize(QSize(24, 24));
    pushButton_6->setStatusTip("Paste");
    horizontalLayout->addWidget(pushButton_6);

    pushButton_7 = new QPushButton(frame);
    pushButton_7->setObjectName("pushButton_7");
    QIcon icon6;
    icon6.addFile(QString::fromUtf8(":/gfx/MoveUp.ico"), QSize(), QIcon::Normal, QIcon::Off);
    pushButton_7->setIcon(icon6);
    pushButton_7->setIconSize(QSize(24, 24));
    pushButton_7->setStatusTip("Move Selected Up");
    horizontalLayout->addWidget(pushButton_7);

    pushButton_8 = new QPushButton(frame);
    pushButton_8->setObjectName("pushButton_8");
    pushButton_8->setStatusTip("Move Selected Down");
    QIcon icon7;
    icon7.addFile(QString::fromUtf8(":/gfx/MoveDown.ico"), QSize(), QIcon::Normal, QIcon::Off);
    pushButton_8->setIcon(icon7);
    pushButton_8->setIconSize(QSize(24, 24));
    horizontalLayout->addWidget(pushButton_8);

    layout->addWidget(frame);

    connect(pushButton,   SIGNAL(clicked()), this, functions[0]);
    connect(pushButton_2, SIGNAL(clicked()), this, functions[1]);
    connect(pushButton_3, SIGNAL(clicked()), this, functions[2]);
    connect(pushButton_4, SIGNAL(clicked()), this, functions[3]);
    connect(pushButton_5, SIGNAL(clicked()), this, functions[4]);
    connect(pushButton_6, SIGNAL(clicked()), this, functions[5]);
    connect(pushButton_7, SIGNAL(clicked()), this, functions[6]);
    connect(pushButton_8, SIGNAL(clicked()), this, functions[7]);
    return frame;
}
#else
QWidget* PowerDialog::createEditButton(QWidget*, QVBoxLayout*, const QList<const char*>&) { return nullptr; }
#endif

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
    _limitations = createTableWidget(parent, layout, { 15, 200 }, "Power Limitations", 75);
#ifndef __wasm__
    _limitationsMenu = createMenu(_limitations, _limitations->font(), { { "New",       &_newLimitation },
                                                                        { "Edit",      &_editLimitation },
                                                                        { "Delete",    &_deleteLimitation },
                                                                        { "-",         },
                                                                        { "Cut",       &_cutLimitation },
                                                                        { "Copy",      &_copyLimitation },
                                                                        { "Paste",     &_pasteLimitation },
                                                                        { "-",         },
                                                                        { "Move Up",   &_moveLimitationUp },
                                                                        { "Move Down", &_moveLimitationDown } } );
    connect(_limitations,        SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(limitationDoubleClicked(QTableWidgetItem*)));
    connect(_limitations,        SIGNAL(customContextMenuRequested(QPoint)),   this, SLOT(limitationsMenu(QPoint)));
    connect(_limitationsMenu,    SIGNAL(aboutToShow()),                        this, SLOT(aboutToShowLimitationsMenu()));
    connect(_newLimitation,      SIGNAL(triggered()),                          this, SLOT(newLimitation()));
    connect(_editLimitation,     SIGNAL(triggered()),                          this, SLOT(editLimitation()));
    connect(_deleteLimitation,   SIGNAL(triggered()),                          this, SLOT(deleteLimitation()));
    connect(_cutLimitation,      SIGNAL(triggered()),                          this, SLOT(cutLimitation()));
    connect(_copyLimitation,     SIGNAL(triggered()),                          this, SLOT(copyLimitation()));
    connect(_pasteLimitation,    SIGNAL(triggered()),                          this, SLOT(pasteLimitation()));
    connect(_moveLimitationUp,   SIGNAL(triggered()),                          this, SLOT(moveLimitationUp()));
    connect(_moveLimitationDown, SIGNAL(triggered()),                          this, SLOT(moveLimitationDown()));
#else
    connect(_limitations,        SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(limitationDoubleClicked(QTableWidgetItem*)));
#endif
    return _limitations;
}

void PowerDialog::setCellLabel(QTableWidget* tbl, int row, int col, QString str, QFont&) {
    QLabel* lbl = new QLabel(str);
    if (row >= tbl->rowCount()) tbl->setRowCount(row + 1);
    lbl->setWordWrap(true);
    tbl->setCellWidget(row, col, lbl);
}

void PowerDialog::aboutToShowAdvantagesMenu() {
    const auto selection = _advantages->selectedRanges();
    bool show = !selection.isEmpty();
    int row = -1;
    if (show) row = selection[0].topRow();
    _editAdvantage->setEnabled(show);
    _deleteAdvantage->setEnabled(show);
    _cutAdvantage->setEnabled(show);
    _copyAdvantage->setEnabled(show);
    _moveAdvantageUp->setEnabled(show && row != 0);
    _moveAdvantageDown->setEnabled(show && row != _advantages->rowCount() - 1);
    QClipboard* clipboard = QGuiApplication::clipboard();
    const QMimeData* clip = clipboard->mimeData();
    bool canPaste = clip->hasFormat("application/advantage");
    _pasteAdvantage->setEnabled(canPaste);
}

void PowerDialog::aboutToShowLimitationsMenu() {
    const auto selection = _limitations->selectedRanges();
    bool show = !selection.isEmpty();
    int row = -1;
    if (show) row = selection[0].topRow();
    _editLimitation->setEnabled(show);
    _deleteLimitation->setEnabled(show);
    _cutLimitation->setEnabled(show);
    _copyLimitation->setEnabled(show);
    _moveLimitationUp->setEnabled(show && row != 0);
    _moveLimitationDown->setEnabled(show && row != _limitations->rowCount() - 1);
    QClipboard* clipboard = QGuiApplication::clipboard();
    const QMimeData* clip = clipboard->mimeData();
    bool canPaste = clip->hasFormat("application/limitation");
    _pasteLimitation->setEnabled(canPaste);
}

void PowerDialog::advantagesMenu(QPoint pos) {
    _advantagesMenu->exec(mapToGlobal(pos
                                      + _advantages->pos()
                                      - QPoint(0, ui->scrollArea->verticalScrollBar()->value())));
}

void PowerDialog::cancel() {
    _accepted = false;
    _done = true;
    reject();
}

void PowerDialog::copyAdvantage() {
    auto selection = _advantages->selectedRanges();
    if (selection.empty()) return;

    QClipboard* clip = QGuiApplication::clipboard();
    QMimeData* data = new QMimeData();
    int row = selection[0].topRow();
    shared_ptr<Modifier> advantage = _power->advantagesList()[row];
    QJsonObject obj = advantage->toJson();
    QJsonDocument doc;
    doc.setObject(obj);
    data->setData("application/advantage", doc.toJson());
    QString text = QString("%1\t%2").arg(advantage->fraction(Modifier::NoStore).toString(), advantage->description());
    data->setData("text/plain", text.toUtf8());
    clip->setMimeData(data);
}

void PowerDialog::copyLimitation() {
    auto selection = _limitations->selectedRanges();
    if (selection.empty()) return;

    QClipboard* clip = QGuiApplication::clipboard();
    QMimeData* data = new QMimeData();
    int row = selection[0].topRow();
    shared_ptr<Modifier> limitation = _power->limitationsList()[row];
    QJsonObject obj = limitation->toJson();
    QJsonDocument doc;
    doc.setObject(obj);
    data->setData("application/limitation", doc.toJson());
    QString text = QString("%1\t%2").arg(limitation->fraction(Modifier::NoStore).toString(), limitation->description());
    data->setData("text/plain", text.toUtf8());
    clip->setMimeData(data);
}

void PowerDialog::cutAdvantage() {
    copyAdvantage();
    deleteAdvantage();
}

void PowerDialog::cutLimitation() {
    copyLimitation();
    deleteLimitation();
}

void PowerDialog::deleteAdvantage() {
    auto selection = _advantages->selectedRanges();
    if (selection.empty()) return;

    int row = selection[0].topRow();
    shared_ptr<Modifier> advantage = _power->advantagesList().takeAt(row);
    _advantages->removeRow(row);
    updateForm();
}

void PowerDialog::deleteLimitation() {
    auto selection = _limitations->selectedRanges();
    if (selection.empty()) return;

    int row = selection[0].topRow();
    shared_ptr<Modifier> limitation = _power->limitationsList().takeAt(row);
    _limitations->removeRow(row);
    updateForm();
}

void PowerDialog::editAdvantage() {
    auto selection = _advantages->selectedRanges();
    if (selection.empty()) return;

    int row = selection[0].topRow();
    shared_ptr<Modifier> advantage = _power->advantagesList()[row];
    _mod = make_shared<ModifiersDialog>(ModifiersDialog::Advantage);
    bool res = _mod->modifier(advantage);
    if (!res) return;
    _mod->open();
}

void PowerDialog::editLimitation() {
    auto selection = _limitations->selectedRanges();
    if (selection.empty()) return;

    int row = selection[0].topRow();
    shared_ptr<Modifier> limitation = _power->limitationsList()[row];
    _mod = make_shared<ModifiersDialog>(ModifiersDialog::Limitation);
    if (!_mod->modifier(limitation)) return;
    _mod->open();
}

void PowerDialog::limitationsMenu(QPoint pos) {
    _limitationsMenu->exec(mapToGlobal(pos
                                       + _limitations->pos()
                                       - QPoint(0, ui->scrollArea->verticalScrollBar()->value())));

}

void PowerDialog::moveAdvantageDown() {
    auto selection = _advantages->selectedRanges();
    if (selection.empty()) return;

    int row = selection[0].topRow();
    if (row >= _power->advantagesList().count() - 1) return;

    shared_ptr<Modifier> advantage = _power->advantagesList().takeAt(row);
    _power->advantagesList().insert(row + 1, advantage);
    updateForm();
}

void PowerDialog::moveLimitationDown() {
    auto selection = _limitations->selectedRanges();
    if (selection.empty()) return;

    int row = selection[0].topRow();
    auto& limitations = _power->limitationsList();
    if (row >= limitations.count() - 1) return;

    shared_ptr<Modifier> limitation = limitations.takeAt(row);
    limitations.insert(row + 1, limitation);
    updateForm();
}

void PowerDialog::moveAdvantageUp() {
    auto selection = _advantages->selectedRanges();
    if (selection.empty()) return;

    int row = selection[0].topRow();
    if (row < 1) return;

    auto& advantages = _power->advantagesList();
    shared_ptr<Modifier> advantage = advantages.takeAt(row);
    advantages.insert(row - 1, advantage);
    updateForm();
}

void PowerDialog::moveLimitationUp() {
    auto selection = _limitations->selectedRanges();
    if (selection.empty()) return;

    int row = selection[0].topRow();
    if (row < 1) return;

    auto& limitations = _power->limitationsList();
    shared_ptr<Modifier> limitation = limitations.takeAt(row);
    limitations.insert(row - 1, limitation);
    updateForm();
}

void PowerDialog::newAdvantage() {
    _mod = make_shared<ModifiersDialog>(ModifiersDialog::Advantage);
    _mod->open();
}

void PowerDialog::newLimitation() {
    _mod = make_shared<ModifiersDialog>(ModifiersDialog::Limitation);
    _mod->open();
}

void PowerDialog::setupPower(shared_ptr<Power>& power) {
    power = Power::FromJson(_power->name(), _power->toJson());
    power->modifiers().clear();
    for (const auto& mod: power->advantagesList()) {
        if (mod == nullptr) continue;

        power->modifiers().append(mod);
    }
    for (const auto& mod: power->limitationsList()) {
        if (mod == nullptr) continue;

        power->modifiers().append(mod);
    }
}

void PowerDialog::ok() {
    _accepted = _done = true;
    Sheet& s = Sheet::ref();

    if (_saved != nullptr && _saved->parent() != nullptr) {
        Power* group = _saved->parent();
        if (!group->isValid(_power)) return;
    }

    bool add = (_saved == nullptr);
    if (add) {
        shared_ptr<Power> work;
        setupPower(work);
        s.addPower(work);
    } else {
        setupPower(_saved);
        _dummy.reset(new Power);
        _dummy = nullptr;
    }

    s.updateDisplay();
    s.changed();
    accept();
}

void PowerDialog::pasteAdvantage() {
    QClipboard* clip = QGuiApplication::clipboard();
    const QMimeData* data = clip->mimeData();
    QByteArray byteArray = data->data("application/advantage");
    QString jsonStr(byteArray);
    QJsonDocument json = QJsonDocument::fromJson(jsonStr.toUtf8());
    QJsonObject obj = json.object();
    QString name = obj["name"].toString();
    shared_ptr<Modifier> advantage = Modifiers::ByName(name)->create(obj);
    _power->advantagesList().append(advantage);

    updateForm();
}

void PowerDialog::pasteLimitation() {
    QClipboard* clip = QGuiApplication::clipboard();
    const QMimeData* data = clip->mimeData();
    QByteArray byteArray = data->data("application/limitation");
    QString jsonStr(byteArray);
    QJsonDocument json = QJsonDocument::fromJson(jsonStr.toUtf8());
    QJsonObject obj = json.object();
    QString name = obj["name"].toString();
    shared_ptr<Modifier> limitation = Modifiers::ByName(name)->create(obj);
    _power->limitationsList().append(limitation);

    updateForm();
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

    QString text = ui->availableComboBox->currentText();
    if (text == "") return;

    _power = Power::ByName(text);
    if (_inMultipower) _power->inMultipower();
    try { _power->createForm(this, layout); } catch (...) { accept(); }

    createLabel(layout, "");
    createLabel(layout, "Advantages");
    createAdvantages(this, layout);
    createEditButton(this, layout, { SLOT(newAdvantage()),
                                     SLOT(editAdvantage()),
                                     SLOT(deleteAdvantage()),
                                     SLOT(cutAdvantage()),
                                     SLOT(copyAdvantage()),
                                     SLOT(pasteAdvantage()),
                                     SLOT(moveAdvantageUp()),
                                     SLOT(moveAdvantageDown()) });
    createLabel(layout, "");
    createLabel(layout, "Limitations");
    createLimitations(this, layout);
    createEditButton(this, layout, { SLOT(newLimitation()),
                                     SLOT(editLimitation()),
                                     SLOT(deleteLimitation()),
                                     SLOT(cutLimitation()),
                                     SLOT(copyLimitation()),
                                     SLOT(pasteLimitation()),
                                     SLOT(moveLimitationUp()),
                                     SLOT(moveLimitationDown()) });
    createLabel(layout, "");
    _points      = createLabel(layout, "-1 Points");
    _description = createLabel(layout, "<incomplete>", WordWrap);

    layout->addStretch(1);
    updateForm();
}

void PowerDialog::pickType(int type) {
    ui->availableComboBox->clear();

    QList<QString> available;
    switch (type) {
    case 0:  available = Power::AdjustmentPowers();     break;
    case 1:  available = Power::AttackPowers();         break;
    case 2:  available = Power::AutomatonPowers();      break;
    case 3:  available = Power::BodyAffectingPowers();  break;
    case 4:  available = Power::DefensePowers();        break;
    case 5:  available = Power::MentalPowers();         break;
    case 6:  available = Power::MovementPowers();       break;
    case 7:  available = Power::SenseAffectingPowers(); break;
    case 8:  available = Power::SensoryPowers();        break;
    case 9:  available = Power::SpecialPowers();        break;
    case 10: available = Power::StandardPowers();       break;
    case 11: available = Power::FrameworkPowers();      break;
    default: available = { };                           break;
    }

    for (const auto& power: available) ui->availableComboBox->addItem(power);
    ui->availableComboBox->setEnabled(available.isEmpty() ? false : true);

    if (_skipUpdate) {
        _skipUpdate = false;
        return;
    }

    QVBoxLayout* layout = static_cast<QVBoxLayout*>(ui->form->layout());
    if (layout == nullptr) {
        layout = new QVBoxLayout(ui->form);
        ui->form->setLayout(layout);
    }

    _power = nullptr;

    Power::ClearForm(layout);
}

PowerDialog& PowerDialog::powerorequipment(shared_ptr<Power> s) {
    QJsonObject obj = s->toJson();
    QString name = obj["name"].toString();
    if (name.isEmpty()) return *this;
    int idx = Power::Available().indexOf(name);
    if (idx == -1) return *this;

    int type = -1;
    if ((idx = Power::AdjustmentPowers().indexOf(name)) != -1)          type =  0;
    else if ((idx = Power::AttackPowers().indexOf(name)) != -1)         type =  1;
    else if ((idx = Power::AutomatonPowers().indexOf(name)) != -1)      type =  2;
    else if ((idx = Power::BodyAffectingPowers().indexOf(name)) != -1)  type =  3;
    else if ((idx = Power::DefensePowers().indexOf(name)) != -1)        type =  4;
    else if ((idx = Power::MentalPowers().indexOf(name)) != -1)         type =  5;
    else if ((idx = Power::MovementPowers().indexOf(name)) != -1)       type =  6;
    else if ((idx = Power::SenseAffectingPowers().indexOf(name)) != -1) type =  7;
    else if ((idx = Power::SensoryPowers().indexOf(name)) != -1)        type =  8;
    else if ((idx = Power::SpecialPowers().indexOf(name)) != -1)        type =  9;
    else if ((idx = Power::StandardPowers().indexOf(name)) != -1)       type = 10;
    else if ((idx = Power::FrameworkPowers().indexOf(name)) != -1)      type = 11;
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
    try { _power->createForm(this, layout); } catch (...) { accept(); return *this; }
    createLabel(layout, "");
    createLabel(layout, "Advantages");
    createAdvantages(this, layout);
    createEditButton(this, layout, { SLOT(newAdvantage()),
                                     SLOT(editAdvantage()),
                                     SLOT(deleteAdvantage()),
                                     SLOT(cutAdvantage()),
                                     SLOT(copyAdvantage()),
                                     SLOT(pasteAdvantage()),
                                     SLOT(movAdvantageUp()),
                                     SLOT(moveAdvantageDown()) });
    createLabel(layout, "");
    createLabel(layout, "Limitations");
    createLimitations(this, layout);
    createEditButton(this, layout, { SLOT(newLimitation()),
                                     SLOT(editLimitation()),
                                     SLOT(deleteLimitation()),
                                     SLOT(cutLimitation()),
                                     SLOT(copyLimitation()),
                                     SLOT(pasteLimitation()),
                                     SLOT(moveLimitationUp()),
                                     SLOT(moveLimitationDown()) });

    createLabel(layout, "");
    _points      = createLabel(layout, "-1 Points");
    _description = createLabel(layout, "<incomplete>", WordWrap);

    layout->addStretch(1);
    _power = s;
    _power->restore();
    updateForm();
    return *this;
}

void PowerDialog::activated(int) {
    QComboBox* comboBox = static_cast<QComboBox*>(sender());
    _power->callback(comboBox, true);
    updateForm();
}

void PowerDialog::buttonPressed(bool) {
    QPushButton* button = static_cast<QPushButton*>(sender());
    _power->callback(button);
    updateForm();
}

void PowerDialog::itemChanged(QTreeWidgetItem* itm, int) {
    QTreeWidget* tree = itm->treeWidget();
    _power->callback(tree);
    updateForm();
}

void PowerDialog::currentIndexChanged(int) {
    QComboBox* comboBox = static_cast<QComboBox*>(sender());
    _power->callback(comboBox);
    updateForm();
}

void PowerDialog::itemSelectionChanged() {
    QTreeWidget* tree = static_cast<QTreeWidget*>(sender());
    _power->callback(tree);
    updateForm();
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

void PowerDialog::setColumns(QTableWidget* tablewidget) {
    if (tablewidget->rowCount() < 1) return;

    QFont font = tablewidget->font();
    int pnt = font.pointSize();
    int sz = pnt * 2;

    int total = 0;
    int len = tablewidget->width();
    int hgt = tablewidget->height();
    for (int i = 0; i < tablewidget->rowCount(); ++i) tablewidget->resizeRowToContents(i);
    for (int i = 1; i < tablewidget->columnCount(); ++i) total += tablewidget->columnWidth(i - 1);
    tablewidget->setColumnWidth(tablewidget->columnCount() - 1, len - total);

    auto horizontalHeader = tablewidget->horizontalHeader();
    horizontalHeader->setMaximumSectionSize(len);
    horizontalHeader->setDefaultSectionSize(10);
    horizontalHeader->setDefaultAlignment(Qt::AlignLeft);
    horizontalHeader->setMaximumSize(len, sz);

    auto verticalHeader = tablewidget->verticalHeader();
    verticalHeader->setMaximumSectionSize(hgt);
}

void PowerDialog::updateForm() {
    if (_power == nullptr) return;

    _power->store();
    Points pts = _power->real();
    if ((!_power->isFramework() || _power->isVPP() || _power->isMultipower()) && pts.points == 0) pts = 1_cp;
    if (_power->isVPP()) pts += _power->pool();
    _points->setText(QString("%1 points").arg(pts.points));
    QString descr = _power->description(Power::ShowEND);
    QFont font = _advantages->font();
    _advantages->setRowCount(0);
    _advantages->update();
    int row = 0;
    for (const auto& mod: _power->advantagesList()) {
        QString val;
        if (mod->isAdder()) val = QString("%1").arg(mod->points(Modifier::NoStore).points);
        else val = mod->fraction(Modifier::NoStore).toString();
        descr += "; (+" + val + ") " + mod->description();
        setCellLabel(_advantages, row, 0, "+" + val, font);
        setCellLabel(_advantages, row, 1, mod->description(), font);
        row++;
    }
    setColumns(_advantages);

    _limitations->setRowCount(0);
    _limitations->update();
    row = 0;
    for (const auto& lim: _power->limitationsList()) {
        descr += "; (" + lim->fraction(Modifier::NoStore).toString() + ") " + lim->description();
        setCellLabel(_limitations, row, 0, lim->fraction(Modifier::NoStore).toString(), font);
        setCellLabel(_limitations, row, 1, lim->description(), font);
        row++;
    }
    setColumns(_limitations);

    _description->setText(descr);
    _ok->setEnabled(_power->description() != "<incomplete>");
}
