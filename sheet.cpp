#include "complicationsdialog.h"
#include "complication.h"
#include "modifier.h"
#include "optiondialog.h"
#include "powers.h"
#include "powerdialog.h"
#include "skilldialog.h"
#include "skilltalentorperk.h"

#include "sheet.h"
#include "ui_sheet.h"
#include "sheet_ui.h"

#ifdef _WIN64
#include <Shlobj.h>
#endif
#include <cmath>

#include <QBuffer>
#include <QClipboard>
#include <QFileDialog>
#include <QFontDatabase>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QMimeData>
#include <QMouseEvent>
#include <QPageSetupDialog>
#include <QPainter>
#include <QPrintDialog>
#include <QScrollBar>
#include <QSettings>
#include <QStandardPaths>

Sheet* Sheet::_sheet = nullptr;

// --- [static functions] ----------------------------------------------------------------------------------
static bool numeric(QString txt) {
    bool ok;
    txt.toInt(&ok, 10);
    return ok;
}

int YesNo(const QString msg, const QString title = "") {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText(!title.isEmpty() ? title : "Are you sure?");
    msgBox.setInformativeText(msg);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    return msgBox.exec();
}

int YesNo(const char* msg, const char* title) {
    return YesNo(QString(msg), QString(title ? "" : nullptr));
}

int YesNoCancel(const char* msg, const char* title) {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText(title ? title : "Are you really sure?");
    msgBox.setInformativeText(msg);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    return msgBox.exec();
}

int OK(const QString msg, const QString title = "") {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText(!title.isEmpty() ? title : "Something has happened.");
    msgBox.setInformativeText(msg);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    return msgBox.exec();
}

int OK(const char* msg, const char* title = nullptr) {
    return OK(QString(msg), QString(title ? "" : nullptr));
}

int OKCancel(const QString msg, const QString title = "") {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText(!title.isEmpty() ? title : "Something bad is about to happened.");
    msgBox.setInformativeText(msg);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    return msgBox.exec();
}

int OKCancel(const char* msg, const char* title = nullptr) {
    return OKCancel(QString(msg), QString(title ? "" : nullptr));
}

int Question(const QString msg, const QString title = "", QFlags<QMessageBox::StandardButton> buttons = { QMessageBox::Yes, QMessageBox::No }) {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText(!title.isEmpty() ? title : "Are you sure?");
    msgBox.setInformativeText(msg);
    msgBox.setStandardButtons(buttons);
    return msgBox.exec();
}

int Question(const char* msg, const char* title = nullptr, QFlags<QMessageBox::StandardButton> buttons = { QMessageBox::Yes, QMessageBox::No }) {
    return Question(QString(msg), QString(title ? title : ""), buttons);
}

int Statement(const QString msg) {
    QMessageBox msgBox;
    msgBox.setInformativeText(msg);
    msgBox.setStandardButtons(QMessageBox::Ok);
    return msgBox.exec();
}

int Statement(const char* msg) {
    return Statement(QString(msg));
}

static class lift {
public:
    long STR;
    long lift;
    long toss;
    QString suffix;
} strTable[] {
    { 0,          0,   0, "kg" },
    { 1,          8,   2, "kg" },
    { 2,         16,   3, "kg" },
    { 3,         25,   4, "kg" },
    { 4,         38,   6, "kg" },
    { 5,         50,   8, "kg" },
    { 8,         75,  12, "kg" },
    { 10,       100,  16, "kg" },
    { 13,       150,  20, "kg" },
    { 15,       200,  24, "kg" },
    { 18,       300,  28, "kg" },
    { 20,       400,  32, "kg" },
    { 23,       600,  36, "kg" },
    { 25,       800,  40, "kg" },
    { 28,      1200,  44, "kg" },
    { 30,      1600,  48, "kg" },
    { 35,      3200,  56, "kg" },
    { 40,      6400,  64, "kg" },
    { 45,     12500,  72, "tons" },
    { 50,     25000,  80, "tons" },
    { 55,     50000,  88, "tons" },
    { 60,    100000,  96, "tons" },
    { 65,    200000, 104, "tons" },
    { 70,    400000, 112, "tons" },
    { 75,    800000, 120, "tons" },
    { 80,   1600000, 128, "ktons" },
    { 85,   3200000, 136, "ktons" },
    { 90,   6400000, 144, "ktons" },
    { 95,  12500000, 152, "ktons" },
    { 100, 25000000, 160, "ktons" }
};

// A is value at 0
// B is value at T
// t is distance from A
// T is distance from A to B
static qlonglong interpolate(int A, int B, int t, int T) {
    double val = (double) A * std::pow((double) B / (double) A, (double) t / (double) T);
    return (qlonglong) (val + 0.5);
}

static int indexOf(int str) {
    int i;
    for (i = 0; strTable[i].STR != 100 && strTable[i].STR < str; ++i)
        ;
    return (strTable[i].STR != 100) ? i : -1;
}

static qlonglong interpolateLift(int str) {
    if (str > 95) {
        int idx = indexOf(95);
        return interpolate(strTable[idx].lift, strTable[idx + 1].lift, str - 95, 5);
    }
    int idx = indexOf(str);
    return interpolate(strTable[idx].lift, strTable[idx + 1].lift, str - strTable[idx].STR, strTable[idx + 1].STR - strTable[idx].STR);
}

static QString liftUnits(int str) {
    if (str > 95) return "ktons";
    return strTable[indexOf(str)].suffix;
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

Sheet_UI Sheet::_Sheet_UI;

Sheet::Sheet(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Sheet)
    , Ui(&_Sheet_UI)
{
    _sheet = this;

    ui->setupUi(this);
    Ui->setupUi(ui->label);
    setupIcons();
    setUnifiedTitleAndToolBarOnMac(true);

    Modifiers mods;

    printer = new QPrinter(QPrinter::HighResolution);

    _dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    _option.load();

    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(focusChanged(QWidget*,QWidget*)));

    connect(ui->menu_File,         SIGNAL(aboutToShow()), this, SLOT(aboutToShowFileMenu()));
    connect(ui->menu_File,         SIGNAL(aboutToHide()), this, SLOT(aboutToHideFileMenu()));
    connect(ui->action_New,        SIGNAL(triggered()),   this, SLOT(newchar()));
    connect(ui->action_Open,       SIGNAL(triggered()),   this, SLOT(open()));
    connect(ui->action_Save,       SIGNAL(triggered()),   this, SLOT(save()));
    connect(ui->actionSave_As,     SIGNAL(triggered()),   this, SLOT(saveAs()));
    connect(ui->action_Print,      SIGNAL(triggered()),   this, SLOT(print()));
    connect(ui->actionPa_ge_Setup, SIGNAL(triggered()),   this, SLOT(pageSetup()));
    connect(ui->actionE_xit,       SIGNAL(triggered()),   this, SLOT(exitClicked()));

    connect(ui->menu_Edit,     SIGNAL(aboutToShow()), this, SLOT(aboutToShowEditMenu()));
    connect(ui->menu_Edit,     SIGNAL(aboutToHide()), this, SLOT(aboutToHideEditMenu()));
    connect(ui->action_Cut,    SIGNAL(triggered()),   this, SLOT(cutCharacter()));
    connect(ui->actionC_opy,   SIGNAL(triggered()),   this, SLOT(copyCharacter()));
    connect(ui->action_Paste,  SIGNAL(triggered()),   this, SLOT(pasteCharacter()));
    connect(ui->actionOptions, SIGNAL(triggered()),   this, SLOT(options()));

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

    connect(Ui->image,      SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(imageMenu(QPoint)));
    connect(Ui->newImage,   SIGNAL(triggered()),                        this, SLOT(newImage()));
    connect(Ui->clearImage, SIGNAL(triggered()),                        this, SLOT(clearImage()));

    connect(Ui->complications,        SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(complicationDoubleClicked(QTableWidgetItem*)));
    connect(Ui->complications,        SIGNAL(customContextMenuRequested(QPoint)),   this, SLOT(complicationsMenu(QPoint)));
    connect(Ui->complicationsMenu,    SIGNAL(aboutToShow()),                        this, SLOT(aboutToShowComplicationsMenu()));
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
    connect(Ui->skillstalentsandperksMenu, SIGNAL(aboutToShow()),                        this, SLOT(aboutToShowSkillsPerksAndTalentsMenu()));
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
    connect(Ui->powersandequipmentMenu,   SIGNAL(aboutToShow()),                        this, SLOT(aboutToShowPowersAndEquipmentMenu()));
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

    installEventFilter(this);
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

void Sheet::closeEvent(QCloseEvent* event) {
    if (checkClose()) event->accept();
    else event->ignore();
}

// --- [WORK] -------------------------------------------------------------------------------------------

QList<QList<int>> phases {
    { },
    { 7 },
    { 6, 12 },
    { 4, 8, 12 },
    { 3, 6, 9, 12 },
    { 3, 5, 8, 10, 12 },
    { 2, 4, 6, 8, 10, 12 },
    { 2, 4, 6, 7, 9, 11, 12 },
    { 2, 3, 5, 6, 8, 9, 11, 12 },
    { 2, 3, 4, 6, 7, 8, 10, 11, 12 },
    { 2, 3, 4, 5, 6, 8, 9, 10, 11, 12 },
    { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 },
    { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 }
};

void Sheet::addPower(shared_ptr<Power>& power) {
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
    updateTotals();
    _changed = true;
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
        if (val == Ui->spdval) {
            int primary = def.characteristic()->base() + def.characteristic()->primary();
            int secondary = primary + def.characteristic()->secondary();
            if (primary > 12 || secondary > 12) {
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

            int primary = def.characteristic()->base() + def.characteristic()->primary();
            int secondary = primary + def.characteristic()->secondary();
            QString end = QString("%1").arg((primary + 4) / 10);
            if (primary != secondary) end += QString("/%1").arg((secondary + 4) / 10);
            Ui->strendcost->setText(end);

            QString lift = formatLift(primary);
            if (primary != secondary) lift += "/" + formatLift(secondary);
            Ui->lift->setText(lift);
        } else if (val == Ui->spdval) {
            if (def.characteristic()->base() < 1) return;
            int primary = def.characteristic()->base() + def.characteristic()->primary();
            int secondary = primary + def.characteristic()->secondary();
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
          else if (val == Ui->pdval) setDefense(def, 0, 1, val);
          else if (val == Ui->edval) setDefense(def, 2, 1, val);
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

bool Sheet::checkClose() {
    if (_changed) {
        try {
            switch (YesNoCancel("Do you want to save your changes first?", "The current character has been changed!")) {
            case QMessageBox::Yes: save(); break;
            case QMessageBox::No:  return true;
            default:               return false;
            }
        } catch (...) { return false; }
        if (_changed) return false;
    }
    return true;
}

void Sheet::delPower(int row) {
    auto power = getPower(row, _character.powersOrEquipment());
    if (power == nullptr) return;

    if (power->parent() == nullptr) {
        int realRow = _character.powersOrEquipment().indexOf(power);
        _character.powersOrEquipment().removeAt(realRow);
    } else {
        auto& list = power->parent()->list();
        int realRow = list.indexOf(power);
        list.removeAt(realRow);
    }
}

int Sheet::displayPowerAndEquipment(int& row, shared_ptr<Power> pe) {
    if (pe == nullptr) return 0;

    QFont font = Ui->powersandequipment->font();
    QFont italic = font;
    italic.setItalic(true);
    QString descr = pe->description(false);
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
    if ((!pe->isFramework() || pe->isVPP() || pe->isMultipower()) && !descr.isEmpty() && pts.toInt() == 0) pts = Fraction(1);
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

QString Sheet::formatLift(int str) {
    int lift = interpolateLift(str);
    QString units = liftUnits(str);
    if (units == "kg") return QString("%1kg").arg(formatNumber(lift));
    QString num;
    if (units == "tons") num = QString("%1").arg(formatNumber(lift / 1000.0));
    else num = QString("%1").arg(formatNumber(lift / 1000000.0));
    if (num.right(2) == ".0") num = num.left(num.length() - 2);
    return num + " " + units;
}

QString Sheet::getCharacter() {
    QString out;
    out += _character.characterName();
    if (!_character.alternateIds().isEmpty()) out += " (" + _character.alternateIds() + ")\n\n";

    QStringList names { "STR", "DEX", "CON", "INT", "EGO", "PRE", "OCV", "DCV", "OMCV", "DMCV", "SPD", "PD", "ED", "REC", "END", "BODY", "STUN" };
    for (int i = 0; i < 17; ++i) {
        auto characteristic = _character.characteristic(i);
        int primary = characteristic.base() + characteristic.primary();
        int secondary = primary + characteristic.secondary();
        if (primary != secondary) out += QString("%1/%2\t").arg(primary).arg(secondary);
        else out += QString("%1\t").arg(primary);
        out += names[i] + QString("\t%1\t").arg(characteristic.points().points);
        if (i < 6) out += characteristic.roll();
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
    out += QString("%1\tExperience Unspent\n").arg(Ui->experienceunspent->text());
    return out;
}

shared_ptr<Power>& Sheet::getPower(int row, QList<shared_ptr<Power>>& in) {
    static shared_ptr<Power> null = nullptr;

    for (auto& power: in) {
        if (power == nullptr) continue;

        if (power->row() == row) return power;
        if (power->isFramework()) {
            auto& p = getPower(row, power->list());
            if (p != null) return p;
        }
    }
    return null;
}

void Sheet::putPower(int row, shared_ptr<Power> power) {
    if (power == nullptr) return;

    shared_ptr<Power> after = getPower(row, _character.powersOrEquipment());
    if (after == nullptr) {
        power->parent(nullptr);
        _character.powersOrEquipment().append(power);
    } else if (after->isFramework()) {
        if (power->parent() == after.get()) {
            int realRow = _character.powersOrEquipment().indexOf(after);
            power->parent(nullptr);
            _character.powersOrEquipment().insert(realRow, power);
        } else after->insert(0, power);
    } else {
        auto parent = after->parent();
        if (parent == nullptr) {
            int realRow = _character.powersOrEquipment().indexOf(after);
            if (power->parent() == nullptr) _character.powersOrEquipment().insert(realRow, power);
            else {
                power->parent(nullptr);
                _character.powersOrEquipment().insert(--realRow, power);
            }
        } else {
            int realRow = parent->list().indexOf(after);
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
                _character.PD().primary(_character.PD().primary() + power->rPD() + power->PD());
                _character.ED().primary(_character.ED().primary() + power->rED() + power->ED());
                _character.rPD() = _character.rPD() + power->rPD();
                _character.rED() = _character.rED() + power->rED();
            } else {
                _character.PD().secondary(_character.PD().primary() + power->rPD() + power->PD());
                _character.ED().secondary(_character.ED().primary() + power->rED() + power->ED());
                _character.temprPD() = _character.temprPD() + power->rPD();
                _character.temprED() = _character.temprED() + power->rED();
            }
        } else if (power->name() == "Barrier") {
            if (power->place() == 2) {
                _character.tempPD() += power->rPD() + power->PD();
                _character.tempED() += power->rED() + power->ED();
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
                _character.PD().primary(_character.PD().primary() + power->rPD() + power->PD());
                _character.ED().primary(_character.ED().primary() + power->rED() + power->ED());
                _character.rPD() = _character.rPD() + power->rPD();
                _character.rED() = _character.rED() + power->rED();
            } else if (power->place() == 2) {
                _character.PD().secondary(_character.PD().primary() + power->rPD() + power->PD());
                _character.ED().secondary(_character.ED().primary() + power->rED() + power->ED());
                _character.temprPD() = _character.temprPD() + power->rPD();
                _character.temprED() = _character.temprED() + power->rED();
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
            for (int i = 0; i < 17; ++i) {
                if (put == 1) _character.characteristic(i).primary(_character.characteristic(i).primary() + power->characteristic(i));
                else _character.characteristic(i).secondary(_character.characteristic(i).secondary() + power->characteristic(i));
            }
            if (power->hasModifier("Resistant")) {
                _character.rPD() += power->characteristic(11);
                _character.rED() += power->characteristic(12);
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

    for (int i = 0; i < 17; ++i) {
        _character.characteristic(i).primary(0);
        _character.characteristic(i).secondary(0);
    }

    for (int i = 0; i < 17; ++i) {
        int base = _character.characteristic(i).base();
        characteristicWidgets[i]->setText(QString("%1").arg(base));
        characteristicEditingFinished(characteristicWidgets[i]);
    }

    rebuildDefenses();
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
            _character.rPD() += skill->rPD();
            _character.rED() += skill->rED();
        }
    }

    rebuildCharFromPowers(_character.powersOrEquipment());

    int primPD = _character.PD().base() + _character.PD().primary();
    int secondPD = _character.PD().secondary();
    int primED = _character.ED().base() + _character.ED().primary();
    int secondED = _character.ED().secondary();

    setDefense(primPD,            secondPD + _character.tempPD(), 0, 1);
    setDefense(_character.rPD(),  _character.temprPD(),           1, 1);
    setDefense(primED,            secondED + _character.tempPD(), 2, 1);
    setDefense(_character.rED(),  _character.temprED(),           3, 1);
    setDefense(_character.MD(),   0,                              4, 1);
    setDefense(_character.PowD(), 0,                              5, 1);
    setDefense(_character.FD(),   0,                              6, 1);
}

void Sheet::rebuildMartialArt(shared_ptr<SkillTalentOrPerk> stp, QFont& font) {
    static QMap<QString, QStringList> table = {
        { "Choke Hold",       { "½", "-2", "+0", "Grab 1 limb, 2d6 NND" } },
        { "Defensive Strike", { "½", "+1", "+3", "STR strike" } },
        { "Killing Strike",   { "½", "-2", "+0", "HKA ½d6" } },
        { "Legsweep",         { "½", "+2", "-1", "STR+1d6, target falls" } },
        { "Martial Block",    { "½", "+2", "+2", "Block, abort" } },
        { "Martial Disarm",   { "½", "-1", "-1", "Disarm, +10 STR" } },
        { "Martial Dodge",    { "½", "——", "+5", "Dodge, abort" } },
        { "Martial Escape",   { "½", "+0", "+0", "+15 STR vs. Grabs" } },
        { "Martial Grab",     { "½", "-1", "-1", "Grab 2 Limbs, +10 STR" } },
        { "Martial Strike",   { "½", "+0", "+2", "STR+2d6" } },
        { "Martial Throw",    { "½", "+0", "+1", "STR+v/10, target falls" } },
        { "Nerve Strike",     { "½", "-1", "+1", "2d6 NND" } },
        { "Offensive Strike", { "½", "-2", "+1", "STR+4d6" } },
        { "Passing Strike",   { "½", "+1", "+0", "STR+v/10, full move" } },
        { "Sacrifice Throw",  { "½", "+2", "+1", "STR, both fall" } }
    };

    auto* man = Ui->attacksandmaneuvers;
    QString d = stp->description().mid(stp->name().length() + 2);
    auto maneuvers = d.split(", ");
    for (const auto& m: maneuvers) {
        int size = man->rowCount();
        int row;
        for (row = 15; row < size; ++row) {
            const auto* cell = man->cellWidget(row, 0);
            if (static_cast<const QLabel*>(cell)->text() == m) break;
        }
        if (row != size) continue;
        setCellLabel(man, row, 0, m, font);
        for (int i = 0; i < 4; i++)  setCellLabel(man, row, i + 1, table[m][i], font);
    }
}

void Sheet::rebuildMartialArts() {
    auto* man = Ui->attacksandmaneuvers;
    QFont font = Ui->smallfont;
    man->setRowCount(15);
    man->update();

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
        }
    }
}

void Sheet::rebuildMovement() {
    _character.running()  = 12;
    _character.leaping()  = 4;
    _character.swimming() = 4;
    QMap<QString, int> movements;
    QMap<QString, QString> units;
    QMap<QString, int> doubles;

    rebuildMoveFromPowers(_character.powersOrEquipment(), movements, units, doubles);

    Ui->movement->setRowCount(4);
    Ui->movement->update();

    QString running = QString("%1m").arg(_character.running());
    QString ncRunning = QString("%1m").arg(2 * _character.running());
    QString swimming = QString("%1m").arg(_character.swimming());
    QString ncSwimming = QString("%1m").arg(2 * _character.swimming());
    QString hLeaping = QString("%1m").arg(_character.leaping());
    QString ncHLeaping = QString("%1m").arg(2 * _character.leaping());
    QString vLeaping = QString("%1m").arg((_character.leaping() + 1) / 2);
    QString ncVLeaping = QString("%1m").arg(2 * (_character.leaping() + 1) / 2);
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
        _character.DCV().cost(15_cp);
        _character.DMCV().cost(9_cp);
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
        _character.DCV().cost(5_cp);
        _character.DMCV().cost(3_cp);
        Ui->pdval->setToolTip("Physical Defense: 1 point");
        Ui->edval->setToolTip("Energy Defense: 1 point");
        Ui->dcvval->setToolTip("Defensive Combat Value: 5 points");
        Ui->dmcvval->setToolTip("Defensive Mental Combat Value: 3 points");
    }
}

void Sheet::rebuildSenseFromPowers(QList<shared_ptr<Power>>& list, QString& senses) {
    for (const auto& power: list) {
        if (power == nullptr) continue;

        if (power->name() == "Enhanced Senses") {
            const auto& split = power->description(false).split(":");
            senses += " <small>" + split[1] + "</small>";
        }
    }
}

void Sheet::rebuildSenses() {
    QString senses = "<b>Enhanced and Unusual Senses</b>";
    rebuildSenseFromPowers(_character.powersOrEquipment(), senses);
    Ui->enhancedandunusualsenses->setText(senses);
}

void Sheet::setupIcons() {
#ifdef _WIN64
    QSettings s("HKEY_CURRENT_USER\\SOFTWARE\\CLASSES", QSettings::NativeFormat);

    QString path = QDir::toNativeSeparators(qApp->applicationFilePath());
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
    QTableWidgetItem* lbl = new QTableWidgetItem(str);
    lbl->setFont(font);
    lbl->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
    lbl->setFlags(lbl->flags() & ~Qt::ItemIsEditable);
    if (row >= tbl->rowCount()) tbl->setRowCount(row + 1);
    tbl->setItem(row, col, lbl);
}

void Sheet::setCellLabel(QTableWidget* tbl, int row, int col, QString str, const QFont& font) {
    QLabel* lbl = new QLabel(str);
    lbl->setFont(font);
    if (row >= tbl->rowCount()) tbl->setRowCount(row + 1);
    tbl->setCellWidget(row, col, lbl);
}

void Sheet::setCellLabel(QTableWidget* tbl, int row, int col, QString str) {
    QLabel* lbl = new QLabel(str);
    if (row >= tbl->rowCount()) tbl->setRowCount(row + 1);
    QLabel* cell = static_cast<QLabel*>(tbl->cellWidget(row, col));
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
    setCellLabel(Ui->defenses, r, c, defense);
}

void Sheet::setDefense(int def, int temp, int r, int c) {
    QString defense = QString("%1").arg(def);
    if (temp != 0) defense += QString("/%1").arg(temp + def);
    setCellLabel(Ui->defenses, r, c, defense);
}

void Sheet::setMaximum(_CharacteristicDef& def, QLabel* set, QLineEdit* cur) {
    int primary = def.characteristic()->base() + def.characteristic()->primary();
    int secondary = primary + def.characteristic()->secondary();
    set->setText(QString("%1").arg(secondary));
    cur->setText(set->text());
}

void Sheet::updateCharacteristics() {
    const auto keys = _widget2Def.keys();
    for (const auto& key: keys) {
        auto& def = _widget2Def[key];
        QLineEdit* characteristic = dynamic_cast<QLineEdit*>(key);
        QString value = def.characteristic()->value();
        characteristic->setText(value);
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
}

void Sheet::updatePower(shared_ptr<Power> power) {
    if (power == nullptr) return;

    if (power->name() == "Skill") updateSkills(power->skill());
    else if (power->name() == "Density Increase") {
        rebuildCharacteristics();
        rebuildDefenses();
    } else if (power->name() == "Growth") {
        rebuildCharacteristics();
        rebuildMovement();
    } else if (power->name() == "Barrier" ||
               power->name() == "Flash Defense" ||
               power->name() == "Mental Defense" ||
               power->name() == "Power Defense" ||
               power->name() == "Resistant Defense") {
        rebuildCharacteristics();
        rebuildDefenses();
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
        rebuildDefenses();
    }
    updatePowersAndEquipment();
}

void Sheet::updatePowersAndEquipment(){
    Ui->powersandequipment->setRowCount(0);
    Ui->powersandequipment->update();

    _powersOrEquipmentPoints = 0_cp;
    int row = 0;
    for (const auto& pe: _character.powersOrEquipment()) {
        if (pe == nullptr) continue;

        _powersOrEquipmentPoints += Points(displayPowerAndEquipment(row, pe));
    }
    Ui->powersandequipment->resizeRowsToContents();

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

        if (stp->name() == "Jack Of All Trades")  _character.hasJackOfAllTrades() = true;
        else if (stp->name() == "Linguist")       _character.hasLinguist() = true;
        else if (stp->name() == "Scholar")        _character.hasScholar() = true;
        else if (stp->name() == "Scientist")      _character.hasScientist() = true;
        else if (stp->name() == "Traveler")       _character.hasTraveler() = true;
        else if (stp->name() == "Well-Connected") _character.hasWellConnected() = true;
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

static Points Min(Points a, Points& b) { return (a < b) ? a : b; }

void Sheet::updateTotals() {
    _totalPoints = characteristicsCost() + _skillsTalentsOrPerksPoints + _powersOrEquipmentPoints;
    Ui->totalpoints->setText(QString("%1/%2")
                             .arg(_totalPoints.points)
                             .arg((_option.totalPoints() - _option.complications()).points + Min(_option.complications(), _complicationPoints).points));
    totalExperienceEarnedEditingFinished();
}

QString Sheet::valueToDice(int value) {
    QString halfDice = "½";
    int dice = value / 5;
    bool half = value % 5 > 2;
    return QString("%1%2d6").arg(dice).arg(half ? halfDice : "");
}

// ---[SLOTS] --------------------------------------------------------------------------------------------

void Sheet::aboutToHideEditMenu() {
    ui->action_Paste->setEnabled(true);
}

void Sheet::aboutToHideFileMenu() {
    ui->action_Save->setEnabled(true);
}

void Sheet::aboutToShowComplicationsMenu() {
    const auto selection = Ui->complications->selectedItems();
    bool show = !selection.isEmpty();
    int row = -1;
    if (show) row = selection[0]->row();
    Ui->editComplication->setEnabled(show);
    Ui->deleteComplication->setEnabled(show);
    Ui->cutComplication->setEnabled(show);
    Ui->copyComplication->setEnabled(show);
    Ui->moveComplicationUp->setEnabled(show && row != 0);
    Ui->moveComplicationDown->setEnabled(show && row != _character.complications().count() - 1);
    QClipboard* clipboard = QGuiApplication::clipboard();
    const QMimeData* clip = clipboard->mimeData();
    bool canPaste = clip->hasFormat("application/complication");
    Ui->pasteComplication->setEnabled(canPaste);
}

void Sheet::aboutToShowEditMenu() {
    QClipboard* clipboard = QGuiApplication::clipboard();
    const QMimeData* clip = clipboard->mimeData();
    bool canPaste = clip->hasFormat("application/hsccuchar");
    ui->action_Paste->setEnabled(canPaste);
}

void Sheet::aboutToShowFileMenu() {
    ui->action_Save->setEnabled(_changed);
}

void Sheet::aboutToShowPowersAndEquipmentMenu() {
    const auto selection = Ui->powersandequipment->selectedItems();
    bool show = !selection.isEmpty();
    int row = -1;
    if (show) row = selection[0]->row();
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
}

void Sheet::aboutToShowSkillsPerksAndTalentsMenu() {
    const auto selection = Ui->skillstalentsandperks->selectedItems();
    bool show = !selection.isEmpty();
    int row = -1;
    if (show) row = selection[0]->row();
    Ui->editSkillTalentOrPerk->setEnabled(show);
    Ui->deleteSkillTalentOrPerk->setEnabled(show);
    Ui->cutSkillTalentOrPerk->setEnabled(show);
    Ui->copySkillTalentOrPerk->setEnabled(show);
    Ui->moveSkillTalentOrPerkUp->setEnabled(show && row != 0);
    Ui->moveSkillTalentOrPerkDown->setEnabled(show && row != _character.skillsTalentsOrPerks().count() - 1);
    QClipboard* clipboard = QGuiApplication::clipboard();
    const QMimeData* clip = clipboard->mimeData();
    bool canPaste = clip->hasFormat("application/skillperkortalent");
    Ui->pasteSkillTalentOrPerk->setEnabled(canPaste);
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
    QJsonDocument doc = _character.copy(_option);
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
    shared_ptr<Complication> complication = _character.complications()[row];
    if (complication == nullptr) return;

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
    shared_ptr<Power> power = getPower(row, _character.powersOrEquipment());
    if (power == nullptr) return;

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
    shared_ptr<SkillTalentOrPerk> skilltalentorperk = _character.skillsTalentsOrPerks()[row];
    if (skilltalentorperk == nullptr) return;

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
    Ui->complicationsMenu->exec(mapToGlobal(pos
                                            + Ui->complications->pos()
                                            - QPoint(0, ui->scrollArea->verticalScrollBar()->value())));
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
    auto selection = Ui->complications->selectedItems();
    int row = selection[0]->row();
    shared_ptr<Complication> complication = _character.complications().takeAt(row);
    if (complication == nullptr) return;

    _complicationPoints -= complication->points(Complication::NoStore);
    Ui->complications->removeRow(row);
    Ui->totalcomplicationpts->setText(QString("%1/%2").arg(_complicationPoints.points).arg(_option.complications().points));
    updateTotals();
    _changed = true;
}

void Sheet::deletePowerOrEquipment() {
    auto selection = Ui->powersandequipment->selectedItems();
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
    int row = selection[0]->row();
    shared_ptr<SkillTalentOrPerk> skilltalentorperk = _character.skillsTalentsOrPerks().takeAt(row);
    if (skilltalentorperk == nullptr) return;

    _skillsTalentsOrPerksPoints -= skilltalentorperk->points(Complication::NoStore);

    updateSkills(skilltalentorperk);

    Ui->skillstalentsandperks->removeRow(row);
    Ui->totalskillstalentsandperkscost->setText(QString("%1").arg(_skillsTalentsOrPerksPoints.points));
    updateTotals();
    _changed = true;
}

void Sheet::editComplication() {
    auto selection = Ui->complications->selectedItems();
    int row = selection[0]->row();
    shared_ptr<Complication> complication = _character.complications()[row];
    if (complication == nullptr) return;

    ComplicationsDialog dlg(this);
    dlg.complication(complication);
    auto old = complication->points();

    if (dlg.exec() == QDialog::Rejected) return;
    if (complication->description().isEmpty()) return;

    QFont font = Ui->complications->font();
    setCell(Ui->complications, row, 0, QString("%1").arg(complication->points().points), font);
    setCell(Ui->complications, row, 1, complication->description(),                      font, WordWrap);

    Ui->complications->resizeRowsToContents();
    _complicationPoints += complication->points() - old;
    Ui->totalcomplicationpts->setText(QString("%1/%2").arg(_complicationPoints.points).arg(_option.complications().points));
    updateTotals();
    _changed = true;
}

void Sheet::editPowerOrEquipment() {
    auto selection = Ui->powersandequipment->selectedItems();
    int row = selection[0]->row();
    shared_ptr<Power>& power = getPower(row, _character.powersOrEquipment());
    if (power == nullptr) return;

    QJsonObject json = power->toJson();
    shared_ptr<Power> work = Power::FromJson(json["name"].toString(), json);
    if (work == nullptr) return;

    work->parent(power->parent());
    PowerDialog dlg(this);
    dlg.powerorequipment(work);

    if (dlg.exec() == QDialog::Rejected) return;
    if (work->description().isEmpty()) return;
    if (power->parent() != nullptr) {
        Power* group = power->parent();
        if (!group->isValid(work)) return;
    }

    power = Power::FromJson(work->name(), work->toJson());
    if (power == nullptr) return;

    power->modifiers().clear();
    for (const auto& mod: power->advantagesList()) {
        if (mod == nullptr) continue;

        power->modifiers().append(mod);
    }
    for (const auto& mod: power->limitationsList()) {
        if (mod == nullptr) continue;

        power->modifiers().append(mod);
    }

    updateDisplay();
    _changed = true;
}

void Sheet::editSkillstalentsandperks() {
    auto selection = Ui->skillstalentsandperks->selectedItems();
    int row = selection[0]->row();
    shared_ptr<SkillTalentOrPerk> skilltalentorperk = _character.skillsTalentsOrPerks()[row];
    if (skilltalentorperk == nullptr) return;

    SkillDialog dlg(this);
    dlg.skilltalentorperk(skilltalentorperk);
    Points old = skilltalentorperk->points(Power::NoStore);

    if (dlg.exec() == QDialog::Rejected) return;
    if (skilltalentorperk->description().isEmpty()) return;

    QFont font = Ui->skillstalentsandperks->font();
    setCell(Ui->skillstalentsandperks, row, 0, QString("%1").arg(skilltalentorperk->points(Power::NoStore).points), font);
    setCell(Ui->skillstalentsandperks, row, 1, skilltalentorperk->description(),                                    font, WordWrap);
    setCell(Ui->skillstalentsandperks, row, 2, skilltalentorperk->roll(),                                           font);

    updateSkills(skilltalentorperk);

    Ui->skillstalentsandperks->resizeRowsToContents();
    _skillsTalentsOrPerksPoints += skilltalentorperk->points() - old;
    Ui->totalskillstalentsandperkscost->setText(QString("%1").arg(_skillsTalentsOrPerksPoints.points));
    updateTotals();
    _changed = true;
}

void Sheet::eyeColorChanged(QString txt) {
    _character.eyeColor(txt);
    _changed = true;
}

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
        focus == Ui->stunval) characteristicChanged(static_cast<QLineEdit*>(focus), "", DontUpdateTotal);
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
    Ui->imageMenu->exec(mapToGlobal(pos + Ui->image->pos() -
                                    QPoint(0, ui->scrollArea->verticalScrollBar()->value())));
}

void Sheet::moveComplicationDown() {
    auto selection = Ui->complications->selectedItems();
    int row = selection[0]->row();
    auto& complications = _character.complications();
    shared_ptr<Complication> complication = complications.takeAt(row);
    complications.insert(row + 1, complication);
    updateComplications();
}

void Sheet::moveComplicationUp() {
    auto selection = Ui->complications->selectedItems();
    int row = selection[0]->row();
    auto& complications = _character.complications();
    shared_ptr<Complication> complication = complications.takeAt(row);
    complications.insert(row - 1, complication);
    updateComplications();
}

void Sheet::movePowerOrEquipmentDown() {
    auto selection = Ui->powersandequipment->selectedItems();
    int row = selection[0]->row();
    auto& powers = _character.powersOrEquipment();
    auto power = getPower(row, powers);
    delPower(row);
    putPower(row + 2, power);
    updatePowersAndEquipment();
}

void Sheet::movePowerOrEquipmentUp() {
    auto selection = Ui->powersandequipment->selectedItems();
    int row = selection[0]->row();
    auto& powers = _character.powersOrEquipment();
    auto power = getPower(row, powers);
    delPower(row);
    putPower(row - 1, power);
    updatePowersAndEquipment();
}

void Sheet::moveSkillTalentOrPerkDown() {
    auto selection = Ui->skillstalentsandperks->selectedItems();
    int row = selection[0]->row();
    auto& stps = _character.skillsTalentsOrPerks();
    auto stp = stps.takeAt(row);
    stps.insert(row + 1, stp);
    updateSkillsTalentsAndPerks();
}

void Sheet::moveSkillTalentOrPerkUp() {
    auto selection = Ui->skillstalentsandperks->selectedItems();
    int row = selection[0]->row();
    auto& skillstalentsorperks = _character.skillsTalentsOrPerks();
    shared_ptr<SkillTalentOrPerk> skilltalentorperk = skillstalentsorperks.takeAt(row);
    skillstalentsorperks.insert(row - 1, skilltalentorperk);
    updateSkillsTalentsAndPerks();
}

void Sheet::newchar() {
    try {
        if (_changed) {
            switch (YesNoCancel("Do you want to save your changes first?", "The current sheet has been changed!")) {
            case QMessageBox::Yes:    save(); break;
            case QMessageBox::Cancel:         return;
            default:                          break;
            }
        }
    } catch (...) { return; }

    _character.erase();
    updateDisplay();
    _changed = false;
}

void Sheet::newComplication() {
    ComplicationsDialog dlg(this);

    if (dlg.exec() == QDialog::Rejected) return;
    shared_ptr<Complication> complication = dlg.complication();
    if (complication == nullptr) return;
    if (complication->description().isEmpty()) return;

    _character.complications().append(complication);

    int row = Ui->complications->rowCount();
    QFont font = Ui->complications->font();
    setCell(Ui->complications, row, 0, QString("%1").arg(complication->points().points), font);
    setCell(Ui->complications, row, 1, complication->description(),                      font, WordWrap);
    Ui->complications->resizeRowsToContents();

    _complicationPoints += complication->points();
    Ui->totalcomplicationpts->setText(QString("%1/%2").arg(_complicationPoints.points).arg(_option.complications().points));
    updateTotals();
    _changed = true;
}

void Sheet::newImage() {
    QString filename = QFileDialog::getOpenFileName(this, "New Image", _dir, "Images (*.png *.xpm *jpg)");
    if (filename.isEmpty()) return;
    clearImage();
    QPixmap pixmap;
    pixmap.load(filename);
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
    _changed = true;
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

    PowerDialog dlg(this);
    if (framework) dlg.multipower();
    if (dlg.exec() == QDialog::Rejected) return;
    shared_ptr<Power> power = dlg.powerorequipment();
    if (power == nullptr) return;
    if (power->description().isEmpty()) return;

    addPower(power);

    updatePower(power);
}

void Sheet::newSkillTalentOrPerk() {
    SkillDialog dlg(this);

    if (dlg.exec() == QDialog::Rejected) return;
    shared_ptr<SkillTalentOrPerk> skilltalentorperk = dlg.skilltalentorperk();
    if (skilltalentorperk == nullptr) return;
    if (skilltalentorperk->description().isEmpty()) return;

    _character.skillsTalentsOrPerks().append(skilltalentorperk);

    int row = Ui->skillstalentsandperks->rowCount();
    QFont font = Ui->skillstalentsandperks->font();
    setCell(Ui->skillstalentsandperks, row, 0, QString("%1").arg(skilltalentorperk->points().points), font);
    setCell(Ui->skillstalentsandperks, row, 1, skilltalentorperk->description(), font, WordWrap);
    setCell(Ui->skillstalentsandperks, row, 2, skilltalentorperk->roll(), font);
    Ui->skillstalentsandperks->resizeRowsToContents();

    updateSkills(skilltalentorperk);

    _skillsTalentsOrPerksPoints += skilltalentorperk->points();
    Ui->totalskillstalentsandperkscost->setText(QString("%1").arg(_skillsTalentsOrPerksPoints.points));
    updateTotals();
    _changed = true;
}

void Sheet::open() {
    try {
        if (_changed) {
            switch (YesNoCancel("Do you want to save your changes first?", "The current sheet has been changed!")) {
            case QMessageBox::Yes:    save(); break;
            case QMessageBox::Cancel:         return;
            default:                          break;
            }
        }
    } catch (...) { return; }

    QString filename = QFileDialog::getOpenFileName(this, "Open File", _dir, "Characters (*.hsccu)");
    if (filename.isEmpty()) return;
    _filename = filename;

    int ext = _filename.lastIndexOf(".hsccu");
    if (ext != -1) _filename = _filename.left(ext);

    int sep = _filename.lastIndexOf("/");
    if (sep != -1) {
        _dir = _filename.left(sep);
        _filename = _filename.mid(sep + 1);
    }

    if (!_character.load(_option, _dir + "/" + _filename)) OK("Can't load \"" + _filename + ".hsccu\" from the \"" + _dir + "\" folder.");
    else {
        updateDisplay();
        _changed = false;
    }
}

void Sheet::options() {
    optionDialog dlg;
    dlg.setComplications(_option.complications().points);
    dlg.setTotalPoints(_option.totalPoints().points);
    if (dlg.exec() != QDialog::Accepted) return;
    _option.complications(Points(dlg.complications()));
    _option.totalPoints(Points(dlg.totalPoints()));
    _option.store();
    updateDisplay();
    _changed = true;
}

void Sheet::pageSetup() {
    QPageSetupDialog dlg(printer, this);
    dlg.exec();
}

void Sheet::pasteCharacter() {
    try {
        if (_changed) {
            switch (YesNoCancel("Do you want to save your changes first?", "The current sheet has been changed!")) {
            case QMessageBox::Yes:    save(); break;
            case QMessageBox::Cancel:         return;
            default:                          break;
            }
        }
    } catch (...) { return; }

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
    updateTotals();
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
    updateTotals();
    _changed = true;
}

void Sheet::playerNameChanged(QString txt) {
    _character.playerName(txt);
    _changed = true;
}

void Sheet::powersandequipmentMenu(QPoint pos) {
    Ui->powersandequipmentMenu->exec(mapToGlobal(pos
                                                 + Ui->powersandequipment->pos()
                                                 - QPoint(0, ui->scrollArea->verticalScrollBar()->value())));
}

void Sheet::print(QPainter& painter, QPoint& offset, QWidget* widget) {
    QString oldStyle = widget->styleSheet();

    QLabel* label = dynamic_cast<QLabel*>(widget);
    if (label) {
        QString style = "QLabel { background: transparent;"
                              "   border-style: none;"
                              " }";
        label->setStyleSheet(style);
    }
    QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(widget);
    if (lineEdit) {
        QString style = "QLineEdit { background: white;"
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
                                   "   border-style: none;"
                         + QString("   font: %2pt \"%1\";").arg(family).arg((pnt * 8 + 5) / 10) +
                                   "   color: black;"
                                   " } "
                       "QHeaderView::section { background-color: white;"
                                           "   border-style: none;"
                                           "   color: black;" +
                                   QString("   font: bold %2pt \"%1\";").arg(family).arg(pnt) +
                                           " }";
        table->setStyleSheet(style);

    }
    QPoint move { widget->x() - offset.x(), widget->y() - offset.y() };
    painter.translate(move);
    widget->render(&painter);
    painter.translate(-move);
    widget->setStyleSheet(oldStyle);
}

void Sheet::print() {
    QPrintDialog dlg(printer, this);
    dlg.setWindowTitle("Print Character");
    if (dlg.exec() != QDialog::Accepted) return;

    QPixmap page1(QString(":/gfx/Page1.png"));
    QPixmap page2(QString(":/gfx/Page2.png"));

    auto pageLayout = printer->pageLayout();
    pageLayout.setOrientation(QPageLayout::Orientation::Portrait);
    printer->setPageLayout(pageLayout);
    printer->setFullPage(false);
    QPainter painter;
    painter.begin(printer);
    QRectF pageRect = printer->pageRect(QPrinter::DevicePixel);
    double pnt = pageRect.height() / (11.0 * 72.0);
    double xscale = (pageRect.width() - 72.0 * pnt) / page1.width();
    double yscale = (pageRect.height() - 72.0 * pnt) / page1.height();
    double scale = qMin(xscale, yscale);
    painter.translate(QPoint({ (int) (36 * pnt), (int) (36 * pnt) }));
    painter.scale(scale, scale);

    QPoint offset { 55, 48 };
    painter.drawImage(QPointF { 0.0, 0.0 }, page1.toImage());
    for (const auto& widget: Ui->widgets) {
        if (widget == nullptr || widget->y() > 1250) continue; // skip things we can't render and are on the seecond page
        print(painter, offset, widget);
    }
    printer->newPage();

    offset = QPoint({ 50, 1352 });
    painter.drawImage(QPointF { 0.0, 0.0 }, page2.toImage());
    for (const auto& widget: Ui->widgets) {
        if (widget == nullptr || widget->y() < 1250) continue; // skip things we can't render and are on the seecond page
        print(painter, offset, widget);
    }
    painter.end();
}

void Sheet::save() {
    if (!_changed) return;
    if (_filename.isEmpty()) {
        QString oldname = _filename;
        _filename = Ui->charactername->text();
        saveAs();
        if (_filename.isEmpty()) _filename = oldname;
        return;
    }

    if (!_character.store(_option, _dir + "/" + _filename)) {
        OK("Can't save to \"" + _filename + ".hsccu\" in the \"" + _dir + "\" folder.");
        throw "";
    }
    else _changed = false;
}

void Sheet::saveAs() {
    QString oldname = _filename;
    _filename = QFileDialog::getSaveFileName(this, "Save File", _dir, "Characters (*.hsccu)");
    if (_filename.isEmpty()) return;

    int ext = _filename.lastIndexOf(".hsccu");
    if (ext != -1) _filename = _filename.left(ext);

    int sep = _filename.lastIndexOf("/");
    if (sep != -1) {
        _dir = _filename.left(sep);
        _filename = _filename.mid(sep + 1);
    }
    try { save(); } catch (...) { _filename = oldname; }
}

void Sheet::skillstalentsandperksMenu(QPoint pos) {
    Ui->skillstalentsandperksMenu->exec(mapToGlobal(pos
                                                    + Ui->skillstalentsandperks->pos()
                                                    - QPoint(0, ui->scrollArea->verticalScrollBar()->value())));
}

void Sheet::totalExperienceEarnedChanged(QString txt) {
    if (numeric(txt) || txt.isEmpty()) {
        _character.xp(Points(txt.toInt()));

        Points total = _option.totalPoints() - _option.complications() + _character.xp() + _option.complications().Min(_complicationPoints);
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
