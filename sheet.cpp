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

Sheet* Sheet::_sheet = nullptr; // NOLINT
shared_ptr<class QMessageBox> Msg::Box; // NOLINT
std::function<void()> Msg::_Cancel; // NOLINT
std::function<void()> Msg::_No; // NOLINT
std::function<void()> Msg::_Ok; // NOLINT
std::function<void ()> Msg::_Yes; // NOLINT

Msg msngr; // NOLINT

void Msg::button(QAbstractButton* btn) {
    QString txt = btn->text();
    if (txt == "Cancel") Msg::_Cancel();
    else if (txt == "&No") Msg::_No();
    else if (txt == "&Ok") Msg::_Ok();
    else if (txt == "&Yes") Msg::_Yes();
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
    Msg::_Yes = yes;
    Msg::_No  = no;
    Msg::Box->setIcon(QMessageBox::Question);
    Msg::Box->setText(!title.isEmpty() ? title : "Are you sure?");
    Msg::Box->setInformativeText(msg);
    Msg::Box->setStandardButtons({ QMessageBox::Yes, QMessageBox::No });
    Msg::Box->open();
}

void YesNoCancel(const QString& msg, std::function<void()> yes, std::function<void()> no, std::function<void()> cancel, const QString& title = "") {
    Msg::Box = make_shared<QMessageBox>();
    Msg::Box->connect(Msg::Box.get(), SIGNAL(buttonClicked(QAbstractButton*)), &msngr, SLOT(button(QAbstractButton*)));
    Msg::_Yes = yes;
    Msg::_No = no;
    Msg::_Cancel = cancel;
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
    Msg::_Ok = ok;
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
    Msg::_Ok = ok;
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
    Msg::_Yes = yes;
    Msg::_No  = no;
    Msg::Box->setIcon(QMessageBox::Question);
    Msg::Box->setText(!title.isEmpty() ? title : "Are you sure?");
    Msg::Box->setInformativeText(msg);
    Msg::Box->setStandardButtons({ QMessageBox::Yes, QMessageBox::No });
    Msg::Box->open();
}

void Statement(const QString& msg) {
    Msg::Box = make_shared<QMessageBox>();
    Msg::Box->connect(Msg::Box.get(), SIGNAL(buttonClicked(QAbstractButton*)), &msngr, SLOT(button(QAbstractButton*)));
    Msg::_Ok = std::bind(&Sheet::doNothing, &Sheet::ref());
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

Sheet_UI Sheet::_Sheet_UI; // NOLINT

Sheet::Sheet(QWidget *parent)
    : QMainWindow(parent)
#ifndef __wasm__
    , ui(new Ui::Sheet)
#else
    , ui(new Ui::wasm)
#endif
    , Ui(&_Sheet_UI)
    , printer(nullptr)
    , _saveChanged(false)
{
    _sheet = this;

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

    Modifiers mods;

    _dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    _option.load();
    ui->optLabel->setVisible(_option.showNotesPage());

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

    _widget2Def = {
        { Ui->strval,  { &_character.STR(),  Ui->strval,  Ui->strpoints, Ui->strroll } },
        { Ui->dexval,  { &_character.DEX(),  Ui->dexval,  Ui->dexpoints, Ui->dexroll } },
        { Ui->conval,  { &_character.CON(),  Ui->conval,  Ui->conpoints, Ui->conroll } },
        { Ui->intval,  { &_character.INT(),  Ui->intval,  Ui->intpoints, Ui->introll } },
        { Ui->egoval,  { &_character.EGO(),  Ui->egoval,  Ui->egopoints, Ui->egoroll } },
        { Ui->preval,  { &_character.PRE(),  Ui->preval,  Ui->prepoints, Ui->preroll } },
        { Ui->ocvval,  { &_character.OCV(),  Ui->ocvval,  Ui->ocvpoints } },
        { Ui->dcvval,  { &_character.DCV(),  Ui->dcvval,  Ui->dcvpoints } },
        { Ui->omcvval, { &_character.OMCV(), Ui->omcvval, Ui->omcvpoints } },
        { Ui->dmcvval, { &_character.DMCV(), Ui->dmcvval, Ui->dmcvpoints } },
        { Ui->spdval,  { &_character.SPD(),  Ui->spdval,  Ui->spdpoints } },
        { Ui->pdval,   { &_character.PD(),   Ui->pdval,   Ui->pdpoints } },
        { Ui->edval,   { &_character.ED(),   Ui->edval,   Ui->edpoints } },
        { Ui->recval,  { &_character.REC(),  Ui->recval,  Ui->recpoints } },
        { Ui->endval,  { &_character.END(),  Ui->endval,  Ui->endpoints } },
        { Ui->bodyval, { &_character.BODY(), Ui->bodyval, Ui->bodypoints } },
        { Ui->stunval, { &_character.STUN(), Ui->stunval, Ui->stunpoints } }
    };

    installEventFilter(dynamic_cast<QObject*>(this));

#ifndef __wasm__
    QStringList args = qApp->arguments(); // NOLINT
    if (args.count() > 1) {
        _filename = QDir::fromNativeSeparators(args[1]);
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

Sheet::~Sheet()
{
    delete printer;
    delete ui;
    // Ui's contents are pointed to by ui->label, don't delete it (double deletes)!  Don't worry, it is not allocated either, static global storage with a pointer to it.
}

// --- [EVENT FILTER] ----------------------------------------------------------------------------------

bool Sheet::eventFilter(QObject* object, QEvent* event) {
    if (event->type() == QEvent::FocusIn && _widget2Def.find(object) != _widget2Def.end()) {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(object);
        edit->setText(QString("%1").arg(_widget2Def[edit].characteristic()->base()));
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
    if (_complicationsMenuDialog != nullptr) closeDialog(_complicationsMenuDialog, me);
    if (_editMenuDialog          != nullptr) closeDialog(_editMenuDialog,          me);
    if (_fileMenuDialog          != nullptr) closeDialog(_fileMenuDialog,          me);
    if (_imgMenuDialog           != nullptr) closeDialog(_imgMenuDialog,           me);
    if (_skillMenuDialog         != nullptr) closeDialog(_skillMenuDialog,         me);
    if (_powerMenuDialog         != nullptr) closeDialog(_powerMenuDialog,         me);
#endif
    if (_optionDlg != nullptr) closeDialog(_optionDlg, me);
    if (_compDlg   != nullptr) closeDialog(_compDlg,   me);
    if (_powerDlg  != nullptr) closeDialog(_powerDlg,  me);
    if (_skillDlg  != nullptr) closeDialog(_skillDlg,  me);
}

void Sheet::mousePressEvent(QMouseEvent* me) {
    closeDialogs(me);
}

void Sheet::closeEvent(QCloseEvent* event) {
    if (checkClose()) event->accept();
    else event->ignore();
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
    } else _character.powersOrEquipment().append(power);

    power->modifiers().clear();
    for (const auto& mod: power->advantagesList()) power->modifiers().append(mod);
    for (const auto& mod: power->limitationsList()) power->modifiers().append(mod);

    updatePower(power);
    updateDisplay();
    _changed = true;
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
    for (const auto& button: buttons) button->setStyleSheet("QPushButton:disabled { border-color: #000; color: #888; } "
                                                            "QPushButton:enabled { border-color: #000; color: #000; } "
                                                            );
}

Points Sheet::characteristicsCost() {
    Points total = 0_cp;
    const auto keys = _widget2Def.keys();
    for (const auto& key: keys) total += _widget2Def[key].characteristic()->points();
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
        val->setText(txt = QString("%1").arg(_widget2Def[val].characteristic()->base()));
    }
    if (numeric(txt) || txt.isEmpty()) {
        auto& def = _widget2Def[val];
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
            QString end = QString("%1").arg((primary + 4) / _option.activePerEND().points);
            if (primary != secondary) end += QString("/%1").arg((secondary + 4) / _option.activePerEND().points);
            Ui->strendcost->setText(end);
            rebuildMartialArts();
            QString lift = formatLift(primary);
            if (primary != secondary) lift += "/" + formatLift(secondary);
            Ui->lift->setText(lift);
        } else if (val == Ui->spdval) {
            if (def.characteristic()->base() < 1) return;
            QList<int> chart = phases[secondary];
            for (auto& x: Ui->phases) x->setText("");
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
    auto& def = _widget2Def[val];
    txt = def.characteristic()->value();
    val->setText(txt);
}

void Sheet::saveThenExit() {
    try { save(); } catch (...) { return; } // bug is saving?
    if (_changed) return; // hit cancel in save as?
    close(); // really exit this time
}

void Sheet::justClose() {
    _changed = false;
    close(); // really exit this time
}

bool Sheet::checkClose() {
    if (_changed) {
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
    for (auto& x: _hitLocations) x = 0;
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
    auto power = getPower(row, _character.powersOrEquipment());
    if (power == nullptr) return;

    if (power->parent() == nullptr) {
        auto realRow = _character.powersOrEquipment().indexOf(power);
        _character.powersOrEquipment().removeAt(realRow);
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
    for (const auto& mod: pe->advantagesList()) {
        if (mod == nullptr) continue;

        if (mod->isAdder()) descr += "; (+" + QString("%1").arg(mod->points(Power::NoStore).points) + " pts) ";
        else descr += "; (+" + mod->fraction(Power::NoStore).toString() + ") ";
        descr += mod->description(false);
    }
    for (const auto& mod: pe->limitationsList()) {
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
    if (pe->isFramework()) _powersOrEquipmentPoints += pe->display(row, Ui->powersandequipment);
    return pts.toInt();
}

#ifndef __wasm__
void Sheet::doLoadImage() {
    loadImage(_character.image());
    _saveChanged = _changed;
    skipLoadImage();
}

void Sheet::skipLoadImage() {
    Ui->notes->setPlainText(_character.notes());
    updateDisplay();
    _changed = _saveChanged;
}
#endif

void Sheet::updateBanner()
{
    QPixmap pixmap(_option.banner());
    pixmap = pixmap.scaled(293, 109, Qt::KeepAspectRatio, Qt::SmoothTransformation); // NOLINT
    Ui->banner1->setPixmap(pixmap);
    Ui->banner2->setPixmap(pixmap);
    Ui->banner3->setPixmap(pixmap);
}

#ifdef __wasm__
void Sheet::fileOpen(const QByteArray& data, QString filename) {
    int ext = filename.lastIndexOf(".hsccu");
    if (ext != -1) _filename = filename.left(ext);

    int sep = _filename.lastIndexOf("/");
    if (sep != -1) {
        _dir = _filename.left(sep);
        _filename = _filename.mid(sep + 1);
    }
    _character.load(_option, data);
    Ui->notes->setPlainText(_character.notes());
    updateDisplay();
    _changed = false;
}
#else
void Sheet::fileOpen() {
    Power::Equipment(); // pre-load equipment if needed

    auto ext = _filename.lastIndexOf(".hsccu");
    if (ext != -1) _filename = _filename.left(ext);

    auto sep = _filename.lastIndexOf("/");
    if (sep != -1) {
        _dir = _filename.left(sep);
        _filename = _filename.mid(sep + 1);
    }

    if (!_character.load(_option, _dir + "/" + _filename))
        OK("Can't load \"" + _filename + ".hsccu\" from the \"" + _dir + "\" folder.", std::bind(&Sheet::doNothing, this));
    else {
        _saveChanged = false;
        QFileInfo imageFile(_character.image());
        if (imageFile.exists()) {
            qulonglong then(_character.imageDate());
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
    out += "Character Name: " + _character.characterName();
    if (!_character.alternateIds().isEmpty()) out += " (" + _character.alternateIds() + ")\n";
    else out += "\n";
    out += "Player Name: " + _character.playerName() + "\n\n";

    out += "Campaign Name: " + _character.campaignName() + "\n";
    out += "Genre: " + _character.genre() + "\n";
    out += "Gamemaster" + _character.gamemaster() + "\n\n";

    out += "Height: " + _character.height() + "\n";
    out += "Weight: " + _character.weight() + "\n";
    out += "Hair Color: " + _character.hairColor() + "\n";
    out += "Eye Color: " + _character.eyeColor() + "\n\n";

    QStringList names { "STR", "DEX", "CON", "INT", "EGO", "PRE", "OCV", "DCV", "OMCV", "DMCV", "SPD", "PD", "ED", "REC", "END", "BODY", "STUN" };
    for (auto i = 0; i < names.count(); ++i) {
        auto characteristic = _character.characteristic(i);
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

    out += QString("Perception Roll:\t%1\n\n").arg(_character.INT().roll());

    int pd = _character.PD().primary() + _character.PD().base() + _character.PD().secondary();
    int rPd = _character.rPD();
    out += QString("PD/rPD:\t%1/%2\n").arg(pd).arg(rPd);
    int ed = _character.ED().primary() + _character.ED().base() + _character.ED().secondary();
    int rEd = _character.rED();
    out += QString("ED/rED:\t%1/%2\n").arg(ed).arg(rEd);
    out += QString("MD:\t%1\n").arg(_character.MD());
    out += QString("PowD:\t%1\n").arg(_character.PowD());
    out += QString("FD:\t%1\n").arg(_character.FD());
    out += "\n";

    out += "Skills, Talents, and Perks\n";
    for (const auto& skill: _character.skillsTalentsOrPerks()) {
        if (skill == nullptr) continue;

        out += QString("%1\t%2\n").arg(skill->points(SkillTalentOrPerk::NoStore).points).arg(skill->description());
    }
    out += QString("%1\tTotal Skills, Talents, and Perks\n\n").arg(Ui->totalskillstalentsandperkscost->text());

    out += "Powers and Equipment\n";
    for (const auto& power: _character.powersOrEquipment()) {
        if (power == nullptr) continue;

        QString end = power->end();
        if (end == "-") end = "";
        out += QString("%1\t%2%3\n").arg(power->points(Power::NoStore).points).arg(power->description(), end.isEmpty() ? "" : "\t[" + end + "]");
        if (power->isFramework()) power->display(out);
    }
    out += QString("%1\tTotal Powers and Equipment\n\n").arg(Ui->totalpowersandequipmentcost->text());

    out += "Complications\n";
    for (const auto& complication: _character.complications()) {
        if (complication == nullptr) continue;

        out += QString("%1\t%2\n").arg(complication->points(Complication::NoStore).points).arg(complication->description());
    }
    out += QString("%1\tTotal Complications Points\n\n").arg(Ui->totalcomplicationpts->text());

    out += QString("%1\tTotal Points\n").arg(Ui->totalpoints->text());
    out += QString("%1\tTotal Experience Earned\n").arg(Ui->totalexperienceearned->text());
    out += QString("%1\tExperience Spent\n").arg(Ui->experiencespent->text());
    out += QString("%1\tExperience Unspent\n\n").arg(Ui->experienceunspent->text());

    out += "Notes:\n" + _character.notes() + "\n";
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
    for (const auto& x: commas) {
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
    int baseDEF = _character.rPD();
    int def = baseDEF + arm->DEF();
    for (const auto& x: locations) if (def > _hitLocations[x]) _hitLocations[x] = def; // NOLINT
}

void Sheet::loadImage(QPixmap& pixmap, QString filename) {
    clearImage();
    QPixmap scaled = pixmap.scaledToWidth(Ui->image->width());
    if (scaled.height() > Ui->image->height()) scaled = pixmap.scaledToHeight(Ui->image->height());
    Ui->image->setPixmap(scaled);
    _character.image() = filename;
    QByteArray sync;
    QBuffer buffer(&sync);
    buffer.open(QIODevice::WriteOnly);
    scaled.save(&buffer, "PNG");
    buffer.close();
    _character.imageData() = sync;
    QFileInfo imageFile(filename);
    QDateTime tm = imageFile.lastModified();
    _character.imageDate() = tm.toSecsSinceEpoch();
    _changed = true;
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

    shared_ptr<Power> after = getPower(row, _character.powersOrEquipment());
    if (after == nullptr) {
        power->parent(nullptr);
        _character.powersOrEquipment().append(power);
    } else if (after->isFramework()) {
        if (power->parent() == after.get()) {
            auto realRow = _character.powersOrEquipment().indexOf(after);
            power->parent(nullptr);
            _character.powersOrEquipment().insert(realRow, power);
        } else after->insert(0, power);
    } else {
        auto parent = after->parent();
        if (parent == nullptr) {
            auto realRow = _character.powersOrEquipment().indexOf(after);
            if (power->parent() == nullptr) _character.powersOrEquipment().insert(realRow, power);
            else {
                power->parent(nullptr);
                _character.powersOrEquipment().insert(--realRow, power);
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
    for (const auto& power: list) {
        if (power == nullptr) continue;

        if (power->name() == "Skill" && power->skill()->name() == "Combat Luck") {
            if (power->skill()->place() == 1) {
                _character.rPD() = _character.rPD() + power->skill()->rPD();
                _character.rED() = _character.rED() + power->skill()->rED();
            } else if (power->skill()->place() == 2){
                _character.temprPD() = _character.temprPD() + power->skill()->rPD();
                _character.temprED() = _character.temprED() + power->skill()->rED();
            }
        } else if (power->name() == "Barrier") {
            if (power->place() == 1) {
                _character.rPD() = _character.rPD() + power->rPD();
                _character.rED() = _character.rED() + power->rED();
            } else if (power->place() == 2) {
                _character.temprPD() += power->rPD() + power->PD();
                _character.temprED() += power->rED() + power->ED();
            }
        } else if (power->name() == "Flash Defense") {
            _character.FD() += power->FD();
        }
        else if (power->name() == "Mental Defense") {
            _character.MD() += power->MD();
        }
        else if (power->name() == "Power Defense") {
            _character.PowD() += power->PowD();
        } else if (power->name() == "Density Increase") {
            _character.STR().secondary(_character.STR().secondary() + power->str());
            _character.rPD() += power->rPD();
            _character.rED() += power->rED();
            if (power->hasModifier("Nonresistant Defense")) {
                _character.PD().secondary(_character.PD().secondary() + power->PD());
                _character.ED().secondary(_character.ED().secondary() + power->ED());
            }
        } else if (power->name() == "Resistant Defense") {
            if (power->place() == 1) {
                _character.rPD() = _character.rPD() + power->rPD() + power->PD();
                _character.rED() = _character.rED() + power->rED() + power->ED();
            } else if (power->place() == 2) {
                _character.temprPD() = _character.temprPD() + power->rPD() + power->PD();
                _character.temprED() = _character.temprED() + power->rED() + power->ED();
            }
        } else if (power->name() == "Growth") {
            auto& sm = power->growthStats();
            _character.STR().secondary(_character.STR().secondary() + sm._STR);
            _character.CON().secondary(_character.CON().secondary() + sm._CON);
            _character.PRE().secondary(_character.PRE().secondary() + sm._PRE);
            _character.PD().secondary(_character.PD().secondary() + sm._PD);
            _character.ED().secondary(_character.ED().secondary() + sm._ED);
            _character.BODY().secondary(_character.BODY().secondary() + sm._BODY);
            _character.STUN().secondary(_character.STUN().secondary() + sm._STUN);
            if (power->hasModifier("Resistant")) {
                _character.rPD() += sm._PD;
                _character.rED() += sm._ED;
            }
        } else if (power->name() == "Characteristics") {
            int put = power->characteristic(-1);
            if (put < 1) continue;
            for (int i = 0; i < 17; ++i) { // NOLINT
                if (put == 1) _character.characteristic(i).primary(_character.characteristic(i).primary() + power->characteristic(i));
                else _character.characteristic(i).secondary(_character.characteristic(i).secondary() + power->characteristic(i));
            }
            if (power->hasModifier("Resistant")) {
                _character.rPD() += power->characteristic(11); // NOLINT
                _character.rED() += power->characteristic(12); // NOLINT
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
        int base = _character.characteristic(i).base();
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
    for (const auto& pow: list) {
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
    for (const auto& stp: _character.skillsTalentsOrPerks()) {
        if (stp == nullptr) continue;

        QString d = rebuildCombatSkillLevel(stp);
        if (d.isEmpty()) continue;
        if (first) first = false;
        else csl += ", ";
        csl += d;
    }

    rebuildCSLPower(_character.powersOrEquipment(), first, csl);

    Ui->combatskilllevels->setHtml(csl);
}

void Sheet::rebuildDefenses() {
    _character.rPD() = 0;
    _character.rED() = 0;
    _character.temprPD() = 0;
    _character.temprED() = 0;
    _character.tempPD() = 0;
    _character.tempED() = 0;
    _character.FD() = 0;
    _character.MD() = 0;

    for (const auto& skill: _character.skillsTalentsOrPerks()) {
        if (skill == nullptr) continue;

        if (skill->name() == "Combat Luck") {
            if (skill->place() == 1) {
                _character.rPD() = _character.rPD() + skill->rPD();
                _character.rED() = _character.rED() + skill->rED();
            } else if (skill->place() == 2){
                _character.temprPD() = _character.temprPD() + skill->rPD();
                _character.temprED() = _character.temprED() + skill->rED();
            }
        }
    }

    for (int i = 0; i < 17; ++i) { // NOLINT
        _character.characteristic(i).primary(0);
        _character.characteristic(i).secondary(0);
    }

    rebuildCharFromPowers(_character.powersOrEquipment());

    int primPD = _character.PD().base() + _character.PD().primary() + _character.rPD();
    int secondPD = primPD + _character.PD().secondary() + _character.temprPD();
    int primED = _character.ED().base() + _character.ED().primary() + _character.rED();
    int secondED = primED + _character.ED().secondary() + _character.temprED();

    if (primPD == secondPD) setDefense(primPD, 0,                 0, 1);
    else                    setDefense(primPD, secondPD - primED, 0, 1);
    if (primED == secondED) setDefense(primED, 0,                 2, 1);
    else                    setDefense(primED, secondED - primED, 2, 1);

    setDefense(_character.rPD(),  _character.temprPD(),           1, 1);
    setDefense(_character.rED(),  _character.temprED(),           3, 1);
    setDefense(_character.MD(),   0,                              4, 1);
    setDefense(_character.PowD(), 0,                              5, 1); // NOLINT
    setDefense(_character.FD(),   0,                              6, 1); // NOLINT
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
        { "Martial Throw",    { "½", "+0", "+1", "(%9+v/10)d6,tgt falls~%1/%2/%3/%4/%5/%6/%7/%8/%9" } },
        { "Nerve Strike",     { "½", "-1", "+1", "2d6 NND~%1/%2/%3/%4/%5/%6/%7/%8/%9" } },
        { "Offensive Strike", { "½", "-2", "+1", "%7~%1/%2/%3/%4/%5/%6/%7/%8/%9" } },                    // STR+4 --> A
        { "Passing Strike",   { "½", "+1", "+0", "(%9+v/10)d6,full mv~%1/%2/%3/%4/%5/%6/%7/%8/%9" } },
        { "Sacrifice Throw",  { "½", "+2", "+1", "%8 STR, both fall~%1/%2/%3/%4/%5/%6/%7/%8/%9" } }      // STR
    };

    auto* man = Ui->attacksandmaneuvers;
    QString d = stp->description().mid(stp->name().length() + 2);
    auto maneuvers = d.split(", ");
    int extraSTR = 0;
    for (const auto& m: maneuvers) {
        if (!table.contains(m)) {
            auto parts = m.split(" ");
            if (parts.size() == 4) extraSTR = parts[0].toInt() * 5; // NOLINT
        }
    }
    int STR = _character.STR().base() + _character.STR().primary() + extraSTR;
    for (const auto& m: maneuvers) {
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
                                                  { "Grab By",      "½†",    "-3",    "-4",  "Move&Grab;+(ͮvel⁄₁₀) STR~%1%2%3%4%5%6"    },
                                                  { "Haymaker",     "½*",    "+0",    "-5",  "+4 DCs to attack~%1%2%3%4%5%6"           },
                                                  { "Move By",      "½†",    "-2",    "-2",  "(%2+ͮvel⁄₁₀)d6; take ⅓~%1%2%3%4%5%6"      }, // STR/2 noD6
#if defined(__wasm__) || defined(unix)
                                                  { "Move Thru",    "½†",    "-ͮ⁄₁₀", "-3",  "(%3+ͮvel⁄₆)d6; take ½ or all~%1%2%3%4%5%6" }, // STR noD6
                                                  { "Mult.Attx",    "1",     "var",   "½",   "Attack multiple times~%1%2%3%4%5%6"      },
#else
                                                  { "Move Through",     "½†", "-ͮ⁄₁₀", "-3", "(%3+ͮ⁄₆)d6; take ½ or all~%1%2%3%4%5%6"    },
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
    int STR = _character.STR().base() + _character.STR().primary();
    int OCV = Ui->ocvval->text().toInt();
    for (const auto& m: maneuvers) {
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

    for (const auto& stp: _character.skillsTalentsOrPerks()) {
        if (stp == nullptr) continue;

        stp->points(SkillTalentOrPerk::NoStore);
        if (stp->name() == "Martial Arts") rebuildMartialArt(stp, font);
    }
    for (const auto& power: _character.powersOrEquipment()) {
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
    for (const auto& power: list) {
        if (power == nullptr) continue;

        if (power->name() == "Growth") {
            auto& sm = power->growthStats();
            _character.running() += sm._running;
        } else if (power->name() == "Running") {
            _character.running() += power->move();
        } else if (power->name() == "Leaping") {
            _character.leaping() += power->move();
        } else if (power->name() == "Swimming") {
            _character.swimming() += power->move();
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
    _character.running()  = 12; // NOLINT
    _character.leaping()  = 4;
    _character.swimming() = 4;
    QMap<QString, int> movements;
    QMap<QString, QString> units;
    QMap<QString, int> doubles;

    rebuildMoveFromPowers(_character.powersOrEquipment(), movements, units, doubles);

    Ui->movement->setRowCount(4);
    Ui->movement->update();

    QString running = QString("%1m").arg(_character.running());
    int mult = searchImprovedNoncombatMovement("Running");
    QString ncRunning = QString("%1m").arg(mult * _character.running());
    QString swimming = QString("%1m").arg(_character.swimming());
    mult = searchImprovedNoncombatMovement("Swimming");
    QString ncSwimming = QString("%1m").arg(mult * _character.swimming());
    QString hLeaping = QString("%1m").arg(_character.leaping());
    mult = searchImprovedNoncombatMovement("Leaping");
    QString ncHLeaping = QString("%1m").arg(mult * _character.leaping());
    int vLeap = (_character.leaping() + 1) / 2;
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
    for (const auto& name: keys) {
        setCellLabel(Ui->movement, row, 0, name,                                                                  font);
        setCellLabel(Ui->movement, row, 1, QString("%1%2").arg(movements[name]).arg(units[name]),                 font);
        setCellLabel(Ui->movement, row, 2, QString("%1%2").arg(doubles[name] * movements[name]).arg(units[name]), font);
        row++;
    }
}

void Sheet::rebuildPowers(bool addTakesNoSTUN) {
    Ui->height->setText(_character.height());
    Ui->weight->setText(_character.weight());

    if (addTakesNoSTUN) _character.hasTakesNoSTUN() = true;
    else {
        _character.hasTakesNoSTUN() = false;
        for (const auto& power: _character.powersOrEquipment()) {
            if (power == nullptr) continue;

            if (power->name() == "Takes No STUNϴ") {
                _character.hasTakesNoSTUN() = true;
                break;
            }
            if (power->isFramework()) {
                for (const auto& pwr: power->list()) {
                    if (pwr == nullptr) continue;

                    if (pwr->name() == "Takes No STUNϴ") {
                        _character.hasTakesNoSTUN() = true;
                        break;
                    }
                }
                if (_character.hasTakesNoSTUN()) break;
            }
        }
    }

    if (_character.hasTakesNoSTUN()) {
        _character.PD().base(1);
        _character.PD().init(1);
        _character.PD().cost(3_cp);
        _character.ED().base(1);
        _character.ED().init(1);
        _character.ED().cost(3_cp);
        _character.DCV().cost(15_cp); // NOLINT
        _character.DMCV().cost(9_cp); // NOLINT
        Ui->pdval->setToolTip("Physical Defense: 3 points");
        Ui->edval->setToolTip("Energy Defense: 3 points");
        Ui->dcvval->setToolTip("Defensive Combat Value: 15 points");
        Ui->dmcvval->setToolTip("Defensive Mental Combat Value: 9 points");
    } else {
        if (_character.PD().cost() == 3_cp) {
            _character.PD().base(2);
            _character.PD().init(2);
            _character.PD().cost(1_cp);
        }
        if (_character.ED().cost() == 3_cp) {
            _character.ED().base(2);
            _character.ED().init(2);
            _character.ED().cost(1_cp);
        }
        _character.DCV().cost(5_cp); // NOLINT
        _character.DMCV().cost(3_cp);
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
    rebuildSenseFromPowers(_character.powersOrEquipment(), senses);
    Ui->enhancedandunusualsenses->setText(senses);
}

int Sheet::searchImprovedNoncombatMovement(QString name) {
    int mult = 0;
    for (const auto& power: _character.powersOrEquipment()) {
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

void Sheet::setCVs(_CharacteristicDef& def, QLabel* set) {
    int primary = def.characteristic()->base() + def.characteristic()->primary();
    int secondary = primary + def.characteristic()->secondary();
    QString cv = QString("%1").arg(primary);
    if (primary != secondary) cv += QString("/%1").arg(secondary);
    set->setText(cv);
}

void Sheet::setCell(QTableWidget* tbl, int row, int col, QString str, const QFont& font, bool) {
    gsl::owner<QTableWidgetItem*> lbl = new QTableWidgetItem(str);
    lbl->setFont(font);
    lbl->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
    lbl->setFlags(lbl->flags() & ~Qt::ItemIsEditable);
    if (row >= tbl->rowCount()) tbl->setRowCount(row + 1);
    tbl->setItem(row, col, lbl);
}

void Sheet::setCellLabel(QTableWidget* tbl, int row, int col, QString str, const QFont& font) {
    gsl::owner<QLabel*> lbl = new QLabel(str);
#ifdef unix
    lbl->setStyleSheet("color: #000;");
#endif
    lbl->setFont(font);
    if (row >= tbl->rowCount()) tbl->setRowCount(row + 1);
    tbl->setCellWidget(row, col, lbl);
}

void Sheet::setCellLabel(QTableWidget* tbl, int row, int col, QString str) {
    gsl::owner<QLabel*> lbl = new QLabel(str);
    if (row >= tbl->rowCount()) tbl->setRowCount(row + 1);
    QLabel* cell = dynamic_cast<QLabel*>(tbl->cellWidget(row, col));
    lbl->setFont(cell->font());
    tbl->setCellWidget(row, col, lbl);
}

void Sheet::setDamage(_CharacteristicDef& def, QLabel* set) {
    int primary = def.characteristic()->base() + def.characteristic()->primary();
    int secondary = primary + def.characteristic()->secondary();
    QString dice = valueToDice(primary);
    if (primary != secondary) dice += "/" + valueToDice(secondary);
    set->setText(dice);
}

void Sheet::setDefense(_CharacteristicDef& def, int r, int c, QLineEdit* val) {
    int primary = def.characteristic()->base() + def.characteristic()->primary();
    int secondary = primary + def.characteristic()->secondary();
    if (val == Ui->pdval) secondary += _character.tempPD();
    if (val == Ui->edval) secondary += _character.tempED();
    QString defense = QString("%1").arg(primary);
    if (primary != secondary) defense += QString("/%1").arg(secondary);
    setCell(Ui->defenses, r, c, defense, Ui->font);
}

void Sheet::setDefense(int def, int temp, int r, int c) {
    QString defense = QString("%1").arg(def);
    if (temp != 0) defense += QString("/%1").arg(temp + def);
    setCell(Ui->defenses, r, c, defense, Ui->font);
}

void Sheet::setMaximum(_CharacteristicDef& def, QLabel* set, QLineEdit* cur) {
    int primary = def.characteristic()->base() + def.characteristic()->primary();
    int secondary = primary + def.characteristic()->secondary();
    set->setText(QString("%1").arg(secondary));
    cur->setText(set->text());
}

void Sheet::updateCharacteristics() {
    const auto keys = _widget2Def.keys();
    _charactersticPoints = 0_cp;
    for (const auto& key: keys) {
        auto& def = _widget2Def[key];
        QLineEdit* characteristic = dynamic_cast<QLineEdit*>(key);
        QString value = def.characteristic()->value();
        characteristic->setText(value);
        _charactersticPoints += def.characteristic()->points();
    }
}

void Sheet::updateCharacter() {
    Ui->charactername->setText(_character.characterName());
    Ui->charactername2->setText(_character.characterName());
    Ui->alternateids->setText(_character.alternateIds());
    Ui->playername->setText(_character.playerName());
    Ui->haircolor->setText(_character.hairColor());
    Ui->eyecolor->setText(_character.eyeColor());
    Ui->campaignname->setText(_character.campaignName());
    Ui->genre->setText(_character.genre());
    Ui->gamemaster->setText(_character.gamemaster());
    Ui->totalexperienceearned->setText(QString("%1").arg(_character.xp().points));
    QPixmap pic;
    pic.loadFromData(_character.imageData());
    Ui->image->setPixmap(pic);
}

void Sheet::updateComplications() {
    Ui->complications->setRowCount(0);
    Ui->complications->update();

    _complicationPoints = 0_cp;
    QFont font = Ui->complications->font();
    int row = 0;
    for (const auto& complication: _character.complications()) {
        if (complication == nullptr) continue;

        QString descr = complication->description();
        if (descr == "-") descr = "";
        Points pts = complication->points(Complication::NoStore);
        setCell(Ui->complications, row, 0, descr.isEmpty() ? "" : QString("%1").arg(pts.points), font);
        setCell(Ui->complications, row, 1, descr.isEmpty() ? "" : descr, font, WordWrap);
        _complicationPoints += pts;
        ++row;
    }
    Ui->complications->resizeRowsToContents();

    Ui->totalcomplicationpts->setText(QString("%1/%2").arg(_complicationPoints.points).arg(_option.complications().points));
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
    for (const auto& x: loc) {
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
        for (const auto x: loc) {
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

    for (const auto& x: mapping) x.lbl->setText(hit2String(x.loc, _hitLocations));

    int def = 0;
    int baseDEF = _character.rPD();
    for (int i = 9; i <= 14; ++i) def += _hitLocations[i] - baseDEF; // NOLINT
    int count = ((_hitLocations[3] != baseDEF) ? 1 : 0) +
        ((_hitLocations[4] != baseDEF) ? 1 : 0) +
        ((_hitLocations[5] != baseDEF) ? 1 : 0); // NOLINT
    if (count > 1) {
        int max = _hitLocations[3];
        for (int i = 4; i <= 5; ++i) // NOLINT
            if (max < _hitLocations[i]) max = _hitLocations[i] - baseDEF; // NOLINT
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

    _powersOrEquipmentPoints = 0_cp;
    int row = 0;
    clearHitLocations();
    for (const auto& pe: _character.powersOrEquipment()) {
        if (pe == nullptr) continue;

        _powersOrEquipmentPoints += Points(displayPowerAndEquipment(row, pe));
    }
    Ui->powersandequipment->resizeRowsToContents();
    updateHitLocations();

    Ui->totalpowersandequipmentcost->setText(QString("%1").arg(_powersOrEquipmentPoints.points));
}

void Sheet::updateSkillRolls() {
    QFont font = Ui->skillstalentsandperks->font();
    int row = 0;
    for (const auto& stp: _character.skillsTalentsOrPerks()) {
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
    _character.clearEnhancers();
    for (const auto& stp: _character.skillsTalentsOrPerks()) {
        if (stp == nullptr) continue;

        if (stp->name() == "Combat Skill Levels" ||
            stp->name() == "Range Skill Levels" ||
            stp->name() == "Skill Levels") rebuildCombatSkillLevels();
        else if (stp->name() == "Martial Arts") rebuildMartialArts();
        else if (stp->name() == "Combat Luck") rebuildCharacteristics();
          else if (stp->name() == "Jack Of All Trades")  _character.hasJackOfAllTrades() = true;
          else if (stp->name() == "Linguist")            _character.hasLinguist() = true;
          else if (stp->name() == "Scholar")             _character.hasScholar() = true;
          else if (stp->name() == "Scientist")           _character.hasScientist() = true;
          else if (stp->name() == "Traveler")            _character.hasTraveler() = true;
          else if (stp->name() == "Well-Connected")      _character.hasWellConnected() = true;
    }

    _skillsTalentsOrPerksPoints = 0_cp;
    QFont font = Ui->skillstalentsandperks->font();
    int row = 0;
    for (const auto& stp: _character.skillsTalentsOrPerks()) {
        if (stp == nullptr) continue;
        QString descr = stp->description();
        if (descr == "-") descr = "";
        Points pts = stp->points(Complication::NoStore);
        setCell(Ui->skillstalentsandperks, row, 0, descr.isEmpty() ? "" : QString("%1").arg(pts.points), font);
        setCell(Ui->skillstalentsandperks, row, 1, descr.isEmpty() ? "" : descr,                         font, WordWrap);
        setCell(Ui->skillstalentsandperks, row, 2, descr.isEmpty() ? "" : stp->roll(),                   font);
        _skillsTalentsOrPerksPoints += pts;
        ++row;
    }
    Ui->skillstalentsandperks->resizeRowsToContents();

    Ui->totalskillstalentsandperkscost->setText(QString("%1").arg(_skillsTalentsOrPerksPoints.points));
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
    _totalPoints = characteristicsCost() + _skillsTalentsOrPerksPoints + _powersOrEquipmentPoints;
    Points pointsEarned = _option.totalPoints() - _option.complications();
    if (_option.complications() < _complicationPoints) pointsEarned += _option.complications();
    else pointsEarned += _complicationPoints;
    Ui->totalpoints->setText(QString("%1/%2").arg(_totalPoints.points).arg(pointsEarned.points));
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
    Ui->moveComplicationDown->setEnabled(show && row != _character.complications().count() - 1);
    Ui->pasteComplication->setEnabled(canPaste);
#else
    bool canPaste = false;
    _complicationsMenuDialog->setEdit(show);
    _complicationsMenuDialog->setDelete(show);
    _complicationsMenuDialog->setCut(show);
    _complicationsMenuDialog->setCopy(show);
    _complicationsMenuDialog->setPaste(canPaste);
    _complicationsMenuDialog->setMoveUp(show && row != 0);
    _complicationsMenuDialog->setMoveDown(show && row != _character.complications().count() - 1);
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
    ui->action_Save->setEnabled(_changed);
}
#endif

void Sheet::aboutToShowPowersAndEquipmentMenu() {
    const auto selection = Ui->powersandequipment->selectedItems();
    bool show = !selection.isEmpty();
    int row = -1;
    if (show) row = selection[0]->row();
#ifdef __wasm__
    bool canPaste = false;
    _powerMenuDialog->setEdit(show);
    _powerMenuDialog->setDelete(show);
    _powerMenuDialog->setCut(show);
    _powerMenuDialog->setCopy(show);
    _powerMenuDialog->setPaste(canPaste);
    _powerMenuDialog->setMoveUp(show && row != 0);
    _powerMenuDialog->setMoveDown(show && row != _character.complications().count() - 1);
#else
    Ui->editPowerOrEquipment->setEnabled(show);
    Ui->deletePowerOrEquipment->setEnabled(show);
    Ui->cutPowerOrEquipment->setEnabled(show);
    Ui->copyPowerOrEquipment->setEnabled(show);
    Ui->movePowerOrEquipmentUp->setEnabled(show && row != 0);
    auto power = getPower(row, _character.powersOrEquipment());
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
    _skillMenuDialog->setEdit(show);
    _skillMenuDialog->setDelete(show);
    _skillMenuDialog->setCut(show);
    _skillMenuDialog->setCopy(show);
    _skillMenuDialog->setPaste(canPaste);
    _skillMenuDialog->setMoveUp(show && row != 0);
    _skillMenuDialog->setMoveDown(show && row != _character.skillsTalentsOrPerks().count() - 1);
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
    Ui->moveSkillTalentOrPerkDown->setEnabled(show && row != _character.skillsTalentsOrPerks().count() - 1);
#endif
}

void Sheet::alternateIdsChanged(QString txt) {
    _character.alternateIds(txt);
    _changed = true;
}

void Sheet::campaignNameChanged(QString txt) {
    _character.campaignName(txt);
    _changed = true;
}

void Sheet::characterNameChanged(QString txt) {
    Ui->charactername2->setText(txt);
    _character.characterName(txt);
    _changed = true;
}

void Sheet::clearImage() {
    Ui->image->clear();
    _character.image() = "";
    _character.imageData().clear();
    _changed = true;
}

void Sheet::copyCharacter() {
    _character.notes() = Ui->notes->toPlainText();

    QJsonDocument doc = _character.copy(_option);
    QClipboard* clip = QGuiApplication::clipboard();
    gsl::owner<QMimeData*> data = new QMimeData();
    data->setData("application/complication", doc.toJson());
    QString text = getCharacter();
    data->setData("text/plain", text.toUtf8());
    clip->setMimeData(data);
}

void Sheet::copyComplication() {
    QClipboard* clip = QGuiApplication::clipboard();
    gsl::owner<QMimeData*> data = new QMimeData();
    auto selection = Ui->complications->selectedItems();
    int row = selection[0]->row();
    shared_ptr<Complication> complication = _character.complications()[row];
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
    gsl::owner<QMimeData*> data = new QMimeData();
    auto selection = Ui->powersandequipment->selectedItems();
    int row = selection[0]->row();
    shared_ptr<Power> power = getPower(row, _character.powersOrEquipment());
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
    gsl::owner<QMimeData*> data = new QMimeData();
    auto selection = Ui->skillstalentsandperks->selectedItems();
    int row = selection[0]->row();
    shared_ptr<SkillTalentOrPerk> skilltalentorperk = _character.skillsTalentsOrPerks()[row];
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
    closeDialogs(nullptr);
    _complicationsMenuDialog = make_shared<ComplicationsMenuDialog>();
    _complicationsMenuDialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    _complicationsMenuDialog->setPos(mapToGlobal(pos + Ui->complications->pos() - QPoint(0, ui->scrollArea->verticalScrollBar()->value())));
    aboutToShowComplicationsMenu();
    _complicationsMenuDialog->open();
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
    shared_ptr<Complication> complication = _character.complications().takeAt(row);
    if (complication == nullptr) return;

    _complicationPoints -= complication->points(Complication::NoStore);
    Ui->complications->removeRow(row);
    Ui->totalcomplicationpts->setText(QString("%1/%2").arg(_complicationPoints.points).arg(_option.complications().points));
    updateDisplay();
    _changed = true;
}

void Sheet::deletePowerOrEquipment() {
    auto selection = Ui->powersandequipment->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    auto power = getPower(row, _character.powersOrEquipment());
    if (power == nullptr) return;

    delPower(row);
    _powersOrEquipmentPoints -= power->points(Power::NoStore);
    updateDisplay();
    _changed = true;
}

void Sheet::deleteSkillstalentsandperks() {
    auto selection = Ui->skillstalentsandperks->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    shared_ptr<SkillTalentOrPerk> skilltalentorperk = _character.skillsTalentsOrPerks().takeAt(row);
    if (skilltalentorperk == nullptr) return;

    _skillsTalentsOrPerksPoints -= skilltalentorperk->points(Complication::NoStore);

    updateSkills(skilltalentorperk);

    Ui->skillstalentsandperks->removeRow(row);
    Ui->totalskillstalentsandperkscost->setText(QString("%1").arg(_skillsTalentsOrPerksPoints.points));
    updateDisplay();
    _changed = true;
}

void Sheet::doneEditComplication() {
    shared_ptr<Complication> complication = _compDlg->complication();
    if (complication->description().isEmpty()) return;

    updateDisplay();
    _changed = true;
}

void Sheet::doneEditSkill() {
    shared_ptr<SkillTalentOrPerk> skilltalentorperk = _skillDlg->skilltalentorperk();
    if (skilltalentorperk->description().isEmpty()) return;

    updateDisplay();
    _changed = true;
}

void Sheet::editComplication() {
    auto selection = Ui->complications->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    shared_ptr<Complication> complication = _character.complications()[row];
    if (complication == nullptr) return;

    _compDlg = make_shared<ComplicationsDialog>(this);
    _compDlg->complication(complication);
    connect(_compDlg.get(), SIGNAL(accepted()), this, SLOT(doneEditComplication()));

#ifdef __wasm__
    closeDialogs(nullptr);
#endif
    _compDlg->open();
}

void Sheet::editPowerOrEquipment() {
    auto selection = Ui->powersandequipment->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    shared_ptr<Power>& power = getPower(row, _character.powersOrEquipment());
    if (power == nullptr) return;

    QJsonObject json = power->toJson();
    shared_ptr<Power> work = Power::FromJson(json["name"].toString(), json);
    if (work == nullptr) return;

    work->parent(power->parent());
    _powerDlg = make_shared<PowerDialog>(this, power);
    _powerDlg->powerorequipment(work);
    _powerDlg->open();
}

void Sheet::editSkillstalentsandperks() {
    auto selection = Ui->skillstalentsandperks->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    shared_ptr<SkillTalentOrPerk> skilltalentorperk = _character.skillsTalentsOrPerks()[row];
    if (skilltalentorperk == nullptr) return;

    _skillDlg = make_shared<SkillDialog>(this);
    _skillDlg->skilltalentorperk(skilltalentorperk);
    connect(_skillDlg.get(), SIGNAL(accepted()), this, SLOT(doneEditSkill()));

#ifdef __wasm__
    closeDialogs(nullptr);
#endif
    _skillDlg->open();
}

void Sheet::eyeColorChanged(QString txt) {
    _character.eyeColor(txt);
    _changed = true;
}

#ifdef __wasm__
void Sheet::editMenu() {
    closeDialogs(nullptr);
    _editMenuDialog = make_shared<EditMenuDialog>();
    _editMenuDialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    _editMenuDialog->open();
}

void Sheet::fileMenu() {
    closeDialogs(nullptr);
    _fileMenuDialog = make_shared<FileMenuDialog>();
    _fileMenuDialog->setSave(_changed);
    _fileMenuDialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    _fileMenuDialog->open();
}
#endif

void Sheet::gamemasterChanged(QString txt) {
    _character.gamemaster(txt);
    _changed = true;
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
    _character.genre(txt);
    _changed = true;
}

void Sheet::hairColorChanged(QString txt) {
    _character.hairColor(txt);
    _changed = true;
}

void Sheet::imageMenu(QPoint pos) {
#ifdef __wasm__
    closeDialogs(nullptr);
    _imgMenuDialog = make_shared<ImgMenuDialog>();
    _imgMenuDialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    _imgMenuDialog->setPos(mapToGlobal(pos + Ui->image->pos() - QPoint(0, ui->scrollArea->verticalScrollBar()->value())));
    _imgMenuDialog->open();
#else
    Ui->imageMenu->exec(mapToGlobal(pos + Ui->image->pos() - QPoint(0, ui->scrollArea->verticalScrollBar()->value())));
#endif
}

void Sheet::moveComplicationDown() {
    auto selection = Ui->complications->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    auto& complications = _character.complications();
    shared_ptr<Complication> complication = complications.takeAt(row);
    complications.insert(row + 1, complication);
    updateComplications();
}

void Sheet::moveComplicationUp() {
    auto selection = Ui->complications->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    auto& complications = _character.complications();
    shared_ptr<Complication> complication = complications.takeAt(row);
    complications.insert(row - 1, complication);
    updateComplications();
}

void Sheet::movePowerOrEquipmentDown() {
    auto selection = Ui->powersandequipment->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    auto& powers = _character.powersOrEquipment();
    auto power = getPower(row, powers);
    delPower(row);
    putPower(row + 2, power);
    updateDisplay();
}

void Sheet::movePowerOrEquipmentUp() {
    auto selection = Ui->powersandequipment->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    auto& powers = _character.powersOrEquipment();
    auto power = getPower(row, powers);
    delPower(row);
    putPower(row - 1, power);
    updateDisplay();
}

void Sheet::moveSkillTalentOrPerkDown() {
    auto selection = Ui->skillstalentsandperks->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    auto& stps = _character.skillsTalentsOrPerks();
    auto stp = stps.takeAt(row);
    stps.insert(row + 1, stp);
    updateSkillsTalentsAndPerks();
}

void Sheet::moveSkillTalentOrPerkUp() {
    auto selection = Ui->skillstalentsandperks->selectedItems();
    if (selection.count() == 0) return;
    int row = selection[0]->row();
    auto& skillstalentsorperks = _character.skillsTalentsOrPerks();
    shared_ptr<SkillTalentOrPerk> skilltalentorperk = skillstalentsorperks.takeAt(row);
    skillstalentsorperks.insert(row - 1, skilltalentorperk);
    updateSkillsTalentsAndPerks();
}

void Sheet::erase() {
    _character.erase();
    Ui->notes->setPlainText("");
    updateDisplay();
    _changed = false;
}

void Sheet::saveThenErase() {
    try { save(); } catch(...) { return; }
    if (_changed) return;
    erase();
}

void Sheet::newchar() {
    if (_changed) {
        YesNoCancel("Do you want to save your changes first?",
                    std::bind(&Sheet::saveThenErase, this),
                    std::bind(&Sheet::erase, this),
                    std::bind(&Sheet::doNothing, this),
                    "The current sheet has been changed!");
    } else erase();
}

void Sheet::acceptComplication() {
    shared_ptr<Complication> complication = _compDlg->complication();
    if (complication == nullptr) return;
    if (complication->description().isEmpty()) return;

    _character.complications().append(complication);

    updateDisplay();
    _changed = true;
}

void Sheet::newComplication() {
    _compDlg = make_shared<ComplicationsDialog>(this);
    connect(_compDlg.get(), SIGNAL(accepted()), this, SLOT(acceptComplication()));

#ifdef __wasm__
    closeDialogs(nullptr);
#endif
    _compDlg->open();
}

void Sheet::newImage() {
#ifdef __wasm__
    QFileDialog::getOpenFileContent("Images (*.png *.xpm *jpg)", [&](const QString &fileName, const QByteArray &fileContent) {
        loadImage(fileContent, fileName);
    });
#else
    QString filename = QFileDialog::getOpenFileName(this, "New Image", _dir, "Images (*.png *.xpm *jpg)");
    if (filename.isEmpty()) return;
    loadImage(filename);
#endif
}

void Sheet::newPowerOrEquipment() {
    bool framework = false;
    auto selection = Ui->powersandequipment->selectedItems();
    if (!selection.isEmpty()) {
        shared_ptr<Power> work = getPower(selection[0]->row(), _character.powersOrEquipment());
        if (work == nullptr) framework = false;
        else if (work->isFramework()) framework = work->isMultipower();
        else {
            auto parent = work->parent();
            framework = parent != nullptr && parent->isMultipower();
        }
    }

    _powerDlg = make_shared<PowerDialog>(this);
    if (framework) _powerDlg->multipower();
    _powerDlg->open();
}

void Sheet::acceptNewSkill() {
    shared_ptr<SkillTalentOrPerk> skilltalentorperk = _skillDlg->skilltalentorperk();
    if (skilltalentorperk == nullptr) return;
    if (skilltalentorperk->description().isEmpty()) return;

    _character.skillsTalentsOrPerks().append(skilltalentorperk);

    updateDisplay();
    _changed = true;
}

void Sheet::newSkillTalentOrPerk() {
    _skillDlg = make_shared<SkillDialog>(this);
    connect(_skillDlg.get(), SIGNAL(accepted()), this, SLOT(acceptNewSkill()));
    _skillDlg->open();
}

void Sheet::noteChanged() {
    _changed = true;
}

void Sheet::doOpen() {
#ifdef __wasm__
    QFileDialog::getOpenFileContent("Characters (*.hsccu)", [&](const QString& fileName, const QByteArray& fileContent) {
        fileOpen(fileContent, fileName);
    });
#else
    QString filename = QFileDialog::getOpenFileName(this, "Open File", _dir, "Characters (*.hsccu)");
    if (filename.isEmpty()) return;
    _filename = filename;

    fileOpen();
#endif
}

void Sheet::saveThenOpen() {
    save();
    if (_changed) return;
    doOpen();
}

void Sheet::open() {
    if (_changed) {
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
    _optionDlg = make_shared<optionDialog>();
    _optionDlg->setBanner(_option.banner());
    _optionDlg->setComplications(_option.complications().points);
    _optionDlg->setShowFrequencyRolls(_option.showFrequencyRolls());
    _optionDlg->setShowNotesPage(_option.showNotesPage());
    _optionDlg->setNormalHumanMaxima(_option.normalHumanMaxima());
    _optionDlg->setActivePointsPerEND(_option.activePerEND().points);
    _optionDlg->setEquipmentFree(_option.equipmentFree());
    _optionDlg->setTotalPoints(_option.totalPoints().points);
    _optionDlg->open();
}

void Sheet::paste() {
    QClipboard* clip = QGuiApplication::clipboard();
    const QMimeData* data = clip->mimeData();
    QByteArray byteArray = data->data("application/hsccucharacter");
    QString jsonStr(byteArray);
    if (byteArray.isEmpty()) return;
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
    _character.erase();
    _character.paste(_option, doc);
    updateDisplay();
    _changed = true;
}

void Sheet::saveThenPaste() {
    try { save(); } catch(...) { return; }
    if (_changed) return;
    paste();
}

void Sheet::pasteCharacter() {
    if (_changed) {
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
    _character.complications().append(complication);

    int row = Ui->complications->rowCount();
    QFont font = Ui->complications->font();
    setCell(Ui->complications, row, 0, QString("%1").arg(complication->points(Complication::NoStore).points), font);
    setCell(Ui->complications, row, 1, complication->description(),                                           font, WordWrap);
    Ui->complications->resizeRowsToContents();

    _complicationPoints += complication->points(Complication::NoStore);
    Ui->totalcomplicationpts->setText(QString("%1/%2").arg(_complicationPoints.points).arg(_option.complications().points));
    updateDisplay();
    _changed = true;
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
    _character.skillsTalentsOrPerks().append(stp);

    int row = Ui->skillstalentsandperks->rowCount();
    QFont font = Ui->skillstalentsandperks->font();
    setCell(Ui->skillstalentsandperks, row, 0, QString("%1").arg(stp->points(Complication::NoStore).points), font);
    setCell(Ui->skillstalentsandperks, row, 1, stp->description(), font, WordWrap);
    setCell(Ui->skillstalentsandperks, row, 2, stp->roll(), font);
    Ui->skillstalentsandperks->resizeRowsToContents();

    updateSkills(stp);

    _skillsTalentsOrPerksPoints += stp->points(Complication::NoStore);
    Ui->totalskillstalentsandperkscost->setText(QString("%1").arg(_skillsTalentsOrPerksPoints.points));
    updateDisplay();
    _changed = true;
}

void Sheet::playerNameChanged(QString txt) {
    _character.playerName(txt);
    _changed = true;
}

void Sheet::powersandequipmentMenu(QPoint pos) {
#ifdef __wasm__
    closeDialogs(nullptr);
    _powerMenuDialog = make_shared<PowerMenuDialog>();
    _powerMenuDialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    _powerMenuDialog->setPos(mapToGlobal(pos + Ui->powersandequipment->pos() - QPoint(0, ui->scrollArea->verticalScrollBar()->value())));
    aboutToShowPowersAndEquipmentMenu();
    _powerMenuDialog->open();
#else
    Ui->powersandequipmentMenu->exec(mapToGlobal(pos
                                                 + Ui->powersandequipment->pos()
                                                 - QPoint(0, ui->scrollArea->verticalScrollBar()->value())));
#endif
}

void Sheet::print() {
    bool saveChanged = _changed;

    if (printer == nullptr) printer = new QPrinter(QPrinter::HighResolution);

    QPrintPreviewDialog preview(printer, this);
    preview.setWindowTitle("Print Chracter");
    preview.setMinimumHeight(600); // NOLINT
    preview.setMinimumWidth(800); // NOLINT
    this->connect(&preview, SIGNAL(paintRequested(QPrinter*)), this, SLOT(printCharacter(QPrinter*)));

    preview.exec();

    _changed = saveChanged; // lots of changed signals get passed around but the character really didn't change
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
    for (const auto& widget: Ui->widgets) {
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
        for (const auto& widget: Ui->widgets) {
            if (widget == nullptr || widget->y() < 1250) continue; // NOLINT  skip things we can't render or are on the first page
            print(painter, offset, widget);
        }
        deletePagefull();
        ++page;
    }

    if (_option.showNotesPage()) {
        QString notes = Ui->notes->toPlainText();
        int max = getPageCount(Ui->notes, scale, &painter);
        offset = QPoint({ 50, 48 }); // NOLINT
        int page = 0;
        while (page < max) {
            printer->newPage();
            painter.drawImage(QPointF { -50.0, -48.0 }, page3.toImage()); // NOLINT
            for (const auto& widget: Ui->hiddenWidgets) {
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
    if (!_changed) return;
    _character.notes() = Ui->notes->toPlainText();

#ifndef __wasm__
    if (_filename.isEmpty()) {
        QString oldname = _filename;
        _filename = Ui->charactername->text();
        saveAs();
        if (_filename.isEmpty()) _filename = oldname;
        return;
    }

    if (!_character.store(_option, _dir + "/" + _filename))
        OK("Can't save to \"" + _filename + ".hsccu\" in the \"" + _dir + "\" folder.", std::bind(&Sheet::doNothing, this));
    else _changed = false;
#else
    if (_filename.isEmpty()) _filename = Ui->charactername->text();
    if (!_character.store(_option, _filename)) {
        OK("Can't save to \"" + _filename + ".hsccu\".", std::bind(&Sheet::doNothing, this));
        throw "";
    }
    else _changed = false;
#endif
}

void Sheet::saveAs() {
    QString oldname = _filename;
    _filename = QFileDialog::getSaveFileName(this, "Save File", _dir, "Characters (*.hsccu)");
    if (_filename.isEmpty()) return;

    auto ext = _filename.lastIndexOf(".hsccu");
    if (ext != -1) _filename = _filename.left(ext);

    auto sep = _filename.lastIndexOf("/");
    if (sep != -1) {
        _dir = _filename.left(sep);
        _filename = _filename.mid(sep + 1);
    }
    try { save(); } catch (...) { _filename = oldname; }
}

void Sheet::skillstalentsandperksMenu(QPoint pos) {
#ifdef __wasm__
    closeDialogs(nullptr);
    _skillMenuDialog = make_shared<SkillMenuDialog>();
    _skillMenuDialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    _skillMenuDialog->setPos(mapToGlobal(pos + Ui->skillstalentsandperks->pos() - QPoint(0, ui->scrollArea->verticalScrollBar()->value())));
    aboutToShowSkillsPerksAndTalentsMenu();
    _skillMenuDialog->open();
#else
    Ui->skillstalentsandperksMenu->exec(mapToGlobal(pos + Ui->skillstalentsandperks->pos() - QPoint(0, ui->scrollArea->verticalScrollBar()->value())));
#endif
}

void Sheet::totalExperienceEarnedChanged(QString txt) {
    if (numeric(txt) || txt.isEmpty()) {
        _character.xp(Points(txt.toInt()));

        Points total = _option.totalPoints() - _option.complications() + _character.xp();
        if (_option.complications() < _complicationPoints) total += _option.complications();
        else total += _complicationPoints;
        Points remaining(0_cp);
        if (total > _totalPoints) remaining = total - _totalPoints;
        Points spent(0_cp);
        if (_totalPoints > _option.totalPoints()) spent = _totalPoints - _option.totalPoints();

        Ui->experiencespent->setText(QString("%1").arg(spent.points));
        Ui->experienceunspent->setText(QString("%1").arg(remaining.points));
        _changed = true;
    } else Ui->totalexperienceearned->undo();
}

void Sheet::totalExperienceEarnedEditingFinished() {
    totalExperienceEarnedChanged(Ui->totalexperienceearned->text());
    if (Ui->totalexperienceearned->text().isEmpty()) Ui->totalexperienceearned->setText("0");
}
