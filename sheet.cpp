#include "sheet.h"
#include "ui_sheet.h"
#include "sheet_ui.h"

// ---[static functions] ----------------------------------------------------------------------------------

static bool numeric(QString txt) {
    bool ok;
    txt.toInt(&ok, 10);
    return ok;
}

// --- [creation/destruction] -----------------------------------------------------------------------------

Sheet::Sheet(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Sheet)
    , Ui(new Sheet_UI)
{
    ui->setupUi(this);
    Ui->setupUi(ui->label);

    connect(Ui->alternateids,          SIGNAL(textEdited(QString)), this, SLOT(alternateIdsChanged(QString)));
    connect(Ui->charactername,         SIGNAL(textEdited(QString)), this, SLOT(characterNameChanged(QString)));
    connect(Ui->playername,            SIGNAL(textEdited(QString)), this, SLOT(playerNameChanged(QString)));
    connect(Ui->totalexperienceearned, SIGNAL(textEdited(QString)), this, SLOT(totalExperienceEarnedChanged(QString)));
}

Sheet::~Sheet()
{
    delete ui;
    // Ui is part of ui, don't delete it!
}

// ---[SLOTS] --------------------------------------------------------------------------------------------

void Sheet::alternateIdsChanged(QString txt) {
    _character.alternateIds(txt);
    _changed = true;
}

void Sheet::characterNameChanged(QString txt) {
    Ui->charactername2->setText(txt);
    _character.characterName(txt);
    _changed = true;
}

void Sheet::playerNameChanged(QString txt) {
    _character.playerName(txt);
    _changed = true;
}

void Sheet::totalExperienceEarnedChanged(QString txt) {
    if (numeric(txt)) {
        _character.xp(txt.toInt());
        // calc experience not spent
        // calc experience spent
        _changed = true;
    } else Ui->totalexperienceearned->undo();
}
