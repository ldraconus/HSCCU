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
#include "skilldialog.h"
#include "skilltalentorperk.h"

#include "sheet.h"
#ifndef __wasm__
#include "ui_sheet.h"
#else
#include "ui_wasm.h"
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
#include <QScrollBar>
#include <QSettings>
#include <QStandardPaths>
#include <QStatusBar>
#include <QToolButton>

Sheet* Sheet::sSheet = nullptr; // NOLINT
shared_ptr<class QMessageBox> Msg::Box; // NOLINT
std::function<void()> Msg::mCancel; // NOLINT
std::function<void()> Msg::mNo; // NOLINT
std::function<void()> Msg::mOk; // NOLINT
std::function<void ()> Msg::mYes; // NOLINT

Msg msngr; // NOLINT

void Msg::button(QAbstractButton* btn) {
    QString txt = btn->text();
    if (txt == "Cancel") Msg::mCancel();
    else if (txt == "&No") Msg::mNo();
    else if (txt == "&Ok") Msg::mOk();
    else if (txt == "&Yes") Msg::mYes();
}

// --- [static functions] ----------------------------------------------------------------------------------
constexpr int Base10 = 10;

static bool numeric(QString txt) {
    bool ok = false;
    txt.toInt(&ok, Base10);
    return ok;
}

void YesNo(const QString& msg, std::function<void()> yes, std::function<void()> no, const QString title = "") {
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

void YesNoCancel(const QString& msg, std::function<void()> yes, std::function<void()> no, std::function<void()> cancel, const QString& title = "") {
    Msg::Box = make_shared<QMessageBox>();
    Msg::Box->connect(Msg::Box.get(), SIGNAL(buttonClicked(QAbstractButton*)), &msngr, SLOT(button(QAbstractButton*)));
    Msg::mYes = yes;
    Msg::mNo = no;
    Msg::mCancel = cancel;
    Msg::Box->setIcon(QMessageBox::Question);
    Msg::Box->setText(title.isEmpty() ? "Are you really sure?" : title);
    Msg::Box->setInformativeText(msg);
    Msg::Box->setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    Msg::Box->setDefaultButton(QMessageBox::Cancel);
    Msg::Box->open();
}

void OK(const QString& msg,
        std::function<void ()> ok,
        const QString& title = "") {
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

void OKCancel(const QString& msg, std::function<void ()> ok, const QString& title = "") {
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

void Question(const QString& msg,
              std::function<void ()> yes,
              std::function<void ()> no,
              const QString& title = "") {
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

Sheet::Sheet(QWidget *parent)
    : QMainWindow(parent)
#ifndef __wasm__
    , ui(new Ui::Sheet)
#else
    , ui(new Ui::wasm)
#endif
    , Ui(&sSheet_UI)
    , printer(nullptr)
    , mSaveChanged(false) {
    sSheet = this;

    ui->setupUi(this);
#ifdef unix
    ui->scrollAreaWidgetContents->setStyleSheet("background: #fff");
    ui->scrollArea->setStyleSheet("color: #000; background: #fff");
#else
    ui->scrollArea->setStyleSheet("color: #000;");
#endif
    Ui->setupUi(ui->label, ui->optLabel);
    setupIcons();
    setUnifiedTitleAndToolBarOnMac(true);
    setAttribute(Qt::WA_AcceptTouchEvents);

    Modifiers mods;

    mDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    mOption.load();
    ui->optLabel->setVisible(mOption.showNotesPage());

    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(focusChanged(QWidget*,QWidget*))); // NOLINT

    updateBanner();

#ifndef __wasm__
    connect(ui->menu_File,         SIGNAL(aboutToShow()), this, SLOT(aboutToShowFileMenu()));
    connect(ui->menu_File,         SIGNAL(aboutToHide()), this, SLOT(aboutToHideFileMenu()));
    connect(ui->action_New,        SIGNAL(triggered()),   this, SLOT(newchar()));
    connect(ui->action_Open,       SIGNAL(triggered()),   this, SLOT(open()));
    connect(ui->action_Save,       SIGNAL(triggered()),   this, SLOT(save()));
    connect(ui->actionSave_As,     SIGNAL(triggered()),   this, SLOT(saveAs()));
    connect(ui->action_Print,      SIGNAL(triggered()),   this, SLOT(print()));
    connect(ui->actionE_xit,       SIGNAL(triggered()),   this, SLOT(exitClicked()));

    connect(ui->menu_Edit,     SIGNAL(aboutToShow()), this, SLOT(aboutToShowEditMenu()));
    connect(ui->menu_Edit,     SIGNAL(aboutToHide()), this, SLOT(aboutToHideEditMenu()));
    connect(ui->action_Cut,    SIGNAL(triggered()),   this, SLOT(cutCharacter()));
    connect(ui->actionC_opy,   SIGNAL(triggered()),   this, SLOT(copyCharacter()));
    connect(ui->action_Paste,  SIGNAL(triggered()),   this, SLOT(pasteCharacter()));
    connect(ui->actionOptions, SIGNAL(triggered()),   this, SLOT(options()));
#else
    action_File = new QAction(this);
    action_File->setObjectName("action_File");
    action_New = new QAction(this);
    action_New->setObjectName("action_New");
    action_Open = new QAction(this);
    action_Open->setObjectName("action_Open");
    action_Save = new QAction(this);
    action_Save->setObjectName("action_Save");
    action_Edit = new QAction(this);
    action_Edit->setObjectName("action_Edit");
    action_Cut = new QAction(this);
    action_Cut->setObjectName("action_Cut");
    actionC_opy = new QAction(this);
    actionC_opy->setObjectName("actionC_opy");
    action_Paste = new QAction(this);
    action_Paste->setObjectName("action_Paste");
    actionOptions = new QAction(this);
    actionOptions->setObjectName("actionOptions");

    fileButton = createToolBarItem(ui->menuBar, "File", "File menu", action_File);
    editButton = createToolBarItem(ui->menuBar, "Edit", "Edit menu", action_Edit);
    connect(action_File,   SIGNAL(triggered()), this, SLOT(fileMenu()));
    connect(action_Edit,   SIGNAL(triggered()), this, SLOT(editMenu()));
    connect(action_New,    SIGNAL(triggered()), this, SLOT(newchar()));
    connect(action_Open,   SIGNAL(triggered()), this, SLOT(open()));
    connect(action_Save,   SIGNAL(triggered()), this, SLOT(save()));
    connect(action_Cut,    SIGNAL(triggered()), this, SLOT(cutCharacter()));
    connect(actionC_opy,   SIGNAL(triggered()), this, SLOT(copyCharacter()));
    connect(action_Paste,  SIGNAL(triggered()), this, SLOT(pasteCharacter()));
    connect(actionOptions, SIGNAL(triggered()), this, SLOT(options()));
#endif

    connect(Ui->alternateids,          SIGNAL(textEdited(QString)), this, SLOT(alternateIdsChanged(QString)));
    connect(Ui->bodyval,               SIGNAL(textEdited(QString)), this, SLOT(valChanged(QString)));
    connect(Ui->bodyval,               SIGNAL(editingFinished()),   this, SLOT(valEditingFinished()));
    connect(Ui->campaignname,          SIGNAL(textEdited(QString)), this, SLOT(campaignNameChanged(QString)));
    connect(Ui->charactername,         SIGNAL(textEdited(QString)), this, SLOT(characterNameChanged(QString)));
    connect(Ui->conval,                SIGNAL(textEdited(QString)), this, SLOT(valChanged(QString)));
    connect(Ui->conval,                SIGNAL(editingFinished()),   this, SLOT(valEditingFinished()));
    connect(Ui->currentbody,           SIGNAL(textEdited(QString)), this, SLOT(currentBODYChanged(QString)));
    connect(Ui->currentbody,           SIGNAL(editingFinished()),   this, SLOT(currentBODYEditingFinished()));
    connect(Ui->currentend,            SIGNAL(textEdited(QString)), this, SLOT(currentENDChanged(QString)));
    connect(Ui->currentend,            SIGNAL(editingFinished()),   this, SLOT(currentENDEditingFinished()));
    connect(Ui->currentstun,           SIGNAL(textEdited(QString)), this, SLOT(currentSTUNChanged(QString)));
    connect(Ui->currentstun,           SIGNAL(editingFinished()),   this, SLOT(currentSTUNEditingFinished()));
    connect(Ui->dcvval,                SIGNAL(textEdited(QString)), this, SLOT(valChanged(QString)));
    connect(Ui->dcvval,                SIGNAL(editingFinished()),   this, SLOT(valEditingFinished()));
    connect(Ui->dexval,                SIGNAL(textEdited(QString)), this, SLOT(valChanged(QString)));
    connect(Ui->dexval,                SIGNAL(editingFinished()),   this, SLOT(valEditingFinished()));
    connect(Ui->dmcvval,               SIGNAL(textEdited(QString)), this, SLOT(valChanged(QString)));
    connect(Ui->dmcvval,               SIGNAL(editingFinished()),   this, SLOT(valEditingFinished()));
    connect(Ui->edval,                 SIGNAL(textEdited(QString)), this, SLOT(valChanged(QString)));
    connect(Ui->edval,                 SIGNAL(editingFinished()),   this, SLOT(valEditingFinished()));
    connect(Ui->egoval,                SIGNAL(textEdited(QString)), this, SLOT(valChanged(QString)));
    connect(Ui->egoval,                SIGNAL(editingFinished()),   this, SLOT(valEditingFinished()));
    connect(Ui->endval,                SIGNAL(textEdited(QString)), this, SLOT(valChanged(QString)));
    connect(Ui->endval,                SIGNAL(editingFinished()),   this, SLOT(valEditingFinished()));
    connect(Ui->eyecolor,              SIGNAL(textEdited(QString)), this, SLOT(eyeColorChanged(QString)));
    connect(Ui->gamemaster,            SIGNAL(textEdited(QString)), this, SLOT(gamemasterChanged(QString)));
    connect(Ui->genre,                 SIGNAL(textEdited(QString)), this, SLOT(genreChanged(QString)));
    connect(Ui->haircolor,             SIGNAL(textEdited(QString)), this, SLOT(hairColorChanged(QString)));
    connect(Ui->intval,                SIGNAL(textEdited(QString)), this, SLOT(valChanged(QString)));
    connect(Ui->intval,                SIGNAL(editingFinished()),   this, SLOT(valEditingFinished()));
    connect(Ui->ocvval,                SIGNAL(textEdited(QString)), this, SLOT(valChanged(QString)));
    connect(Ui->ocvval,                SIGNAL(editingFinished()),   this, SLOT(valEditingFinished()));
    connect(Ui->omcvval,               SIGNAL(textEdited(QString)), this, SLOT(valChanged(QString)));
    connect(Ui->omcvval,               SIGNAL(editingFinished()),   this, SLOT(valEditingFinished()));
    connect(Ui->pdval,                 SIGNAL(textEdited(QString)), this, SLOT(valChanged(QString)));
    connect(Ui->pdval,                 SIGNAL(editingFinished()),   this, SLOT(valEditingFinished()));
    connect(Ui->playername,            SIGNAL(textEdited(QString)), this, SLOT(playerNameChanged(QString)));
    connect(Ui->preval,                SIGNAL(textEdited(QString)), this, SLOT(valChanged(QString)));
    connect(Ui->preval,                SIGNAL(editingFinished()),   this, SLOT(valEditingFinished()));
    connect(Ui->recval,                SIGNAL(textEdited(QString)), this, SLOT(valChanged(QString)));
    connect(Ui->recval,                SIGNAL(editingFinished()),   this, SLOT(valEditingFinished()));
    connect(Ui->spdval,                SIGNAL(textEdited(QString)), this, SLOT(valChanged(QString)));
    connect(Ui->spdval,                SIGNAL(editingFinished()),   this, SLOT(valEditingFinished()));
    connect(Ui->strval,                SIGNAL(textEdited(QString)), this, SLOT(valChanged(QString)));
    connect(Ui->strval,                SIGNAL(editingFinished()),   this, SLOT(valEditingFinished()));
    connect(Ui->stunval,               SIGNAL(textEdited(QString)), this, SLOT(valChanged(QString)));
    connect(Ui->stunval,               SIGNAL(editingFinished()),   this, SLOT(valEditingFinished()));
    connect(Ui->totalexperienceearned, SIGNAL(textEdited(QString)), this, SLOT(totalExperienceEarnedChanged(QString)));
    connect(Ui->totalexperienceearned, SIGNAL(editingFinished()),   this, SLOT(totalExperienceEarnedEditingFinished()));
    connect(Ui->height,                SIGNAL(textEdited(QString)), this, SLOT(heightChanged(QString)));
    connect(Ui->weight,                SIGNAL(textEdited(QString)), this, SLOT(weightChanged(QString)));
    connect(Ui->notes,                 SIGNAL(textChanged()),       this, SLOT(noteChanged()));

    connect(Ui->image,      SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(imageMenu(QPoint)));
#ifndef __wasm__
    connect(Ui->newImage,   SIGNAL(triggered()),                        this, SLOT(newImage()));
    connect(Ui->clearImage, SIGNAL(triggered()),                        this, SLOT(clearImage()));
#else
//    connect(Ui->image,      SIGNAL(showMenu()),                         this, SLOT(outsideImageArea()));
#endif

    connect(Ui->complications,        SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(complicationDoubleClicked(QTableWidgetItem*)));
    connect(Ui->complications,        SIGNAL(customContextMenuRequested(QPoint)),   this, SLOT(complicationsMenu(QPoint)));
#ifdef __wasm__
//    connect(Ui->complications,        SIGNAL(showmenu()),                           this, SLOT(aboutToShowComplicationsMenu()));
#else
    connect(Ui->complicationsMenu,    SIGNAL(aboutToShow()),                        this, SLOT(aboutToShowComplicationsMenu()));
#endif
    connect(Ui->newComplication,      SIGNAL(triggered()),                          this, SLOT(newComplication()));
    connect(Ui->editComplication,     SIGNAL(triggered()),                          this, SLOT(editComplication()));
    connect(Ui->deleteComplication,   SIGNAL(triggered()),                          this, SLOT(deleteComplication()));
    connect(Ui->cutComplication,      SIGNAL(triggered()),                          this, SLOT(cutComplication()));
    connect(Ui->copyComplication,     SIGNAL(triggered()),                          this, SLOT(copyComplication()));
    connect(Ui->pasteComplication,    SIGNAL(triggered()),                          this, SLOT(pasteComplication()));
    connect(Ui->moveComplicationUp,   SIGNAL(triggered()),                          this, SLOT(moveComplicationUp()));
    connect(Ui->moveComplicationDown, SIGNAL(triggered()),                          this, SLOT(moveComplicationDown()));


    connect(Ui->skillstalentsandperks,     SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(skillstalentsandperksDoubleClicked(QTableWidgetItem*)));
    connect(Ui->skillstalentsandperks,     SIGNAL(customContextMenuRequested(QPoint)),   this, SLOT(skillstalentsandperksMenu(QPoint)));
#ifdef __wasm__
//    connect(Ui->skillstalentsandperks,     SIGNAL(showmenu()),                           this, SLOT(aboutToShowSkillsPerksAndTalentsMenu()));
#else
    connect(Ui->skillstalentsandperksMenu, SIGNAL(aboutToShow()),                        this, SLOT(aboutToShowSkillsPerksAndTalentsMenu()));
#endif
    connect(Ui->newSkillTalentOrPerk,      SIGNAL(triggered()),                          this, SLOT(newSkillTalentOrPerk()));
    connect(Ui->editSkillTalentOrPerk,     SIGNAL(triggered()),                          this, SLOT(editSkillstalentsandperks()));
    connect(Ui->deleteSkillTalentOrPerk,   SIGNAL(triggered()),                          this, SLOT(deleteSkillstalentsandperks()));
    connect(Ui->cutSkillTalentOrPerk,      SIGNAL(triggered()),                          this, SLOT(cutSkillTalentOrPerk()));
    connect(Ui->copySkillTalentOrPerk,     SIGNAL(triggered()),                          this, SLOT(copySkillTalentOrPerk()));
    connect(Ui->pasteSkillTalentOrPerk,    SIGNAL(triggered()),                          this, SLOT(pasteSkillTalentOrPerk()));
    connect(Ui->moveSkillTalentOrPerkUp,   SIGNAL(triggered()),                          this, SLOT(moveSkillTalentOrPerkUp()));
    connect(Ui->moveSkillTalentOrPerkDown, SIGNAL(triggered()),                          this, SLOT(moveSkillTalentOrPerkDown()));

    connect(Ui->powersandequipment,       SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(powersandequipmentDoubleClicked(QTableWidgetItem*)));
    connect(Ui->powersandequipment,       SIGNAL(customContextMenuRequested(QPoint)),   this, SLOT(powersandequipmentMenu(QPoint)));
#ifdef __wasm__
//   connect(Ui->powersandequipment,       SIGNAL(showmenu()),                           this, SLOT(aboutToShowPowersAndEquipmentMenu()));
#else
    connect(Ui->powersandequipmentMenu,   SIGNAL(aboutToShow()),                        this, SLOT(aboutToShowPowersAndEquipmentMenu()));
#endif
    connect(Ui->newPowerOrEquipment,      SIGNAL(triggered()),                          this, SLOT(newPowerOrEquipment()));
    connect(Ui->editPowerOrEquipment,     SIGNAL(triggered()),                          this, SLOT(editPowerOrEquipment()));
    connect(Ui->deletePowerOrEquipment,   SIGNAL(triggered()),                          this, SLOT(deletePowerOrEquipment()));
    connect(Ui->cutPowerOrEquipment,      SIGNAL(triggered()),                          this, SLOT(cutPowerOrEquipment()));
    connect(Ui->copyPowerOrEquipment,     SIGNAL(triggered()),                          this, SLOT(copyPowerOrEquipment()));
    connect(Ui->pastePowerOrEquipment,    SIGNAL(triggered()),                          this, SLOT(pastePowerOrEquipment()));
    connect(Ui->movePowerOrEquipmentUp,   SIGNAL(triggered()),                          this, SLOT(movePowerOrEquipmentUp()));
    connect(Ui->movePowerOrEquipmentDown, SIGNAL(triggered()),                          this, SLOT(movePowerOrEquipmentDown()));

    mWidget2Def = {
        { Ui->strval,  { &mCharacter.STR(),  Ui->strval,  Ui->strpoints, Ui->strroll } },
        { Ui->dexval,  { &mCharacter.DEX(),  Ui->dexval,  Ui->dexpoints, Ui->dexroll } },
        { Ui->conval,  { &mCharacter.CON(),  Ui->conval,  Ui->conpoints, Ui->conroll } },
        { Ui->intval,  { &mCharacter.INT(),  Ui->intval,  Ui->intpoints, Ui->introll } },
        { Ui->egoval,  { &mCharacter.EGO(),  Ui->egoval,  Ui->egopoints, Ui->egoroll } },
        { Ui->preval,  { &mCharacter.PRE(),  Ui->preval,  Ui->prepoints, Ui->preroll } },
        { Ui->ocvval,  { &mCharacter.OCV(),  Ui->ocvval,  Ui->ocvpoints } },
        { Ui->dcvval,  { &mCharacter.DCV(),  Ui->dcvval,  Ui->dcvpoints } },
        { Ui->omcvval, { &mCharacter.OMCV(), Ui->omcvval, Ui->omcvpoints } },
        { Ui->dmcvval, { &mCharacter.DMCV(), Ui->dmcvval, Ui->dmcvpoints } },
        { Ui->spdval,  { &mCharacter.SPD(),  Ui->spdval,  Ui->spdpoints } },
        { Ui->pdval,   { &mCharacter.PD(),   Ui->pdval,   Ui->pdpoints } },
        { Ui->edval,   { &mCharacter.ED(),   Ui->edval,   Ui->edpoints } },
        { Ui->recval,  { &mCharacter.REC(),  Ui->recval,  Ui->recpoints } },
        { Ui->endval,  { &mCharacter.END(),  Ui->endval,  Ui->endpoints } },
        { Ui->bodyval, { &mCharacter.BODY(), Ui->bodyval, Ui->bodypoints } },
        { Ui->stunval, { &mCharacter.STUN(), Ui->stunval, Ui->stunpoints } }
    };

    installEventFilter(dynamic_cast<QObject*>(this));

#ifndef __wasm__
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
}

Sheet::~Sheet() {
    delete printer;
    delete ui;
    // Ui's contents are pointed to by ui->label, don't delete it (double deletes)!  Don't worry, it is not allocated either, static global storage with a pointer to it.
}

// --- [EVENT FILTER] ----------------------------------------------------------------------------------

bool Sheet::eventFilter(QObject* object, QEvent* event) {
    if (event->type() == QEvent::FocusIn && mWidget2Def.find(object) != mWidget2Def.end()) {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(object);
        edit->setText(QString("%1").arg(mWidget2Def[edit].characteristic()->base()));
    }
    return false;
}

static void closeDialog(shared_ptr<QDialog> dlg, QMouseEvent* me) {
    if (dlg == nullptr) return;
    QRect dialogRect = dlg->geometry();
    if (dlg->isVisible() && (me == nullptr || !dialogRect.contains(me->pos()))) dlg->done(QDialog::Rejected);
}

void Sheet::closeDialogs(QMouseEvent* me) {
#ifdef __wasm__
    if (mComplicationsMenuDialog != nullptr) closeDialog(mComplicationsMenuDialog, me);
    if (mEditMenuDialog          != nullptr) closeDialog(mEditMenuDialog,          me);
    if (mFileMenuDialog          != nullptr) closeDialog(mFileMenuDialog,          me);
    if (mImgMenuDialog           != nullptr) closeDialog(mImgMenuDialog,           me);
    if (mSkillMenuDialog         != nullptr) closeDialog(mSkillMenuDialog,         me);
    if (mPowerMenuDialog         != nullptr) closeDialog(mPowerMenuDialog,         me);
#endif
    if (mOptionDlg != nullptr) closeDialog(mOptionDlg, me);
    if (mCompDlg   != nullptr) closeDialog(mCompDlg,   me);
    if (mPowerDlg  != nullptr) closeDialog(mPowerDlg,  me);
    if (mSkillDlg  != nullptr) closeDialog(mSkillDlg,  me);
}

void Sheet::mousePressEvent(QMouseEvent* me) {
    closeDialogs(me);
}

void Sheet::closeEvent(QCloseEvent* event) {
    if (checkClose()) event->accept();
    else event->ignore();
}

bool Sheet::event(QEvent* e) {
    auto type = e->type();
    if (type == QEvent::TouchBegin ||
        type == QEvent::TouchUpdate ||
        type == QEvent::TouchEnd) {
        auto *te = static_cast<QTouchEvent *>(e);
        const auto &pts = te->points();

        // just single finger events at this time
        if (pts.size() == 1) {
            auto pos = pts[0];

            // only handle menu press at this time
            if (type == QEvent::TouchBegin) {
                mTouchStart = pos.globalPosition();
                QPoint pnt = pos.position().toPoint();
                bool powers = Ui->powersandequipment->geometry().contains(pnt);
                bool skills = Ui->skillstalentsandperks->geometry().contains(pnt);
                bool complications = Ui->complications->geometry().contains(pnt);
                if (powers || skills || complications) {
                    mExpired = false;
                    mRunning = true;
                    mLongPressTimer.singleShot(500, [this, pnt, powers, skills, complications]() {
                        mExpired = true;
                        mRunning = false;
                        if (powers) powersandequipmentMenu(pnt);
                        else if (skills) skillstalentsandperksMenu(pnt);
                        else if (complications) complicationsMenu(pnt);
                    });
                }
            } else if (type == QEvent::TouchUpdate) {
                if ((mTouchStart - pos.globalGrabPosition()).manhattanLength() > 15) {
                    mLongPressTimer.stop();
                    mExpired = true;
                    mRunning = false;
                }
            } else if (type == QEvent::TouchEnd) {
                mLongPressTimer.stop();
                mExpired = true;
                mRunning = false;
            }
        }
    }

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
    auto selection = Ui->powersandequipment->selectedItems();
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

void Sheet::fixButtonBox(QDialogButtonBox *bb)
{
    /*
    bb->setStyleSheet("QPushButton { border-color: #888; color: #888; background: #fff; } "
                      "QPushButton:default { border-color: #000; color: #000; } "
                      "QPushButton[enabled=\"true\"] { border-color: #000; color: #000; }"
                      "QAbstractButton { border-color: #888; color: #888; background: #fff; } "
                      "QAbstractButton:default { border-color: #000; color: #000; } "
                      "QAbstractButton[enabled=\"true\"] { border-color: #000; color: #000; }"
                      "QDialogButtonBox { border-color: #888; color: #888; background: #fff; } "
                      "QDialogButtonBox:default { border-color: #000; color: #000; } ");
    */
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
    Ui->totalcost->setText(QString("%1").arg(total.points));
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
        if (val == Ui->spdval) {
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

        if (val == Ui->strval) {
            setDamage(def, Ui->hthdamage);
            QString end = QString("%1").arg((primary + 4) / mOption.activePerEND().points);
            if (primary != secondary) end += QString("/%1").arg((secondary + 4) / mOption.activePerEND().points);
            Ui->strendcost->setText(end);
            rebuildMartialArts();
            QString lift = formatLift(primary);
            if (primary != secondary) lift += "/" + formatLift(secondary);
            Ui->lift->setText(lift);
        } else if (val == Ui->spdval) {
            if (def.characteristic()->base() < 1) return;
            QList<int> chart = phases[secondary];
            for (auto& x: std::as_const(Ui->phases)) x->setText("");
            for (const auto& x: chart) Ui->phases[x - 1]->setText("X");
        } else if (val == Ui->ocvval) setCVs(def, Ui->baseocv);
          else if (val == Ui->omcvval) setCVs(def, Ui->baseomcv);
          else if (val == Ui->dcvval) setCVs(def, Ui->basedcv);
          else if (val == Ui->dmcvval) setCVs(def, Ui->basedmcv);
          else if (val == Ui->intval) Ui->perceptionroll->setText(def.roll()->text());
          else if (val == Ui->preval) setDamage(def, Ui->presenceattack);
          else if (val == Ui->endval) setMaximum(def, Ui->maximumend, Ui->currentend);
          else if (val == Ui->bodyval) setMaximum(def, Ui->maximumbody, Ui->currentbody);
          else if (val == Ui->stunval) setMaximum(def, Ui->maximumstun, Ui->currentstun);
          else if (val == Ui->pdval || val == Ui->edval) rebuildDefenses();
        if (update) updateTotals();
    } else val->undo();
}

void Sheet::characteristicEditingFinished(QLineEdit* val) {
    QString txt = val->text();
    characteristicChanged(val, txt);

    if (txt.isEmpty()) {
        if (sender() == Ui->spdval) {
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
QWidget* Sheet::createToolBarItem(QToolBar* sb, const QString name, const QString tip, QAction* action) {
    QToolButton *tb = new QToolButton();
    tb->setText(name);
    tb->setObjectName(name);
    tb->setToolTip(tip);
    tb->setDefaultAction(action);
    tb->setToolButtonStyle(Qt::ToolButtonTextOnly);
    action->setText(name);
    action->setToolTip(tip);
    sb->addWidget(tb);
    return tb;
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
    deletePagefull(Ui->skillstalentsandperks);
    deletePagefull(Ui->complications);
    deletePagefull(Ui->powersandequipment);
}

int Sheet::displayPowerAndEquipment(int& row, shared_ptr<Power> pe) {
    if (pe == nullptr) return 0;

    QFont font = Ui->powersandequipment->font();
    QFont italic = font;
    italic.setItalic(true);
    QString descr = pe->description(false);

    if (pe->isEquipment() && pe->name() == "Armor") hitLocations(pe);

    if (descr == "-") descr = "";
    for (const auto& mod: std::as_const(pe->advantagesList())) {
        if (mod == nullptr) continue;

        if (mod->isAdder()) descr += "; (+" + QString("%1").arg(mod->points(Power::NoStore).points) + " pts) ";
        else descr += "; (+" + mod->fraction(Power::NoStore).toString() + ") ";
        descr += mod->description(false);
    }
    for (const auto& mod: std::as_const(pe->limitationsList())) {
        if (mod == nullptr) continue;

        descr += "; (-" + mod->fraction(Power::NoStore).abs().toString() + ") " + mod->description(false);
    }
    Fraction pts(pe->real().points);
    if (((!pe->isFramework() && !pe->isEquipment()) || pe->isVPP() || pe->isMultipower()) && !descr.isEmpty() && pts.toInt() == 0) pts = Fraction(1);
    if (pe->isVPP()) pts += pe->pool().points;
    if (pts.toInt() != 0) setCell(Ui->powersandequipment, row, 0, QString("%1").arg(pts.toInt()), font);
    else setCell(Ui->powersandequipment, row, 0, "", font);
    setCell(Ui->powersandequipment, row, 1, pe->nickname(), italic);
    setCell(Ui->powersandequipment, row, 2, descr, font, WordWrap);
    QString end = pe->end();
    if (end == "-") end = "";
    setCell(Ui->powersandequipment, row, 3, end, font);
    pe->row(row);
    ++row;
    if (pe->isFramework()) mPowersOrEquipmentPoints += pe->display(row, Ui->powersandequipment);
    return pts.toInt();
}

#ifndef __wasm__
void Sheet::doLoadImage() {
    loadImage(mCharacter.image());
    mSaveChanged = mChanged;
    skipLoadImage();
}

void Sheet::skipLoadImage() {
    Ui->notes->setPlainText(mCharacter.notes());
    updateDisplay();
    mChanged = mSaveChanged;
}
#endif

void Sheet::updateBanner() {
    QPixmap pixmap(mOption.banner());
    pixmap = pixmap.scaled(293, 109, Qt::KeepAspectRatio, Qt::SmoothTransformation); // NOLINT
    Ui->banner1->setPixmap(pixmap);
    Ui->banner2->setPixmap(pixmap);
    Ui->banner3->setPixmap(pixmap);
}

#ifdef __wasm__
void Sheet::fileOpen(const QByteArray& data, QString filename) {
    int ext = filename.lastIndexOf(".hsccu");
    if (ext != -1) mFilename = filename.left(ext);

    int sep = mFilename.lastIndexOf("/");
    if (sep != -1) {
        mDir = mFilename.left(sep);
        mFilename = mFilename.mid(sep + 1);
    }
    mCharacter.load(mOption, data);
    Ui->notes->setPlainText(mCharacter.notes());
    updateDisplay();
    mChanged = false;
}
#else
void Sheet::fileOpen() {
    Power::Equipment(); // pre-load equipment if needed

    auto ext = mFilename.lastIndexOf(".hsccu");
    if (ext != -1) mFilename = mFilename.left(ext);

    auto sep = mFilename.lastIndexOf("/");
    if (sep != -1) {
        mDir = mFilename.left(sep);
        mFilename = mFilename.mid(sep + 1);
    }

    if (!mCharacter.load(mOption, mDir + "/" + mFilename))
        OK("Can't load \"" + mFilename + ".hsccu\" from the \"" + mDir + "\" folder.", std::bind(&Sheet::doNothing, this));
    else {
        mSaveChanged = false;
        QFileInfo imageFile(mCharacter.image());
        if (imageFile.exists()) {
            qulonglong then(mCharacter.imageDate());
            qulonglong file(imageFile.lastModified().toSecsSinceEpoch());
            if (file > then) YesNo("Character image on disk has changed.\n\n"
                                   "Do you want to update the image in\n"
                                   "the character sheet?",
                                   std::bind(&Sheet::doLoadImage, this),
                                   std::bind(&Sheet::skipLoadImage, this));
            else skipLoadImage();
        } else skipLoadImage();
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
        if (names[i] == "STUN") out += Ui->totalcost->text();
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
    for (const auto& skill: std::as_const(mCharacter.skillsTalentsOrPerks())) {
        if (skill == nullptr) continue;

        out += QString("%1\t%2\n").arg(skill->points(SkillTalentOrPerk::NoStore).points).arg(skill->description());
    }
    out += QString("%1\tTotal Skills, Talents, and Perks\n\n").arg(Ui->totalskillstalentsandperkscost->text());

    out += "Powers and Equipment\n";
    for (const auto& power: std::as_const(mCharacter.powersOrEquipment())) {
        if (power == nullptr) continue;

        QString end = power->end();
        if (end == "-") end = "";
        out += QString("%1\t%2%3\n").arg(power->points(Power::NoStore).points).arg(power->description(), end.isEmpty() ? "" : "\t[" + end + "]");
        if (power->isFramework()) power->display(out);
    }
    out += QString("%1\tTotal Powers and Equipment\n\n").arg(Ui->totalpowersandequipmentcost->text());

    out += "Complications\n";
    for (const auto& complication: std::as_const(mCharacter.complications())) {
        if (complication == nullptr) continue;

        out += QString("%1\t%2\n").arg(complication->points(Complication::NoStore).points).arg(complication->description());
    }
    out += QString("%1\tTotal Complications Points\n\n").arg(Ui->totalcomplicationpts->text());

    out += QString("%1\tTotal Points\n").arg(Ui->totalpoints->text());
    out += QString("%1\tTotal Experience Earned\n").arg(Ui->totalexperienceearned->text());
    out += QString("%1\tExperience Spent\n").arg(Ui->experiencespent->text());
    out += QString("%1\tExperience Unspent\n\n").arg(Ui->experienceunspent->text());

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
    int pages = getPageCount(Ui->skillstalentsandperks);
    int next = getPageCount(Ui->complications);
    if (next > pages) pages = next;
    next = getPageCount(Ui->powersandequipment);
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

void Sheet::loadImage(QPixmap& pixmap, QString filename) {
    clearImage();
    QPixmap scaled = pixmap.scaledToWidth(Ui->image->width());
    if (scaled.height() > Ui->image->height()) scaled = pixmap.scaledToHeight(Ui->image->height());
    Ui->image->setPixmap(scaled);
    mCharacter.image() = filename;
    QByteArray sync;
    QBuffer buffer(&sync);
    buffer.open(QIODevice::WriteOnly);
    scaled.save(&buffer, "PNG");
    buffer.close();
    mCharacter.imageData() = sync;
    QFileInfo imageFile(filename);
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

void Sheet::loadImage(QString filename) {
    QPixmap pixmap;
    pixmap.load(filename);
    loadImage(pixmap, filename);
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

    QLabel* label = dynamic_cast<QLabel*>(widget);
    if (label) {
        QString style;
        if (label->font() == Ui->smallBoldWideFont || label->font() == Ui->headerFont)
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
        Ui->strval,  Ui->dexval, Ui->conval, Ui->intval,  Ui->egoval,
        Ui->preval,  Ui->ocvval, Ui->dcvval, Ui->omcvval, Ui->dmcvval,
        Ui->spdval,  Ui->pdval,  Ui->edval,  Ui->recval,  Ui->endval,
        Ui->bodyval, Ui->stunval
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

    stp->points(SkillTalentOrPerk::NoStore);
    if (stp->name() == "Combat Skill Levels" ||
        (stp->name() == "Skill Levels" && stp->description().contains("Overall"))) return stp->description().mid(stp->name().length() + 2);
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

    Ui->combatskilllevels->setHtml(csl);
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

    QFont font = Ui->narrowTableFont;
    auto* def = Ui->defenses;
    def->setRowCount(0);
    def->update();

    setCellLabel(def, 0, 0, "Normal PD ",      font);
    setCellLabel(def, 0, 1, strPrimPD,         font);
    setCellLabel(def, 1, 0, "Resistant PD ",   font);
    setCellLabel(def, 1, 1, strRPD,            font);
    setCellLabel(def, 2, 0, "Normal ED ",      font);
    setCellLabel(def, 2, 1, strPrimED,         font);
    setCellLabel(def, 3, 0, "Resistant ED ",   font);
    setCellLabel(def, 3, 1, strRED,            font);
    setCellLabel(def, 4, 0, "Mental Defense ", font);
    setCellLabel(def, 4, 1, strMD,             font);
    setCellLabel(def, 5, 0, "Power Defense ",  font);
    setCellLabel(def, 5, 1, strPowD,           font);
    setCellLabel(def, 6, 0, "Flash Defense ",  font);
    setCellLabel(def, 6, 1, strFD,             font);
//    delete Ui->defenses;
//    Ui->defenses->deleteLater();
//    Ui->defenses = nullptr;
//    Ui->defenses = Ui->createTableWidget(ui->label, narrowTableFont,
//                                         {   "Type",           "Amount/Effect" },
//                                         { { "Normal PD ",      strPrimPD },
//                                           { "Resistant PD ",   strRPD    },
//                                           { "Normal ED ",      strPrimED },
//                                           { "Resistant ED ",   strRED    },
//                                           { "Mental Defense ", strMD     },
//                                           { "Power Defense ",  strPowD   },
//                                           { "Flash Defense ",  strFD     }}, { 392, 739 }, { 249, 270 }); // NOLINT
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

    auto* man = Ui->attacksandmaneuvers;
    QString d = stp->description().mid(stp->name().length() + 2);
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
            const auto* cell = man->cellWidget(row, 0);
            if (dynamic_cast<const QLabel*>(cell)->text() == m) break;
        }
        if (row != size) continue;
        if (table.contains(m)) {
            setCellLabel(man, row, 0, m, font);
            for (int i = 0; i < 4; i++) {
                QString x = table[m][i];
                if (i == 3) x = QString(x)
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
                setCellLabel(man, row, i + 1, t[0], font);
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
#if defined(__wasm__) || defined(unix)
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
    auto* man = Ui->attacksandmaneuvers;
    man->setRowCount(0);
    man->update();

    int row = 0;
    int STR = mCharacter.STR().base() + mCharacter.STR().primary();
    int OCV = Ui->ocvval->text().toInt();
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
            setCellLabel(man, row, i, t[0], font);
        }
        ++row;
    }
}

void Sheet::rebuildMartialArts() {
    auto* man = Ui->attacksandmaneuvers;
    QFont font = Ui->smallfont;
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
    for (int i = 1; i < man->columnCount(); ++i) man->resizeColumnToContents(i - 1);
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

    Ui->movement->setRowCount(4);
    Ui->movement->update();

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
    setCellLabel(Ui->movement, 0, 1, running);
    setCellLabel(Ui->movement, 0, 2, ncRunning);
    setCellLabel(Ui->movement, 1, 1, swimming);
    setCellLabel(Ui->movement, 1, 2, ncSwimming);
    setCellLabel(Ui->movement, 2, 1, hLeaping);
    setCellLabel(Ui->movement, 2, 2, ncHLeaping);
    setCellLabel(Ui->movement, 3, 1, vLeaping);
    setCellLabel(Ui->movement, 3, 2, ncVLeaping);
    const auto keys = movements.keys();
    int row = 4;
    QFont font = Ui->movement->cellWidget(0, 1)->font();
    for (const auto& name: std::as_const(keys)) {
        setCellLabel(Ui->movement, row, 0, name,                                                                  font);
        setCellLabel(Ui->movement, row, 1, QString("%1%2").arg(movements[name]).arg(units[name]),                 font);
        setCellLabel(Ui->movement, row, 2, QString("%1%2").arg(doubles[name] * movements[name]).arg(units[name]), font);
        row++;
    }
}

void Sheet::rebuildPowers(bool addTakesNoSTUN) {
    Ui->height->setText(mCharacter.height());
    Ui->weight->setText(mCharacter.weight());

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
        Ui->pdval->setToolTip("Physical Defense: 3 points");
        Ui->edval->setToolTip("Energy Defense: 3 points");
        Ui->dcvval->setToolTip("Defensive Combat Value: 15 points");
        Ui->dmcvval->setToolTip("Defensive Mental Combat Value: 9 points");
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
        Ui->pdval->setToolTip("Physical Defense: 1 point");
        Ui->edval->setToolTip("Energy Defense: 1 point");
        Ui->dcvval->setToolTip("Defensive Combat Value: 5 points");
        Ui->dmcvval->setToolTip("Defensive Mental Combat Value: 3 points");
    }
}

void Sheet::rebuildSenseFromPowers(QList<shared_ptr<Power>>& list, QString& senses) {
    bool first = true;
    for (const auto& power: list) {
        if (power == nullptr) continue;

        if (power->name() == "Enhanced Senses") {
            const auto& split = power->description(false).split(":");
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
    Ui->enhancedandunusualsenses->setText(senses);
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
#ifdef unix
    lbl->setStyleSheet("color: #000;");
#endif
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
    if (val == Ui->pdval) secondary += mCharacter.tempPD();
    if (val == Ui->edval) secondary += mCharacter.tempED();
    QString defense = QString("%1").arg(primary);
    if (primary != secondary) defense += QString("/%1").arg(secondary);
    setCell(Ui->defenses, r, c, defense, Ui->font);
}

QString Sheet::setDefense(int def, int temp) {
    QString result = QString::number(def);
    if (temp != 0) result += "/" + QString::number(temp);
    return result;
}

void Sheet::setDefense(int def, int temp, int r, int c) {
    QString defense = QString("%1").arg(def);
    if (temp != 0) defense += QString("/%1").arg(temp + def);
    setCell(Ui->defenses, r, c, defense, Ui->font);
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
        QString value = def.characteristic()->value();
        characteristic->setText(value);
        mCharactersticPoints += def.characteristic()->points();
    }
}

void Sheet::updateCharacter() {
    Ui->charactername->setText(mCharacter.characterName());
    Ui->charactername2->setText(mCharacter.characterName());
    Ui->alternateids->setText(mCharacter.alternateIds());
    Ui->playername->setText(mCharacter.playerName());
    Ui->haircolor->setText(mCharacter.hairColor());
    Ui->eyecolor->setText(mCharacter.eyeColor());
    Ui->campaignname->setText(mCharacter.campaignName());
    Ui->genre->setText(mCharacter.genre());
    Ui->gamemaster->setText(mCharacter.gamemaster());
    Ui->totalexperienceearned->setText(QString("%1").arg(mCharacter.xp().points));
    QPixmap pic;
    pic.loadFromData(mCharacter.imageData());
    Ui->image->setPixmap(pic);
}

void Sheet::updateComplications() {
    Ui->complications->setRowCount(0);
    Ui->complications->update();

    mComplicationPoints = 0_cp;
    QFont font = Ui->complications->font();
    int row = 0;
    for (const auto& complication: std::as_const(mCharacter.complications())) {
        if (complication == nullptr) continue;

        QString descr = complication->description();
        if (descr == "-") descr = "";
        Points pts = complication->points(Complication::NoStore);
        setCell(Ui->complications, row, 0, descr.isEmpty() ? "" : QString("%1").arg(pts.points), font);
        setCell(Ui->complications, row, 1, descr.isEmpty() ? "" : descr, font, WordWrap);
        mComplicationPoints += pts;
        ++row;
    }
    Ui->complications->resizeRowsToContents();

    Ui->totalcomplicationpts->setText(QString("%1/%2").arg(mComplicationPoints.points).arg(mOption.complications().points));
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
        {     Ui->head, {3, 4, 5}}, // NOLINT
        {    Ui->hands,       {6}}, // NOLINT
        {     Ui->arms,    {7, 8}}, // NOLINT
        {Ui->shoulders,       {9}}, // NOLINT
        {    Ui->chest,  {10, 11}}, // NOLINT
        {  Ui->stomach,      {12}}, // NOLINT
        {   Ui->vitals,      {13}}, // NOLINT
        {   Ui->thighs,      {14}}, // NOLINT
        {     Ui->legs,  {15, 16}}, // NOLINT
        {     Ui->feet,  {17, 18}}  // NOLINT
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
    Ui->averageDEF->setText(QString("%1").arg(def));
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
    Ui->powersandequipment->setRowCount(0);
    Ui->powersandequipment->update();

    mPowersOrEquipmentPoints = 0_cp;
    int row = 0;
    clearHitLocations();
    for (const auto& pe: std::as_const(mCharacter.powersOrEquipment())) {
        if (pe == nullptr) continue;

        mPowersOrEquipmentPoints += Points(displayPowerAndEquipment(row, pe));
    }
    Ui->powersandequipment->resizeRowsToContents();
    updateHitLocations();

    Ui->totalpowersandequipmentcost->setText(QString("%1").arg(mPowersOrEquipmentPoints.points));
}

void Sheet::updateSkillRolls() {
    QFont font = Ui->skillstalentsandperks->font();
    int row = 0;
    for (const auto& stp: std::as_const(mCharacter.skillsTalentsOrPerks())) {
        if (stp == nullptr) continue;

        stp->points(Complication::NoStore);
        setCell(Ui->skillstalentsandperks, row, 2, stp->roll(), font);
        ++row;
    }
    Ui->skillstalentsandperks->resizeRowsToContents();
}

void Sheet::updateSkillsTalentsAndPerks(){
    Ui->skillstalentsandperks->setRowCount(0);
    Ui->skillstalentsandperks->update();
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

    mSkillsTalentsOrPerksPoints = 0_cp;
    QFont font = Ui->skillstalentsandperks->font();
    int row = 0;
    for (const auto& stp: std::as_const(mCharacter.skillsTalentsOrPerks())) {
        if (stp == nullptr) continue;
        QString descr = stp->description();
        if (descr == "-") descr = "";
        Points pts = stp->points(Complication::NoStore);
        setCell(Ui->skillstalentsandperks, row, 0, descr.isEmpty() ? "" : QString("%1").arg(pts.points), font);
        setCell(Ui->skillstalentsandperks, row, 1, descr.isEmpty() ? "" : descr,                         font, WordWrap);
        setCell(Ui->skillstalentsandperks, row, 2, descr.isEmpty() ? "" : stp->roll(),                   font);
        mSkillsTalentsOrPerksPoints += pts;
        ++row;
    }
    Ui->skillstalentsandperks->resizeRowsToContents();

    Ui->totalskillstalentsandperkscost->setText(QString("%1").arg(mSkillsTalentsOrPerksPoints.points));
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
    Ui->totalpoints->setText(QString("%1/%2").arg(mTotalPoints.points).arg(pointsEarned.points));
    totalExperienceEarnedEditingFinished();
}

QString Sheet::valueToDice(int value, bool showD6) {
    QString halfDice = "½";
    int dice = value / 5; // NOLINT
    bool half = value % 5 > 2; // NOLINT
    return QString("%1%2%3").arg(dice).arg(half ? halfDice : "", showD6 ? "d6" : "");
}

// ---[SLOTS] --------------------------------------------------------------------------------------------

#ifndef __wasm__
void Sheet::aboutToHideEditMenu() {
    ui->action_Paste->setEnabled(true);
}

void Sheet::aboutToHideFileMenu() {
    ui->action_Save->setEnabled(true);
}
#endif

void Sheet::aboutToShowComplicationsMenu() {
    const auto selection = Ui->complications->selectedItems();
    bool show = !selection.isEmpty();
    int row = -1;
    if (show) row = selection[0]->row();
#ifndef __wasm__
    QClipboard* clipboard = QGuiApplication::clipboard();
    const QMimeData* clip = clipboard->mimeData();
    bool canPaste = clip->hasFormat("application/complication");

    Ui->editComplication->setEnabled(show);
    Ui->deleteComplication->setEnabled(show);
    Ui->cutComplication->setEnabled(show);
    Ui->copyComplication->setEnabled(show);
    Ui->moveComplicationUp->setEnabled(show && row != 0);
    Ui->moveComplicationDown->setEnabled(show && row != mCharacter.complications().count() - 1);
    Ui->pasteComplication->setEnabled(canPaste);
#else
    bool canPaste = false;
    // see if anything is under the mouse pointer: select it if there is
    mComplicationsMenuDialog->setEdit(show);
    mComplicationsMenuDialog->setDelete(show);
    mComplicationsMenuDialog->setCut(show);
    mComplicationsMenuDialog->setCopy(show);
    mComplicationsMenuDialog->setPaste(canPaste);
    mComplicationsMenuDialog->setMoveUp(show && row != 0);
    mComplicationsMenuDialog->setMoveDown(show && mCharacter.complications().count() != 0 && row != mCharacter.complications().count() - 1);
#endif
}

#ifndef __wasm__
void Sheet::aboutToShowEditMenu() {
    QClipboard* clipboard = QGuiApplication::clipboard();
    const QMimeData* clip = clipboard->mimeData();
    bool canPaste = clip->hasFormat("application/hsccuchar");
    ui->action_Paste->setEnabled(canPaste);
}

void Sheet::aboutToShowFileMenu() {
    ui->action_Save->setEnabled(mChanged);
}
#endif

void Sheet::aboutToShowPowersAndEquipmentMenu() {
    const auto selection = Ui->powersandequipment->selectedItems();
    bool show = !selection.isEmpty();
    int row = -1;
    if (show) row = selection[0]->row();
#ifdef __wasm__
    bool canPaste = false;
    mPowerMenuDialog->setEdit(show);
    mPowerMenuDialog->setDelete(show);
    mPowerMenuDialog->setCut(show);
    mPowerMenuDialog->setCopy(show);
    mPowerMenuDialog->setPaste(canPaste);
    mPowerMenuDialog->setMoveUp(show && row != 0);
    mPowerMenuDialog->setMoveDown(show && row != mCharacter.complications().count() - 1);
#else
    Ui->editPowerOrEquipment->setEnabled(show);
    Ui->deletePowerOrEquipment->setEnabled(show);
    Ui->cutPowerOrEquipment->setEnabled(show);
    Ui->copyPowerOrEquipment->setEnabled(show);
    Ui->movePowerOrEquipmentUp->setEnabled(show && row != 0);
    auto power = getPower(row, mCharacter.powersOrEquipment());
    Ui->movePowerOrEquipmentDown->setEnabled(show && (row != Ui->powersandequipment->rowCount() - 1 || power->parent() != nullptr));
    QClipboard* clipboard = QGuiApplication::clipboard();
    const QMimeData* clip = clipboard->mimeData();
    bool canPaste = clip->hasFormat("application/powerorequipment");
    Ui->pastePowerOrEquipment->setEnabled(canPaste);
#endif
}

void Sheet::aboutToShowSkillsPerksAndTalentsMenu() {
    const auto selection = Ui->skillstalentsandperks->selectedItems();
    bool show = !selection.isEmpty();
    int row = -1;
    if (show) row = selection[0]->row();
#ifdef __wasm__
    bool canPaste = false;
    mSkillMenuDialog->setEdit(show);
    mSkillMenuDialog->setDelete(show);
    mSkillMenuDialog->setCut(show);
    mSkillMenuDialog->setCopy(show);
    mSkillMenuDialog->setPaste(canPaste);
    mSkillMenuDialog->setMoveUp(show && row != 0);
    mSkillMenuDialog->setMoveDown(show && row != mCharacter.skillsTalentsOrPerks().count() - 1);
#else
    QClipboard* clipboard = QGuiApplication::clipboard();
    const QMimeData* clip = clipboard->mimeData();
    bool canPaste = clip->hasFormat("application/skillperkortalent");

    Ui->editSkillTalentOrPerk->setEnabled(show);
    Ui->deleteSkillTalentOrPerk->setEnabled(show);
    Ui->cutSkillTalentOrPerk->setEnabled(show);
    Ui->copySkillTalentOrPerk->setEnabled(show);
    Ui->pasteSkillTalentOrPerk->setEnabled(canPaste);
    Ui->moveSkillTalentOrPerkUp->setEnabled(show && row != 0);
    Ui->moveSkillTalentOrPerkDown->setEnabled(show && row != mCharacter.skillsTalentsOrPerks().count() - 1);
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
    Ui->charactername2->setText(txt);
    mCharacter.characterName(txt);
    mChanged = true;
}

void Sheet::clearImage() {
    Ui->image->clear();
    mCharacter.image() = "";
    mCharacter.imageData().clear();
    mChanged = true;
}

void Sheet::copyCharacter() {
    mCharacter.notes() = Ui->notes->toPlainText();

    QJsonDocument doc = mCharacter.copy(mOption);
    QClipboard* clip = QGuiApplication::clipboard();
    QMimeData* data = new QMimeData();
    data->setData("application/complication", doc.toJson());
    QString text = getCharacter();
    data->setData("text/plain", text.toUtf8());
    clip->setMimeData(data);
}

void Sheet::copyComplication() {
    QClipboard* clip = QGuiApplication::clipboard();
    QMimeData* data = new QMimeData();
    auto selection = Ui->complications->selectedItems();
    int row = selection[0]->row();
    shared_ptr<Complication> complication = mCharacter.complications()[row];
    QJsonObject obj = complication->toJson();
    QJsonDocument doc;
    doc.setObject(obj);
    data->setData("application/complication", doc.toJson());
    QString text = QString("%1\t%2").arg(complication->points(Complication::NoStore).points).arg(complication->description());
    data->setData("text/plain", text.toUtf8());
    clip->setMimeData(data);
}

void Sheet::copyPowerOrEquipment() {
    QClipboard* clip = QGuiApplication::clipboard();
    QMimeData* data = new QMimeData();
    auto selection = Ui->powersandequipment->selectedItems();
    int row = selection[0]->row();
    shared_ptr<Power> power = getPower(row, mCharacter.powersOrEquipment());
    QJsonObject obj = power->toJson();
    QJsonDocument doc;
    doc.setObject(obj);
    data->setData("application/powerorequipment", doc.toJson());
    QString text = QString("%1\t%2").arg(power->points(Power::NoStore).points).arg(power->description());
    data->setData("text/plain", text.toUtf8());
    clip->setMimeData(data);
}

void Sheet::copySkillTalentOrPerk() {
    QClipboard* clip = QGuiApplication::clipboard();
    QMimeData* data = new QMimeData();
    auto selection = Ui->skillstalentsandperks->selectedItems();
    int row = selection[0]->row();
    shared_ptr<SkillTalentOrPerk> skilltalentorperk = mCharacter.skillsTalentsOrPerks()[row];
    QJsonObject obj = skilltalentorperk->toJson();
    QJsonDocument doc;
    doc.setObject(obj);
    data->setData("application/skillperkortalent", doc.toJson());
    QString text = QString("%1\t%2\t%3").arg(skilltalentorperk->points(SkillTalentOrPerk::NoStore).points)
            .arg(skilltalentorperk->description(), skilltalentorperk->roll());
    data->setData("text/plain", text.toUtf8());
    clip->setMimeData(data);
}

void Sheet::complicationsMenu(QPoint pos) {
#ifdef __wasm__
    int row = Ui->complications->rowAt(pos.y());
    Ui->complications->selectRow(row);
    closeDialogs(nullptr);
    mComplicationsMenuDialog = make_shared<ComplicationsMenuDialog>();
    mComplicationsMenuDialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    mComplicationsMenuDialog->setPos(mapToGlobal(pos + Ui->complications->pos() - QPoint(0, ui->scrollArea->verticalScrollBar()->value())));
    aboutToShowComplicationsMenu();
    mComplicationsMenuDialog->open();
#else
    Ui->complicationsMenu->exec(mapToGlobal(pos + Ui->complications->pos() - QPoint(0, ui->scrollArea->verticalScrollBar()->value())));
#endif
}

void Sheet::currentBODYChanged(QString txt) {
    if (!numeric(txt) && !txt.isEmpty()) Ui->currentbody->undo();
}

void Sheet::currentBODYEditingFinished() {
    currentBODYChanged(Ui->currentbody->text());
    if (Ui->currentbody->text().isEmpty()) Ui->currentbody->setText("0");
}

void Sheet::currentENDChanged(QString txt) {
    if (!numeric(txt) && !txt.isEmpty()) Ui->currentend->undo();
}

void Sheet::currentENDEditingFinished() {
    currentENDChanged(Ui->currentend->text());
    if (Ui->currentend->text().isEmpty()) Ui->currentend->setText("0");
}

void Sheet::currentSTUNChanged(QString txt) {
    if (!numeric(txt) && !txt.isEmpty()) Ui->currentstun->undo();
}

void Sheet::currentSTUNEditingFinished() {
    currentSTUNChanged(Ui->currentstun->text());
    if (Ui->currentstun->text().isEmpty()) Ui->currentstun->setText("0");
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

    auto selection = Ui->complications->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    shared_ptr<Complication> complication = mCharacter.complications().takeAt(row);
    if (complication == nullptr) return;

    mComplicationPoints -= complication->points(Complication::NoStore);
    Ui->complications->removeRow(row);
    Ui->totalcomplicationpts->setText(QString("%1/%2").arg(mComplicationPoints.points).arg(mOption.complications().points));
    updateDisplay();
    mChanged = true;
}

void Sheet::deletePowerOrEquipment() {
    auto selection = Ui->powersandequipment->selectedItems();
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
    auto selection = Ui->skillstalentsandperks->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    shared_ptr<SkillTalentOrPerk> skilltalentorperk = mCharacter.skillsTalentsOrPerks().takeAt(row);
    if (skilltalentorperk == nullptr) return;

    mSkillsTalentsOrPerksPoints -= skilltalentorperk->points(Complication::NoStore);

    updateSkills(skilltalentorperk);

    Ui->skillstalentsandperks->removeRow(row);
    Ui->totalskillstalentsandperkscost->setText(QString("%1").arg(mSkillsTalentsOrPerksPoints.points));
    updateDisplay();
    mChanged = true;
}

void Sheet::doneEditComplication() {
    shared_ptr<Complication> complication = mCompDlg->complication();
    if (complication->description().isEmpty()) return;

    updateDisplay();
    mChanged = true;
}

void Sheet::doneEditSkill() {
    shared_ptr<SkillTalentOrPerk> skilltalentorperk = mSkillDlg->skilltalentorperk();
    if (skilltalentorperk->description().isEmpty()) return;

    updateDisplay();
    mChanged = true;
}

void Sheet::editComplication() {
    auto selection = Ui->complications->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    shared_ptr<Complication> complication = mCharacter.complications()[row];
    if (complication == nullptr) return;

    mCompDlg = make_shared<ComplicationsDialog>(this);
    mCompDlg->complication(complication);
    connect(mCompDlg.get(), SIGNAL(accepted()), this, SLOT(doneEditComplication()));

#ifdef __wasm__
    closeDialogs(nullptr);
#endif
    mCompDlg->open();
}

void Sheet::editPowerOrEquipment() {
    auto selection = Ui->powersandequipment->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    shared_ptr<Power>& power = getPower(row, mCharacter.powersOrEquipment());
    if (power == nullptr) return;

    QJsonObject json = power->toJson();
    shared_ptr<Power> work = Power::FromJson(json["name"].toString(), json);
    if (work == nullptr) return;

    work->parent(power->parent());
    mPowerDlg = make_shared<PowerDialog>(this, power);
    mPowerDlg->powerorequipment(work);
    mPowerDlg->open();
}

void Sheet::editSkillstalentsandperks() {
    auto selection = Ui->skillstalentsandperks->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    shared_ptr<SkillTalentOrPerk> skilltalentorperk = mCharacter.skillsTalentsOrPerks()[row];
    if (skilltalentorperk == nullptr) return;

    mSkillDlg = make_shared<SkillDialog>(this);
    mSkillDlg->skilltalentorperk(skilltalentorperk);
    connect(mSkillDlg.get(), SIGNAL(accepted()), this, SLOT(doneEditSkill()));

#ifdef __wasm__
    closeDialogs(nullptr);
#endif
    mSkillDlg->open();
}

void Sheet::eyeColorChanged(QString txt) {
    mCharacter.eyeColor(txt);
    mChanged = true;
}

#ifdef __wasm__
void Sheet::editMenu() {
    closeDialogs(nullptr);
    mEditMenuDialog = make_shared<EditMenuDialog>();
    mEditMenuDialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    mEditMenuDialog->open();
}

void Sheet::fileMenu() {
    closeDialogs(nullptr);
    mFileMenuDialog = make_shared<FileMenuDialog>();
    mFileMenuDialog->setSave(mChanged);
    mFileMenuDialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    mFileMenuDialog->open();
}
#endif

void Sheet::gamemasterChanged(QString txt) {
    mCharacter.gamemaster(txt);
    mChanged = true;
}

void Sheet::focusChanged(QWidget*, QWidget* focus) {
    if (focus == Ui->strval  ||
        focus == Ui->dexval  ||
        focus == Ui->conval  ||
        focus == Ui->intval  ||
        focus == Ui->egoval  ||
        focus == Ui->preval  ||
        focus == Ui->ocvval  ||
        focus == Ui->dcvval  ||
        focus == Ui->omcvval ||
        focus == Ui->dmcvval ||
        focus == Ui->spdval  ||
        focus == Ui->pdval   ||
        focus == Ui->edval   ||
        focus == Ui->recval  ||
        focus == Ui->endval  ||
        focus == Ui->bodyval ||
        focus == Ui->stunval) characteristicChanged(dynamic_cast<QLineEdit*>(focus), "", DontUpdateTotal);
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
#ifdef __wasm__
    closeDialogs(nullptr);
    mImgMenuDialog = make_shared<ImgMenuDialog>();
    mImgMenuDialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    mImgMenuDialog->setPos(mapToGlobal(pos + Ui->image->pos() - QPoint(0, ui->scrollArea->verticalScrollBar()->value())));
    mImgMenuDialog->open();
#else
    Ui->imageMenu->exec(mapToGlobal(pos + Ui->image->pos() - QPoint(0, ui->scrollArea->verticalScrollBar()->value())));
#endif
}

void Sheet::moveComplicationDown() {
    auto selection = Ui->complications->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    auto& complications = mCharacter.complications();
    shared_ptr<Complication> complication = complications.takeAt(row);
    complications.insert(row + 1, complication);
    updateComplications();
}

void Sheet::moveComplicationUp() {
    auto selection = Ui->complications->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    auto& complications = mCharacter.complications();
    shared_ptr<Complication> complication = complications.takeAt(row);
    complications.insert(row - 1, complication);
    updateComplications();
}

void Sheet::movePowerOrEquipmentDown() {
    auto selection = Ui->powersandequipment->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    auto& powers = mCharacter.powersOrEquipment();
    auto power = getPower(row, powers);
    delPower(row);
    putPower(row + 2, power);
    updateDisplay();
}

void Sheet::movePowerOrEquipmentUp() {
    auto selection = Ui->powersandequipment->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    auto& powers = mCharacter.powersOrEquipment();
    auto power = getPower(row, powers);
    delPower(row);
    putPower(row - 1, power);
    updateDisplay();
}

void Sheet::moveSkillTalentOrPerkDown() {
    auto selection = Ui->skillstalentsandperks->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    auto& stps = mCharacter.skillsTalentsOrPerks();
    auto stp = stps.takeAt(row);
    stps.insert(row + 1, stp);
    updateSkillsTalentsAndPerks();
}

void Sheet::moveSkillTalentOrPerkUp() {
    auto selection = Ui->skillstalentsandperks->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    auto& skillstalentsorperks = mCharacter.skillsTalentsOrPerks();
    shared_ptr<SkillTalentOrPerk> skilltalentorperk = skillstalentsorperks.takeAt(row);
    skillstalentsorperks.insert(row - 1, skilltalentorperk);
    updateSkillsTalentsAndPerks();
}

void Sheet::erase() {
    mCharacter.erase();
    Ui->notes->setPlainText("");
    updateDisplay();
    mChanged = false;
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
    shared_ptr<Complication> complication = mCompDlg->complication();
    if (complication == nullptr) return;
    if (complication->description().isEmpty()) return;

    mCharacter.complications().append(complication);

    updateDisplay();
    mChanged = true;
}

void Sheet::newComplication() {
    mCompDlg = make_shared<ComplicationsDialog>(this);
    connect(mCompDlg.get(), SIGNAL(accepted()), this, SLOT(acceptComplication()));

#ifdef __wasm__
    closeDialogs(nullptr);
#endif
    mCompDlg->open();
}

void Sheet::newImage() {
#ifdef __wasm__
    QFileDialog::getOpenFileContent("Images (*.png *.xpm *jpg)", [&](const QString &fileName, const QByteArray &fileContent) {
        loadImage(fileContent, fileName);
    });
#else
    QString filename = QFileDialog::getOpenFileName(this, "New Image", mDir, "Images (*.png *.xpm *jpg)");
    if (filename.isEmpty()) return;
    loadImage(filename);
#endif
}

void Sheet::newPowerOrEquipment() {
    bool framework = false;
    auto selection = Ui->powersandequipment->selectedItems();
    if (!selection.isEmpty()) {
        shared_ptr<Power> work = getPower(selection[0]->row(), mCharacter.powersOrEquipment());
        if (work == nullptr) framework = false;
        else if (work->isFramework()) framework = work->isMultipower();
        else {
            auto parent = work->parent();
            framework = parent != nullptr && parent->isMultipower();
        }
    }

    mPowerDlg = make_shared<PowerDialog>(this);
    if (framework) mPowerDlg->multipower();
    mPowerDlg->open();
}

void Sheet::acceptNewSkill() {
    shared_ptr<SkillTalentOrPerk> skilltalentorperk = mSkillDlg->skilltalentorperk();
    if (skilltalentorperk == nullptr) return;
    if (skilltalentorperk->description().isEmpty()) return;

    mCharacter.skillsTalentsOrPerks().append(skilltalentorperk);

    updateDisplay();
    mChanged = true;
}

void Sheet::newSkillTalentOrPerk() {
    mSkillDlg = make_shared<SkillDialog>(this);
    connect(mSkillDlg.get(), SIGNAL(accepted()), this, SLOT(acceptNewSkill()));
    mSkillDlg->open();
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
    QString filename = QFileDialog::getOpenFileName(this, "Open File", mDir, "Characters (*.hsccu)");
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

#ifdef __wasm__
void Sheet::outsideImageArea() {
    closeDialogs(nullptr);
}
#endif

void Sheet::options() {
    mOptionDlg = make_shared<optionDialog>();
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
    const QMimeData* data = clip->mimeData();
    QByteArray byteArray = data->data("application/hsccucharacter");
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
    QClipboard* clip = QGuiApplication::clipboard();
    const QMimeData* data = clip->mimeData();
    QByteArray byteArray = data->data("application/complication");
    QString jsonStr(byteArray);
    QJsonDocument json = QJsonDocument::fromJson(jsonStr.toUtf8());
    QJsonObject obj = json.object();
    QString name = obj["name"].toString();
    shared_ptr<Complication> complication = Complication::FromJson(name, obj);
    mCharacter.complications().append(complication);

    int row = Ui->complications->rowCount();
    QFont font = Ui->complications->font();
    setCell(Ui->complications, row, 0, QString("%1").arg(complication->points(Complication::NoStore).points), font);
    setCell(Ui->complications, row, 1, complication->description(),                                           font, WordWrap);
    Ui->complications->resizeRowsToContents();

    mComplicationPoints += complication->points(Complication::NoStore);
    Ui->totalcomplicationpts->setText(QString("%1/%2").arg(mComplicationPoints.points).arg(mOption.complications().points));
    updateDisplay();
    mChanged = true;
}

void Sheet::pastePowerOrEquipment() {
    QClipboard* clip = QGuiApplication::clipboard();
    const QMimeData* data = clip->mimeData();
    QByteArray byteArray = data->data("application/powerorequipment");
    QString jsonStr(byteArray);
    QJsonDocument json = QJsonDocument::fromJson(jsonStr.toUtf8());
    QJsonObject obj = json.object();
    QString name = obj["name"].toString();
    shared_ptr<Power> power = Power::FromJson(name, obj);
    addPower(power);
}

void Sheet::pasteSkillTalentOrPerk() {
    QClipboard* clip = QGuiApplication::clipboard();
    const QMimeData* data = clip->mimeData();
    QByteArray byteArray = data->data("application/skillperkortalent");
    QString jsonStr(byteArray);
    QJsonDocument json = QJsonDocument::fromJson(jsonStr.toUtf8());
    QJsonObject obj = json.object();
    QString name = obj["name"].toString();
    shared_ptr<SkillTalentOrPerk> stp = SkillTalentOrPerk::FromJson(name, obj);
    mCharacter.skillsTalentsOrPerks().append(stp);

    int row = Ui->skillstalentsandperks->rowCount();
    QFont font = Ui->skillstalentsandperks->font();
    setCell(Ui->skillstalentsandperks, row, 0, QString("%1").arg(stp->points(Complication::NoStore).points), font);
    setCell(Ui->skillstalentsandperks, row, 1, stp->description(), font, WordWrap);
    setCell(Ui->skillstalentsandperks, row, 2, stp->roll(), font);
    Ui->skillstalentsandperks->resizeRowsToContents();

    updateSkills(stp);

    mSkillsTalentsOrPerksPoints += stp->points(Complication::NoStore);
    Ui->totalskillstalentsandperkscost->setText(QString("%1").arg(mSkillsTalentsOrPerksPoints.points));
    updateDisplay();
    mChanged = true;
}

void Sheet::playerNameChanged(QString txt) {
    mCharacter.playerName(txt);
    mChanged = true;
}

void Sheet::powersandequipmentMenu(QPoint pos) {
#ifdef __wasm__
    int row = Ui->powersandequipment->rowAt(pos.y());
    Ui->powersandequipment->selectRow(row);
    closeDialogs(nullptr);
    mPowerMenuDialog = make_shared<PowerMenuDialog>();
    mPowerMenuDialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    mPowerMenuDialog->setPos(mapToGlobal(pos + Ui->powersandequipment->pos() - QPoint(0, ui->scrollArea->verticalScrollBar()->value())));
    aboutToShowPowersAndEquipmentMenu();
    mPowerMenuDialog->open();
#else
    Ui->powersandequipmentMenu->exec(mapToGlobal(pos
                                                 + Ui->powersandequipment->pos()
                                                 - QPoint(0, ui->scrollArea->verticalScrollBar()->value())));
#endif
}

void Sheet::print() {
    bool saveChanged = mChanged;

    if (printer == nullptr) printer = new QPrinter(QPrinter::HighResolution);

    QPrintPreviewDialog preview(printer, this);
    preview.setWindowTitle("Print Chracter");
    preview.setMinimumHeight(600); // NOLINT
    preview.setMinimumWidth(800); // NOLINT
    this->connect(&preview, SIGNAL(paintRequested(QPrinter*)), this, SLOT(printCharacter(QPrinter*)));

    preview.exec();

    mChanged = saveChanged; // lots of changed signals get passed around but the character really didn't change
}

void Sheet::printCharacter(QPrinter* printer) {
    QPixmap page1(QString(":/gfx/Page1.png"));
    QPixmap page2(QString(":/gfx/Page2.png"));
    QPixmap page3(QString(":/gfx/Page3.png"));

    auto pageLayout = printer->pageLayout();
    pageLayout.setOrientation(QPageLayout::Orientation::Portrait);
    printer->setPageLayout(pageLayout);
    printer->setFullPage(false);
    QPainter painter;
    painter.begin(printer);
    QRectF pageRect = printer->pageRect(QPrinter::DevicePixel);
    double pnt = pageRect.height() / (11.0 * 72.0); // NOLINT
    double xscale = (pageRect.width() - 72.0 * pnt) / page1.width(); // NOLINT
    double yscale = (pageRect.height() - 72.0 * pnt) / page1.height(); // NOLINT
    double scale = qMin(xscale, yscale);
    painter.translate(QPoint({ (int) (36 * pnt), (int) (36 * pnt) })); // NOLINT
    painter.scale(scale, scale);

    QPoint offset { 55, 48 }; // NOLINT
    painter.drawImage(QPointF { 0.0, 0.0 }, page1.toImage());
    for (int i = 0; i < Ui->widgets.count(); ++i) {
        auto& widget = Ui->widgets[i];
        if (widget == nullptr || widget->y() > 1250) continue; // NOLINT  skip things we can't render or are on the seecond page
        print(painter, offset, widget);
    }

    int skillTop        = Ui->skillstalentsandperks->verticalScrollBar()->value();
    int complicationTop = Ui->complications->verticalScrollBar()->value();
    int powerTop        = Ui->powersandequipment->verticalScrollBar()->value();
    int notesTop        = Ui->notes->verticalScrollBar()->value();

    preparePrint(Ui->skillstalentsandperks);
    preparePrint(Ui->complications);
    preparePrint(Ui->powersandequipment);
    preparePrint(Ui->notes);

    update();

    int page = 0;
    offset = QPoint({ 50, 1352 }); // NOLINT
    int max = getPageCount();
    while (page < max) {
        printer->newPage();
        painter.drawImage(QPointF { 0.0, 0.0 }, page2.toImage());
        for (int i = 0; i < Ui->widgets.count(); ++i) {
            auto& widget = Ui->widgets[i];
            if (widget == nullptr || widget->y() < 1250) continue; // NOLINT  skip things we can't render or are on the first page
            print(painter, offset, widget);
        }
        deletePagefull();
        ++page;
    }

    if (mOption.showNotesPage()) {
        QString notes = Ui->notes->toPlainText();
        int max = getPageCount(Ui->notes, scale, &painter);
        offset = QPoint({ 50, 48 }); // NOLINT
        int page = 0;
        while (page < max) {
            printer->newPage();
            painter.drawImage(QPointF { -50.0, -48.0 }, page3.toImage()); // NOLINT
            for (int i = 0; i < Ui->hiddenWidgets.count(); ++i) {
                auto& widget = Ui->widgets[i];
                if (widget == nullptr) continue; // skip things we can't render
                print(painter, offset, widget);
            }
            deletePagefull(Ui->notes, scale, &painter);
            ++page;
        }
        Ui->notes->setPlainText(notes);
    }

    painter.end();

    Ui->skillstalentsandperks->verticalScrollBar()->setValue(skillTop);
    Ui->complications->verticalScrollBar()->setValue(complicationTop);
    Ui->powersandequipment->verticalScrollBar()->setValue(powerTop);
    Ui->notes->verticalScrollBar()->setValue(notesTop);

    Ui->skillstalentsandperks->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    Ui->complications->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    Ui->powersandequipment->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    Ui->notes->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    updateDisplay();
}

void Sheet::save() {
    if (!mChanged) return;
    mCharacter.notes() = Ui->notes->toPlainText();

#ifndef __wasm__
    if (mFilename.isEmpty()) {
        QString oldname = mFilename;
        mFilename = Ui->charactername->text();
        saveAs();
        if (mFilename.isEmpty()) mFilename = oldname;
        return;
    }

    if (!mCharacter.store(mOption, mDir + "/" + mFilename))
        OK("Can't save to \"" + mFilename + ".hsccu\" in the \"" + mDir + "\" folder.", std::bind(&Sheet::doNothing, this));
    else mChanged = false;
#else
    if (mFilename.isEmpty()) mFilename = Ui->charactername->text();
    if (!mCharacter.store(mOption, mFilename)) {
        OK("Can't save to \"" + mFilename + ".hsccu\".", std::bind(&Sheet::doNothing, this));
        throw "";
    }
    else mChanged = false;
#endif
}

void Sheet::saveAs() {
    QString oldname = mFilename;
    mFilename = QFileDialog::getSaveFileName(this, "Save File", mDir, "Characters (*.hsccu)");
    if (mFilename.isEmpty()) return;

    auto ext = mFilename.lastIndexOf(".hsccu");
    if (ext != -1) mFilename = mFilename.left(ext);

    auto sep = mFilename.lastIndexOf("/");
    if (sep != -1) {
        mDir = mFilename.left(sep);
        mFilename = mFilename.mid(sep + 1);
    }
    try { save(); } catch (...) { mFilename = oldname; }
}

void Sheet::skillstalentsandperksMenu(QPoint pos) {
#ifdef __wasm__
    int row = Ui->skillstalentsandperks->rowAt(pos.y());
    Ui->skillstalentsandperks->selectRow(row);
    closeDialogs(nullptr);
    mSkillMenuDialog = make_shared<SkillMenuDialog>();
    mSkillMenuDialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    mSkillMenuDialog->setPos(mapToGlobal(pos + Ui->skillstalentsandperks->pos() - QPoint(0, ui->scrollArea->verticalScrollBar()->value())));
    aboutToShowSkillsPerksAndTalentsMenu();
    mSkillMenuDialog->open();
#else
    Ui->skillstalentsandperksMenu->exec(mapToGlobal(pos + Ui->skillstalentsandperks->pos() - QPoint(0, ui->scrollArea->verticalScrollBar()->value())));
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

        Ui->experiencespent->setText(QString("%1").arg(spent.points));
        Ui->experienceunspent->setText(QString("%1").arg(remaining.points));
        mChanged = true;
    } else Ui->totalexperienceearned->undo();
}

void Sheet::totalExperienceEarnedEditingFinished() {
    totalExperienceEarnedChanged(Ui->totalexperienceearned->text());
    if (Ui->totalexperienceearned->text().isEmpty()) Ui->totalexperienceearned->setText("0");
}
