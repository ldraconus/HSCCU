#include "complicationsdialog.h"
#include "complication.h"
#include "powers.h"
#include "powerdialog.h"
#include "skilldialog.h"
#include "skilltalentorperk.h"

#include "sheet.h"
#include "ui_sheet.h"
#include "sheet_ui.h"

#include <cmath>

#include <QClipboard>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QMimeData>
#include <QMouseEvent>
#include <QScrollBar>
#include <QStandardPaths>

Sheet* Sheet::_sheet = nullptr;

// --- [static functions] ----------------------------------------------------------------------------------

static int min(const int a, const int b) {
    return (a < b) ? a : b;
}

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
    ui->setupUi(this);
    Ui->setupUi(ui->label);

    _sheet = this;

    _dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    _option.totalPoints(400)
           .complications(75);

    connect(ui->menu_File,     SIGNAL(aboutToShow()), this, SLOT(aboutToShowFileMenu()));
    connect(ui->menu_File,     SIGNAL(aboutToHide()), this, SLOT(aboutToHideFileMenu()));
    connect(ui->action_New,    SIGNAL(triggered()),   this, SLOT(newchar()));
    connect(ui->action_Open,   SIGNAL(triggered()),   this, SLOT(open()));
    connect(ui->action_Save,   SIGNAL(triggered()),   this, SLOT(save()));
    connect(ui->actionSave_As, SIGNAL(triggered()),   this, SLOT(saveAs()));

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

    connect(Ui->powersandequipment,     SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(powersandequipmentDoubleClicked(QTableWidgetItem*)));
    connect(Ui->powersandequipment,     SIGNAL(customContextMenuRequested(QPoint)),   this, SLOT(powersandequipmentMenu(QPoint)));
    connect(Ui->powersandequipmentMenu, SIGNAL(aboutToShow()),                        this, SLOT(aboutToShowPowersAndEquipmentMenu()));
    connect(Ui->newPowerOrEquipment,    SIGNAL(triggered()),                          this, SLOT(newPowerOrEquipment()));
    connect(Ui->editSkillTalentOrPerk,  SIGNAL(triggered()),                          this, SLOT(editSkillstalentsandperks()));
//    connect(Ui->deleteSkillTalentOrPerk,   SIGNAL(triggered()),                          this, SLOT(deleteSkillstalentsandperks()));
//    connect(Ui->cutSkillTalentOrPerk,      SIGNAL(triggered()),                          this, SLOT(cutSkillTalentOrPerk()));
//    connect(Ui->copySkillTalentOrPerk,     SIGNAL(triggered()),                          this, SLOT(copySkillTalentOrPerk()));
//    connect(Ui->pasteSkillTalentOrPerk,    SIGNAL(triggered()),                          this, SLOT(pasteSkillTalentOrPerk()));
//    connect(Ui->moveSkillTalentOrPerkUp,   SIGNAL(triggered()),                          this, SLOT(moveSkillTalentOrPerkUp()));
//    connect(Ui->moveSkillTalentOrPerkDown, SIGNAL(triggered()),                          this, SLOT(moveSkillTalentOrPerkDown()));

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
    delete ui;
    // Ui is pointer into ui->label, don't delete it!  Don't worry, it is not allocate either, static global storage with a pointer to it.
}

// --- [EVENT FILTER] ----------------------------------------------------------------------------------

bool Sheet::eventFilter(QObject* object, QEvent* event) {
    if (event->type() == QEvent::FocusIn && _widget2Def.find(object) != _widget2Def.end()) {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(object);
        edit->setText(QString("%1").arg(_widget2Def[edit].characteristic()->base()));
    }
    return false;
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

int Sheet::characteristicsCost() {
    int total = 0;
    const auto keys = _widget2Def.keys();
    for (const auto& key: keys) {
        auto& def = _widget2Def[key];
        total += def.characteristic()->points();
    }
    Ui->totalcost->setText(QString("%1").arg(total));
    return total;
}

void Sheet::characteristicChanged(QLineEdit* val, QString txt, bool update) {
    if (numeric(txt) || txt.isEmpty()) {
        auto& def = _widget2Def[val];
        int save = def.characteristic()->base();
        def.characteristic()->base(txt.toInt());
        if (val == Ui->spdval) {
            int primary = def.characteristic()->base() + def.characteristic()->primary();
            int secondary = primary + def.characteristic()->secondary();
            if (primary > 12 || secondary > 12) {
                val->undo();
                def.characteristic()->base(save);
                return;
            }
        }
        def.points()->setText(QString("%1").arg(def.characteristic()->points()));
        if (def.roll()) def.roll()->setText(def.characteristic()->roll());

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
          else if (val == Ui->pdval) setDefense(def, 0, 1);
          else if (val == Ui->edval) setDefense(def, 2, 1);

        if (update) updateTotals();

        // [TODO] go through powers & skills looking for stat references and update all of them regardless of appropriateness.
    } else val->undo();
}

void Sheet::characteristicEditingFinished(QLineEdit* val) {
    characteristicChanged(val, val->text());
    if (val->text().isEmpty()) {
        if (sender() == Ui->spdval) {
            val->setText("1");
            characteristicChanged(val, "1");
        }
        val->setText("0");
    }
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

void Sheet::setDamage(_CharacteristicDef& def, QLabel* set) {
    int primary = def.characteristic()->base() + def.characteristic()->primary();
    int secondary = primary + def.characteristic()->secondary();
    QString dice = valueToDice(primary);
    if (primary != secondary) dice += "/" + valueToDice(secondary);
    set->setText(dice);
}

void Sheet::setDefense(_CharacteristicDef& def, int r, int c) {
    int primary = def.characteristic()->base() + def.characteristic()->primary();
    int secondary = primary + def.characteristic()->secondary();
    QString defense = QString("%1").arg(primary);
    if (primary != secondary) defense += QString("/%1").arg(secondary);
    setCell(Ui->defenses, r, c, defense, Ui->defenses->font());
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
        int primary = def.characteristic()->base() + def.characteristic()->primary();
        int secondary = primary + def.characteristic()->secondary();
        QLineEdit* characteristic = dynamic_cast<QLineEdit*>(key);
        QString value = QString("%1").arg(primary);
        if (primary != secondary) value += QString("/%1").arg(secondary);
        characteristic->setText(value);
        characteristicChanged(characteristic, characteristic->text(), DontUpdateTotal);
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
    Ui->totalexperienceearned->setText(QString("%1").arg(_character.xp()));
}

void Sheet::updateComplications() {
    Ui->complications->setRowCount(0);
    Ui->complications->update();

    _complicationPoints = 0;
    QFont font = Ui->complications->font();
    int row = 0;
    for (const auto& complication: _character.complications()) {
        int pts = complication->points(Complication::NoStore);
        setCell(Ui->complications, row, 0, QString("%1").arg(pts), font);
        setCell(Ui->complications, row, 1, complication->description(), font, WordWrap);
        _complicationPoints += pts;
        ++row;
    }
    Ui->complications->resizeRowsToContents();

    Ui->totalcomplicationpts->setText(QString("%1/%2").arg(_complicationPoints).arg(_option.complications()));
}

void Sheet::updateSkillsTalentsAndPerks(){
    Ui->skillstalentsandperks->setRowCount(0);
    Ui->skillstalentsandperks->update();

    _skillsTalentsOrPerksPoints = 0;
    QFont font = Ui->skillstalentsandperks->font();
    int row = 0;
    for (const auto& stp: _character.skillsTalentsOrPerks()) {
        int pts = stp->points(Complication::NoStore);
        setCell(Ui->skillstalentsandperks, row, 0, QString("%1").arg(pts), font);
        setCell(Ui->skillstalentsandperks, row, 1, stp->description(), font, WordWrap);
        setCell(Ui->skillstalentsandperks, row, 2, stp->roll(), font);
        _skillsTalentsOrPerksPoints += pts;
        ++row;
    }
    Ui->skillstalentsandperks->resizeRowsToContents();

    Ui->totalskillstalentsandperkscost->setText(QString("%1").arg(_skillsTalentsOrPerksPoints));
}

void Sheet::updatePowersAndEquipment(){
    Ui->powersandequipment->setRowCount(0);
    Ui->powersandequipment->update();

    _powersOrEquiomentPoints = 0;
    QFont font = Ui->powersandequipment->font();
    int row = 0;
    for (const auto& pe: _character.powersOrEquipment()) {
        int pts = pe->points(Power::NoStore);
        setCell(Ui->powersandequipment, row, 0, QString("%1").arg(pts), font);
        setCell(Ui->powersandequipment, row, 1, pe->nickname(), font);
        setCell(Ui->powersandequipment, row, 2, pe->description(), font, WordWrap);
        setCell(Ui->powersandequipment, row, 3, pe->end(), font);
        _skillsTalentsOrPerksPoints += pts;
        ++row;
    }
    Ui->powersandequipment->resizeRowsToContents();

    Ui->totalskillstalentsandperkscost->setText(QString("%1").arg(_powersOrEquiomentPoints));
}

void Sheet::updateDisplay() {
    updateCharacter();
    updateCharacteristics();
    updateComplications();
    updatePowersAndEquipment();
    updateSkillsTalentsAndPerks();
    updateTotals();
}

void Sheet::updateTotals() {
    _totalPoints = characteristicsCost() + _skillsTalentsOrPerksPoints + _powersOrEquiomentPoints;
    Ui->totalpoints->setText(QString("%1/%2")
                             .arg(_totalPoints)
                             .arg(_option.totalPoints() - _option.complications() + min(_option.complications(), _complicationPoints)));
    totalExperienceEarnedEditingFinished();
}

QString Sheet::valueToDice(int value) {
    QString halfDice = "½";
    int dice = value / 5;
    bool half = value % 5 > 2;
    return QString("%1%2d6").arg(dice).arg(half ? halfDice : "");
}

// ---[SLOTS] --------------------------------------------------------------------------------------------

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

void Sheet::aboutToShowFileMenu() {
    ui->action_Save->setEnabled(_changed);
}

void Sheet::aboutToShowPowersAndEquipmentMenu() {
    // [TODO] - Basically copy one the following function ;-)
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

void Sheet::copyComplication() {
    QClipboard* clip = QGuiApplication::clipboard();
    QMimeData* data = new QMimeData();
    auto selection = Ui->complications->selectedItems();
    int row = selection[0]->row();
    Complication* complication = _character.complications()[row];
    QJsonObject obj = complication->toJson();
    QJsonDocument doc;
    doc.setObject(obj);
    data->setData("application/complication", doc.toJson());
    QString text = QString("%1\t%2").arg(complication->points(Complication::NoStore)).arg(complication->description());
    data->setData("text/plain", text.toUtf8());
    clip->setMimeData(data);
}

void Sheet::copySkillTalentOrPerk() {
    QClipboard* clip = QGuiApplication::clipboard();
    QMimeData* data = new QMimeData();
    auto selection = Ui->skillstalentsandperks->selectedItems();
    int row = selection[0]->row();
    SkillTalentOrPerk* skilltalentorperk = _character.skillsTalentsOrPerks()[row];
    QJsonObject obj = skilltalentorperk->toJson();
    QJsonDocument doc;
    doc.setObject(obj);
    data->setData("application/skillperkortalent", doc.toJson());
    QString text = QString("%1\t%2\t%3").arg(skilltalentorperk->points(SkillTalentOrPerk::NoStore))
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

void Sheet::cutComplication() {
    copyComplication();
    deleteComplication();
}

void Sheet::cutSkillTalentOrPerk() {
    copySkillTalentOrPerk();
    deleteSkillstalentsandperks();
}

void Sheet::deleteComplication() {
    auto selection = Ui->complications->selectedItems();
    int row = selection[0]->row();
    Complication* complication = _character.complications().takeAt(row);
    _complicationPoints -= complication->points(Complication::NoStore);
    Ui->complications->removeRow(row);
    Ui->totalcomplicationpts->setText(QString("%1/%2").arg(_complicationPoints).arg(_option.complications()));
    updateTotals();
    _changed = true;
}

void Sheet::deleteSkillstalentsandperks() {
    auto selection = Ui->skillstalentsandperks->selectedItems();
    int row = selection[0]->row();
    SkillTalentOrPerk* skilltalentorperk = _character.skillsTalentsOrPerks().takeAt(row);
    _skillsTalentsOrPerksPoints -= skilltalentorperk->points(Complication::NoStore);
    Ui->skillstalentsandperks->removeRow(row);
    Ui->totalskillstalentsandperkscost->setText(QString("%1").arg(_skillsTalentsOrPerksPoints));
    updateTotals();
    _changed = true;
}

void Sheet::editComplication() {
    auto selection = Ui->complications->selectedItems();
    int row = selection[0]->row();
    Complication* complication = _character.complications()[row];
    ComplicationsDialog dlg(this);
    dlg.complication(complication);
    int old = complication->points();

    if (dlg.exec() == QDialog::Rejected) return;
    if (complication->description().isEmpty()) return;

    QFont font = Ui->complications->font();
    setCell(Ui->complications, row, 0, QString("%1").arg(complication->points()), font);
    setCell(Ui->complications, row, 1, complication->description(),               font, WordWrap);

    Ui->complications->resizeRowsToContents();
    _complicationPoints += complication->points() - old;
    Ui->totalcomplicationpts->setText(QString("%1/%2").arg(_complicationPoints).arg(_option.complications()));
    updateTotals();
    _changed = true;
}

void Sheet::editPowerOrEquipment() {
    // [TODO] Pretty much copy the method below, changing the names of the innocent ;-)
}

void Sheet::editSkillstalentsandperks() {
    auto selection = Ui->skillstalentsandperks->selectedItems();
    int row = selection[0]->row();
    SkillTalentOrPerk* skilltalentorperk = _character.skillsTalentsOrPerks()[row];
    SkillDialog dlg(this);
    dlg.skilltalentorperk(skilltalentorperk);
    int old = skilltalentorperk->points();

    if (dlg.exec() == QDialog::Rejected) return;
    if (skilltalentorperk->description().isEmpty()) return;

    QFont font = Ui->skillstalentsandperks->font();
    setCell(Ui->skillstalentsandperks, row, 0, QString("%1").arg(skilltalentorperk->points()), font);
    setCell(Ui->skillstalentsandperks, row, 1, skilltalentorperk->description(),               font, WordWrap);
    setCell(Ui->skillstalentsandperks, row, 2, skilltalentorperk->roll(),                      font);

    Ui->skillstalentsandperks->resizeRowsToContents();
    _skillsTalentsOrPerksPoints += skilltalentorperk->points() - old;
    Ui->totalskillstalentsandperkscost->setText(QString("%1").arg(_skillsTalentsOrPerksPoints));
    updateTotals();
    _changed = true;

}

void Sheet::eyeColorChanged(QString txt) {
    _character.eyeColor(txt);
    _changed = true;
}

void Sheet::gamemasterChanged(QString txt) {
    _character.genre(txt);
    _changed = true;
}

void Sheet::genreChanged(QString txt) {
    _character.genre(txt);
    _changed = true;
}

void Sheet::hairColorChanged(QString txt) {
    _character.hairColor(txt);
    _changed = true;
}

void Sheet::moveComplicationDown() {
    auto selection = Ui->complications->selectedItems();
    int row = selection[0]->row();
    auto& complications = _character.complications();
    Complication* complication = complications.takeAt(row);
    complications.insert(row + 1, complication);
    updateComplications();
}

void Sheet::moveComplicationUp() {
    auto selection = Ui->complications->selectedItems();
    int row = selection[0]->row();
    auto& complications = _character.complications();
    Complication* complication = complications.takeAt(row);
    complications.insert(row - 1, complication);
    updateComplications();
}

void Sheet::moveSkillTalentOrPerkDown() {
    auto selection = Ui->skillstalentsandperks->selectedItems();
    int row = selection[0]->row();
    auto& stps = _character.skillsTalentsOrPerks();
    auto* stp = stps.takeAt(row);
    stps.insert(row + 1, stp);
    updateSkillsTalentsAndPerks();
}

void Sheet::moveSkillTalentOrPerkUp() {
    auto selection = Ui->skillstalentsandperks->selectedItems();
    int row = selection[0]->row();
    auto& skillstalentsorperks = _character.skillsTalentsOrPerks();
    SkillTalentOrPerk* skilltalentorperk = skillstalentsorperks.takeAt(row);
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
    Complication* complication = dlg.complication();
    if (complication->description().isEmpty()) return;

    _character.complications().append(complication);

    int row = Ui->complications->rowCount();
    QFont font = Ui->complications->font();
    setCell(Ui->complications, row, 0, QString("%1").arg(complication->points()), font);
    setCell(Ui->complications, row, 1, complication->description(), font, WordWrap);
    Ui->complications->resizeRowsToContents();

    _complicationPoints += complication->points();
    Ui->totalcomplicationpts->setText(QString("%1/%2").arg(_complicationPoints).arg(_option.complications()));
    updateTotals();
    _changed = true;
}

void Sheet::newPowerOrEquipment() {
    PowerDialog dlg(this);

    if (dlg.exec() == QDialog::Rejected) return;
    Power* powerorequipment = dlg.powerorequipment();
    if (powerorequipment->description().isEmpty()) return;

    _character.powersOrEquipment().append(powerorequipment);

    int row = Ui->powersandequipment->rowCount();
    QFont font = Ui->powersandequipment->font();
    setCell(Ui->powersandequipment, row, 0, QString("%1").arg(powerorequipment->points()), font);
    setCell(Ui->powersandequipment, row, 1, QString("%1").arg(powerorequipment->nickname()), font);
    setCell(Ui->powersandequipment, row, 2, powerorequipment->description(), font, WordWrap);
    setCell(Ui->powersandequipment, row, 3, powerorequipment->end(), font);
    Ui->powersandequipment->resizeRowsToContents();

    _powersOrEquiomentPoints += powerorequipment->points();
    Ui->totalpowersandequipmentcost->setText(QString("%1").arg(_powersOrEquiomentPoints));
    updateTotals();
    _changed = true;

}

void Sheet::newSkillTalentOrPerk() {
    SkillDialog dlg(this);

    if (dlg.exec() == QDialog::Rejected) return;
    SkillTalentOrPerk* skilltalentorperk = dlg.skilltalentorperk();
    if (skilltalentorperk->description().isEmpty()) return;

    _character.skillsTalentsOrPerks().append(skilltalentorperk);

    int row = Ui->skillstalentsandperks->rowCount();
    QFont font = Ui->skillstalentsandperks->font();
    setCell(Ui->skillstalentsandperks, row, 0, QString("%1").arg(skilltalentorperk->points()), font);
    setCell(Ui->skillstalentsandperks, row, 1, skilltalentorperk->description(), font, WordWrap);
    setCell(Ui->skillstalentsandperks, row, 2, skilltalentorperk->roll(), font);
    Ui->skillstalentsandperks->resizeRowsToContents();

    _skillsTalentsOrPerksPoints += skilltalentorperk->points();
    Ui->totalskillstalentsandperkscost->setText(QString("%1").arg(_skillsTalentsOrPerksPoints));
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

    if (!_character.load(_dir + "/" + _filename)) OK("Can't load \"" + _filename + ".hsccu\" from the \"" + _dir + "\" folder.");
    else {
        updateDisplay();
        _changed = false;
    }
}

void Sheet::pasteComplication() {
    QClipboard* clip = QGuiApplication::clipboard();
    const QMimeData* data = clip->mimeData();
    QByteArray byteArray = data->data("application/complication");
    QString jsonStr(byteArray);
    QJsonDocument json = QJsonDocument::fromJson(jsonStr.toUtf8());
    QJsonObject obj = json.object();
    QString name = obj["name"].toString();
    Complication* complication = Complication::FromJson(name, obj);
    _character.complications().append(complication);

    int row = Ui->complications->rowCount();
    QFont font = Ui->complications->font();
    setCell(Ui->complications, row, 0, QString("%1").arg(complication->points(Complication::NoStore)), font);
    setCell(Ui->complications, row, 1, complication->description(), font, WordWrap);
    Ui->complications->resizeRowsToContents();

    _complicationPoints += complication->points(Complication::NoStore);
    Ui->totalcomplicationpts->setText(QString("%1/%2").arg(_complicationPoints).arg(_option.complications()));
    updateTotals();
    _changed = true;
}

void Sheet::pasteSkillTalentOrPerk() {
    QClipboard* clip = QGuiApplication::clipboard();
    const QMimeData* data = clip->mimeData();
    QByteArray byteArray = data->data("application/skillperkortalent");
    QString jsonStr(byteArray);
    QJsonDocument json = QJsonDocument::fromJson(jsonStr.toUtf8());
    QJsonObject obj = json.object();
    QString name = obj["name"].toString();
    SkillTalentOrPerk* stp = SkillTalentOrPerk::FromJson(name, obj);
    _character.skillsTalentsOrPerks().append(stp);

    int row = Ui->skillstalentsandperks->rowCount();
    QFont font = Ui->skillstalentsandperks->font();
    setCell(Ui->skillstalentsandperks, row, 0, QString("%1").arg(stp->points(Complication::NoStore)), font);
    setCell(Ui->skillstalentsandperks, row, 1, stp->description(), font, WordWrap);
    setCell(Ui->skillstalentsandperks, row, 2, stp->roll(), font);
    Ui->skillstalentsandperks->resizeRowsToContents();

    _skillsTalentsOrPerksPoints += stp->points(Complication::NoStore);
    Ui->totalskillstalentsandperkscost->setText(QString("%1").arg(_skillsTalentsOrPerksPoints));
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

void Sheet::save() {
    if (!_changed) return;
    if (_filename.isEmpty()) {
        QString oldname = _filename;
        _filename = Ui->charactername->text();
        saveAs();
        if (_filename == "") _filename = oldname;
        return;
    }

    if (!_character.store(_dir + "/" + _filename)) {
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
        _character.xp(txt.toInt());

        int total = _option.totalPoints() - _option.complications() + _character.xp() + min(_option.complications(), _complicationPoints);
        int remaining = total - _totalPoints;
        int spent;
        if (_totalPoints < _option.totalPoints()) spent = 0;
        else spent = _totalPoints - _option.totalPoints();

        Ui->experiencespent->setText(QString("%1").arg(spent));
        Ui->experienceunspent->setText(QString("%1").arg(remaining));
        _changed = true;
    } else Ui->totalexperienceearned->undo();
}

void Sheet::totalExperienceEarnedEditingFinished() {
    totalExperienceEarnedChanged(Ui->totalexperienceearned->text());
    if (Ui->totalexperienceearned->text().isEmpty()) Ui->totalexperienceearned->setText("0");
}
