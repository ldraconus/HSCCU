#include "Equipment.h"
#include "complicationsdialog.h"
#include "complication.h"
#include "modifier.h"
#ifdef __wasm__
#include "editmenudialog.h"
#include "filemenudialog.h"
#endif
#include "optiondialog.h"
#include "powers.h"
#include "powerdialog.h"
#include "printdialog.h"
#include "printer.h"
#include "skilldialog.h"
#include "skilltalentorperk.h"

#include "sheet.h"
#ifdef __wasm__
#include "ui_wasm.h"
#else
#include "ui_sheet.h"
#endif
#include "sheet_ui.h"

#include "shared.h"

#ifdef _WIN64
#include <Shlobj.h>
#endif

#include <cmath>
#include <functional>

#include <QBuffer>
#include <QClipboard>
#include <QDirIterator>
#include <QFileDialog>
#include <QFontDatabase>
#include <QGestureEvent>
#include <QImageReader>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QMessageBox>
#include <QMimeData>
#include <QMouseEvent>
#include <QPageSetupDialog>
#include <QPainter>
#include <QPaintEngine>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QProcess>
#include <QSaveFile>
#include <QScrollBar>
#include <QScroller>
#include <QSettings>
#include <QStandardPaths>
#include <QStatusBar>
#include <QToolButton>
#include <QWindow>

Sheet* Sheet::sSheet = nullptr; // NOLINT
shared_ptr<class QMessageBox> Msg::Box; // NOLINT
std::function<void()>         Msg::mCancel; // NOLINT
std::function<void()>         Msg::mNo; // NOLINT
std::function<void()>         Msg::mOk; // NOLINT
std::function<void()>         Msg::mYes; // NOLINT

Msg msngr; // NOLINT

void Msg::button(QAbstractButton* btn) {
    switch (Msg::Box->standardButton(btn)) {
    case QMessageBox::Yes:
        Msg::mYes();
        break;

    case QMessageBox::No:
        Msg::mNo();
        break;

    case QMessageBox::Cancel:
        Msg::mCancel();
        break;

    default:
        break;
    }
}

// --- [static functions] ----------------------------------------------------------------------------------
constexpr int Base10 = 10;

static bool numeric(QString txt) {
    bool ok = false;
    txt.toInt(&ok, Base10);
    return ok;
}

void YesNo(const QString& msg, std::function<void()> yes, std::function<void()> no, const QString title) {
    Msg::Box = make_shared<QMessageBox>();
    Msg::Box->connect(Msg::Box.get(), SIGNAL(buttonClicked(QAbstractButton*)), &msngr, SLOT(button(QAbstractButton*)));
    Msg::mYes = yes;
    Msg::mNo  = no;
    Msg::Box->setIcon(QMessageBox::Question);
    Msg::Box->setText(!title.isEmpty() ? title : "Are you sure?");
    Msg::Box->setInformativeText(msg);
    Msg::Box->setStandardButtons({ QMessageBox::Yes, QMessageBox::No });
    Msg::Box->open();
}

void YesNoCancel(const QString& msg, std::function<void()> yes, std::function<void()> no, std::function<void()> cancel, const QString& title) {
    Msg::Box = make_shared<QMessageBox>();
    Msg::Box->connect(Msg::Box.get(), SIGNAL(buttonClicked(QAbstractButton*)), &msngr, SLOT(button(QAbstractButton*)));
    Msg::mYes = yes;
    Msg::mNo = no;
    Msg::mCancel = cancel;
    Msg::Box->setIcon(QMessageBox::Question);
    Msg::Box->setText(title.isEmpty() ? "Are you really sure?" : title);
    Msg::Box->setInformativeText(msg);
    Msg::Box->setStandardButtons({ QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel });
    Msg::Box->setDefaultButton(QMessageBox::Cancel);
    Msg::Box->open();
}

void OK(const QString& msg, std::function<void ()> ok, const QString& title) {
    Msg::Box = make_shared<QMessageBox>();
    Msg::Box->connect(Msg::Box.get(), SIGNAL(buttonClicked(QAbstractButton*)), &msngr, SLOT(button(QAbstractButton*)));
    Msg::mOk = ok;
    Msg::Box->setIcon(QMessageBox::Warning);
    Msg::Box->setText(!title.isEmpty() ? title : "Something has happened.");
    Msg::Box->setInformativeText(msg);
    Msg::Box->setStandardButtons(QMessageBox::Ok);
    Msg::Box->setDefaultButton(QMessageBox::Ok);
    Msg::Box->open();
}

void OKCancel(const QString& msg, std::function<void ()> ok, std::function<void ()> cancel, const QString& title) {
    Msg::Box = make_shared<QMessageBox>();
    Msg::Box->connect(Msg::Box.get(), SIGNAL(buttonClicked(QAbstractButton*)), &msngr, SLOT(button(QAbstractButton*)));
    Msg::mOk = ok;
    Msg::Box->setIcon(QMessageBox::Critical);
    Msg::Box->setText(!title.isEmpty() ? title : "Something bad is about to happened.");
    Msg::Box->setInformativeText(msg);
    Msg::Box->setStandardButtons(QMessageBox::Ok);
    Msg::Box->setDefaultButton(QMessageBox::Ok);
    Msg::Box->open();
}

void Question(const QString& msg,  std::function<void ()> yes, std::function<void ()> no, const QString& title) {
    Msg::Box = make_shared<QMessageBox>();
    Msg::Box->connect(Msg::Box.get(), SIGNAL(buttonClicked(QAbstractButton*)), &msngr, SLOT(button(QAbstractButton*)));
    Msg::mYes = yes;
    Msg::mNo  = no;
    Msg::Box->setIcon(QMessageBox::Question);
    Msg::Box->setText(!title.isEmpty() ? title : "Are you sure?");
    Msg::Box->setInformativeText(msg);
    Msg::Box->setStandardButtons({ QMessageBox::Yes, QMessageBox::No });
    Msg::Box->open();
}

void Statement(const QString& msg) {
    Msg::Box = make_shared<QMessageBox>();
    Msg::Box->connect(Msg::Box.get(), SIGNAL(buttonClicked(QAbstractButton*)), &msngr, SLOT(button(QAbstractButton*)));
    Msg::mOk = std::bind(&Sheet::doNothing, &Sheet::ref());
    Msg::Box->setInformativeText(msg);
    Msg::Box->setStandardButtons(QMessageBox::Ok);
    Msg::Box->setDefaultButton(QMessageBox::Ok);
    Msg::Box->open();
}

static class lift { // NOLINT
public:
    lift(int x, int y, int z, const QString& s)
        : STR(x)
        , _lift(y)
        , toss(z)
        , suffix(s) { }
    long STR;
    long _lift;
    long toss;
    QString suffix;
} strTable[] { // NOLINT
    { 0,          0,   0, "kg" },    // NOLINT
    { 1,          8,   2, "kg" },    // NOLINT
    { 2,         16,   3, "kg" },    // NOLINT
    { 3,         25,   4, "kg" },    // NOLINT
    { 4,         38,   6, "kg" },    // NOLINT
    { 5,         50,   8, "kg" },    // NOLINT
    { 8,         75,  12, "kg" },    // NOLINT
    { 10,       100,  16, "kg" },    // NOLINT
    { 13,       150,  20, "kg" },    // NOLINT
    { 15,       200,  24, "kg" },    // NOLINT
    { 18,       300,  28, "kg" },    // NOLINT
    { 20,       400,  32, "kg" },    // NOLINT
    { 23,       600,  36, "kg" },    // NOLINT
    { 25,       800,  40, "kg" },    // NOLINT
    { 28,      1200,  44, "kg" },    // NOLINT
    { 30,      1600,  48, "kg" },    // NOLINT
    { 35,      3200,  56, "kg" },    // NOLINT
    { 40,      6400,  64, "kg" },    // NOLINT
    { 45,     12500,  72, "tons" },  // NOLINT
    { 50,     25000,  80, "tons" },  // NOLINT
    { 55,     50000,  88, "tons" },  // NOLINT
    { 60,    100000,  96, "tons" },  // NOLINT
    { 65,    200000, 104, "tons" },  // NOLINT
    { 70,    400000, 112, "tons" },  // NOLINT
    { 75,    800000, 120, "tons" },  // NOLINT
    { 80,   1600000, 128, "ktons" }, // NOLINT
    { 85,   3200000, 136, "ktons" }, // NOLINT
    { 90,   6400000, 144, "ktons" }, // NOLINT
    { 95,  12500000, 152, "ktons" }, // NOLINT
    { 100, 25000000, 160, "ktons" }  // NOLINT
};

// A is value at 0
// B is value at T
// t is distance from A
// T is distance from A to B
static qlonglong interpolate(int A, int B, int t, int T) {
    double val = (double) A * std::pow((double) B / (double) A, (double) t / (double) T);
    return (qlonglong) (val + 0.5); // NOLINT
}

static int indexOf(int str) {
    int i = 0;
    for (; strTable[i].STR != 100 && strTable[i].STR < str; ++i) // NOLINT
        ;
    return (strTable[i].STR != 100) ? i : -1; // NOLINT
}

static qlonglong interpolateLift(int str) {
    if (str > 95) { // NOLINT
        int idx = indexOf(95); // NOLINT
        return interpolate(strTable[idx]._lift, strTable[idx + 1]._lift, str - 95, 5); // NOLINT
    }
    int idx = indexOf(str);
    return interpolate(strTable[idx]._lift, strTable[idx + 1]._lift, str - strTable[idx].STR, strTable[idx + 1].STR - strTable[idx].STR); // NOLINT
}

static QString liftUnits(int str) {
    if (str > 95) return "ktons"; // NOLINT
    return strTable[indexOf(str)].suffix; // NOLINT
}

static QString formatNumber(int num) {
    QLocale locale;
    return locale.toString(num);
}

static QString formatNumber(double num) {
    QLocale locale;
    return locale.toString(num, 'f', 1);
}

// --- [creation/destruction] -----------------------------------------------------------------------------

Sheet_UI Sheet::sSheet_UI; // NOLINT

class Sheet::Dialogs sDialog;

Sheet::Sheet(QWidget *parent)
    : QMainWindow(parent)
#ifdef __wasm__
    , mUi(new Ui::wasm)
#else
    , mUi(new Ui::Sheet)
#endif
    , mUI(&sSheet_UI)
    , mSaveChanged(false) {

    sSheet = this;

    mUi->setupUi(this);

    mUi->graphicsView->setStyleSheet("color: #000; background: #fff");
    mUI->setupUi(nullptr, nullptr);
    mUI->mWidget->setStyleSheet("color: #000; background: #fff");

#ifdef Q_OS_ANDROID
    mUi->menubar->setNativeMenuBar(false);
    auto* scrollWidget = mUi->graphicsView;
    QScroller::grabGesture(scrollWidget->viewport(), QScroller::TouchGesture);
    for (auto* table: findChildren<QTableWidget*>()) {
        table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        QScroller::grabGesture(table->viewport(), QScroller::TouchGesture);
    }

    qApp->setStyleSheet(qApp->styleSheet() + R"(QScrollBar:vertical {
                                                    background: #F0F0F0;
                                                    width: 24px;
                                                    margin: 0px;
                                                }

                                                QScrollBar::handle:vertical {
                                                    background: #A0A0A0;
                                                    min-height: 48px;
                                                    border: 1px solid #808080;
                                                    border-radius: 8px;
                                                    margin: 2px;
                                                }

                                                QScrollBar:horizontal {
                                                    background: #F0F0F0;
                                                    height: 24px;
                                                    margin: 0px;
                                                }

                                                QScrollBar::handle:horizontal {
                                                    background: #A0A0A0;
                                                    min-width: 48px;
                                                    border: 1px solid #808080;
                                                    border-radius: 8px;
                                                    margin: 2px;
                                                }

                                                QScrollBar::add-line,
                                                QScrollBar::sub-line {
                                                    width: 0px;
                                                    height: 0px;
                                                }

                                                QScrollBar::add-page,
                                                QScrollBar::sub-page {
                                                    background: none;
                                                }

                                                QMenu::item {
                                                    color: black;
                                                    background-color: white;
                                                }

                                                QMenu::item:selected {
                                                    color: white;
                                                    background-color: #707070;
                                                }

                                                QMenu::item:disabled {
                                                    color: #A0A0A0;
                                                    background-color: #909090;
                                                })");
#endif
    setupIcons();
    setUnifiedTitleAndToolBarOnMac(true);
    setAttribute(Qt::WA_AcceptTouchEvents);

    Modifiers mods;

    mDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    mOption.load();

    connect(qApp, &QApplication::focusChanged, this, &Sheet::focusChanged);

    updateBanner();

#if !defined(__wasm__)
    connect(mUi->menu_File,     &QMenu::aboutToShow, this, &Sheet::aboutToShowFileMenu);
    connect(mUi->menu_File,     &QMenu::aboutToHide, this, &Sheet::aboutToHideFileMenu);
#ifdef Q_OS_ANDROID
    connect(mUi->action_New,    &QAction::triggered, this, [this] { QTimer::singleShot(100, this, [this]() { Sheet::newchar();        }); }, Qt::QueuedConnection);
    connect(mUi->action_Open,   &QAction::triggered, this, [this] { QTimer::singleShot(100, this, [this]() { Sheet::open();           }); }, Qt::QueuedConnection);
    connect(mUi->action_Save,   &QAction::triggered, this, [this] { QTimer::singleShot(100, this, [this]() { Sheet::save();           }); }, Qt::QueuedConnection);
    connect(mUi->actionSave_As, &QAction::triggered, this, [this] { QTimer::singleShot(100, this, [this]() { Sheet::saveAs();         }); }, Qt::QueuedConnection);
    connect(mUi->action_Print,  &QAction::triggered, this, [this] { QTimer::singleShot(100, this, [this]() { Sheet::printSheet();     }); }, Qt::QueuedConnection);
    connect(mUi->actionE_xit,   &QAction::triggered, this, [this] { QTimer::singleShot(100, this, [this]() { Sheet::exitClicked();    }); }, Qt::QueuedConnection);
    connect(mUi->actionOptions, &QAction::triggered, this, [this] { QTimer::singleShot(100, this, [this]() { Sheet::options();        }); }, Qt::QueuedConnection);
    connect(mUi->actionOptions, &QAction::triggered, this, [this] { QTimer::singleShot(100, this, [this]() { Sheet::cutCharacter();   }); }, Qt::QueuedConnection);
    connect(mUi->action_Paste,  &QAction::triggered, this, [this] { QTimer::singleShot(100, this, [this]() { Sheet::pasteCharacter(); }); }, Qt::QueuedConnection);
#else
    connect(mUi->action_New,    &QAction::triggered, this, &Sheet::newchar);
    connect(mUi->action_Open,   &QAction::triggered, this, &Sheet::open);
    connect(mUi->action_Save,   &QAction::triggered, this, &Sheet::save);
    connect(mUi->actionSave_As, &QAction::triggered, this, &Sheet::saveAs);
    connect(mUi->action_Print,  &QAction::triggered, this, &Sheet::printSheet);
    connect(mUi->actionE_xit,   &QAction::triggered, this, &Sheet::exitClicked);
    connect(mUi->actionOptions, &QAction::triggered, this, &Sheet::options);
    connect(mUi->action_Cut,    &QAction::triggered, this, &Sheet::cutCharacter);
    connect(mUi->action_Paste,  &QAction::triggered, this, &Sheet::pasteCharacter);
#endif
    connect(mUi->menu_Edit,     &QMenu::aboutToShow, this, &Sheet::aboutToShowEditMenu);
    connect(mUi->menu_Edit,     &QMenu::aboutToHide, this, &Sheet::aboutToHideEditMenu);
    connect(mUi->actionC_opy,   &QAction::triggered, this, &Sheet::copyCharacter);
#else
    fileButton = createToolBarItem(mUi->menuBar, "File", "File menu");
    connect(fileButton, &QToolButton::clicked, this, &Sheet::fileMenu);
    createMenuItem(action_New,  "action_New",  SLOT(newchar()));
    createMenuItem(action_Open, "action_Open", SLOT(open()));
    createMenuItem(action_Save, "action_Save", SLOT(save()));

    editButton = createToolBarItem(mUi->menuBar, "Edit", "Edit menu");
    connect(editButton, &QToolButton::clicked, this, &Sheet::editMenu);
    createMenuItem(action_Cut,    "action_Cut",    SLOT(cutCharacter()));
    createMenuItem(actionC_opy,   "actionC_opy",   SLOT(copyCharacter()));
    createMenuItem(action_Paste,  "action_Paste",  SLOT(pasteCharacter()));
    createMenuItem(actionOptions, "actionOptions", SLOT(options()));

    viewButton = createToolBarItem(mUi->menuBar, "View", "View Menu");
    connect(viewButton, &QToolButton::clicked, this, &Sheet::viewMenu);

    imageButton = createToolBarItem(mUi->menuBar, "Image", "Image menu");
    connect(imageButton, &QToolButton::clicked, this, &Sheet::imgMenu);

    skillsTalentsAndPerksButton = createToolBarItem(mUi->menuBar, "Skills", "Skills, Talents, & Perks menu");
    connect(skillsTalentsAndPerksButton, &QToolButton::clicked, this, &Sheet::stpMenu);

    complicationsButton = createToolBarItem(mUi->menuBar, "Complications", "Complications menu");
    connect(complicationsButton, &QToolButton::clicked, this, &Sheet::compMenu);

    powersAndEquipmentButton = createToolBarItem(mUi->menuBar, "Power", "Power & Equipment menu");
    connect(powersAndEquipmentButton, &QToolButton::clicked, this, &Sheet::powerMenu);
#endif

    connect(mUI->alternateids,          &QLineEdit::textEdited,       this, &Sheet::alternateIdsChanged);
    connect(mUI->bodyval,               &QLineEdit::textEdited,       this, &Sheet::valChanged);
    connect(mUI->bodyval,               &QLineEdit::editingFinished,  this, &Sheet::valEditingFinished);
    connect(mUI->campaignname,          &QLineEdit::textEdited,       this, &Sheet::campaignNameChanged);
    connect(mUI->charactername,         &QLineEdit::textEdited,       this, &Sheet::characterNameChanged);
    connect(mUI->conval,                &QLineEdit::textEdited,       this, &Sheet::valChanged);
    connect(mUI->conval,                &QLineEdit::editingFinished,  this, &Sheet::valEditingFinished);
    connect(mUI->currentbody,           &QLineEdit::textEdited,       this, &Sheet::currentBODYChanged);
    connect(mUI->currentbody,           &QLineEdit::editingFinished,  this, &Sheet::currentBODYEditingFinished);
    connect(mUI->currentend,            &QLineEdit::textEdited,       this, &Sheet::currentENDChanged);
    connect(mUI->currentend,            &QLineEdit::editingFinished,  this, &Sheet::currentENDEditingFinished);
    connect(mUI->currentstun,           &QLineEdit::textEdited,       this, &Sheet::currentSTUNChanged);
    connect(mUI->currentstun,           &QLineEdit::editingFinished,  this, &Sheet::currentSTUNEditingFinished);
    connect(mUI->dcvval,                &QLineEdit::textEdited,       this, &Sheet::valChanged);
    connect(mUI->dcvval,                &QLineEdit::editingFinished,  this, &Sheet::valEditingFinished);
    connect(mUI->dexval,                &QLineEdit::textEdited,       this, &Sheet::valChanged);
    connect(mUI->dexval,                &QLineEdit::editingFinished,  this, &Sheet::valEditingFinished);
    connect(mUI->dmcvval,               &QLineEdit::textEdited,       this, &Sheet::valChanged);
    connect(mUI->dmcvval,               &QLineEdit::editingFinished,  this, &Sheet::valEditingFinished);
    connect(mUI->edval,                 &QLineEdit::textEdited,       this, &Sheet::valChanged);
    connect(mUI->edval,                 &QLineEdit::editingFinished,  this, &Sheet::valEditingFinished);
    connect(mUI->egoval,                &QLineEdit::textEdited,       this, &Sheet::valChanged);
    connect(mUI->egoval,                &QLineEdit::editingFinished,  this, &Sheet::valEditingFinished);
    connect(mUI->endval,                &QLineEdit::textEdited,       this, &Sheet::valChanged);
    connect(mUI->endval,                &QLineEdit::editingFinished,  this, &Sheet::valEditingFinished);
    connect(mUI->eyecolor,              &QLineEdit::textEdited,       this, &Sheet::eyeColorChanged);
    connect(mUI->gamemaster,            &QLineEdit::textEdited,       this, &Sheet::gamemasterChanged);
    connect(mUI->genre,                 &QLineEdit::textEdited,       this, &Sheet::genreChanged);
    connect(mUI->haircolor,             &QLineEdit::textEdited,       this, &Sheet::hairColorChanged);
    connect(mUI->intval,                &QLineEdit::textEdited,       this, &Sheet::valChanged);
    connect(mUI->intval,                &QLineEdit::editingFinished,  this, &Sheet::valEditingFinished);
    connect(mUI->ocvval,                &QLineEdit::textEdited,       this, &Sheet::valChanged);
    connect(mUI->ocvval,                &QLineEdit::editingFinished,  this, &Sheet::valEditingFinished);
    connect(mUI->omcvval,               &QLineEdit::textEdited,       this, &Sheet::valChanged);
    connect(mUI->omcvval,               &QLineEdit::editingFinished,  this, &Sheet::valEditingFinished);
    connect(mUI->pdval,                 &QLineEdit::textEdited,       this, &Sheet::valChanged);
    connect(mUI->pdval,                 &QLineEdit::editingFinished,  this, &Sheet::valEditingFinished);
    connect(mUI->playername,            &QLineEdit::textEdited,       this, &Sheet::playerNameChanged);
    connect(mUI->preval,                &QLineEdit::textEdited,       this, &Sheet::valChanged);
    connect(mUI->preval,                &QLineEdit::editingFinished,  this, &Sheet::valEditingFinished);
    connect(mUI->recval,                &QLineEdit::textEdited,       this, &Sheet::valChanged);
    connect(mUI->recval,                &QLineEdit::editingFinished,  this, &Sheet::valEditingFinished);
    connect(mUI->spdval,                &QLineEdit::textEdited,       this, &Sheet::valChanged);
    connect(mUI->spdval,                &QLineEdit::editingFinished,  this, &Sheet::valEditingFinished);
    connect(mUI->strval,                &QLineEdit::textEdited,       this, &Sheet::valChanged);
    connect(mUI->strval,                &QLineEdit::editingFinished,  this, &Sheet::valEditingFinished);
    connect(mUI->stunval,               &QLineEdit::textEdited,       this, &Sheet::valChanged);
    connect(mUI->stunval,               &QLineEdit::editingFinished,  this, &Sheet::valEditingFinished);
    connect(mUI->totalexperienceearned, &QLineEdit::textEdited,       this, &Sheet::totalExperienceEarnedChanged);
    connect(mUI->totalexperienceearned, &QLineEdit::editingFinished,  this, &Sheet::totalExperienceEarnedEditingFinished);
    connect(mUI->height,                &QLineEdit::textEdited,       this, &Sheet::heightChanged);
    connect(mUI->weight,                &QLineEdit::textEdited,       this, &Sheet::weightChanged);
    connect(mUI->notes,                 &QPlainTextEdit::textChanged, this, &Sheet::noteChanged);

#ifndef __wasm__
    connect(mUi->action0_5,             &QAction::triggered,          this, [this] { zoom(dynamic_cast<QObject*>(sender())); });
    connect(mUi->action0_75,            &QAction::triggered,          this, [this] { zoom(dynamic_cast<QObject*>(sender())); });
    connect(mUi->action0_9,             &QAction::triggered,          this, [this] { zoom(dynamic_cast<QObject*>(sender())); });
    connect(mUi->action1_0,             &QAction::triggered,          this, [this] { zoom(dynamic_cast<QObject*>(sender())); });
    connect(mUi->action1_25,            &QAction::triggered,          this, [this] { zoom(dynamic_cast<QObject*>(sender())); });
    connect(mUi->action1_5,             &QAction::triggered,          this, [this] { zoom(dynamic_cast<QObject*>(sender())); });
    connect(mUi->action2_0,             &QAction::triggered,          this, [this] { zoom(dynamic_cast<QObject*>(sender())); });
    connect(mUi->action3_0,             &QAction::triggered,          this, [this] { zoom(dynamic_cast<QObject*>(sender())); });
    connect(mUi->actionZoom_In,         &QAction::triggered,          this, [this] { zoomIn(); });
    connect(mUi->actionZoom_Out,        &QAction::triggered,          this, [this] { zoomOut(); });

    mZooms.append(mUi->action0_5);
    mZooms.append(mUi->action0_75);
    mZooms.append(mUi->action0_9);
    mZooms.append(mUi->action1_0);
    mZooms.append(mUi->action1_25);
    mZooms.append(mUi->action1_5);
    mZooms.append(mUi->action2_0);
    mZooms.append(mUi->action3_0);
#endif

    setTableSelectionMode(mUI->skillstalentsandperks);
    setTableSelectionMode(mUI->complications);
    setTableSelectionMode(mUI->powersandequipment);


    connect(mUI->image,      &QMenu::customContextMenuRequested, this, &Sheet::imageMenu);
    connect(mUI->newImage,   &QAction::triggered,                this, &Sheet::newImage);
    connect(mUI->clearImage, &QAction::triggered,                this, &Sheet::clearImage);


    connect(mUI->complications,        &ClickableTable::itemDoubleClicked, this, &Sheet::complicationDoubleClicked);
    connect(mUI->complications,        &QMenu::customContextMenuRequested, this, &Sheet::complicationsMenu);
#if !defined(__wasm__) && !defined(Q_OS_ANDROID)
    connect(mUI->complicationsMenu,    &QMenu::aboutToShow,                this, &Sheet::aboutToShowComplicationsMenu);
#endif
    connect(mUI->newComplication,      &QAction::triggered,                this, &Sheet::newComplication);
    connect(mUI->editComplication,     &QAction::triggered,                this, &Sheet::editComplication);
    connect(mUI->deleteComplication,   &QAction::triggered,                this, &Sheet::deleteComplication);
    connect(mUI->cutComplication,      &QAction::triggered,                this, &Sheet::cutComplication);
    connect(mUI->copyComplication,     &QAction::triggered,                this, &Sheet::copyComplication);
    connect(mUI->pasteComplication,    &QAction::triggered,                this, &Sheet::pasteComplication);
    connect(mUI->moveComplicationUp,   &QAction::triggered,                this, &Sheet::moveComplicationUp);
    connect(mUI->moveComplicationDown, &QAction::triggered,                this, &Sheet::moveComplicationDown);


    connect(mUI->skillstalentsandperks,     &ClickableTable::itemDoubleClicked, this, &Sheet::skillstalentsandperksDoubleClicked);
    connect(mUI->skillstalentsandperks,     &QMenu::customContextMenuRequested, this, &Sheet::skillstalentsandperksMenu);
#if !defined(__wasm__) && !defined(Q_OS_ANDROID)
    connect(mUI->skillstalentsandperksMenu, &QMenu::aboutToShow,                this, &Sheet::aboutToShowSkillsPerksAndTalentsMenu);
#endif
    connect(mUI->newSkillTalentOrPerk,      &QAction::triggered,                this, &Sheet::newSkillTalentOrPerk);
    connect(mUI->editSkillTalentOrPerk,     &QAction::triggered,                this, &Sheet::editSkillstalentsandperks);
    connect(mUI->deleteSkillTalentOrPerk,   &QAction::triggered,                this, &Sheet::deleteSkillstalentsandperks);
    connect(mUI->cutSkillTalentOrPerk,      &QAction::triggered,                this, &Sheet::cutSkillTalentOrPerk);
    connect(mUI->copySkillTalentOrPerk,     &QAction::triggered,                this, &Sheet::copySkillTalentOrPerk);
    connect(mUI->pasteSkillTalentOrPerk,    &QAction::triggered,                this, &Sheet::pasteSkillTalentOrPerk);
    connect(mUI->moveSkillTalentOrPerkUp,   &QAction::triggered,                this, &Sheet::moveSkillTalentOrPerkUp);
    connect(mUI->moveSkillTalentOrPerkDown, &QAction::triggered,                this, &Sheet::moveSkillTalentOrPerkDown);


    connect(mUI->powersandequipment,       &ClickableTable::itemDoubleClicked, this, &Sheet::powersandequipmentDoubleClicked);
    connect(mUI->powersandequipment,       &QMenu::customContextMenuRequested, this, &Sheet::powersandequipmentMenu);
#if !defined(__wasm__) && !defined(Q_OS_ANDROID)
    connect(mUI->powersandequipmentMenu,   &QMenu::aboutToShow,                this, &Sheet::aboutToShowPowersAndEquipmentMenu);
#endif
    connect(mUI->newPowerOrEquipment,      &QAction::triggered,                this, &Sheet::newPowerOrEquipment);
    connect(mUI->editPowerOrEquipment,     &QAction::triggered,                this, &Sheet::editPowerOrEquipment);
    connect(mUI->deletePowerOrEquipment,   &QAction::triggered,                this, &Sheet::deletePowerOrEquipment);
    connect(mUI->cutPowerOrEquipment,      &QAction::triggered,                this, &Sheet::cutPowerOrEquipment);
    connect(mUI->copyPowerOrEquipment,     &QAction::triggered,                this, &Sheet::copyPowerOrEquipment);
    connect(mUI->pastePowerOrEquipment,    &QAction::triggered,                this, &Sheet::pastePowerOrEquipment);
    connect(mUI->movePowerOrEquipmentUp,   &QAction::triggered,                this, &Sheet::movePowerOrEquipmentUp);
    connect(mUI->movePowerOrEquipmentDown, &QAction::triggered,                this, &Sheet::movePowerOrEquipmentDown);


    mWidget2Def = {
        { mUI->strval,  { &mCharacter.STR(),  mUI->strval,  mUI->strpoints, mUI->strroll } },
        { mUI->dexval,  { &mCharacter.DEX(),  mUI->dexval,  mUI->dexpoints, mUI->dexroll } },
        { mUI->conval,  { &mCharacter.CON(),  mUI->conval,  mUI->conpoints, mUI->conroll } },
        { mUI->intval,  { &mCharacter.INT(),  mUI->intval,  mUI->intpoints, mUI->introll } },
        { mUI->egoval,  { &mCharacter.EGO(),  mUI->egoval,  mUI->egopoints, mUI->egoroll } },
        { mUI->preval,  { &mCharacter.PRE(),  mUI->preval,  mUI->prepoints, mUI->preroll } },
        { mUI->ocvval,  { &mCharacter.OCV(),  mUI->ocvval,  mUI->ocvpoints } },
        { mUI->dcvval,  { &mCharacter.DCV(),  mUI->dcvval,  mUI->dcvpoints } },
        { mUI->omcvval, { &mCharacter.OMCV(), mUI->omcvval, mUI->omcvpoints } },
        { mUI->dmcvval, { &mCharacter.DMCV(), mUI->dmcvval, mUI->dmcvpoints } },
        { mUI->spdval,  { &mCharacter.SPD(),  mUI->spdval,  mUI->spdpoints } },
        { mUI->pdval,   { &mCharacter.PD(),   mUI->pdval,   mUI->pdpoints } },
        { mUI->edval,   { &mCharacter.ED(),   mUI->edval,   mUI->edpoints } },
        { mUI->recval,  { &mCharacter.REC(),  mUI->recval,  mUI->recpoints } },
        { mUI->endval,  { &mCharacter.END(),  mUI->endval,  mUI->endpoints } },
        { mUI->bodyval, { &mCharacter.BODY(), mUI->bodyval, mUI->bodypoints } },
        { mUI->stunval, { &mCharacter.STUN(), mUI->stunval, mUI->stunpoints } }
    };

    for (auto it = mWidget2Def.begin(); it != mWidget2Def.end(); ++it) it->value()->installEventFilter(this);

#ifndef __wasm__
#ifdef Q_OS_ANDROID
    connect(qApp, &QGuiApplication::applicationStateChanged, this, [this](Qt::ApplicationState state) {
        if (state == Qt::ApplicationInactive ||
            state == Qt::ApplicationHidden ||
            state == Qt::ApplicationSuspended) saveRecoveryState();
    });
    recoverState();
#else
    QStringList args = qApp->arguments(); // NOLINT
    if (args.count() > 1) {
        mFilename = QDir::fromNativeSeparators(args[1]);
        fileOpen();
        QProcess subfile;
        subfile.setProgram(args[0]);
        for (int i = 2; i < args.count(); ++i) {
            QStringList subArgs { args[i] };
            subfile.setArguments(subArgs);
            subfile.startDetached();
        }
    }
#endif
#else
#endif
}

Sheet::~Sheet() {
    delete mUi;
    // Ui's contents are pointed to by ui->label, don't delete it (double deletes)!
    // Don't worry, the delete of ui->label delete everything Ui points to as well.
}

Sheet::Dialogs Sheet::sDialog{};

// --- [EVENT FILTER] ----------------------------------------------------------------------------------

bool Sheet::eventFilter(QObject* object, QEvent* event) {
    if (object == mUi->graphicsView->viewport() && event->type() == QEvent::Gesture) {
        auto* ge = static_cast<QGestureEvent*>(event);
        if (auto* pinch = static_cast<QPinchGesture*>(ge->gesture(Qt::PinchGesture))) {
            if (pinch->state() == Qt::GestureStarted) mStartScale = mUi->graphicsView->transform().m11();

            qreal scale = mStartScale * pinch->totalScaleFactor();

            scale = qBound(0.5, scale, 3.0);
            zoom(scale);

            return true;
        }
    }

    if (event->type() == QEvent::Wheel) {
        auto* we = static_cast<QWheelEvent*>(event);
        if (we->modifiers().testFlag(Qt::ControlModifier)) {
            static constexpr auto zoomStep = 0.05;
            if (we->angleDelta().y() > 0)      zoom(mStartScale + zoomStep);
            else if (we->angleDelta().y() < 0) zoom(mStartScale - zoomStep);
            return true;
        }
    }

    if (event->type() == QEvent::FocusIn && mWidget2Def.find(object) != mWidget2Def.end()) {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(object);
        edit->setText(QString("%1").arg(mWidget2Def[edit].characteristic()->base()));
    }

    return QMainWindow::eventFilter(object, event);
}

static void closeDialog(shared_ptr<QDialog> dlg, QMouseEvent* me) {
    if (dlg == nullptr) return;
    QRect dialogRect = dlg->geometry();
    if (dlg->isVisible() && (me == nullptr || !dialogRect.contains(me->pos()))) dlg->done(QDialog::Rejected);
}

void Sheet::closeDialogs(QMouseEvent* me) {
#if defined(__wasm__) || defined(Q_OS_ANDROID)
    if (sDialog.ComplicationsMenu != nullptr) closeDialog(sDialog.ComplicationsMenu, me);
#ifdef __wasm__
    if (sDialog.EditMenu          != nullptr) closeDialog(sDialog.EditMenu,          me);
    if (sDialog.FileMenu          != nullptr) closeDialog(sDialog.FileMenu,          me);
    if (sDialog.ViewMenu          != nullptr) closeDialog(sDialog.ViewMenu,          me);
#endif
    if (sDialog.ImgMenu           != nullptr) closeDialog(sDialog.ImgMenu,           me);
    if (sDialog.SkillMenu         != nullptr) closeDialog(sDialog.SkillMenu,         me);
    if (sDialog.PowerMenu         != nullptr) closeDialog(sDialog.PowerMenu,         me);
#endif
    if (sDialog.Print             != nullptr) closeDialog(sDialog.Print,             me);
    if (sDialog.Option            != nullptr) closeDialog(sDialog.Option,            me);
    if (sDialog.Complications     != nullptr) closeDialog(sDialog.Complications,     me);
    if (sDialog.Power             != nullptr) closeDialog(sDialog.Power,             me);
    if (sDialog.Skill             != nullptr) closeDialog(sDialog.Skill,             me);
}

void Sheet::mousePressEvent(QMouseEvent* me) {
    closeDialogs(me);
}

void Sheet::showEvent(QShowEvent* se) {
    QMainWindow::showEvent(se);
    QTimer::singleShot(0, this, [this]() { setNotes(mOption.showNotesPage()); });
}

void Sheet::closeEvent(QCloseEvent* event) {
    if (checkClose()) event->accept();
    else event->ignore();
}

bool Sheet::event(QEvent* e) {
    return QMainWindow::event(e);
}

// --- [WORK] -------------------------------------------------------------------------------------------

QList<QList<int>> phases {                    // NOLINT
    { },
    { 7 },                                    // NOLINT
    { 6, 12 },                                // NOLINT
    { 4, 8, 12 },                             // NOLINT
    { 3, 6, 9, 12 },                          // NOLINT
    { 3, 5, 8, 10, 12 },                      // NOLINT
    { 2, 4, 6, 8, 10, 12 },                   // NOLINT
    { 2, 4, 6, 7, 9, 11, 12 },                // NOLINT
    { 2, 3, 5, 6, 8, 9, 11, 12 },             // NOLINT
    { 2, 3, 4, 6, 7, 8, 10, 11, 12 },         // NOLINT
    { 2, 3, 4, 5, 6, 8, 9, 10, 11, 12 },      // NOLINT
    { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 },   // NOLINT
    { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 } // NOLINT
};

void Sheet::addPower(shared_ptr<Power> power) {
    if (power == nullptr) return;

    int row = -1;
    auto selection = mUI->powersandequipment->selectedItems();
    if (!selection.isEmpty()) {
        row = selection[0]->row();
        putPower(row, power);
    } else mCharacter.powersOrEquipment().append(power);

    power->modifiers().clear();
    for (const auto& mod: std::as_const(power->advantagesList())) power->modifiers().append(mod);
    for (const auto& mod: std::as_const(power->limitationsList())) power->modifiers().append(mod);

    updatePower(power);
    updateDisplay();
    mChanged = true;
}

void Sheet::fixButtonBox(QDialogButtonBox *bb) {
    bb->setStyleSheet("QDialogButtonBox { border-color: #888; color: #888; background: #fff; } "
                      "QDialogButtonBox:default { border-color: #000; color: #000; } ");
    QList<QAbstractButton*> buttons = bb->buttons();
    for (auto i = 0 ; i < buttons.count(); ++i) {
        auto* button = buttons[i];
        button->setStyleSheet("QPushButton { "
                              "  background-color: cyan; "
                              "  color: black; "
                              "  border: 1px solid gray; "
                              "  height: 23; "
                              "  width: 75; "
                              "  border-radius: 6px; "
                              "} "
                              "QPushButton:default { "
                              "  border: 3px solid gray; "
                              "}"
                              "QPushButton:disabled { "
                              "  color: gray; "
                              "}"
                              );
    }
}

Points Sheet::characteristicsCost() {
    Points total = 0_cp;
    const auto keys = mWidget2Def.keys();
    for (const auto& key: std::as_const(keys)) total += mWidget2Def[key].characteristic()->points();
    mUI->totalcost->setText(QString("%1").arg(total.points));
    return total;
}

void Sheet::characteristicChanged(QLineEdit* val, QString txt, bool update) {
    if (txt.contains('/')) {
        auto values = txt.split("/");
        if (values.count() != 2 ||
            !numeric(values[0]) || !numeric(values[1])) {
            val->undo();
            return;
        }
        val->setText(txt = QString("%1").arg(mWidget2Def[val].characteristic()->base()));
    }
    if (numeric(txt) || txt.isEmpty()) {
        auto& def = mWidget2Def[val];
        int save = def.characteristic()->base();
        if (!txt.isEmpty()) def.characteristic()->base(txt.toInt());
        int primary = def.characteristic()->base() + def.characteristic()->primary();
        int secondary = primary + def.characteristic()->secondary();
        if (val == mUI->spdval) {
            if (primary > 12 || secondary > 12) { // NOLINT
                val->undo();
                def.characteristic()->base(save);
                return;
            }
        }
        def.points()->setText(QString("%1").arg(def.characteristic()->points().points));
        if (def.roll()) {
            def.roll()->setText(def.characteristic()->roll());
            updateSkillRolls();
        }

        if (val == mUI->strval) {
            setDamage(def, mUI->hthdamage);
            QString end = QString("%1").arg((primary + 4) / mOption.activePerEND().points);
            if (primary != secondary) end += QString("/%1").arg((secondary + 4) / mOption.activePerEND().points);
            mUI->strendcost->setText(end);
            rebuildMartialArts();
            QString lift = formatLift(primary);
            if (primary != secondary) lift += "/" + formatLift(secondary);
            mUI->lift->setText(lift);
        } else if (val == mUI->spdval) {
            if (def.characteristic()->base() < 1) return;
            QList<int> chart = phases[secondary];
            for (auto& x: std::as_const(mUI->phases)) x->setText("");
            for (const auto& x: chart) mUI->phases[x - 1]->setText("X");
        } else if (val == mUI->ocvval) setCVs(def, mUI->baseocv);
          else if (val == mUI->omcvval) setCVs(def, mUI->baseomcv);
          else if (val == mUI->dcvval) setCVs(def, mUI->basedcv);
          else if (val == mUI->dmcvval) setCVs(def, mUI->basedmcv);
          else if (val == mUI->intval) mUI->perceptionroll->setText(def.roll()->text());
          else if (val == mUI->preval) setDamage(def, mUI->presenceattack);
          else if (val == mUI->endval) setMaximum(def, mUI->maximumend, mUI->currentend);
          else if (val == mUI->bodyval) setMaximum(def, mUI->maximumbody, mUI->currentbody);
          else if (val == mUI->stunval) setMaximum(def, mUI->maximumstun, mUI->currentstun);
          else if (val == mUI->pdval || val == mUI->edval) rebuildDefenses();
        if (update) updateTotals();
    } else val->undo();
}

void Sheet::characteristicEditingFinished(QLineEdit* val) {
    QString txt = val->text();
    characteristicChanged(val, txt);

    if (txt.isEmpty()) {
        if (sender() == mUI->spdval) {
            txt = "1";
            characteristicChanged(val, txt);
        }
        txt = "0";
    }
    auto& def = mWidget2Def[val];
    txt = def.characteristic()->value();
    val->setText(txt);
}

void Sheet::saveThenExit() {
    try { save(); } catch (...) { return; } // bug is saving?
    if (mChanged) return; // hit cancel in save as?
    close(); // really exit this time
}

void Sheet::justClose() {
    mChanged = false;
    close(); // really exit this time
}

bool Sheet::checkClose() {
    if (mChanged) {
        YesNoCancel("Do you want to save your changes first?",
                    std::bind(&Sheet::saveThenExit, this),
                    std::bind(&Sheet::justClose, this),
                    std::bind(&Sheet::doNothing, this),
                    "The current character has been changed!");
        return false;
    }
    return true;
}

void Sheet::clearHitLocations() {
    for (auto& x: mHitLocations) x = 0;
}

#ifdef __wasm__
QToolButton* Sheet::createToolBarItem(QToolBar* sb, const QString name, const QString tip) {
    QToolButton *tb = new QToolButton(sb);
    tb->setText(name);
    tb->setObjectName(name);
    tb->setToolTip(tip);
    tb->setToolButtonStyle(Qt::ToolButtonTextOnly);
    sb->addWidget(tb);
    return tb;
}

void Sheet::createMenuItem(QAction*& action, const QString& name, const char* slot) {
    action = new QAction(this);
    action->setObjectName("name");
    connect(action, SIGNAL(triggered()), this, slot);
}

void Sheet::createMenuItem(QAction*& action, const QString& name, std::function<void()> func) {
    action = new QAction(this);
    action->setObjectName("name");
    connect(action, &QAction::triggered, this, func);
}

QWidget* Sheet::createToolBarItem(QToolBar* sb, QAction* at, const QString name, const QString tip, QAction* action) {
    QToolButton *tb = new QToolButton();
    tb->setText(name);
    tb->setObjectName(name);
    tb->setToolTip(tip);
    tb->setDefaultAction(action);
    tb->setToolButtonStyle(Qt::ToolButtonTextOnly);
    action->setText(name);
    action->setToolTip(tip);
    sb->insertWidget(at, tb);
    return tb;
}

QWidget* Sheet::createToolBarItem(QToolBar* sb, const QString name) {
    QLabel *tb = new QLabel();
    tb->setText(name);
    tb->setObjectName(name);
    sb->addWidget(tb);
    return tb;
}

QWidget* Sheet::createToolBarItem(QToolBar* sb, QAction* at, const QString name) {
    QLabel *tb = new QLabel();
    tb->setText(name);
    tb->setObjectName(name);
    sb->insertWidget(at, tb);
    return tb;
}
#endif

void Sheet::delPower(int row) {
    auto power = getPower(row, mCharacter.powersOrEquipment());
    if (power == nullptr) return;

    if (power->parent() == nullptr) {
        auto realRow = mCharacter.powersOrEquipment().indexOf(power);
        mCharacter.powersOrEquipment().removeAt(realRow);
    } else {
        auto& list = power->parent()->list();
        auto realRow = list.indexOf(power);
        list.removeAt(realRow);
    }
}

void Sheet::deletePagefull(QPlainTextEdit* txt, double scale, QPainter* pnt) {
    int lines = getPageLines(txt, scale, pnt);
    txt->moveCursor(QTextCursor::Start);
    for (int i = 0; i < lines; ++i) txt->moveCursor(QTextCursor::Down, QTextCursor::KeepAnchor);
    txt->textCursor().removeSelectedText();
}

void Sheet::deletePagefull(QTableWidget* tbl) {
    int hgt = tbl->size().height();
    int found = 0;
    int rows = tbl->rowCount();
    for (int i = 0; i < rows; ++i) {
        int h = tbl->rowHeight(0);
        found += h;
        tbl->removeRow(0);
        if (found > hgt) break;
    }
}

void Sheet::deletePagefull() {
    deletePagefull(mUI->skillstalentsandperks);
    deletePagefull(mUI->complications);
    deletePagefull(mUI->powersandequipment);
}

int Sheet::displayPowerAndEquipment(int& row, shared_ptr<Power> pe) {
    if (pe == nullptr) return 0;

    QFont font = mUI->powersandequipment->font();
    QFont italic = font;
    italic.setItalic(true);
    QString descr = option().abbreviations() ? pe->abbreviation(false) : pe->description(false);

    if (pe->isEquipment() && pe->name() == "Armor") hitLocations(pe);

    if (descr == "-") descr = "";
    bool abbr = option().abbreviations();
    for (const auto& mod: std::as_const(pe->advantagesList())) {
        if (mod == nullptr) continue;

        if (mod->isAdder()) descr += "; (+" + QString("%1").arg(mod->points(Power::NoStore).points) + " pts) ";
        else descr += "; (+" + mod->fraction(Power::NoStore).toString() + ") ";
        descr += abbr ? mod->abbreviation(false) : mod->description(false);
    }
    for (const auto& mod: std::as_const(pe->limitationsList())) {
        if (mod == nullptr) continue;

        descr += "; (-" + mod->fraction(Power::NoStore).abs().toString() + ") " + (abbr? mod->abbreviation(false) : mod->description(false));
    }
    Fraction pts(pe->real().points);
    if (((!pe->isFramework() && !pe->isEquipment()) || pe->isVPP() || pe->isMultipower()) && !descr.isEmpty() && pts.toInt() == 0) pts = Fraction(1);
    if (pe->isVPP()) pts += pe->pool().points;
    if (pts.toInt() != 0) setCell(mUI->powersandequipment, row, 0, QString("%1").arg(pts.toInt()), font);
    else setCell(mUI->powersandequipment, row, 0, "", font);
    setCell(mUI->powersandequipment, row, 1, pe->nickname(), italic);
    setCell(mUI->powersandequipment, row, 2, descr, font, WordWrap);
    QString end = pe->end();
    if (end == "-") end = "";
    setCell(mUI->powersandequipment, row, 3, end, font);
    pe->row(row);
    ++row;
    if (pe->isFramework()) mPowersOrEquipmentPoints += pe->display(row, mUI->powersandequipment);
    return pts.toInt();
}

void Sheet::finishLoad() {
    mUI->notes->setPlainText(mCharacter.notes());
    updateDisplay();
    mChanged = mSaveChanged;
}

void Sheet::updateBanner() {
    QPixmap pixmap(mOption.banner());
    pixmap = pixmap.scaled(293, 109, Qt::KeepAspectRatio, Qt::SmoothTransformation); // NOLINT
    mUI->banner1->setPixmap(pixmap);
    mUI->banner2->setPixmap(pixmap);
    mUI->banner3->setPixmap(pixmap);
}

#ifdef __wasm__
void Sheet::fileOpen(const QByteArray& data, QString filename) {
    if (!mCharacter.load(mOption, data)) OK("Can't load \"" + mFilename.toString() +"\".", std::bind(&Sheet::doNothing, this));
    else {
        mSaveChanged = false;
        finishLoad();
    }
}

#elif defined(Q_OS_ANDROID)
void Sheet::fileOpen() {
    Power::Equipment(); // pre-load equipment if needed

    if (!mCharacter.load(mOption, mFilename)) OK("Can't load \"" + mFilename.toString() +"\".", std::bind(&Sheet::doNothing, this));
    else {
        mSaveChanged = false;
        finishLoad(); // timestamp don't make sense on android
    }
}

#else
void Sheet::doLoadImage() {
    loadImage(mCharacter.image());
    mSaveChanged = mChanged;
    finishLoad();
}

void Sheet::fileOpen() {
    Power::Equipment(); // pre-load equipment if needed

    if (!mCharacter.load(mOption, mFilename)) OK("Can't load \"" + mFilename.toString() +"\".", std::bind(&Sheet::doNothing, this));
    else {
        mSaveChanged = false;
        QFileInfo imageFile(mCharacter.image().isLocalFile() ? mCharacter.image().toLocalFile() : mCharacter.image().toString());
        if (imageFile.exists()) {
            qulonglong then(mCharacter.imageDate());
            qulonglong file(imageFile.lastModified().toSecsSinceEpoch());
            if (file > then) YesNo("Character image on disk has changed.\n\n"
                                   "Do you want to update the image in\n"
                                   "the character sheet?",
                                   std::bind(&Sheet::doLoadImage, this),
                                   std::bind(&Sheet::finishLoad, this));
            else finishLoad();
        } else finishLoad();
    }
}
#endif

QString Sheet::formatLift(int str) {
    auto lift = interpolateLift(str);
    QString units = liftUnits(str);
    if (units == "kg") return QString("%1kg").arg(formatNumber((int) lift));
    QString num;
    if (units == "tons") num = QString("%1").arg(formatNumber(lift / 1000.0)); // NOLINT
    else num = QString("%1").arg(formatNumber(lift / 1000000.0)); // NOLINT
    if (num.right(2) == ".0") num = num.left(num.length() - 2);
    return num + " " + units;
}

QStringList Sheet::getBanners()
{
    QStringList banners;
    QDirIterator it(":/gfx/");
    while (it.hasNext()) {
        if (it.fileName().endsWith("-Banner.png")) banners.append(it.filePath());
        it.next();
    }
    return banners;
}

QString Sheet::getCharacter() {
    QString out;
    out += "Character Name: " + mCharacter.characterName();
    if (!mCharacter.alternateIds().isEmpty()) out += " (" + mCharacter.alternateIds() + ")\n";
    else out += "\n";
    out += "Player Name: " + mCharacter.playerName() + "\n\n";

    out += "Campaign Name: " + mCharacter.campaignName() + "\n";
    out += "Genre: " + mCharacter.genre() + "\n";
    out += "Gamemaster" + mCharacter.gamemaster() + "\n\n";

    out += "Height: " + mCharacter.height() + "\n";
    out += "Weight: " + mCharacter.weight() + "\n";
    out += "Hair Color: " + mCharacter.hairColor() + "\n";
    out += "Eye Color: " + mCharacter.eyeColor() + "\n\n";

    QStringList names { "STR", "DEX", "CON", "INT", "EGO", "PRE", "OCV", "DCV", "OMCV", "DMCV", "SPD", "PD", "ED", "REC", "END", "BODY", "STUN" };
    for (auto i = 0; i < names.count(); ++i) {
        auto characteristic = mCharacter.characteristic(i);
        int primary = characteristic.base() + characteristic.primary();
        int secondary = primary + characteristic.secondary();
        if (primary != secondary) out += QString("%1/%2\t").arg(primary).arg(secondary);
        else out += QString("%1\t").arg(primary);
        out += names[i] + QString("\t%1\t").arg(characteristic.points().points);
        if (i < 6) out += characteristic.roll(); // NOLINT
        if (names[i] == "BODY") out += "Total Cost";
        if (names[i] == "STUN") out += mUI->totalcost->text();
        out += "\n";
    }
    out += "\n";

    out += QString("Perception Roll:\t%1\n\n").arg(mCharacter.INT().roll());

    int pd = mCharacter.PD().primary() + mCharacter.PD().base() + mCharacter.PD().secondary();
    int rPd = mCharacter.rPD();
    out += QString("PD/rPD:\t%1/%2\n").arg(pd).arg(rPd);
    int ed = mCharacter.ED().primary() + mCharacter.ED().base() + mCharacter.ED().secondary();
    int rEd = mCharacter.rED();
    out += QString("ED/rED:\t%1/%2\n").arg(ed).arg(rEd);
    out += QString("MD:\t%1\n").arg(mCharacter.MD());
    out += QString("PowD:\t%1\n").arg(mCharacter.PowD());
    out += QString("FD:\t%1\n").arg(mCharacter.FD());
    out += "\n";

    out += "Skills, Talents, and Perks\n";
    bool abbr = option().abbreviations();
    for (const auto& skill: std::as_const(mCharacter.skillsTalentsOrPerks())) {
        if (skill == nullptr) continue;

        out += QString("%1\t%2\n").arg(skill->points(SkillTalentOrPerk::NoStore).points).arg(abbr ? skill->abbreviation() : skill->description());
    }
    out += QString("%1\tTotal Skills, Talents, and Perks\n\n").arg(mUI->totalskillstalentsandperkscost->text());

    out += "Powers and Equipment\n";
    for (const auto& power: std::as_const(mCharacter.powersOrEquipment())) {
        if (power == nullptr) continue;

        QString end = power->end();
        if (end == "-") end = "";
        out += QString("%1\t%2%3\n").arg(power->points(Power::NoStore).points).arg(abbr ? power->abbreviation() : power->description(), end.isEmpty() ? "" : "\t[" + end + "]");
        if (power->isFramework()) power->display(out);
    }
    out += QString("%1\tTotal Powers and Equipment\n\n").arg(mUI->totalpowersandequipmentcost->text());

    out += "Complications\n";
    for (const auto& complication: std::as_const(mCharacter.complications())) {
        if (complication == nullptr) continue;

        out += QString("%1\t%2\n").arg(complication->points(Complication::NoStore).points).arg(abbr ? complication->abbreviation() : complication->description());
    }
    out += QString("%1\tTotal Complications Points\n\n").arg(mUI->totalcomplicationpts->text());

    out += QString("%1\tTotal Points\n").arg(mUI->totalpoints->text());
    out += QString("%1\tTotal Experience Earned\n").arg(mUI->totalexperienceearned->text());
    out += QString("%1\tExperience Spent\n").arg(mUI->experiencespent->text());
    out += QString("%1\tExperience Unspent\n\n").arg(mUI->experienceunspent->text());

    out += "Notes:\n" + mCharacter.notes() + "\n";
    return out;
}

int Sheet::getPageLines(QPlainTextEdit* txt, double scale, QPainter* pnt) {
    QFontMetrics metrics = pnt->fontMetrics();
    return (int) (txt->height() * scale) / metrics.lineSpacing();
}

int Sheet::getPageCount(QPlainTextEdit* txt, double scale, QPainter* pnt) {
    QTextCursor cursor = txt->textCursor();
    cursor.movePosition(QTextCursor::Start);
    int count = 1;
    while (!cursor.atEnd()) {
        if (!cursor.movePosition(QTextCursor::Down)) break;
        ++count;
    }
    if (count == 1) return 1; // document pageSize undetermined if no text in the document, this prevents running into that case
    int pageLines = getPageLines(txt, scale, pnt);
    return count / pageLines + (count % pageLines != 0 ? 1 : 0);
}

int Sheet::getPageCount(QTableWidget* tbl) {
    int hgt = tbl->size().height();
    int needed = 0;
    int pages = 1;
    bool blank = false;
    QFont font = tbl->font();
    int rows = tbl->rowCount();
    for (int i = 0; i < rows; ++i) {
        int h = tbl->rowHeight(i);
        if (needed + h > hgt) {
            if (!blank) {
                blank = true;
                tbl->insertRow(i);
                ++rows;
                for (int j = 0; j < tbl->columnCount(); ++j) setCellLabel(tbl, i, j, " ", font);
                tbl->setRowHeight(i, tbl->horizontalHeader()->height());
                --i;
            } else {
                needed = h;
                ++pages;
                blank = false;
            }
        } else {
            needed += h;
            blank = false;
        }
    }
    return pages;
}

int Sheet::getPageCount() {
    int pages = getPageCount(mUI->skillstalentsandperks);
    int next = getPageCount(mUI->complications);
    if (next > pages) pages = next;
    next = getPageCount(mUI->powersandequipment);
    if (next > pages) pages = next;
    return pages;
}

shared_ptr<Power>& Sheet::getPower(int row, QList<shared_ptr<Power>>& in) {
    static shared_ptr<Power> null = nullptr;

    for (shared_ptr<Power>& power: in) {
        if (power == nullptr) continue;

        if (power->row() == row) return power;
        if (power->isFramework()) {
            shared_ptr<Power>& p = getPower(row, power->list());
            if (p != null) return p;
        }
    }
    return null;
}

QList<int> expandHitLocations(const QString& loc) {
    QList<int> expanded;
    QList<QString> commas = loc.split(",");
    for (auto i = 0; i < commas.count(); ++i) {
        auto& x = commas[i];
        QList<QString> dashes = x.split("-");
        if (dashes.count() == 1) expanded.append(dashes[0].toInt());
        int start = dashes[0].toInt();
        int end = start;
        if (dashes.count() != 1) end = dashes[1].toInt();
        for (int y = start; y <= end; ++y) expanded.append(y);
    }
    return expanded;
}

void Sheet::hitLocations(std::shared_ptr<Power>& pe) {
    Armor* arm = dynamic_cast<Armor*>(pe.get());
    QList<int> locations = expandHitLocations(arm->hitLocations());
    int baseDEF = mCharacter.rPD();
    int def = baseDEF + arm->DEF();
    for (const auto& x: std::as_const(locations)) if (def > mHitLocations[x]) mHitLocations[x] = def; // NOLINT
}

void Sheet::loadImage(QPixmap& pixmap, QUrl filename) {
    clearImage();
    QPixmap scaled = pixmap.scaledToWidth(mUI->image->width());
    if (scaled.height() > mUI->image->height()) scaled = pixmap.scaledToHeight(mUI->image->height());
    mUI->image->setPixmap(scaled);
    mCharacter.image() = filename;
    QByteArray sync;
    QBuffer buffer(&sync);
    buffer.open(QIODevice::WriteOnly);
    scaled.save(&buffer, "PNG");
    buffer.close();
    mCharacter.imageData() = sync;
    QFileInfo imageFile(filename.isLocalFile() ? filename.toLocalFile() : filename.toString());
    QDateTime tm = imageFile.lastModified();
    mCharacter.imageDate() = tm.toSecsSinceEpoch();
    mChanged = true;
}

#ifdef __wasm__
void Sheet::loadImage(const QByteArray& data, QString filename) {
    QPixmap pixmap;
    pixmap.loadFromData(data);
    loadImage(pixmap, filename);
}
#endif

void Sheet::loadImage(QUrl url) {
    QString source = url.isLocalFile() ? url.toLocalFile() : url.toString();
    QFile file(source);

    if (!file.open(QIODevice::ReadOnly)) return;

    QImageReader reader(&file);
    reader.setAutoTransform(true);

    QImage image = reader.read();

    if (image.isNull()) return;

    QPixmap pixmap = QPixmap::fromImage(image);
    loadImage(pixmap, url);
}

void Sheet::preparePrint(QPlainTextEdit* txt) {
    txt->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    txt->verticalScrollBar()->setValue(0);
}

void Sheet::preparePrint(QTableWidget* tbl) {
    tbl->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tbl->verticalScrollBar()->setValue(0);
}

void Sheet::print(QPainter& painter, QPoint& offset, QWidget* widget) {
    QString oldStyle = widget->styleSheet();
    auto& options = Sheet::ref().option();
    QLabel* label = dynamic_cast<QLabel*>(widget);
    if (label) {
        QString style;
        if (label->font() == mUI->smallBoldWideFont || label->font() == mUI->headerFont)
            style = "QLabel { background: transparent;"
                          "   color: black; "
                          "   border-style: none;"
                          " }";
        else
            style = "QLabel { background: white;"
                          "   color: black; "
                          "   border-style: none;"
                          " }";
        label->setStyleSheet(style);
    }
    QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(widget);
    if (lineEdit) {
        if (options.greenfields() && lineEdit->styleSheet().contains("green")) return;
        QString style = "QLineEdit { background: white;"
                                 "   color: black; "
                                 "   border-style: none;"
                                 " }";
        lineEdit->setStyleSheet(style);
    }
    QTableWidget* table = dynamic_cast<QTableWidget*>(widget);
    if (table) {
        QFont font = table->font();
        QString family = font.family();
        int pnt = font.pointSize();
        QString style ="QTableWidget { gridline-color: white;"
                                   "   background-color: white;"
                                   "   selection-background-color: white;"
                                   "   border-style: none;"
                         + QString("   font: %2pt \"%1\";").arg(family).arg((pnt * 8 + 5) / 10) + // NOLINT
                                   "   color: black;"
                                   "   selection-color: black;"
                                   " } "
                      "QTableWidgetItem { background-color: white;"
                                      "   color: black;"
                                      " }"
                      "QHeaderView::section { background-color: white;"
                                          "   border-style: none;"
                                          "   color: black;" +
                                  QString("   font: bold %2pt \"%1\";").arg(family).arg(pnt) +
                                          " }";
        table->setStyleSheet(style);
    }
    QPlainTextEdit* text = dynamic_cast<QPlainTextEdit*>(widget);
    if (text) {
        QString style ="QPlainTextEdit { gridline-color: white;"
                                     "   background-color: white;"
                                     "   selection-background-color: white;"
                                     "   border-style: none;"
                                     "   color: black;"
                                     "   selection-color: black;"
                                     " }";
        text->setStyleSheet(style);
    }
    QPoint move { widget->x() - offset.x(), widget->y() - offset.y() };
    painter.translate(move);
    widget->render(&painter);
    painter.translate(-move);
    widget->setStyleSheet(oldStyle);
}

void Sheet::putPower(int row, shared_ptr<Power> power) {
    if (power == nullptr) return;

    shared_ptr<Power> after = getPower(row, mCharacter.powersOrEquipment());
    if (after == nullptr) {
        power->parent(nullptr);
        mCharacter.powersOrEquipment().append(power);
    } else if (after->isFramework()) {
        if (power->parent() == after.get()) {
            auto realRow = mCharacter.powersOrEquipment().indexOf(after);
            power->parent(nullptr);
            mCharacter.powersOrEquipment().insert(realRow, power);
        } else after->insert(0, power);
    } else {
        auto parent = after->parent();
        if (parent == nullptr) {
            auto realRow = mCharacter.powersOrEquipment().indexOf(after);
            if (power->parent() == nullptr) mCharacter.powersOrEquipment().insert(realRow, power);
            else {
                power->parent(nullptr);
                mCharacter.powersOrEquipment().insert(--realRow, power);
            }
        } else {
            auto realRow = parent->list().indexOf(after);
            if (power->parent() == nullptr) parent->list().insert(++realRow, power);
            else parent->list().insert(realRow, power);
            power->parent(parent);
        }
    }
}

void Sheet::rebuildCharFromPowers(QList<shared_ptr<Power>>& list) {
    for (const auto& power: std::as_const(list)) {
        if (power == nullptr) continue;

        if (power->name() == "Skill" && power->skill()->name() == "Combat Luck") {
            if (power->skill()->place() == 1) {
                mCharacter.rPD() = mCharacter.rPD() + power->skill()->rPD();
                mCharacter.rED() = mCharacter.rED() + power->skill()->rED();
            } else if (power->skill()->place() == 2){
                mCharacter.temprPD() = mCharacter.temprPD() + power->skill()->rPD();
                mCharacter.temprED() = mCharacter.temprED() + power->skill()->rED();
            }
        } else if (power->name() == "Barrier") {
            if (power->place() == 1) {
                mCharacter.rPD() = mCharacter.rPD() + power->rPD();
                mCharacter.rED() = mCharacter.rED() + power->rED();
            } else if (power->place() == 2) {
                mCharacter.temprPD() += power->rPD() + power->PD();
                mCharacter.temprED() += power->rED() + power->ED();
            }
        } else if (power->name() == "Flash Defense") {
            mCharacter.FD() += power->FD();
        }
        else if (power->name() == "Mental Defense") {
            mCharacter.MD() += power->MD();
        }
        else if (power->name() == "Power Defense") {
            mCharacter.PowD() += power->PowD();
        } else if (power->name() == "Density Increase") {
            mCharacter.STR().secondary(mCharacter.STR().secondary() + power->str());
            mCharacter.rPD() += power->rPD();
            mCharacter.rED() += power->rED();
            if (power->hasModifier("Nonresistant Defense")) {
                mCharacter.PD().secondary(mCharacter.PD().secondary() + power->PD());
                mCharacter.ED().secondary(mCharacter.ED().secondary() + power->ED());
            }
        } else if (power->name() == "Resistant Defense") {
            if (power->place() == 1) {
                mCharacter.rPD() = mCharacter.rPD() + power->rPD() + power->PD();
                mCharacter.rED() = mCharacter.rED() + power->rED() + power->ED();
            } else if (power->place() == 2) {
                mCharacter.temprPD() = mCharacter.temprPD() + power->rPD() + power->PD();
                mCharacter.temprED() = mCharacter.temprED() + power->rED() + power->ED();
            }
        } else if (power->name() == "Growth") {
            auto& sm = power->growthStats();
            mCharacter.STR().secondary(mCharacter.STR().secondary() + sm.mSTR);
            mCharacter.CON().secondary(mCharacter.CON().secondary() + sm.mCON);
            mCharacter.PRE().secondary(mCharacter.PRE().secondary() + sm.mPRE);
            mCharacter.PD().secondary(mCharacter.PD().secondary() + sm.mPD);
            mCharacter.ED().secondary(mCharacter.ED().secondary() + sm.mED);
            mCharacter.BODY().secondary(mCharacter.BODY().secondary() + sm.mBODY);
            mCharacter.STUN().secondary(mCharacter.STUN().secondary() + sm.mSTUN);
            if (power->hasModifier("Resistant")) {
                mCharacter.rPD() += sm.mPD;
                mCharacter.rED() += sm.mED;
            }
        } else if (power->name() == "Characteristics") {
            int put = power->characteristic(-1);
            if (put < 1) continue;
            for (int i = 0; i < 17; ++i) { // NOLINT
                if (put == 1) mCharacter.characteristic(i).primary(mCharacter.characteristic(i).primary() + power->characteristic(i));
                else mCharacter.characteristic(i).secondary(mCharacter.characteristic(i).secondary() + power->characteristic(i));
            }
            if (power->hasModifier("Resistant")) {
                mCharacter.rPD() += power->characteristic(11); // NOLINT
                mCharacter.rED() += power->characteristic(12); // NOLINT
            }
        } else if (power->isFramework()) rebuildCharFromPowers(power->list());
    }
}

void Sheet::rebuildCharacteristics() {
    QList<QLineEdit*> characteristicWidgets {
        mUI->strval,  mUI->dexval, mUI->conval, mUI->intval,  mUI->egoval,
        mUI->preval,  mUI->ocvval, mUI->dcvval, mUI->omcvval, mUI->dmcvval,
        mUI->spdval,  mUI->pdval,  mUI->edval,  mUI->recval,  mUI->endval,
        mUI->bodyval, mUI->stunval
    };

    rebuildDefenses();

    for (int i = 0; i < characteristicWidgets.count(); ++i) {
        int base = mCharacter.characteristic(i).base();
        characteristicWidgets[i]->setText(QString("%1").arg(base));
        characteristicEditingFinished(characteristicWidgets[i]);
    }
}

QString Sheet::rebuildCombatSkillLevel(shared_ptr<SkillTalentOrPerk> stp) {
    if (stp == nullptr) return "";

    bool abbr = option().abbreviations();
    stp->points(SkillTalentOrPerk::NoStore);
    QString descr = abbr ? stp->abbreviation() : stp->description();
    if (stp->name() == "Combat Skill Levels" ||
        (stp->name() == "Skill Levels" && descr.contains("Overall"))) return descr.mid(stp->name().length() + 2);
    return "";
}

void Sheet::rebuildCSLPower(QList<shared_ptr<Power>>& list, bool& first, QString& csl) {
    for (const auto& pow: std::as_const(list)) {
        if (pow == nullptr) continue;

        pow->points(Power::NoStore);
        if (pow->name() == "Skill") {
            QString d = rebuildCombatSkillLevel(pow->skill());
            if (d.isEmpty()) continue;
            if (first) first = false;
            else csl += ", ";
            csl += d;
        }

        if (pow->isFramework()) rebuildCSLPower(pow->list(), first, csl);
    }
}

void Sheet::rebuildCombatSkillLevels() {
    bool first = true;
    QString csl = "<b>Combat Skill Levels</b> ";
    for (const auto& stp: std::as_const(mCharacter.skillsTalentsOrPerks())) {
        if (stp == nullptr) continue;

        QString d = rebuildCombatSkillLevel(stp);
        if (d.isEmpty()) continue;
        if (first) first = false;
        else csl += ", ";
        csl += d;
    }

    rebuildCSLPower(mCharacter.powersOrEquipment(), first, csl);

    mUI->combatskilllevels->setHtml(csl);
}

void Sheet::rebuildDefenses() {
    mCharacter.rPD() = 0;
    mCharacter.rED() = 0;
    mCharacter.temprPD() = 0;
    mCharacter.temprED() = 0;
    mCharacter.tempPD() = 0;
    mCharacter.tempED() = 0;
    mCharacter.FD() = 0;
    mCharacter.MD() = 0;

    for (const auto& skill: std::as_const(mCharacter.skillsTalentsOrPerks())) {
        if (skill == nullptr) continue;

        if (skill->name() == "Combat Luck") {
            if (skill->place() == 1) {
                mCharacter.rPD() = mCharacter.rPD() + skill->rPD();
                mCharacter.rED() = mCharacter.rED() + skill->rED();
            } else if (skill->place() == 2){
                mCharacter.temprPD() = mCharacter.temprPD() + skill->rPD();
                mCharacter.temprED() = mCharacter.temprED() + skill->rED();
            }
        }
    }

    for (int i = 0; i < 17; ++i) { // NOLINT
        mCharacter.characteristic(i).primary(0);
        mCharacter.characteristic(i).secondary(0);
    }

    rebuildCharFromPowers(mCharacter.powersOrEquipment());

    int primPD = mCharacter.PD().base() + mCharacter.PD().primary() + mCharacter.rPD();
    int secondPD = primPD + mCharacter.PD().secondary() + mCharacter.temprPD();
    int primED = mCharacter.ED().base() + mCharacter.ED().primary() + mCharacter.rED();
    int secondED = primED + mCharacter.ED().secondary() + mCharacter.temprED();

    QString strPrimPD;
    QString strPrimED;
    QString strRPD;
    QString strRED;
    QString strMD;
    QString strPowD;
    QString strFD;
    if (primPD == secondPD) strPrimPD = setDefense(primPD, 0                );
    else                    strPrimPD = setDefense(primPD, secondPD - primED);
    if (primED == secondED) strPrimED = setDefense(primED, 0                );
    else                    strPrimED = setDefense(primED, secondED - primED);

    strRPD  = setDefense(mCharacter.rPD(),  mCharacter.temprPD());
    strRED  = setDefense(mCharacter.rED(),  mCharacter.temprED());
    strMD   = setDefense(mCharacter.MD(),   0                   );
    strPowD = setDefense(mCharacter.PowD(), 0                   ); // NOLINT
    strFD   = setDefense(mCharacter.FD(),   0                   ); // NOLINT

    QFont font = mUI->narrowTableFont;
    auto* def = mUI->defenses;
    def->setRowCount(0);
    def->update();

    setCell(def, 0, 0, "Normal PD ",      font);
    setCell(def, 0, 1, strPrimPD,         font);
    setCell(def, 1, 0, "Resistant PD ",   font);
    setCell(def, 1, 1, strRPD,            font);
    setCell(def, 2, 0, "Normal ED ",      font);
    setCell(def, 2, 1, strPrimED,         font);
    setCell(def, 3, 0, "Resistant ED ",   font);
    setCell(def, 3, 1, strRED,            font);
    setCell(def, 4, 0, "Mental Defense ", font);
    setCell(def, 4, 1, strMD,             font);
    setCell(def, 5, 0, "Power Defense ",  font);
    setCell(def, 5, 1, strPowD,           font);
    setCell(def, 6, 0, "Flash Defense ",  font);
    setCell(def, 6, 1, strFD,             font);
}

QString Sheet::KAwSTR(int STR) {
    int dice = STR / 15; // NOLINT
    int rem = STR % 15; // NOLINT
    int extra = rem / 10 + 1; // NOLINT
    if (extra == 1) {
        if (rem >= 5) extra = 1; // NOLINT
        else extra = 0;
    }
    return QString("%1%2d6%3").arg(dice).arg((extra == 2) ? Fraction(1, 2).toString() : "",(extra == 1) ? "+1" : "");
}

void Sheet::rebuildMartialArt(shared_ptr<SkillTalentOrPerk> stp, QFont& font) {
    static QMap<QString, QStringList> table = {
        { "Choke Hold",       { "½", "-2", "+0", "Grab 1 limb, 2d6 NND~%1/%2/%3/%4/%5/%6/%7/%8/%9" } },
        { "Defensive Strike", { "½", "+1", "+3", "%1 strike~%1/%2/%3/%4/%5/%6/%7/%8/%9" } },             // STR --> A
        { "Killing Strike",   { "½", "-2", "+0", "HKA %2~%1/%2/%3/%4/%5/%6/%7/%8/%9" } },                // STR --> KA
        { "Legsweep",         { "½", "+2", "-1", "%3, target falls~%1/%2/%3/%4/%5/%6/%7/%8/%9" } },      // STR+1 --> A
        { "Martial Block",    { "½", "+2", "+2", "Block, abort~%1/%2/%3/%4/%5/%6/%7/%8/%9" } },
        { "Martial Disarm",   { "½", "-1", "-1", "Disarm, %4 STR~%1/%2/%3/%4/%5/%6/%7/%8/%9" } },        // STR+10
        { "Martial Dodge",    { "½", "——", "+5", "Dodge, abort~%1/%2/%3/%4/%5/%6/%7/%8/%9" } },
        { "Martial Escape",   { "½", "+0", "+0", "%5 STR vs. Grabs~%1/%2/%3/%4/%5/%6/%7/%8/%9" } },      // STR+15
        { "Martial Grab",     { "½", "-1", "-1", "Grab 2 Limbs, +%4 STR~%1/%2/%3/%4/%5/%6/%7/%8/%9" } },
        { "Martial Strike",   { "½", "+0", "+2", "%6~%1/%2/%3/%4/%5/%6/%7/%8/%9" } },                    // STR+2 --> A
        { "Martial Throw",    { "½", "+0", "+1", "(%9+ͮ⁄₁₀)d6,tgt falls~%1/%2/%3/%4/%5/%6/%7/%8/%9" } },
        { "Nerve Strike",     { "½", "-1", "+1", "2d6 NND~%1/%2/%3/%4/%5/%6/%7/%8/%9" } },
        { "Offensive Strike", { "½", "-2", "+1", "%7~%1/%2/%3/%4/%5/%6/%7/%8/%9" } },                    // STR+4 --> A
        { "Passing Strike",   { "½", "+1", "+0", "(%9+ͮ⁄₁₀)d6,full mv~%1/%2/%3/%4/%5/%6/%7/%8/%9" } },
        { "Sacrifice Throw",  { "½", "+2", "+1", "%8 STR, both fall~%1/%2/%3/%4/%5/%6/%7/%8/%9" } }      // STR
    };

    auto* man = mUI->attacksandmaneuvers;
    bool abbr = option().abbreviations();
    QString descr = abbr ? stp->abbreviation() : stp->description();
    QString d = descr.mid(stp->name().length() + 2);
    auto maneuvers = d.split(", ");
    int extraSTR = 0;
    for (auto i = 0; i < maneuvers.count(); ++i) {
        auto& m = maneuvers[i];
        if (!table.contains(m)) {
            auto parts = m.split(" ");
            if (parts.size() == 4) extraSTR = parts[0].toInt() * 5; // NOLINT
        }
    }
    int STR = mCharacter.STR().base() + mCharacter.STR().primary() + extraSTR;
    for (auto i = 0; i < maneuvers.count(); ++i) {
        auto& m = maneuvers[i];
        int size = man->rowCount();
        int row = 15; // NOLINT
        for (; row < size; ++row) {
            const auto* cell = man->item(row, 0);
            if (dynamic_cast<const QTableWidgetItem*>(cell)->text() == m) break;
        }
        if (row != size) continue;
        if (table.contains(m)) {
            setCell(man, row, 0, m, font);
            for (int j = 0; j < 4; j++) {
                QString x = table[m][j];
                if (j == 3) x = QString(x)
                                     .arg(valueToDice(STR),              // 1
                                          KAwSTR(STR),                   // 2
                                          valueToDice(STR + 5))          // 3 NOLINT
                                     .arg(STR + 10)                      // 4 NOLINT
                                     .arg(STR + 15)                      // 5 NOLINT
                                     .arg(valueToDice(STR + 10),         // 6 NOLINT
                                          valueToDice(STR + 20))         // 7 NOLINT
                                     .arg(STR)                           // 8
                                     .arg(valueToDice(STR, noD6));       // 9
                QStringList t = x.split("~");
                setCell(man, row, j + 1, t[0], font);
            }
        }
    }
}

void Sheet::rebuildBasicManeuvers(QFont& font) {
    const static QList<QStringList> maneuvers = { { "Block",        "½",     "+0",    "+0",  "Block, abort~%1%2%3%4%5%6"               },
                                                  { "Brace",        "0",     "+2",    "½",   "+2 OCV vs R Mod~%1%2%3%4%5%6"            },
                                                  { "Disarm",       "½",     "-2",    "+0",  "Disarm, %1 v. STR~%1%2%3%4%5%6"          }, // STR
                                                  { "Dodge",        "½",     "——",    "+3",  "Abort vs. all attacks~%1%2%3%4%5%6"      },
                                                  { "Grab",         "½",     "-1",    "-2",  "Grab 2 limbs~%1%2%3%4%5%6"               },
                                                  { "Grab By",      "½†",    "-3",    "-4",  "Move&Grab;+(ͮ⁄₁₀) STR~%1%2%3%4%5%6"      },
                                                  { "Haymaker",     "½*",    "+0",    "-5",  "+4 DCs to attack~%1%2%3%4%5%6"           },
                                                  { "Move By",      "½†",    "-2",    "-2",  "(%2+ͮ⁄₁₀)d6; take ⅓~%1%2%3%4%5%6"        }, // STR/2 noD6
#if (defined(__wasm__) || defined(unix)) && !defined(Q_OS_ANDROID)
                                                  { "Move Thru",    "½†",    "-ͮ⁄₁₀", "-3",  "(%3+ͮ⁄₆)d6; take ½ or all~%1%2%3%4%5%6"  }, // STR noD6
                                                  { "Mult.Attx",    "1",     "var",   "½",   "Attack multiple times~%1%2%3%4%5%6"      },
#else
                                                  { "Move Through",     "½†", "-ͮ⁄₁₀", "-3", "(%3+ͮ⁄₆)d6; take ½ or all~%1%2%3%4%5%6"  },
                                                  { "Multiple Attacks", "1",  "var",   "½",  "Attack multiple times~%1%2%3%4%5%6"      },
#endif
                                                  { "Set",          "1",     "+1",    "+0",  "Ranged attacks only~%1%2%3%4%5%6"        },
                                                  { "Shove",        "½",     "-1",    "-1",  "Push %4m~%1%2%3%4%5%6"                   }, // STR/5
                                                  { "Strike",       "½",     "+0",    "+0",  "%5 or weapon~%1%2%3%4%5%6"               }, // STR d6
                                                  { "Throw",        "½",     "+0",    "+0",  "Throw w/%5d6 dmg~%1%2%3%4%5%6"           },
                                                  { "Trip",         "½",     "-1",    "-2",  "Knock target prone~%1%2%3%4%5%6"         }
                                                };
    auto* man = mUI->attacksandmaneuvers;
    man->setRowCount(0);
    man->update();

    int row = 0;
    int STR = mCharacter.STR().base() + mCharacter.STR().primary();
    int OCV = mUI->ocvval->text().toInt();
    for (const auto& m: std::as_const(maneuvers)) {
        for (int i = 0; i < 5; i++) { // NOLINT
            QString x = m[i];
            if (i == 4) x = QString(x)
                               .arg(STR)                        // 1
                               .arg(valueToDice(STR/ 2, noD6),  // 2
                                    valueToDice(STR, noD6))     // 3
                               .arg((STR + 2) / 5)              // 4 // NOLINT
                               .arg(valueToDice(STR))           // 5
                               .arg(OCV);                       // 6
            QStringList t = x.split("~");
            setCell(man, row, i, t[0], font);
        }
        ++row;
    }
}

void Sheet::rebuildMartialArts() {
    auto* man = mUI->attacksandmaneuvers;
    QFont font = mUI->smallfont;
    rebuildBasicManeuvers(font);

    for (const auto& stp: std::as_const(mCharacter.skillsTalentsOrPerks())) {
        if (stp == nullptr) continue;

        stp->points(SkillTalentOrPerk::NoStore);
        if (stp->name() == "Martial Arts") rebuildMartialArt(stp, font);
    }
    for (const auto& power: std::as_const(mCharacter.powersOrEquipment())) {
        if (power == nullptr) continue;

        power->points(Power::NoStore);
        if (power->name() == "Skill") {
            const auto& stp = power->skill();
            if (stp->name() == "Martial Arts") rebuildMartialArt(stp, font);
        }
    }
    man->resizeRowsToContents();
}

void Sheet::rebuildMoveFromPowers(QList<shared_ptr<Power>>& list,
                                  QMap<QString, int>& movements,
                                  QMap<QString, QString>& units,
                                  QMap<QString, int>& doubles) {
    for (const auto& power: std::as_const(list)) {
        if (power == nullptr) continue;

        if (power->name() == "Growth") {
            auto& sm = power->growthStats();
            mCharacter.running() += sm.mRunning;
        } else if (power->name() == "Running") {
            mCharacter.running() += power->move();
        } else if (power->name() == "Leaping") {
            mCharacter.leaping() += power->move();
        } else if (power->name() == "Swimming") {
            mCharacter.swimming() += power->move();
        } else if (power->name() == "FTL Travel" ||
                   power->name() == "Flight" ||
                   power->name() == "Swinging" ||
                   power->name() == "Teleportation" ||
                   power->name() == "Tunneling") {
            movements[power->name()] += power->move();
            units[power->name()] = power->units();
            doubles[power->name()] = power->doubling();
        } else if (power->isFramework()) {
            rebuildMoveFromPowers(power->list(), movements, units, doubles);
        }
    }
}

void Sheet::rebuildMovement() {
    mCharacter.running()  = 12; // NOLINT
    mCharacter.leaping()  = 4;
    mCharacter.swimming() = 4;
    QMap<QString, int> movements;
    QMap<QString, QString> units;
    QMap<QString, int> doubles;

    rebuildMoveFromPowers(mCharacter.powersOrEquipment(), movements, units, doubles);

    mUI->movement->setRowCount(4);
    mUI->movement->update();

    QString running = QString("%1m").arg(mCharacter.running());
    int mult = searchImprovedNoncombatMovement("Running");
    QString ncRunning = QString("%1m").arg(mult * mCharacter.running());
    QString swimming = QString("%1m").arg(mCharacter.swimming());
    mult = searchImprovedNoncombatMovement("Swimming");
    QString ncSwimming = QString("%1m").arg(mult * mCharacter.swimming());
    QString hLeaping = QString("%1m").arg(mCharacter.leaping());
    mult = searchImprovedNoncombatMovement("Leaping");
    QString ncHLeaping = QString("%1m").arg(mult * mCharacter.leaping());
    int vLeap = (mCharacter.leaping() + 1) / 2;
    QString vLeaping = QString("%1m").arg(vLeap);
    QString ncVLeaping = QString("%1m").arg(mult * vLeap);
    QFont font = mUI->movement->item(0, 1)->font();
    setCell(mUI->movement, 0, 1, running,    font);
    setCell(mUI->movement, 0, 2, ncRunning,  font);
    setCell(mUI->movement, 1, 1, swimming,   font);
    setCell(mUI->movement, 1, 2, ncSwimming, font);
    setCell(mUI->movement, 2, 1, hLeaping,   font);
    setCell(mUI->movement, 2, 2, ncHLeaping, font);
    setCell(mUI->movement, 3, 1, vLeaping,   font);
    setCell(mUI->movement, 3, 2, ncVLeaping, font);
    const auto keys = movements.keys();
    int row = 4;
    for (const auto& name: std::as_const(keys)) {
        setCell(mUI->movement, row, 0, name,                                                                  font);
        setCell(mUI->movement, row, 1, QString("%1%2").arg(movements[name]).arg(units[name]),                 font);
        setCell(mUI->movement, row, 2, QString("%1%2").arg(doubles[name] * movements[name]).arg(units[name]), font);
        row++;
    }
}

void Sheet::rebuildPowers(bool addTakesNoSTUN) {
    mUI->height->setText(mCharacter.height());
    mUI->weight->setText(mCharacter.weight());

    if (addTakesNoSTUN) mCharacter.hasTakesNoSTUN() = true;
    else {
        mCharacter.hasTakesNoSTUN() = false;
        for (const auto& power: std::as_const(mCharacter.powersOrEquipment())) {
            if (power == nullptr) continue;

            if (power->name() == "Takes No STUNϴ") {
                mCharacter.hasTakesNoSTUN() = true;
                break;
            }
            if (power->isFramework()) {
                for (const auto& pwr: std::as_const(power->list())) {
                    if (pwr == nullptr) continue;

                    if (pwr->name() == "Takes No STUNϴ") {
                        mCharacter.hasTakesNoSTUN() = true;
                        break;
                    }
                }
                if (mCharacter.hasTakesNoSTUN()) break;
            }
        }
    }

    if (mCharacter.hasTakesNoSTUN()) {
        mCharacter.PD().base(1);
        mCharacter.PD().init(1);
        mCharacter.PD().cost(3_cp);
        mCharacter.ED().base(1);
        mCharacter.ED().init(1);
        mCharacter.ED().cost(3_cp);
        mCharacter.DCV().cost(15_cp); // NOLINT
        mCharacter.DMCV().cost(9_cp); // NOLINT
        mUI->pdval->setToolTip("Physical Defense: 3 points");
        mUI->edval->setToolTip("Energy Defense: 3 points");
        mUI->dcvval->setToolTip("Defensive Combat Value: 15 points");
        mUI->dmcvval->setToolTip("Defensive Mental Combat Value: 9 points");
    } else {
        if (mCharacter.PD().cost() == 3_cp) {
            mCharacter.PD().base(2);
            mCharacter.PD().init(2);
            mCharacter.PD().cost(1_cp);
        }
        if (mCharacter.ED().cost() == 3_cp) {
            mCharacter.ED().base(2);
            mCharacter.ED().init(2);
            mCharacter.ED().cost(1_cp);
        }
        mCharacter.DCV().cost(5_cp); // NOLINT
        mCharacter.DMCV().cost(3_cp);
        mUI->pdval->setToolTip("Physical Defense: 1 point");
        mUI->edval->setToolTip("Energy Defense: 1 point");
        mUI->dcvval->setToolTip("Defensive Combat Value: 5 points");
        mUI->dmcvval->setToolTip("Defensive Mental Combat Value: 3 points");
    }
}

void Sheet::rebuildSenseFromPowers(QList<shared_ptr<Power>>& list, QString& senses) {
    bool first = true;
    bool abbr = option().abbreviations();
    for (const auto& power: list) {
        if (power == nullptr) continue;

        if (power->name() == "Enhanced Senses") {
            QString descr = abbr ? power->abbreviation(false) : power->description(false);
            const auto& split = descr.split(":");
            senses += " <small>" + QString((first ? "" : ", ")) + split[1] + "</small>";
            first = false;
        } else if (power->isFramework()) {
            QString more;
            rebuildSenseFromPowers(power->list(), more);
            senses += QString((first ? "" : ", ")) + more;
            first = false;
        }
    }
}

void Sheet::rebuildSenses() {
    QString senses = "<b>Enhanced and Unusual Senses</b>";
    rebuildSenseFromPowers(mCharacter.powersOrEquipment(), senses);
    mUI->enhancedandunusualsenses->setText(senses);
}

bool Sheet::recoverSession(QJsonDocument& json) {
    // if the file does not exist: return false
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(path);
    QString stateFile(path + "/HSCCU.state");

    QFile file(stateFile);
    if (!file.exists()) return false;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

    QByteArray dat(file.readAll());
    file.close();
    QDir().remove(stateFile);

    QString jsonStr(dat);
    json = QJsonDocument::fromJson(jsonStr.toUtf8());

    return true;
}

void Sheet::recoverState() {
    QJsonDocument inState;
    if (!recoverSession(inState)) return;
    if (!inState.isObject()) return;
    QJsonObject state(inState.object());

    if (!state.contains("character") || !state["character"].isObject()) return;
    if (!state.contains("filename") || !state["filename"].isString()) return;
    if (!state.contains("dirty") || !state["dirty"].isBool()) return;
    QJsonDocument character;
    character.setObject(state["character"].toObject());
    mCharacter.fromJson(mOption, character);
    mFilename = state["filename"].toString();
    mChanged = state["dirty"].toBool();
    updateDisplay();
    if (state.contains("power") && state["power"].isObject()) {
        sDialog.Power = std::shared_ptr<PowerDialog> (new PowerDialog(this), [](PowerDialog* d) { d->deleteLater(); });
        sDialog.Power = std::make_shared<PowerDialog>(this);
        sDialog.Power->restore(state["power]"].toObject());
        sDialog.Power->open();
    } else if (state.contains("complications") && state["complications"].isObject()) {
        sDialog.Complications = std::shared_ptr<ComplicationsDialog> (new ComplicationsDialog(this), [](ComplicationsDialog* d) { d->deleteLater(); });
        sDialog.Complications->restore(state["complications"].toObject());
        sDialog.Complications->open();
    } else if (state.contains("skill") && state["skill"].isObject()) {
        auto skillDlg = (sDialog.Skill = std::shared_ptr<SkillDialog> (new SkillDialog(this), [](SkillDialog* d) { d->deleteLater(); }));
        sDialog.Skill->restore(state["skill"].toObject());
        sDialog.Skill->open();
    }
}

void Sheet::saveRecoveryState() {
    QJsonObject state;

    state["character"]  = mCharacter.toJson(mOption).object();
    state["filename"]   = mFilename.toString();
    state["dirty"]      = mChanged;
         if (sDialog.Power)         state["power"]         = sDialog.Power->powerorequipment()->toJson();
    else if (sDialog.Complications) state["complications"] = sDialog.Complications->complication()->toJson();
    else if (sDialog.Skill)         state["skill"]         = sDialog.Skill->skilltalentorperk()->toJson();

    saveSession(state);
}

void Sheet::saveSession(const QJsonObject& json) {
    QJsonDocument state;
    state.setObject(json);

    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(path);
    path += "/HSCCU.state";

    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)) return;

    file.write(state.toJson());
    file.commit();
}

int Sheet::searchImprovedNoncombatMovement(QString name) {
    int mult = 0;
    for (const auto& power: mCharacter.powersOrEquipment()) {
        if (power->name() == name) {
            for (const auto& mod: power->modifiers()) if (mod->name() == "Improved Noncombat Movement") if (mod->doubling() > mult) mult = mod->doubling();
        } else if (power->isFramework()) {
            for (const auto& pwr: power->list()) {
                if (pwr->name() == name) {
                    for (const auto& mod: pwr->modifiers()) if (mod->name() == "Improved Noncombat Movement") if (mod->doubling() > mult) mult = mod->doubling();
                }
            }
        }
    }

    return (int) pow(2, mult + 1);
}

void Sheet::setupIcons() {
#ifdef _WIN64
    QSettings s("HKEY_CURRENT_USER\\SOFTWARE\\CLASSES", QSettings::NativeFormat);

    QString path = QDir::toNativeSeparators(qApp->applicationFilePath()); // NOLINT
    s.setValue(".hsccu/DefaultIcon/.", path);
    s.setValue(".hsccu/.","softwareonhand.hsccu.v1");
    s.setValue("softwareonhand.hsccu.v1/shell/open/command/.", QStringLiteral("\"%1\"").arg(path) + " \"%1\"");

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
#endif
}

void Sheet::setCVs(cCharacteristicDef& def, QLabel* set) {
    int primary = def.characteristic()->base() + def.characteristic()->primary();
    int secondary = primary + def.characteristic()->secondary();
    QString cv = QString("%1").arg(primary);
    if (primary != secondary) cv += QString("/%1").arg(secondary);
    set->setText(cv);
}

void Sheet::setCell(QTableWidget* tbl, int row, int col, QString str, const QFont& font, bool) {
    if (row >= tbl->rowCount()) tbl->setRowCount(row + 1);

    QTableWidgetItem* item = nullptr;
    if (item = tbl->item(row, col); item) item->setText(str);
    else tbl->setItem(row, col, item = new QTableWidgetItem(str));
    item->setFont(font);
    item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
}

void Sheet::setCellLabel(QTableWidget* tbl, int row, int col, QString str, const QFont& font) {
    QLabel* lbl = new QLabel(str);
    lbl->setStyleSheet("color: #000; background: #fff;");
    lbl->setFont(font);
    if (row >= tbl->rowCount()) tbl->setRowCount(row + 1);
    tbl->setCellWidget(row, col, lbl);
}

void Sheet::setCellLabel(QTableWidget* tbl, int row, int col, QString str) {
    QLabel* lbl = new QLabel(str);
    if (row >= tbl->rowCount()) tbl->setRowCount(row + 1);
    QLabel* cell = dynamic_cast<QLabel*>(tbl->cellWidget(row, col));
    lbl->setFont(cell->font());
    tbl->setCellWidget(row, col, lbl);
}

void Sheet::setNotes(bool visible) {
    mOptLabel->setVisible(Sheet::ref().option().showNotesPage());
    mUI->mWidget->adjustSize();
    mUi->graphicsView->scene()->setSceneRect(mUI->mWidget->geometry());
    updateDisplay();
    changed();
}

void Sheet::setDamage(cCharacteristicDef& def, QLabel* set) {
    int primary = def.characteristic()->base() + def.characteristic()->primary();
    int secondary = primary + def.characteristic()->secondary();
    QString dice = valueToDice(primary);
    if (primary != secondary) dice += "/" + valueToDice(secondary);
    set->setText(dice);
}

void Sheet::setDefense(cCharacteristicDef& def, int r, int c, QLineEdit* val) {
    int primary = def.characteristic()->base() + def.characteristic()->primary();
    int secondary = primary + def.characteristic()->secondary();
    if (val == mUI->pdval) secondary += mCharacter.tempPD();
    if (val == mUI->edval) secondary += mCharacter.tempED();
    QString defense = QString("%1").arg(primary);
    if (primary != secondary) defense += QString("/%1").arg(secondary);
    setCell(mUI->defenses, r, c, defense, mUI->font);
}

QString Sheet::setDefense(int def, int temp) {
    QString result = QString::number(def);
    if (temp != 0) result += "/" + QString::number(temp);
    return result;
}

void Sheet::setDefense(int def, int temp, int r, int c) {
    QString defense = QString("%1").arg(def);
    if (temp != 0) defense += QString("/%1").arg(temp + def);
    setCell(mUI->defenses, r, c, defense, mUI->font);
}

void Sheet::setMaximum(cCharacteristicDef& def, QLabel* set, QLineEdit* cur) {
    int primary = def.characteristic()->base() + def.characteristic()->primary();
    int secondary = primary + def.characteristic()->secondary();
    set->setText(QString("%1").arg(secondary));
    cur->setText(set->text());
}

void Sheet::updateCharacteristics() {
    const auto keys = mWidget2Def.keys();
    mCharactersticPoints = 0_cp;
    for (const auto& key: std::as_const(keys)) {
        auto& def = mWidget2Def[key];
        QLineEdit* characteristic = dynamic_cast<QLineEdit*>(key);
        QString val = def.characteristic()->value();
        characteristic->setText(val);
        mCharactersticPoints += def.characteristic()->points();
    }
}

void Sheet::updateCharacter() {
    mUI->charactername->setText(mCharacter.characterName());
    mUI->charactername2->setText(mCharacter.characterName());
    mUI->alternateids->setText(mCharacter.alternateIds());
    mUI->playername->setText(mCharacter.playerName());
    mUI->haircolor->setText(mCharacter.hairColor());
    mUI->eyecolor->setText(mCharacter.eyeColor());
    mUI->campaignname->setText(mCharacter.campaignName());
    mUI->genre->setText(mCharacter.genre());
    mUI->gamemaster->setText(mCharacter.gamemaster());
    mUI->totalexperienceearned->setText(QString("%1").arg(mCharacter.xp().points));
    QPixmap pic;
    pic.loadFromData(mCharacter.imageData());
    mUI->image->setPixmap(pic);
}

void Sheet::updateComplications() {
    mUI->complications->setRowCount(0);
    mUI->complications->update();

    mComplicationPoints = 0_cp;
    QFont font = mUI->complications->font();
    int row = 0;
    bool abbr = option().abbreviations();
    for (const auto& complication: std::as_const(mCharacter.complications())) {
        if (complication == nullptr) continue;

        QString descr = abbr ? complication->abbreviation() : complication->description();
        if (descr == "-") descr = "";
        Points pts = complication->points(Complication::NoStore);
        setCell(mUI->complications, row, 0, descr.isEmpty() ? "" : QString("%1").arg(pts.points), font);
        setCell(mUI->complications, row, 1, descr.isEmpty() ? "" : descr, font, WordWrap);
        mComplicationPoints += pts;
        ++row;
    }
    mUI->complications->resizeRowsToContents();

    mUI->totalcomplicationpts->setText(QString("%1/%2").arg(mComplicationPoints.points).arg(mOption.complications().points));
}

struct hitLocationMapping {
    QLabel* lbl = nullptr;
    QList<int> loc;
};

static QString hit2String(const QList<int>& loc, std::array<int, 19>& hitLoc) { // NOLINT
    if (loc.count() == 1) return QString("%1").arg(hitLoc[loc[0]]); // NOLINT
    bool same = true;
    bool first = true;
    int comp = 0;
    for (const auto& x: std::as_const(loc)) {
        if (first) {
            first = false;
            comp = hitLoc[x]; // NOLINT
        } else if (comp != hitLoc[x]) { // NOLINT
            same = false;
            break;
        }
    }
    QString res = "";
    if (same) return QString("%1").arg(hitLoc[loc[0]]); // NOLINT
    else {
        first = true;
        for (const auto x: std::as_const(loc)) {
            if (first) first = false;
            else res += "/";
            res += QString("%1").arg(hitLoc[x]); // NOLINT
        }
    }
    return res;
}

void Sheet::updateHitLocations() {
    hitLocationMapping mapping[] = { // NOLINT
        {     mUI->head, {3, 4, 5}}, // NOLINT
        {    mUI->hands,       {6}}, // NOLINT
        {     mUI->arms,    {7, 8}}, // NOLINT
        {mUI->shoulders,       {9}}, // NOLINT
        {    mUI->chest,  {10, 11}}, // NOLINT
        {  mUI->stomach,      {12}}, // NOLINT
        {   mUI->vitals,      {13}}, // NOLINT
        {   mUI->thighs,      {14}}, // NOLINT
        {     mUI->legs,  {15, 16}}, // NOLINT
        {     mUI->feet,  {17, 18}}  // NOLINT
    };

    for (const auto& x: std::as_const(mapping)) x.lbl->setText(hit2String(x.loc, mHitLocations));

    int def = 0;
    int baseDEF = mCharacter.rPD();
    for (int i = 9; i <= 14; ++i) def += mHitLocations[i] - baseDEF; // NOLINT
    int count = ((mHitLocations[3] != baseDEF) ? 1 : 0) +
        ((mHitLocations[4] != baseDEF) ? 1 : 0) +
        ((mHitLocations[5] != baseDEF) ? 1 : 0); // NOLINT
    if (count > 1) {
        int max = mHitLocations[3];
        for (int i = 4; i <= 5; ++i) // NOLINT
            if (max < mHitLocations[i]) max = mHitLocations[i] - baseDEF; // NOLINT
        def += max;
    }
    def = baseDEF + (def + 3) / 7; // NOLINT
    mUI->averageDEF->setText(QString("%1").arg(def));
}

void Sheet::updateDisplay() {
    updateCharacter();
    rebuildPowers(false);
    rebuildCharacteristics();
    updateCharacteristics();
    updateComplications();
    updatePowersAndEquipment();
    updateSkillsTalentsAndPerks();
    rebuildCombatSkillLevels();
    rebuildMartialArts();
    rebuildMovement();
    updateTotals();
    updateBanner();
}

void Sheet::updatePower(shared_ptr<Power> power) {
    if (power == nullptr) return;

    if (power->name() == "Skill") updateSkills(power->skill());
    else if (power->name() == "Density Increase") {
        rebuildCharacteristics();
    } else if (power->name() == "Growth") {
        rebuildCharacteristics();
        rebuildMovement();
    } else if (power->name() == "Barrier" ||
               power->name() == "Flash Defense" ||
               power->name() == "Mental Defense" ||
               power->name() == "Power Defense" ||
               power->name() == "Resistant Defense") {
        rebuildCharacteristics();
    } else if (power->name() == "FTL Travel" ||
               power->name() == "Flight" ||
               power->name() == "Leaping" ||
               power->name() == "Running" ||
               power->name() == "Swimming" ||
               power->name() == "Swinging" ||
               power->name() == "Teleportation" ||
               power->name() == "Tunneling") {
        rebuildMovement();
    } else if (power->name() == "Enhanced Senses") {
        rebuildSenses();
    } else if (power->name() == "Characteristics") {
        rebuildCharacteristics();
    } else if (power->name() == "Takes No STUNϴ") {
        rebuildPowers(true);
        rebuildCharacteristics();
    }
    updatePowersAndEquipment();
}

void Sheet::updatePowersAndEquipment() {
    mUI->powersandequipment->setRowCount(0);
    mUI->powersandequipment->update();

    mPowersOrEquipmentPoints = 0_cp;
    int row = 0;
    clearHitLocations();
    for (const auto& pe: std::as_const(mCharacter.powersOrEquipment())) {
        if (pe == nullptr) continue;

        mPowersOrEquipmentPoints += Points(displayPowerAndEquipment(row, pe));
    }
    mUI->powersandequipment->resizeRowsToContents();
    updateHitLocations();

    mUI->totalpowersandequipmentcost->setText(QString("%1").arg(mPowersOrEquipmentPoints.points));
}

void Sheet::updateSkillRolls() {
    QFont font = mUI->skillstalentsandperks->font();
    int row = 0;
    for (const auto& stp: std::as_const(mCharacter.skillsTalentsOrPerks())) {
        if (stp == nullptr) continue;

        stp->points(Complication::NoStore);
        setCell(mUI->skillstalentsandperks, row, 2, stp->roll(), font);
        ++row;
    }
    mUI->skillstalentsandperks->resizeRowsToContents();
}

void Sheet::updateSkillsTalentsAndPerks(){
    mUI->skillstalentsandperks->setRowCount(0);
    mUI->skillstalentsandperks->update();
    mCharacter.clearEnhancers();
    for (const auto& stp: std::as_const(mCharacter.skillsTalentsOrPerks())) {
        if (stp == nullptr) continue;

        if (stp->name() == "Combat Skill Levels" ||
            stp->name() == "Range Skill Levels" ||
            stp->name() == "Skill Levels") rebuildCombatSkillLevels();
        else if (stp->name() == "Martial Arts") rebuildMartialArts();
        else if (stp->name() == "Combat Luck") rebuildCharacteristics();
          else if (stp->name() == "Jack Of All Trades")  mCharacter.hasJackOfAllTrades() = true;
          else if (stp->name() == "Linguist")            mCharacter.hasLinguist() = true;
          else if (stp->name() == "Scholar")             mCharacter.hasScholar() = true;
          else if (stp->name() == "Scientist")           mCharacter.hasScientist() = true;
          else if (stp->name() == "Traveler")            mCharacter.hasTraveler() = true;
          else if (stp->name() == "Well-Connected")      mCharacter.hasWellConnected() = true;
    }

    bool abbr = option().abbreviations();
    mSkillsTalentsOrPerksPoints = 0_cp;
    QFont font = mUI->skillstalentsandperks->font();
    int row = 0;
    for (const auto& stp: std::as_const(mCharacter.skillsTalentsOrPerks())) {
        if (stp == nullptr) continue;
        QString descr = abbr ? stp->abbreviation() : stp->description();
        if (descr == "-") descr = "";
        Points pts = stp->points(Complication::NoStore);
        setCell(mUI->skillstalentsandperks, row, 0, descr.isEmpty() ? "" : QString("%1").arg(pts.points), font);
        setCell(mUI->skillstalentsandperks, row, 1, descr.isEmpty() ? "" : descr,                         font, WordWrap);
        setCell(mUI->skillstalentsandperks, row, 2, descr.isEmpty() ? "" : stp->roll(),                   font);
        mSkillsTalentsOrPerksPoints += pts;
        ++row;
    }
    mUI->skillstalentsandperks->resizeRowsToContents();

    mUI->totalskillstalentsandperkscost->setText(QString("%1").arg(mSkillsTalentsOrPerksPoints.points));
}

void Sheet::updateSkills(shared_ptr<SkillTalentOrPerk> skilltalentorperk) {
    if (skilltalentorperk == nullptr) return;

    if (skilltalentorperk->name() == "Combat Skill Levels" ||
        skilltalentorperk->name() == "Range Skill Levels" ||
        skilltalentorperk->name() == "Skill Levels") rebuildCombatSkillLevels();
    else if (skilltalentorperk->name() == "Combat Luck") {
        rebuildCharacteristics();
        rebuildDefenses();
    }
    else if (skilltalentorperk->name() == "Martial Arts") rebuildMartialArts();
    else if (skilltalentorperk->name() == "Jack Of All Trades" ||
             skilltalentorperk->name() == "Linguist" ||
             skilltalentorperk->name() == "Scholar" ||
             skilltalentorperk->name() == "Scientist" ||
             skilltalentorperk->name() == "Traveler" ||
             skilltalentorperk->name() == "Well-Connected") updateSkillsTalentsAndPerks();
}

void Sheet::updateTotals() {
    mTotalPoints = characteristicsCost() + mSkillsTalentsOrPerksPoints + mPowersOrEquipmentPoints;
    Points pointsEarned = mOption.totalPoints() - mOption.complications();
    if (mOption.complications() < mComplicationPoints) pointsEarned += mOption.complications();
    else pointsEarned += mComplicationPoints;
    mUI->totalpoints->setText(QString("%1/%2").arg(mTotalPoints.points).arg(pointsEarned.points));
    totalExperienceEarnedEditingFinished();
}

QString Sheet::valueToDice(int val, bool showD6) {
    QString halfDice = "½";
    int dice = val / 5; // NOLINT
    bool half = val % 5 > 2; // NOLINT
    return QString("%1%2%3").arg(dice).arg(half ? halfDice : "", showD6 ? "d6" : "");
}

static qreal scales[] { 0.5, 0.75, 0.9, 1.0, 1.25, 1.5, 2.0, 3.0 };
static int numScales = sizeof(scales) / sizeof(qreal);

int  Sheet::zoom() {
    for (int i = 0; i < numScales; ++i) {
        if (qAbs(mStartScale - scales[i]) < 0.01) return i;
    }
    return -1;
}

void Sheet::zoom(QObject* zm) {
    QString txt;
    if (QAction* act = dynamic_cast<QAction*>(zm); act) txt = act->text();
    else if (QPushButton* btn = dynamic_cast<QPushButton*>(zm); btn) txt = btn->text().mid(1).trimmed();
    else return;

    bool ok = true;
    double dec = txt.remove("%").toDouble(&ok) / 100.0;
    if (!ok) return;

    zoom(dec);
}

void Sheet::zoom(qreal zm) {
    if (zm < 0.5) zm = 0.5;
    if (zm > 3.0) zm = 3.0;
    mUi->graphicsView->resetTransform();
    mUi->graphicsView->scale(zm, zm);
#ifndef __wasm__
    for (const auto& action: std::as_const(mZooms)) action->setChecked(false);
    for (int i = 0; i < numScales; ++i) {
        if (qAbs(zm - scales[i]) < 0.01) {
            mZooms[i]->setChecked(true);
            break;
        }
    }
#endif
    mStartScale = zm;
}

void Sheet::zoomIn() {
    if (mStartScale >= 3.0) return;
    int i;
    for (i = 0; i < numScales; ++i) {
        if (scales[i] > mStartScale) break;
    }
    zoom(scales[i]);
}

void Sheet::zoomOut() {
    if (mStartScale <= 0.5) return;
    int i;
    for (i = numScales - 1; i != 0; --i) {
        if (scales[i] < mStartScale) break;
    }
    zoom(scales[i]);
}

// ---[SLOTS] --------------------------------------------------------------------------------------------

#ifndef __wasm__
void Sheet::aboutToHideEditMenu() {
    mUi->action_Paste->setEnabled(true);
}

void Sheet::aboutToHideFileMenu() {
    mUi->action_Save->setEnabled(true);
}

#endif

void Sheet::aboutToShowComplicationsMenu() {
    const auto selection = mUI->complications->selectedItems();
    bool show = !selection.isEmpty();
    int row = -1;
    if (show) row = selection[0]->row();
#if !defined(__wasm__) && !defined(Q_OS_ANDROID)
    QClipboard* clipboard = QGuiApplication::clipboard();
    const QMimeData* clip = clipboard->mimeData();
    bool canPaste = clip->hasFormat("application/complication");

    mUI->editComplication->setEnabled(show);
    mUI->deleteComplication->setEnabled(show);
    mUI->cutComplication->setEnabled(show);
    mUI->copyComplication->setEnabled(show);
    mUI->moveComplicationUp->setEnabled(show && row != 0);
    mUI->moveComplicationDown->setEnabled(show && row != mCharacter.complications().count() - 1);
    mUI->pasteComplication->setEnabled(canPaste);
#else
    bool canPaste = false;
    // see if anything is under the mouse pointer: select it if there is
    auto compMenuDialog = sDialog.ComplicationsMenu;
    compMenuDialog->setEdit(show);
    compMenuDialog->setDelete(show);
    compMenuDialog->setCut(show);
    compMenuDialog->setCopy(show);
    compMenuDialog->setPaste(canPaste);
    compMenuDialog->setMoveUp(show && row != 0);
    compMenuDialog->setMoveDown(show && mCharacter.complications().count() != 0 && row != mCharacter.complications().count() - 1);
#endif
}

#ifndef __wasm__
void Sheet::aboutToShowEditMenu() {
    QClipboard* clipboard = QGuiApplication::clipboard();
    const QMimeData* clip = clipboard->mimeData();
    bool canPaste = clip->hasFormat("application/hsccuchar");
    mUi->action_Paste->setEnabled(canPaste);
}

void Sheet::aboutToShowFileMenu() {
    mUi->action_Save->setEnabled(mChanged);
}
#endif

void Sheet::aboutToShowPowersAndEquipmentMenu() {
    const auto selection = mUI->powersandequipment->selectedItems();
    bool show = !selection.isEmpty();
    int row = -1;
    if (show) row = selection[0]->row();
#if defined(__wasm__) || defined(Q_OS_ANDROID)
    auto powerMenuDialog = sDialog.PowerMenu;
    powerMenuDialog->setEdit(show);
    powerMenuDialog->setDelete(show);
    powerMenuDialog->setCut(show);
    powerMenuDialog->setCopy(show);
    powerMenuDialog->setMoveUp(show && row != 0);
    powerMenuDialog->setMoveDown(show && row !=mUI->powersandequipment->rowCount() - 1);
    auto power = getPower(row, mCharacter.powersOrEquipment());
    mUI->movePowerOrEquipmentDown->setEnabled(show && (row != mUI->powersandequipment->rowCount() - 1 || power->parent() != nullptr));
#ifdef __wasm__
    bool canPaste = false;
#else
    QClipboard* clipboard = QGuiApplication::clipboard();
    const QMimeData* clip = clipboard->mimeData();
    bool canPaste = clip->hasFormat("application/powerorequipment");
#endif
    powerMenuDialog->setPaste(canPaste);
#else
    mUI->editPowerOrEquipment->setEnabled(show);
    mUI->deletePowerOrEquipment->setEnabled(show);
    mUI->cutPowerOrEquipment->setEnabled(show);
    mUI->copyPowerOrEquipment->setEnabled(show);
    mUI->movePowerOrEquipmentUp->setEnabled(show && row != 0);
    auto power = getPower(row, mCharacter.powersOrEquipment());
    mUI->movePowerOrEquipmentDown->setEnabled(show && (row != mUI->powersandequipment->rowCount() - 1 || power->parent() != nullptr));
    QClipboard* clipboard = QGuiApplication::clipboard();
    const QMimeData* clip = clipboard->mimeData();
    bool canPaste = clip->hasFormat("application/powerorequipment");
    mUI->pastePowerOrEquipment->setEnabled(canPaste);
#endif
}

void Sheet::aboutToShowSkillsPerksAndTalentsMenu() {
    const auto selection = mUI->skillstalentsandperks->selectedItems();
    bool show = !selection.isEmpty();
    int row = -1;
    if (show) row = selection[0]->row();
#if defined(__wasm__) || defined(Q_OS_ANDROID)
#ifdef __wasm__
    bool canPaste = false;
#else
    QClipboard* clipboard = QGuiApplication::clipboard();
    const QMimeData* clip = clipboard->mimeData();
    bool canPaste = clip->hasFormat("application/skillperkortalent");
#endif
    auto skillMenuDialog = sDialog.SkillMenu;
    skillMenuDialog->setEdit(show);
    skillMenuDialog->setDelete(show);
    skillMenuDialog->setCut(show);
    skillMenuDialog->setCopy(show);
    skillMenuDialog->setPaste(canPaste);
    skillMenuDialog->setMoveUp(show && row != 0);
    skillMenuDialog->setMoveDown(show && row != mCharacter.skillsTalentsOrPerks().count() - 1);
#else
    QClipboard* clipboard = QGuiApplication::clipboard();
    const QMimeData* clip = clipboard->mimeData();
    bool canPaste = clip->hasFormat("application/skillperkortalent");

    mUI->editSkillTalentOrPerk->setEnabled(show);
    mUI->deleteSkillTalentOrPerk->setEnabled(show);
    mUI->cutSkillTalentOrPerk->setEnabled(show);
    mUI->copySkillTalentOrPerk->setEnabled(show);
    mUI->pasteSkillTalentOrPerk->setEnabled(canPaste);
    mUI->moveSkillTalentOrPerkUp->setEnabled(show && row != 0);
    mUI->moveSkillTalentOrPerkDown->setEnabled(show && row != mCharacter.skillsTalentsOrPerks().count() - 1);
#endif
}

void Sheet::alternateIdsChanged(QString txt) {
    mCharacter.alternateIds(txt);
    mChanged = true;
}

void Sheet::campaignNameChanged(QString txt) {
    mCharacter.campaignName(txt);
    mChanged = true;
}

void Sheet::characterNameChanged(QString txt) {
    mUI->charactername2->setText(txt);
    mCharacter.characterName(txt);
    mChanged = true;
}

void Sheet::clearImage() {
    mUI->image->clear();
    mCharacter.image() = "";
    mCharacter.imageData().clear();
    mChanged = true;
}

void Sheet::copyCharacter() {
    mCharacter.notes() = mUI->notes->toPlainText();

    QJsonDocument doc = mCharacter.copy(mOption);
    QClipboard* clip = QGuiApplication::clipboard();
    QMimeData* dat = new QMimeData();
    dat->setData("application/complication", doc.toJson());
    QString text = getCharacter();
    dat->setData("text/plain", text.toUtf8());
    clip->setMimeData(dat);
}

void Sheet::copyComplication() {
    bool abbr = option().abbreviations();
    QClipboard* clip = QGuiApplication::clipboard();
    QMimeData* dat = new QMimeData();
    auto selection = mUI->complications->selectedItems();
    int row = selection[0]->row();
    shared_ptr<Complication> complication = mCharacter.complications()[row];
    QJsonObject obj = complication->toJson();
    QJsonDocument doc;
    doc.setObject(obj);
    dat->setData("application/complication", doc.toJson());
    QString descr = abbr ? complication->abbreviation() : complication->description();
    QString text = QString("%1\t%2").arg(complication->points(Complication::NoStore).points).arg(descr);
    dat->setData("text/plain", text.toUtf8());
    clip->setMimeData(dat);
}

void Sheet::copyPowerOrEquipment() {
    bool abbr = option().abbreviations();
    QClipboard* clip = QGuiApplication::clipboard();
    QMimeData* dat = new QMimeData();
    auto selection = mUI->powersandequipment->selectedItems();
    int row = selection[0]->row();
    shared_ptr<Power> power = getPower(row, mCharacter.powersOrEquipment());
    QJsonObject obj = power->toJson();
    QJsonDocument doc;
    doc.setObject(obj);
    dat->setData("application/powerorequipment", doc.toJson());
    QString descr = abbr ? power->abbreviation() : power->description();
    QString text = QString("%1\t%2").arg(power->points(Power::NoStore).points).arg(descr);
    dat->setData("text/plain", text.toUtf8());
    clip->setMimeData(dat);
}

void Sheet::copySkillTalentOrPerk() {
    bool abbr = option().abbreviations();
    QClipboard* clip = QGuiApplication::clipboard();
    QMimeData* dat = new QMimeData();
    auto selection = mUI->skillstalentsandperks->selectedItems();
    int row = selection[0]->row();
    shared_ptr<SkillTalentOrPerk> skilltalentorperk = mCharacter.skillsTalentsOrPerks()[row];
    QJsonObject obj = skilltalentorperk->toJson();
    QJsonDocument doc;
    doc.setObject(obj);
    dat->setData("application/skillperkortalent", doc.toJson());
    QString descr = abbr ? skilltalentorperk->abbreviation() : skilltalentorperk->description();
    QString text = QString("%1\t%2\t%3").arg(skilltalentorperk->points(SkillTalentOrPerk::NoStore).points)
            .arg(descr, skilltalentorperk->roll());
    dat->setData("text/plain", text.toUtf8());
    clip->setMimeData(dat);
}

void Sheet::complicationsMenu(QPoint pos) {
#if defined(__wasm__) || defined(Q_OS_ANDROID)
    int row = mUI->complications->rowAt(mUI->complications->viewport()->mapFromGlobal(pos).y());
    mUI->complications->selectRow(row);
    closeDialogs(nullptr);
#ifdef Q_OS_ANDROID
    auto compMenuDialog = (sDialog.ComplicationsMenu = std::shared_ptr<ComplicationsMenuDialog> (new ComplicationsMenuDialog(), [](ComplicationsMenuDialog* d) { d->deleteLater(); }));
#else
    auto compMenuDialog = (sDialog.ComplicationsMenu = std::shared_ptr<ComplicationsMenuDialog> (new ComplicationsMenuDialog()));
#endif
    compMenuDialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    compMenuDialog->setPos(pos);
    aboutToShowComplicationsMenu();
    compMenuDialog->open();
#else
    mUI->complicationsMenu->exec(pos);
#endif
}

void Sheet::currentBODYChanged(QString txt) {
    if (!numeric(txt) && !txt.isEmpty()) mUI->currentbody->undo();
}

void Sheet::currentBODYEditingFinished() {
    currentBODYChanged(mUI->currentbody->text());
    if (mUI->currentbody->text().isEmpty()) mUI->currentbody->setText("0");
}

void Sheet::currentENDChanged(QString txt) {
    if (!numeric(txt) && !txt.isEmpty()) mUI->currentend->undo();
}

void Sheet::currentENDEditingFinished() {
    currentENDChanged(mUI->currentend->text());
    if (mUI->currentend->text().isEmpty()) mUI->currentend->setText("0");
}

void Sheet::currentSTUNChanged(QString txt) {
    if (!numeric(txt) && !txt.isEmpty()) mUI->currentstun->undo();
}

void Sheet::currentSTUNEditingFinished() {
    currentSTUNChanged(mUI->currentstun->text());
    if (mUI->currentstun->text().isEmpty()) mUI->currentstun->setText("0");
}

void Sheet::cutCharacter() {
    copyCharacter();
    newchar();
}

void Sheet::cutComplication() {
    copyComplication();
    deleteComplication();
}

void Sheet::cutPowerOrEquipment() {
    copyPowerOrEquipment();
    deletePowerOrEquipment();
}

void Sheet::cutSkillTalentOrPerk() {
    copySkillTalentOrPerk();
    deleteSkillstalentsandperks();
}

void Sheet::deleteComplication() {
#ifdef __wasm__
    closeDialogs(nullptr);
#endif

    auto selection = mUI->complications->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    shared_ptr<Complication> complication = mCharacter.complications().takeAt(row);
    if (complication == nullptr) return;

    mComplicationPoints -= complication->points(Complication::NoStore);
    mUI->complications->removeRow(row);
    mUI->totalcomplicationpts->setText(QString("%1/%2").arg(mComplicationPoints.points).arg(mOption.complications().points));
    updateDisplay();
    mChanged = true;
}

void Sheet::deletePowerOrEquipment() {
    auto selection = mUI->powersandequipment->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    auto power = getPower(row, mCharacter.powersOrEquipment());
    if (power == nullptr) return;

    delPower(row);
    mPowersOrEquipmentPoints -= power->points(Power::NoStore);
    updateDisplay();
    mChanged = true;
}

void Sheet::deleteSkillstalentsandperks() {
    auto selection = mUI->skillstalentsandperks->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    shared_ptr<SkillTalentOrPerk> skilltalentorperk = mCharacter.skillsTalentsOrPerks().takeAt(row);
    if (skilltalentorperk == nullptr) return;

    mSkillsTalentsOrPerksPoints -= skilltalentorperk->points(Complication::NoStore);

    updateSkills(skilltalentorperk);

    mUI->skillstalentsandperks->removeRow(row);
    mUI->totalskillstalentsandperkscost->setText(QString("%1").arg(mSkillsTalentsOrPerksPoints.points));
    updateDisplay();
    mChanged = true;
}

void Sheet::doneEditComplication() {
    auto compDlg = sDialog.Complications;
    shared_ptr<Complication> complication = compDlg->complication();
    if (complication->description().isEmpty()) return;

    updateDisplay();
    mChanged = true;
}

void Sheet::doneEditSkill() {
    auto skillDlg = sDialog.Skill;
    shared_ptr<SkillTalentOrPerk> skilltalentorperk = skillDlg->skilltalentorperk();
    if (skilltalentorperk->description().isEmpty()) return;

    updateDisplay();
    mChanged = true;
}

void Sheet::editComplication() {
    auto selection = mUI->complications->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    shared_ptr<Complication> complication = mCharacter.complications()[row];
    if (complication == nullptr) return;

    auto compDlg = (sDialog.Complications = std::shared_ptr<ComplicationsDialog> (new ComplicationsDialog(this), [](ComplicationsDialog* d) { d->deleteLater(); }));
    compDlg->complication(complication);
    connect(compDlg.get(), SIGNAL(accepted()), this, SLOT(doneEditComplication()));

#ifdef __wasm__
    closeDialogs(nullptr);
#endif
    compDlg->open();
}

void Sheet::editPowerOrEquipment() {
    auto selection = mUI->powersandequipment->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    shared_ptr<Power>& power = getPower(row, mCharacter.powersOrEquipment());
    if (power == nullptr) return;

    auto powerDlg = (sDialog.Power = std::shared_ptr<PowerDialog> (new PowerDialog(this, power), [](PowerDialog* d) { d->deleteLater(); }));
    powerDlg->powerorequipment(power);
    powerDlg->open();
}

void Sheet::editSkillstalentsandperks() {
    auto selection = mUI->skillstalentsandperks->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    shared_ptr<SkillTalentOrPerk> skilltalentorperk = mCharacter.skillsTalentsOrPerks()[row];
    if (skilltalentorperk == nullptr) return;

    auto skillDlg = (sDialog.Skill = std::shared_ptr<SkillDialog> (new SkillDialog(this), [](SkillDialog* d) { d->deleteLater(); }));
    skillDlg->skilltalentorperk(skilltalentorperk);
    connect(skillDlg.get(), SIGNAL(accepted()), this, SLOT(doneEditSkill()));

#ifdef __wasm__
    closeDialogs(nullptr);
#endif
    skillDlg->open();
}

void Sheet::eyeColorChanged(QString txt) {
    mCharacter.eyeColor(txt);
    mChanged = true;
}

#if defined(__wasm__) || defined(Q_OS_ANDROID)
#ifdef __wasm__
void Sheet::editMenu(bool) {
    closeDialogs(nullptr);
#ifdef Q_OS_ANDROID
    auto editMenuDialog = (sDialog.EditMenu = std::shared_ptr<EditMenuDialog> (new EditMenuDialog(), [](EditMenuDialog* d) { d->deleteLater(); }));
#else
    auto editMenuDialog = (sDialog.EditMenu = std::make_shared<EditMenuDialog>());
#endif
    editMenuDialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    editMenuDialog->open();
}

void Sheet::fileMenu(bool) {
    closeDialogs(nullptr);
#ifdef Q_OS_ANDROID
    auto fileMenuDialog = (sDialog.FileMenu = std::shared_ptr<FileMenuDialog> (new FileMenuDialog(), [](FileMenuDialog* d) { d->deleteLater(); }));
#else
    auto fileMenuDialog = (sDialog.FileMenu = std::make_shared<FileMenuDialog>());
#endif
    fileMenuDialog->setSave(mChanged);
    fileMenuDialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    fileMenuDialog->open();
}
#endif
void Sheet::imgMenu(bool) {
    closeDialogs(nullptr);
#ifdef Q_OS_ANDROID
    auto imgMenuDialog = (sDialog.ImgMenu = std::shared_ptr<ImgMenuDialog> (new ImgMenuDialog(), [](ImgMenuDialog* d) { d->deleteLater(); }));
#else
    auto imgMenuDialog = (sDialog.ImgMenu = std::make_shared<ImgMenuDialog>());
#endif
    imgMenuDialog->setPos(QPoint());
    imgMenuDialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    imgMenuDialog->open();
}

void Sheet::powerMenu(bool) {
    closeDialogs(nullptr);
#ifdef Q_OS_ANDROID
    auto powerMenuDialog = (sDialog.PowerMenu = std::shared_ptr<PowerMenuDialog> (new PowerMenuDialog(), [](PowerMenuDialog* d) { d->deleteLater(); }));
#else
    auto powerMenuDialog = (sDialog.PowerMenu = std::make_shared<PowerMenuDialog>());
#endif
    aboutToShowPowersAndEquipmentMenu();
    powerMenuDialog->setPos(QPoint());
    powerMenuDialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    powerMenuDialog->open();
}

void Sheet::viewMenu(bool) {
    closeDialogs(nullptr);
#ifdef __wasm__
    auto viewMenuDialog = (sDialog.ViewMenu = std::make_shared<ViewMenuDialog>());
    viewMenuDialog->setPos(QPoint());
    viewMenuDialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    viewMenuDialog->open();
#endif
}

void Sheet::stpMenu(bool) {
    closeDialogs(nullptr);
#ifdef Q_OS_ANDROID
    auto skillMenuDialog = (sDialog.SkillMenu = std::shared_ptr<SkillMenuDialog> (new SkillMenuDialog(), [](SkillMenuDialog* d) { d->deleteLater(); }));
#else
    auto skillMenuDialog = (sDialog.SkillMenu = std::make_shared<SkillMenuDialog>());
#endif
    aboutToShowSkillsPerksAndTalentsMenu();
    skillMenuDialog->setPos(QPoint());
    skillMenuDialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    skillMenuDialog->open();
}

void Sheet::compMenu(bool) {
    closeDialogs(nullptr);
#ifdef Q_OS_ANDROID
    auto compMenuDialog = (sDialog.ComplicationsMenu = std::shared_ptr<ComplicationsMenuDialog> (new ComplicationsMenuDialog(), [](ComplicationsMenuDialog* d) { d->deleteLater(); }));
#else
    auto compMenuDialog = (sDialog.ComplicationsMenu = std::make_shared<ComplicationsMenuDialog>());
#endif
    aboutToShowComplicationsMenu();
    compMenuDialog->setPos(QPoint());
    compMenuDialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    compMenuDialog->open();
}
#endif

void Sheet::gamemasterChanged(QString txt) {
    mCharacter.gamemaster(txt);
    mChanged = true;
}

void Sheet::focusChanged(QWidget*, QWidget* focus) {
    if (focus == mUI->strval  ||
        focus == mUI->dexval  ||
        focus == mUI->conval  ||
        focus == mUI->intval  ||
        focus == mUI->egoval  ||
        focus == mUI->preval  ||
        focus == mUI->ocvval  ||
        focus == mUI->dcvval  ||
        focus == mUI->omcvval ||
        focus == mUI->dmcvval ||
        focus == mUI->spdval  ||
        focus == mUI->pdval   ||
        focus == mUI->edval   ||
        focus == mUI->recval  ||
        focus == mUI->endval  ||
        focus == mUI->bodyval ||
        focus == mUI->stunval) characteristicChanged(dynamic_cast<QLineEdit*>(focus), "", DontUpdateTotal);
}

void Sheet::genreChanged(QString txt) {
    mCharacter.genre(txt);
    mChanged = true;
}

void Sheet::hairColorChanged(QString txt) {
    mCharacter.hairColor(txt);
    mChanged = true;
}

void Sheet::imageMenu(QPoint pos) {
#if defined(__wasm__) || defined(Q_OS_ANDROID)
    closeDialogs(nullptr);
#ifdef Q_OS_ANDROID
    auto imgMenuDialog = (sDialog.ImgMenu = std::shared_ptr<ImgMenuDialog> (new ImgMenuDialog(), [](ImgMenuDialog* d) { d->deleteLater(); }));
#else
    auto imgMenuDialog = (sDialog.ImgMenu = std::shared_ptr<ImgMenuDialog> (new ImgMenuDialog()));
#endif
    imgMenuDialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    imgMenuDialog->setPos(pos);
    imgMenuDialog->open();
#else
    mUI->imageMenu->exec(pos);
#endif
}

void Sheet::moveComplicationDown() {
    auto selection = mUI->complications->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    auto& complications = mCharacter.complications();
    shared_ptr<Complication> complication = complications.takeAt(row);
    complications.insert(row + 1, complication);
    updateComplications();
}

void Sheet::moveComplicationUp() {
    auto selection = mUI->complications->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    auto& complications = mCharacter.complications();
    shared_ptr<Complication> complication = complications.takeAt(row);
    complications.insert(row - 1, complication);
    updateComplications();
}

void Sheet::movePowerOrEquipmentDown() {
    auto selection = mUI->powersandequipment->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    auto& powers = mCharacter.powersOrEquipment();
    auto power = getPower(row, powers);
    delPower(row);
    putPower(row + 2, power);
    updateDisplay();
}

void Sheet::movePowerOrEquipmentUp() {
    auto selection = mUI->powersandequipment->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    auto& powers = mCharacter.powersOrEquipment();
    auto power = getPower(row, powers);
    delPower(row);
    putPower(row - 1, power);
    updateDisplay();
}

void Sheet::moveSkillTalentOrPerkDown() {
    auto selection = mUI->skillstalentsandperks->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    auto& stps = mCharacter.skillsTalentsOrPerks();
    auto stp = stps.takeAt(row);
    stps.insert(row + 1, stp);
    updateSkillsTalentsAndPerks();
}

void Sheet::moveSkillTalentOrPerkUp() {
    auto selection = mUI->skillstalentsandperks->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    auto& skillstalentsorperks = mCharacter.skillsTalentsOrPerks();
    shared_ptr<SkillTalentOrPerk> skilltalentorperk = skillstalentsorperks.takeAt(row);
    skillstalentsorperks.insert(row - 1, skilltalentorperk);
    updateSkillsTalentsAndPerks();
}

void Sheet::erase() {
    mCharacter.erase();
    mUI->notes->setPlainText("");
    updateDisplay();
    mChanged = false;
    mFilename.clear();
}

void Sheet::saveThenErase() {
    try { save(); } catch(...) { return; }
    if (mChanged) return;
    erase();
}

void Sheet::newchar() {
    if (mChanged) {
        YesNoCancel("Do you want to save your changes first?",
                    std::bind(&Sheet::saveThenErase, this),
                    std::bind(&Sheet::erase, this),
                    std::bind(&Sheet::doNothing, this),
                    "The current sheet has been changed!");
    } else erase();
}

void Sheet::acceptComplication() {
    auto complication = sDialog.Complications->complication();
    if (complication == nullptr) return;
    if (complication->description().isEmpty()) return;

    mCharacter.complications().append(complication);

    updateDisplay();
    mChanged = true;
}

void Sheet:: newComplication() {
    auto compDlg = (sDialog.Complications = std::shared_ptr<ComplicationsDialog> (new ComplicationsDialog(this), [](ComplicationsDialog* d) { d->deleteLater(); }));
    connect(compDlg.get(), SIGNAL(accepted()), this, SLOT(acceptComplication()));

#ifdef __wasm__
    closeDialogs(nullptr);
#endif
    compDlg->open();
}

void Sheet::newImage() {
#ifdef __wasm__
    QFileDialog::getOpenFileContent("Images (*.png *.xpm *jpg)", [&](const QString &fileName, const QByteArray &fileContent) {
        loadImage(fileContent, fileName);
    });
#else
    QUrl filename = QFileDialog::getOpenFileUrl(this, "New Image", QUrl(), "Images (*.png *.xpm *.jpg *.jpeg *.bmp *.webp)");
    if (filename.isEmpty()) return;
    loadImage(filename);
#endif
}

void Sheet::newPowerOrEquipment() {
    bool framework = false;
    auto selection = mUI->powersandequipment->selectedItems();
    if (!selection.isEmpty()) {
        shared_ptr<Power> work = getPower(selection[0]->row(), mCharacter.powersOrEquipment());
        if (work == nullptr) framework = false;
        else if (work->isFramework()) framework = work->isMultipower();
        else {
            auto parent = work->parent();
            framework = parent != nullptr && parent->isMultipower();
        }
    }

    auto powerDlg = (sDialog.Power = std::shared_ptr<PowerDialog> (new PowerDialog(this), [](PowerDialog* d) { d->deleteLater(); }));
    if (framework) powerDlg->multipower();
    powerDlg->open();
}

void Sheet::acceptNewSkill() {
    auto skilltalentorperk = sDialog.Skill->skilltalentorperk();
    if (skilltalentorperk == nullptr) return;
    if (skilltalentorperk->description().isEmpty()) return;

    mCharacter.skillsTalentsOrPerks().append(skilltalentorperk);

    updateDisplay();
    mChanged = true;
}

void Sheet::newSkillTalentOrPerk() {
    auto skillDlg = (sDialog.Skill = std::shared_ptr<SkillDialog> (new SkillDialog(this), [](SkillDialog* d){ d->deleteLater(); }));
    connect(skillDlg.get(), SIGNAL(accepted()), this, SLOT(acceptNewSkill()));
    skillDlg->open();
}

void Sheet::noteChanged() {
    mChanged = true;
}

void Sheet::doOpen() {
#ifdef __wasm__
    QFileDialog::getOpenFileContent("Characters (*.hsccu)", [&](const QString& fileName, const QByteArray& fileContent) {
        fileOpen(fileContent, fileName);
    });
#else
    if (mFilename.isEmpty()) mFilename = QUrl::fromLocalFile(mDir + "/" + mCharacter.characterName() + ".hsccu");
    QUrl filename = QFileDialog::getOpenFileUrl(this, "Open File", mFilename, "Characters (*.hsccu)");
    if (filename.isEmpty()) return;
    mFilename = filename;

    fileOpen();
#endif
}

void Sheet::saveThenOpen() {
    save();
    if (mChanged) return;
    doOpen();
}

void Sheet::open() {
    if (mChanged) {
        YesNoCancel("Do you want to save your changes first?",
                    std::bind(&Sheet::saveThenOpen, this),
                    std::bind(&Sheet::doOpen, this),
                    std::bind(&Sheet::doNothing, this),
                    "The current sheet has been changed!");
    } else doOpen();
}

#if defined(__wasm__) || defined(Q_OS_ANDROID)
void Sheet::outsideImageArea() {
    closeDialogs(nullptr);
}
#endif

void Sheet::options() {
    mOptionDlg = make_shared<optionDialog>();
    mOptionDlg->setAbbreviations(mOption.abbreviations());
    mOptionDlg->setGreenFields(mOption.greenfields());
    mOptionDlg->setBanner(mOption.banner());
    mOptionDlg->setComplications(mOption.complications().points);
    mOptionDlg->setShowFrequencyRolls(mOption.showFrequencyRolls());
    mOptionDlg->setShowNotesPage(mOption.showNotesPage());
    mOptionDlg->setNormalHumanMaxima(mOption.normalHumanMaxima());
    mOptionDlg->setActivePointsPerEND(mOption.activePerEND().points);
    mOptionDlg->setEquipmentFree(mOption.equipmentFree());
    mOptionDlg->setTotalPoints(mOption.totalPoints().points);
    mOptionDlg->open();
}

void Sheet::paste() {
    QClipboard* clip = QGuiApplication::clipboard();
    const QMimeData* dat = clip->mimeData();
    QByteArray byteArray = dat->data("application/hsccucharacter");
    QString jsonStr(byteArray);
    if (byteArray.isEmpty()) return;
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
    mCharacter.erase();
    mCharacter.paste(mOption, doc);
    updateDisplay();
    mChanged = true;
}

void Sheet::saveThenPaste() {
    try { save(); } catch(...) { return; }
    if (mChanged) return;
    paste();
}

void Sheet::pasteCharacter() {
    if (mChanged) {
        YesNoCancel("Do you want to save your changes first?",
                    std::bind(&Sheet::saveThenPaste, this),
                    std::bind(&Sheet::paste, this),
                    std::bind(&Sheet::doNothing, this),
                    "The current sheet has been changed!");
    } else paste();
}

void Sheet::pasteComplication() {
    bool abbr = option().abbreviations();
    QClipboard* clip = QGuiApplication::clipboard();
    const QMimeData* dat = clip->mimeData();
    QByteArray byteArray = dat->data("application/complication");
    QString jsonStr(byteArray);
    QJsonDocument json = QJsonDocument::fromJson(jsonStr.toUtf8());
    QJsonObject obj = json.object();
    QString name = obj["name"].toString();
    shared_ptr<Complication> complication = Complication::FromJson(name, obj);
    mCharacter.complications().append(complication);

    int row = mUI->complications->rowCount();
    QFont font = mUI->complications->font();
    QString descr = abbr ? complication->abbreviation() : complication->description();
    setCell(mUI->complications, row, 0, QString("%1").arg(complication->points(Complication::NoStore).points), font);
    setCell(mUI->complications, row, 1, descr,                                                                 font, WordWrap);
    mUI->complications->resizeRowsToContents();

    mComplicationPoints += complication->points(Complication::NoStore);
    mUI->totalcomplicationpts->setText(QString("%1/%2").arg(mComplicationPoints.points).arg(mOption.complications().points));
    updateDisplay();
    mChanged = true;
}

void Sheet::pastePowerOrEquipment() {
    QClipboard* clip = QGuiApplication::clipboard();
    const QMimeData* dat = clip->mimeData();
    QByteArray byteArray = dat->data("application/powerorequipment");
    QString jsonStr(byteArray);
    QJsonDocument json = QJsonDocument::fromJson(jsonStr.toUtf8());
    QJsonObject obj = json.object();
    QString name = obj["name"].toString();
    shared_ptr<Power> power = Power::FromJson(name, obj);
    addPower(power);
}

void Sheet::pasteSkillTalentOrPerk() {
    bool abbr = option().abbreviations();
    QClipboard* clip = QGuiApplication::clipboard();
    const QMimeData* dat = clip->mimeData();
    QByteArray byteArray = dat->data("application/skillperkortalent");
    QString jsonStr(byteArray);
    QJsonDocument json = QJsonDocument::fromJson(jsonStr.toUtf8());
    QJsonObject obj = json.object();
    QString name = obj["name"].toString();
    shared_ptr<SkillTalentOrPerk> stp = SkillTalentOrPerk::FromJson(name, obj);
    mCharacter.skillsTalentsOrPerks().append(stp);

    int row = mUI->skillstalentsandperks->rowCount();
    QFont font = mUI->skillstalentsandperks->font();
    QString descr = abbr ? stp->abbreviation() : stp->description();
    setCell(mUI->skillstalentsandperks, row, 0, QString("%1").arg(stp->points(Complication::NoStore).points), font);
    setCell(mUI->skillstalentsandperks, row, 1, descr, font, WordWrap);
    setCell(mUI->skillstalentsandperks, row, 2, stp->roll(), font);
    mUI->skillstalentsandperks->resizeRowsToContents();

    updateSkills(stp);

    mSkillsTalentsOrPerksPoints += stp->points(Complication::NoStore);
    mUI->totalskillstalentsandperkscost->setText(QString("%1").arg(mSkillsTalentsOrPerksPoints.points));
    updateDisplay();
    mChanged = true;
}

void Sheet::playerNameChanged(QString txt) {
    mCharacter.playerName(txt);
    mChanged = true;
}

void Sheet::powersandequipmentMenu(QPoint pos) {
#if defined( __wasm__) || defined(Q_OS_ANDROID)
    int row = mUI->powersandequipment->rowAt(mUI->powersandequipment->viewport()->mapFromGlobal(pos).y());
    mUI->powersandequipment->selectRow(row);
    closeDialogs(nullptr);
#ifdef Q_OS_ANDROID
    auto powerMenuDialog = (sDialog.PowerMenu = std::shared_ptr<PowerMenuDialog> (new PowerMenuDialog(), [](PowerMenuDialog* d) { d->deleteLater(); }));
#else
    auto powerMenuDialog = (sDialog.PowerMenu = std::shared_ptr<PowerMenuDialog> (new PowerMenuDialog()));
#endif
    powerMenuDialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    powerMenuDialog->setPos(pos);
    aboutToShowPowersAndEquipmentMenu();
    powerMenuDialog->open();
#else
    mUI->powersandequipmentMenu->exec(pos);
#endif
}

void Sheet::printSheet() {
    bool saveChanged = mChanged;

    auto printDlg = (sDialog.Print = std::shared_ptr<PrintDialog> (new PrintDialog(this), [](PrintDialog* d) { d->deleteLater(); }));
    printDlg->open();

    mChanged = saveChanged; // lots of changed signals get passed around but the character really didn't change
}

static constexpr auto pdfPPI = 72.0;
static constexpr auto halfInch = pdfPPI / 2.0;
static constexpr auto fullPage = 11.0;

void Sheet::printCharacter(Printer* printer) {
    QPixmap page1(QString(":/gfx/Page1.png"));
    QPixmap page2(QString(":/gfx/Page2.png"));
    QPixmap page3(QString(":/gfx/Page3.png"));

    auto pageLayout = printer->qprinter()->pageLayout();
    pageLayout.setOrientation(printer->pageOrientation());
    printer->qprinter()->setPageLayout(pageLayout);
    printer->setFullPage(false);
    QPainter painter;
    painter.begin(printer->qprinter());
    QRectF pageRect = printer->pageRect(QPrinter::DevicePixel);
    double pnt = pageRect.height() / (fullPage * pdfPPI);
    double xscale = (pageRect.width() - pdfPPI * pnt) / page1.width();
    double yscale = (pageRect.height() - pdfPPI * pnt) / page1.height();
    double scale = qMin(xscale, yscale);
    painter.translate(QPoint({ (int) (halfInch * pnt), (int) (halfInch * pnt) }));
    painter.scale(scale, scale);

    QPoint offset { 55, 48 }; // NOLINT
    painter.drawImage(QPointF { 0.0, 0.0 }, page1.toImage());
    for (int i = 0; i < mUI->widgets.count(); ++i) {
        auto& widget = mUI->widgets[i];
        if (widget == nullptr || widget->y() > 1250) continue; // NOLINT  skip things we can't render or are on the seecond page
        print(painter, offset, widget);
    }

    int skillTop        = mUI->skillstalentsandperks->verticalScrollBar()->value();
    int complicationTop = mUI->complications->verticalScrollBar()->value();
    int powerTop        = mUI->powersandequipment->verticalScrollBar()->value();
    int notesTop        = mUI->notes->verticalScrollBar()->value();

    preparePrint(mUI->skillstalentsandperks);
    preparePrint(mUI->complications);
    preparePrint(mUI->powersandequipment);
    preparePrint(mUI->notes);

    update();

    int page = 0;
    offset = QPoint({ 50, 1352 }); // NOLINT
    int max = getPageCount();
    while (page < max) {
        printer->newPage();
        painter.drawImage(QPointF { 0.0, 0.0 }, page2.toImage());
        for (int i = 0; i < mUI->widgets.count(); ++i) {
            auto& widget = mUI->widgets[i];
            if (widget == nullptr || widget->y() < 1250) continue; // NOLINT  skip things we can't render or are on the first page
            print(painter, offset, widget);
        }
        deletePagefull();
        ++page;
    }

    if (mOption.showNotesPage()) {
        QString notes = mUI->notes->toPlainText();
        int maxCnt = getPageCount(mUI->notes, scale, &painter);
        offset = QPoint({ 50, 48 }); // NOLINT
        int pageCnt = 0;
        while (pageCnt < maxCnt) {
            printer->newPage();
            painter.drawImage(QPointF { -50.0, -48.0 }, page3.toImage()); // NOLINT
            for (int i = 0; i < mUI->hiddenWidgets.count(); ++i) {
                auto& widget = mUI->hiddenWidgets[i];
                if (widget == nullptr) continue; // skip things we can't render
                print(painter, offset, widget);
            }
            deletePagefull(mUI->notes, scale, &painter);
            ++page;
        }
        mUI->notes->setPlainText(notes);
    }

    painter.end();

    mUI->skillstalentsandperks->verticalScrollBar()->setValue(skillTop);
    mUI->complications->verticalScrollBar()->setValue(complicationTop);
    mUI->powersandequipment->verticalScrollBar()->setValue(powerTop);
    mUI->notes->verticalScrollBar()->setValue(notesTop);

    mUI->skillstalentsandperks->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mUI->complications->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mUI->powersandequipment->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mUI->notes->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    updateDisplay();
}

void Sheet::save() {
    if (!mChanged) return;
    mCharacter.notes() = mUI->notes->toPlainText();

#ifndef __wasm__
    if (mFilename.isEmpty()) {
        QUrl oldname = mFilename;
        mFilename = QUrl::fromLocalFile(mDir + "/" + mUI->charactername->text() + ".hsccu");
        saveAs();
        if (mFilename.isEmpty()) mFilename = oldname;
        return;
    }

    if (!mCharacter.store(mOption, mFilename)) OK("Can't save to \"" + mFilename.toString() + "\" in the \"" + mDir + "\" folder.", std::bind(&Sheet::doNothing, this));
    else mChanged = false;
#else
    if (mFilename.isEmpty()) mFilename = mUI->charactername->text();
    if (!mCharacter.store(mOption, mFilename.toString())) {
        OK("Can't save to \"" + mFilename.toString() + ".", std::bind(&Sheet::doNothing, this));
        throw "";
    }
    else mChanged = false;
#endif
}

void Sheet::saveAs() {
    QUrl oldname = mFilename;
#ifdef __wasm__
    mFilename = QFileDialog::getSaveFileName(this, "Save File", mDir, "Characters (*.hsccu)");
#endif
    if (mFilename.isEmpty()) mFilename = QUrl::fromLocalFile(mDir + "/" + mCharacter.characterName() + ".hsccu");
    mFilename = QFileDialog::getSaveFileUrl(this, "Save File", mFilename, "Characters (*.hsccu)");
    if (mFilename.isEmpty()) {
        mFilename = oldname;
        return;
    }

    try { save(); } catch (...) { mFilename = oldname; }
}

void Sheet::skillstalentsandperksMenu(QPoint pos) {
#if defined(__wasm__) || defined(Q_OS_ANDROID)
    int row = mUI->skillstalentsandperks->rowAt(mUI->skillstalentsandperks->viewport()->mapFromGlobal(pos).y());
    mUI->skillstalentsandperks->selectRow(row);
    closeDialogs(nullptr);
#ifdef Q_OS_ANDROID
    auto skillMenuDialog = (sDialog.SkillMenu = std::shared_ptr<SkillMenuDialog> (new SkillMenuDialog(), [](SkillMenuDialog* d) { d->deleteLater(); }));
#else
    auto skillMenuDialog = (sDialog.SkillMenu = std::shared_ptr<SkillMenuDialog> (new SkillMenuDialog()));
#endif
    skillMenuDialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    skillMenuDialog->setPos(pos);
    aboutToShowSkillsPerksAndTalentsMenu();
    skillMenuDialog->open();
#else
    mUI->skillstalentsandperksMenu->exec(pos);
#endif
}

void Sheet::totalExperienceEarnedChanged(QString txt) {
    if (numeric(txt) || txt.isEmpty()) {
        mCharacter.xp(Points(txt.toInt()));

        Points total = mOption.totalPoints() - mOption.complications() + mCharacter.xp();
        if (mOption.complications() < mComplicationPoints) total += mOption.complications();
        else total += mComplicationPoints;
        Points remaining(0_cp);
        if (total > mTotalPoints) remaining = total - mTotalPoints;
        Points spent(0_cp);
        if (mTotalPoints > mOption.totalPoints()) spent = mTotalPoints - mOption.totalPoints();

        mUI->experiencespent->setText(QString("%1").arg(spent.points));
        mUI->experienceunspent->setText(QString("%1").arg(remaining.points));
        mChanged = true;
    } else mUI->totalexperienceearned->undo();
}

void Sheet::totalExperienceEarnedEditingFinished() {
    totalExperienceEarnedChanged(mUI->totalexperienceearned->text());
    if (mUI->totalexperienceearned->text().isEmpty()) mUI->totalexperienceearned->setText("0");
}

void Sheet::setTableSelectionMode(QTableWidget* table) {
}
