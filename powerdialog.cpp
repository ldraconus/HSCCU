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

PowerDialog*      PowerDialog::mPtr = nullptr; // NOLINT
shared_ptr<Power> PowerDialog::_dummy = nullptr; // NOLINT

PowerDialog::PowerDialog(QWidget *parent, shared_ptr<Power>& save)
    : Dialog(parent)
    , ui(new Ui::PowerDialog)
    , mSaved(save) {
    QFont font({ QString("Segoe UIHS") });
    setFont(font);

    ui->setupUi(this);

    setStyleSheet("color: #000; background: #fff;");

    ui->powerTypeComboBox->addItem("Adjustment Powers");
    ui->powerTypeComboBox->addItem("Attack Powers");
    ui->powerTypeComboBox->addItem("Automaton Powers\317\264");
    ui->powerTypeComboBox->addItem("Body Affecting Powers");
    ui->powerTypeComboBox->addItem("Defense Powers");
    ui->powerTypeComboBox->addItem("Mental Powers");
    ui->powerTypeComboBox->addItem("Movement Powers");
    ui->powerTypeComboBox->addItem("Sense Affecting Powers");
    ui->powerTypeComboBox->addItem("Sensory Powers");
    ui->powerTypeComboBox->addItem("Special Powers");
    ui->powerTypeComboBox->addItem("Standard Powers");
    ui->powerTypeComboBox->addItem("Groups & Pools");
    if (Sheet::ref().option().equipmentFree()) ui->powerTypeComboBox->addItem("Equipment");

    mPtr = this;
    mInMultipower = false;

    connect(ui->powerTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(pickType(int)));
    connect(ui->availableComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(pickOne(int)));

    Sheet::ref().fixButtonBox(ui->buttonBox);
    mAccepted = false;

    mOk = ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok);
    mCancel = ui->buttonBox->button(QDialogButtonBox::StandardButton::Cancel);

    mOk->setEnabled(false);

    connect(mOk,     SIGNAL(clicked()), this, SLOT(ok()));
    connect(mCancel, SIGNAL(clicked()), this, SLOT(cancel()));

    QTimer::singleShot(100, this, &PowerDialog::doUpdate);
}

PowerDialog::~PowerDialog() {
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
    horizontalHeader->setDefaultSectionSize(10); // NOLINT
    horizontalHeader->setDefaultAlignment(Qt::AlignLeft);
    horizontalHeader->setMaximumSize(r.width(), h != -1 ? h : r.height());
    tablewidget->setSelectionMode(QAbstractItemView::SingleSelection);
    tablewidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tablewidget->setFont(font);
    QString family = font.family();
    tablewidget->setStyleSheet("QTableWidget { gridline-color: white;"
                                           "   background-color: white;"
                                           "   border-color: black;"
                                 + QString("   font: %2pt \"%1\";").arg(family).arg(pnt) + // NOLINT
                                           "   color: black;"
                                           " } "
                               "QTableWidget::item { background-color: white;"
                                                 " }"
                               "QTableWidget::item:selected { background-color: cyan;"
                                                          " }");
    tablewidget->setColumnCount(int(columns.count()));
    tablewidget->setRowCount(0);

    tablewidget->setToolTip(w);

    if (h != -1) r.setHeight(h);

    int total = 0;
    for (int i = 1; i < tablewidget->columnCount(); ++i) total += tablewidget->columnWidth(i - 1);
    tablewidget->setColumnWidth(int(columns.count()) - 1, int(r.width()) - total);

    tablewidget->setGeometry(r);
    layout->addWidget(tablewidget);

    return tablewidget;
}

QTableWidget* PowerDialog::createAdvantages(QWidget* parent, QVBoxLayout* layout) {
    if (mEquipment) return nullptr;
    mAdvantages = createTableWidget(parent, layout, { 15, 200 }, "Power Advantages", 75); // NOLINT
#ifndef __wasm__
    mAdvantagesMenu = createMenu(mAdvantages, mAdvantages->font(), { { "New",       &mNewAdvantage },
                                                                     { "Edit",      &mEditAdvantage },
                                                                     { "Delete",    &mDeleteAdvantage },
                                                                     { "-",         },
                                                                     { "Cut",       &mCutAdvantage },
                                                                     { "Copy",      &mCopyAdvantage },
                                                                     { "Paste",     &mPasteAdvantage },
                                                                     { "-",         },
                                                                     { "Move Up",   &mMoveAdvantageUp },
                                                                     { "Move Down", &mMoveAdvantageDown } } );
    connect(mAdvantages,        SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(advantageDoubleClicked(QTableWidgetItem*)));
    connect(mAdvantages,        SIGNAL(customContextMenuRequested(QPoint)),   this, SLOT(advantagesMenu(QPoint)));
    connect(mAdvantagesMenu,    SIGNAL(aboutToShow()),                        this, SLOT(aboutToShowAdvantagesMenu()));
    connect(mNewAdvantage,      SIGNAL(triggered()),                          this, SLOT(newAdvantage()));
    connect(mEditAdvantage,     SIGNAL(triggered()),                          this, SLOT(editAdvantage()));
    connect(mDeleteAdvantage,   SIGNAL(triggered()),                          this, SLOT(deleteAdvantage()));
    connect(mCutAdvantage,      SIGNAL(triggered()),                          this, SLOT(cutAdvantage()));
    connect(mCopyAdvantage,     SIGNAL(triggered()),                          this, SLOT(copyAdvantage()));
    connect(mPasteAdvantage,    SIGNAL(triggered()),                          this, SLOT(pasteAdvantage()));
    connect(mMoveAdvantageUp,   SIGNAL(triggered()),                          this, SLOT(moveAdvantageUp()));
    connect(mMoveAdvantageDown, SIGNAL(triggered()),                          this, SLOT(moveAdvantageDown()));
#else
    connect(mAdvantages,        SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(advantageDoubleClicked(QTableWidgetItem*)));
#endif

    return mAdvantages;
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
    if (mEquipment) return nullptr;
    mLimitations = createTableWidget(parent, layout, { 15, 200 }, "Power Limitations", 75);
#ifndef __wasm__
    mLimitationsMenu = createMenu(mLimitations, mLimitations->font(), { { "New",       &mNewLimitation },
                                                                        { "Edit",      &mEditLimitation },
                                                                        { "Delete",    &mDeleteLimitation },
                                                                        { "-",         },
                                                                        { "Cut",       &mCutLimitation },
                                                                        { "Copy",      &mCopyLimitation },
                                                                        { "Paste",     &mPasteLimitation },
                                                                        { "-",         },
                                                                        { "Move Up",   &mMoveLimitationUp },
                                                                        { "Move Down", &mMoveLimitationDown } } );
    connect(mLimitations,        SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(limitationDoubleClicked(QTableWidgetItem*)));
    connect(mLimitations,        SIGNAL(customContextMenuRequested(QPoint)),   this, SLOT(limitationsMenu(QPoint)));
    connect(mLimitationsMenu,    SIGNAL(aboutToShow()),                        this, SLOT(aboutToShowLimitationsMenu()));
    connect(mNewLimitation,      SIGNAL(triggered()),                          this, SLOT(newLimitation()));
    connect(mEditLimitation,     SIGNAL(triggered()),                          this, SLOT(editLimitation()));
    connect(mDeleteLimitation,   SIGNAL(triggered()),                          this, SLOT(deleteLimitation()));
    connect(mCutLimitation,      SIGNAL(triggered()),                          this, SLOT(cutLimitation()));
    connect(mCopyLimitation,     SIGNAL(triggered()),                          this, SLOT(copyLimitation()));
    connect(mPasteLimitation,    SIGNAL(triggered()),                          this, SLOT(pasteLimitation()));
    connect(mMoveLimitationUp,   SIGNAL(triggered()),                          this, SLOT(moveLimitationUp()));
    connect(mMoveLimitationDown, SIGNAL(triggered()),                          this, SLOT(moveLimitationDown()));
#else
    connect(mLimitations,        SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(limitationDoubleClicked(QTableWidgetItem*)));
#endif
    return mLimitations;
}

void PowerDialog::setCellLabel(QTableWidget* tbl, int row, int col, QString str, QFont&) {
    QLabel* lbl = new QLabel(str);
    if (row >= tbl->rowCount()) tbl->setRowCount(row + 1);
    lbl->setWordWrap(true);
    tbl->setCellWidget(row, col, lbl);
}

void PowerDialog::aboutToShowAdvantagesMenu() {
    if (mEquipment) return;
    const auto selection = mAdvantages->selectedRanges();
    bool show = !selection.isEmpty();
    int row = -1;
    if (show) row = selection[0].topRow();
    mEditAdvantage->setEnabled(show);
    mDeleteAdvantage->setEnabled(show);
    mCutAdvantage->setEnabled(show);
    mCopyAdvantage->setEnabled(show);
    mMoveAdvantageUp->setEnabled(show && row != 0);
    mMoveAdvantageDown->setEnabled(show && row != mAdvantages->rowCount() - 1);
    QClipboard* clipboard = QGuiApplication::clipboard();
    const QMimeData* clip = clipboard->mimeData();
    bool canPaste = clip->hasFormat("application/advantage");
    mPasteAdvantage->setEnabled(canPaste);
}

void PowerDialog::aboutToShowLimitationsMenu() {
    if (mEquipment) return;
    const auto selection = mLimitations->selectedRanges();
    bool show = !selection.isEmpty();
    int row = -1;
    if (show) row = selection[0].topRow();
    mEditLimitation->setEnabled(show);
    mDeleteLimitation->setEnabled(show);
    mCutLimitation->setEnabled(show);
    mCopyLimitation->setEnabled(show);
    mMoveLimitationUp->setEnabled(show && row != 0);
    mMoveLimitationDown->setEnabled(show && row != mLimitations->rowCount() - 1);
    QClipboard* clipboard = QGuiApplication::clipboard();
    const QMimeData* clip = clipboard->mimeData();
    bool canPaste = clip->hasFormat("application/limitation");
    mPasteLimitation->setEnabled(canPaste);
}

void PowerDialog::advantagesMenu(QPoint pos) {
    mAdvantagesMenu->exec(mapToGlobal(pos
                                      + mAdvantages->pos()
                                      - QPoint(0, ui->scrollArea->verticalScrollBar()->value())));
}

void PowerDialog::cancel() {
    mAccepted = false;
    mDone = true;
    reject();
}

void PowerDialog::copyAdvantage() {
    if (mEquipment) return;
    auto selection = mAdvantages->selectedRanges();
    if (selection.empty()) return;

    QClipboard* clip = QGuiApplication::clipboard();
    QMimeData* data = new QMimeData();
    int row = selection[0].topRow();
    shared_ptr<Modifier> advantage = mPower->advantagesList()[row];
    QJsonObject obj = advantage->toJson();
    QJsonDocument doc;
    doc.setObject(obj);
    data->setData("application/advantage", doc.toJson());
    QString text = QString("%1\t%2").arg(advantage->fraction(Modifier::NoStore).toString(), advantage->description());
    data->setData("text/plain", text.toUtf8());
    clip->setMimeData(data);
}

void PowerDialog::copyLimitation() {
    if (mEquipment) return;
    auto selection = mLimitations->selectedRanges();
    if (selection.empty()) return;

    QClipboard* clip = QGuiApplication::clipboard();
    QMimeData* data = new QMimeData();
    int row = selection[0].topRow();
    shared_ptr<Modifier> limitation = mPower->limitationsList()[row];
    QJsonObject obj = limitation->toJson();
    QJsonDocument doc;
    doc.setObject(obj);
    data->setData("application/limitation", doc.toJson());
    QString text = QString("%1\t%2").arg(limitation->fraction(Modifier::NoStore).toString(), limitation->description());
    data->setData("text/plain", text.toUtf8());
    clip->setMimeData(data);
}

void PowerDialog::cutAdvantage() {
    if (mEquipment) return;
    copyAdvantage();
    deleteAdvantage();
}

void PowerDialog::cutLimitation() {
    if (mEquipment) return;
    copyLimitation();
    deleteLimitation();
}

void PowerDialog::deleteAdvantage() {
    if (mEquipment) return;
    auto selection = mAdvantages->selectedRanges();
    if (selection.empty()) return;

    int row = selection[0].topRow();
    shared_ptr<Modifier> advantage = mPower->advantagesList().takeAt(row);
    mAdvantages->removeRow(row);
    updateForm();
}

void PowerDialog::deleteLimitation() {
    if (mEquipment) return;
    auto selection = mLimitations->selectedRanges();
    if (selection.empty()) return;

    int row = selection[0].topRow();
    shared_ptr<Modifier> limitation = mPower->limitationsList().takeAt(row);
    mLimitations->removeRow(row);
    updateForm();
}

void PowerDialog::editAdvantage() {
    if (mEquipment) return;
    auto selection = mAdvantages->selectedRanges();
    if (selection.empty()) return;

    int row = selection[0].topRow();
    shared_ptr<Modifier> advantage = mPower->advantagesList()[row];
    mMod = make_shared<ModifiersDialog>(ModifiersDialog::Advantage);
    bool res = mMod->modifier(advantage);
    if (!res) return;
    mMod->open();
}

void PowerDialog::editLimitation() {
    if (mEquipment) return;
    auto selection = mLimitations->selectedRanges();
    if (selection.empty()) return;

    int row = selection[0].topRow();
    shared_ptr<Modifier> limitation = mPower->limitationsList()[row];
    mMod = make_shared<ModifiersDialog>(ModifiersDialog::Limitation);
    if (!mMod->modifier(limitation)) return;
    mMod->open();
}

void PowerDialog::limitationsMenu(QPoint pos) {
    mLimitationsMenu->exec(mapToGlobal(pos
                                       + mLimitations->pos()
                                       - QPoint(0, ui->scrollArea->verticalScrollBar()->value())));

}

void PowerDialog::moveAdvantageDown() {
    if (mEquipment) return;
    auto selection = mAdvantages->selectedRanges();
    if (selection.empty()) return;

    int row = selection[0].topRow();
    if (row >= mPower->advantagesList().count() - 1) return;

    shared_ptr<Modifier> advantage = mPower->advantagesList().takeAt(row);
    mPower->advantagesList().insert(row + 1, advantage);
    updateForm();
}

void PowerDialog::moveLimitationDown() {
    if (mEquipment) return;
    auto selection = mLimitations->selectedRanges();
    if (selection.empty()) return;

    int row = selection[0].topRow();
    auto& limitations = mPower->limitationsList();
    if (row >= limitations.count() - 1) return;

    shared_ptr<Modifier> limitation = limitations.takeAt(row);
    limitations.insert(row + 1, limitation);
    updateForm();
}

void PowerDialog::moveAdvantageUp() {
    if (mEquipment) return;
    auto selection = mAdvantages->selectedRanges();
    if (selection.empty()) return;

    int row = selection[0].topRow();
    if (row < 1) return;

    auto& advantages = mPower->advantagesList();
    shared_ptr<Modifier> advantage = advantages.takeAt(row);
    advantages.insert(row - 1, advantage);
    updateForm();
}

void PowerDialog::moveLimitationUp() {
    if (mEquipment) return;
    auto selection = mLimitations->selectedRanges();
    if (selection.empty()) return;

    int row = selection[0].topRow();
    if (row < 1) return;

    auto& limitations = mPower->limitationsList();
    shared_ptr<Modifier> limitation = limitations.takeAt(row);
    limitations.insert(row - 1, limitation);
    updateForm();
}

void PowerDialog::newAdvantage() {
    if (mEquipment) return;
    mMod = make_shared<ModifiersDialog>(ModifiersDialog::Advantage);
    mMod->open();
}

void PowerDialog::newLimitation() {
    if (mEquipment) return;
    mMod = make_shared<ModifiersDialog>(ModifiersDialog::Limitation);
    mMod->open();
}

void PowerDialog::setupPower(shared_ptr<Power>& power) {
    power = Power::FromJson(mPower->name(), mPower->toJson());
    if (mEquipment) return;
    power->modifiers().clear();
    for (const auto& mod: std::as_const(power->advantagesList())) {
        if (mod == nullptr) continue;

        power->modifiers().append(mod);
    }
    for (const auto& mod: std::as_const(power->limitationsList())) {
        if (mod == nullptr) continue;

        power->modifiers().append(mod);
    }
}

void PowerDialog::ok() {
    mAccepted = mDone = true;
    Sheet& s = Sheet::ref();

    if (mSaved != nullptr && mSaved->parent() != nullptr) {
        Power* group = mSaved->parent();
        if (!group->isValid(mPower)) {
            if (mErrorMsg) mErrorMsg->setText("Invalid power. Too many points to fit into group or not valid for pool");
            return;
        }
    }

    bool add = (mSaved == nullptr);
    if (add) {
        shared_ptr<Power> work;
        setupPower(work);
        s.addPower(work);
    } else {
        setupPower(mSaved);
        _dummy.reset(new Power);
        _dummy = nullptr;
    }

    s.updateDisplay();
    s.changed();
    accept();
}

void PowerDialog::pasteAdvantage() {
    if (mEquipment) return;
    QClipboard* clip = QGuiApplication::clipboard();
    const QMimeData* data = clip->mimeData();
    QByteArray byteArray = data->data("application/advantage");
    QString jsonStr(byteArray);
    QJsonDocument json = QJsonDocument::fromJson(jsonStr.toUtf8());
    QJsonObject obj = json.object();
    QString name = obj["name"].toString();
    shared_ptr<Modifier> advantage = Modifiers::ByName(name)->create(obj);
    mPower->advantagesList().append(advantage);

    updateForm();
}

void PowerDialog::pasteLimitation() {
    if (mEquipment) return;
    QClipboard* clip = QGuiApplication::clipboard();
    const QMimeData* data = clip->mimeData();
    QByteArray byteArray = data->data("application/limitation");
    QString jsonStr(byteArray);
    QJsonDocument json = QJsonDocument::fromJson(jsonStr.toUtf8());
    QJsonObject obj = json.object();
    QString name = obj["name"].toString();
    shared_ptr<Modifier> limitation = Modifiers::ByName(name)->create(obj);
    mPower->limitationsList().append(limitation);

    updateForm();
}

void PowerDialog::pickOne(int) {
    if (mSkipUpdate) {
        mSkipUpdate = false;
        return;
    }

    QVBoxLayout* layout = dynamic_cast<QVBoxLayout*>(ui->form->layout());
    if (layout == nullptr) {
        layout = new QVBoxLayout(ui->form);
        ui->form->setLayout(layout);
    }

    QString text = ui->availableComboBox->currentText();
    if (text == "") return;

    mPower = Power::ByName(text);
    if (mInMultipower) mPower->inMultipower();
    try { mPower->createForm(this, layout); } catch (...) { accept(); }

    createLabel(layout, "");
    if (!mEquipment) {
        createLabel(layout, "Advantages");
        createAdvantages(this, layout);
        createEditButton(this, layout, {SLOT(newAdvantage()), SLOT(editAdvantage()), SLOT(deleteAdvantage()), SLOT(cutAdvantage()), SLOT(copyAdvantage()), SLOT(pasteAdvantage()), SLOT(moveAdvantageUp()), SLOT(moveAdvantageDown())});
        createLabel(layout, "");
        createLabel(layout, "Limitations");
        createLimitations(this, layout);
        createEditButton(this, layout, {SLOT(newLimitation()), SLOT(editLimitation()), SLOT(deleteLimitation()), SLOT(cutLimitation()), SLOT(copyLimitation()), SLOT(pasteLimitation()), SLOT(moveLimitationUp()), SLOT(moveLimitationDown())});
    }
    createLabel(layout, "");
    if (!mEquipment) mPoints = createLabel(layout, "-1 Points");
    else mPoints = nullptr;
    mDescription = createLabel(layout, "<incomplete>", WordWrap);
    mErrorMsg = createLabel(layout, "", WordWrap);

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
    case 12: available = Power::Equipment();            break;
    default: available = { };                           break;
    }

    for (const auto& power: std::as_const(available)) ui->availableComboBox->addItem(power);
    ui->availableComboBox->setEnabled(available.isEmpty() ? false : true);

    if (mSkipUpdate) {
        mSkipUpdate = false;
        return;
    }

    QVBoxLayout* layout = static_cast<QVBoxLayout*>(ui->form->layout());
    if (layout == nullptr) {
        layout = new QVBoxLayout(ui->form);
        ui->form->setLayout(layout);
    }

    mPower = nullptr;

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
    else if ((idx = Power::Equipment().indexOf(name)) != -1)            type = 12;
    else return *this;

    mSkipUpdate = true;
    ui->powerTypeComboBox->setCurrentIndex(type);
    ui->powerTypeComboBox->setEnabled(false);
    mSkipUpdate = true;
    ui->availableComboBox->setCurrentIndex(idx);
    ui->availableComboBox->setEnabled(false);

    QVBoxLayout* layout = static_cast<QVBoxLayout*>(ui->form->layout());
    if (layout == nullptr) {
        layout = new QVBoxLayout(ui->form);
        ui->form->setLayout(layout);
    }

    mPower = s;
    try { mPower->createForm(this, layout); } catch (...) { accept(); return *this; }
    if (type != 12) {
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
    }

    createLabel(layout, "");
    if (type != 12) mPoints = createLabel(layout, "-1 Points");
    mDescription = createLabel(layout, "<incomplete>", WordWrap);

    layout->addStretch(1);
    mPower = s;
    mPower->restore();
    updateForm();
    return *this;
}

void PowerDialog::activated(int) {
    QComboBox* comboBox = dynamic_cast<QComboBox*>(sender());
    mPower->callback(comboBox, true);
    updateForm();
}

void PowerDialog::buttonPressed(bool) {
    QPushButton* button = dynamic_cast<QPushButton*>(sender());
    mPower->callback(button);
    updateForm();
}

void PowerDialog::itemChanged(QTreeWidgetItem* itm, int) {
    QTreeWidget* tree = itm->treeWidget();
    mPower->callback(tree);
    updateForm();
}

void PowerDialog::currentIndexChanged(int) {
    QComboBox* comboBox = dynamic_cast<QComboBox*>(sender());
    mPower->callback(comboBox);
    updateForm();
}

void PowerDialog::itemSelectionChanged() {
    QTreeWidget* tree = dynamic_cast<QTreeWidget*>(sender());
    mPower->callback(tree);
    updateForm();
}

void PowerDialog::stateChanged(bool) {
    QCheckBox* checkBox = dynamic_cast<QCheckBox*>(sender());
    mPower->callback(checkBox);
    updateForm();
}

void PowerDialog::textChanged(QString) {
    QLineEdit* text = dynamic_cast<QLineEdit*>(sender());
    mPower->callback(text);
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
    if (mPower == nullptr) return;

    mPower->store();
    Points pts = mPower->real();
    if (!mEquipment) {
        if ((!mPower->isFramework() || mPower->isVPP() || mPower->isMultipower()) && pts.points == 0) pts = 1_cp;
        if (mPower->isVPP()) pts += mPower->pool();
        mPoints->setText(QString("%1 points").arg(pts.points));
    }
    QString descr = mPower->description(Power::ShowEND);
    if (!mPower->isEquipment()) {
        QFont font = mAdvantages->font();
        mAdvantages->setRowCount(0);
        mAdvantages->update();
        int row = 0;
        for (const auto& mod: std::as_const(mPower->advantagesList())) {
            QString val;
            if (mod->isAdder()) val = QString("%1").arg(mod->points(Modifier::NoStore).points);
            else val = mod->fraction(Modifier::NoStore).toString();
            descr += "; (+" + val + ") " + mod->description();
            setCellLabel(mAdvantages, row, 0, "+" + val, font);
            setCellLabel(mAdvantages, row, 1, mod->description(), font);
            row++;
        }
        setColumns(mAdvantages);

        mLimitations->setRowCount(0);
        mLimitations->update();
        row = 0;
        for (const auto& lim: std::as_const(mPower->limitationsList())) {
            descr += "; (" + lim->fraction(Modifier::NoStore).toString() + ") " + lim->description();
            setCellLabel(mLimitations, row, 0, lim->fraction(Modifier::NoStore).toString(), font);
            setCellLabel(mLimitations, row, 1, lim->description(), font);
            row++;
        }
        setColumns(mLimitations);
    }

    mDescription->setText(descr);
    if (mErrorMsg) mErrorMsg->setText("");
    mOk->setEnabled(mPower->description() != "<incomplete>");
}
