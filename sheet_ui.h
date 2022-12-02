#pragma once

#include "qscreen.h"
#include <QAction>
#include <QLineEdit>
#include <QGridLayout>
#include <QLabel>
#include <QFrame>
#include <QHeaderView>
#include <QMainWindow>
#include <QMenu>
#include <QTableView>
#include <QTableWidget>
#include <QTextEdit>

class Sheet_UI
{
private:
    void moveTo(QWidget* w, int x, int y, int l = -1, int h = -1) {
        QRect r = w->geometry();
        r.moveTo({ x, y });
        if (l != -1) r.setWidth(l);
        if (h != -1) r.setHeight(h);
        w->setGeometry(r);
    }

    QLabel* createLabel(QWidget* parent, QFont& font, QString val, int x, int y, int l, int h = -1) {
        QLabel* label = new QLabel(parent);
        label->setFont(font);
        label->setText(val);
        if (h == -1) {
            h = l;
            l = -1;
        }
        moveTo(label, x, y, l, h);
        return label;
    }

    QLineEdit* createLineEdit(QWidget* parent, QFont& font, int x, int y, int l, int h, QString w = "") {
        return createLineEdit(parent, font, "", x, y, l, h, w);
    }

    QLineEdit* createLineEdit(QWidget* parent, QFont& font, QString val, int x, int y, int l, int h, QString w = "") {
        QString style = "QLineEdit { background: cyan;"
                                 "   border-style: none;"
                                 " }";
        return createLineEdit(parent, font, style, val, x, y, l, h, w);
    }

    QLineEdit* createLineEdit(QWidget* parent, QFont& font, QString style, QString val, int x, int y, int l, int h, QString w = "") {
        QLineEdit* lineedit = new QLineEdit(parent);
        lineedit->setFont(font);
        lineedit->setText(val);
        lineedit->setStyleSheet(style);
        lineedit->setToolTip(w);
        moveTo(lineedit, x, y, l, h);
        QFrame* line = new QFrame(parent);
        line->setLineWidth(1);
        line->setFrameShape(QFrame::HLine);
        QFont f = lineedit->font();
        QFontMetrics metrics(f);
        moveTo(line, x, y + h - metrics.descent() + 2, l, 1);
        return lineedit;
    }

    struct menuItems {
        menuItems()
            : text("-")
            , action(nullptr) { }
        menuItems(QString x)
            : text(x)
            , action(nullptr) { }
        menuItems(QString x, QAction** y)
            : text(x)
            , action(y) { }
        menuItems(const menuItems& mi)
            : text(mi.text)
            , action(mi.action) { }
        menuItems(menuItems&& mi)
            : text(mi.text)
            , action(mi.action) { }
        QString text;
        QAction** action;
    };

    QMenu* createMenu(QWidget* parent, QFont& font, QList<menuItems> items) {
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

    const bool Selectable = true;

    QTableWidget* createTableWidget(QWidget* parent, QFont& font, QStringList headers, QList<QStringList> vals, int x, int y, int l, int h,
                                    bool selectable = false) {
        return createTableWidget(parent, font, headers, vals, x, y, l, h, "", selectable);
    }

    QTableWidget* createTableWidget(QWidget* parent, QFont& font, QStringList headers, QList<QStringList> vals, int x, int y, int l, int h,
                                    QString w, bool selectable = false) {
        QTableWidget* tablewidget = new QTableWidget(parent);
        tablewidget->setContextMenuPolicy(Qt::CustomContextMenu);
        tablewidget->setWordWrap(true);
        auto verticalHeader = tablewidget->verticalHeader();
        int pnt = font.pointSize();
        int sz = pnt * 2;
        verticalHeader->setVisible(false);
        verticalHeader->setMinimumSectionSize(1);
        verticalHeader->setMaximumSectionSize(selectable ? h : sz);
        verticalHeader->setDefaultSectionSize(1);
        verticalHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
        auto horizontalHeader = tablewidget->horizontalHeader();
        horizontalHeader->setStretchLastSection(true);
        horizontalHeader->setMaximumSectionSize(l);
        horizontalHeader->setDefaultSectionSize(10);
        horizontalHeader->setDefaultAlignment(Qt::AlignLeft);
        horizontalHeader->setMaximumSize(l, sz);
        tablewidget->setSelectionMode(selectable ? QAbstractItemView::SingleSelection : QAbstractItemView::NoSelection);
        tablewidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        tablewidget->setFont(font);
        QString family = font.family();
        if (selectable)
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
        else
            tablewidget->setStyleSheet("QTableWidget { selection-color: black;"
                                                   "   selection-background-color: white;"
                                                   "   gridline-color: white;"
                                                   "   border-style: none;"
                                                   "   color: black;" +
                                           QString("   font: bold %2pt \"%1\";").arg(family).arg(pnt) +
                                                   " } "
                                       "QHeaderView::section { background-color: white;"
                                                           "   border-style: none;"
                                                           "   color: black;" +
                                                   QString("   font: bold %2pt \"%1\";").arg(family).arg(pnt) +
                                                           " }");
        tablewidget->setColumnCount(headers.size());
        tablewidget->setRowCount(vals.size());
        tablewidget->setHorizontalHeaderLabels(headers);
        int i = 0;
        for (i = 0; i < vals.size(); ++i) for (int j = 0; j < vals[i].size(); ++j) {
            QLabel* cell = new QLabel(vals[i][j]);
            cell->setFont(font);
            tablewidget->setCellWidget(i, j, cell);
        }
        tablewidget->setToolTip(w);
        moveTo(tablewidget, x, y, l, h);
        int total = 0;
        for (i = 0; i < tablewidget->rowCount(); ++i) tablewidget->resizeRowToContents(i);
        for (i = 1; i < tablewidget->columnCount(); ++i) {
            tablewidget->resizeColumnToContents(i - 1);
            total += tablewidget->columnWidth(i - 1);
        }
        tablewidget->setColumnWidth(headers.size() - 1, l - total);

        return tablewidget;
    }

    QTextEdit* createTextEdit(QWidget* parent, QFont& font, QString val, int x, int y, int l, int h) {
        return createTextEdit(parent, font, val, x, y, l, h, "");
    }

    QTextEdit* createTextEdit(QWidget* parent, QFont& font, QString val, int x, int y, int l, int h, QString w) {
        QTextEdit* textedit = new QTextEdit(parent);
        textedit->setFont(font);
        textedit->setHtml(val);
        textedit->setStyleSheet("QTextEdit { border-style: none; }");
        textedit->setReadOnly(true);
        textedit->setToolTip(w);
        moveTo(textedit, x, y, l, h);
        return textedit;
    }

public:
    QLineEdit* alternateids = nullptr;
    QLineEdit* charactername = nullptr;
    QLineEdit* playername = nullptr;

    QLineEdit* strval     = nullptr;
    QLineEdit* dexval     = nullptr;
    QLineEdit* conval     = nullptr;
    QLineEdit* intval     = nullptr;
    QLineEdit* egoval     = nullptr;
    QLineEdit* preval     = nullptr;
    QLineEdit* ocvval     = nullptr;
    QLineEdit* dcvval     = nullptr;
    QLineEdit* omcvval    = nullptr;
    QLineEdit* dmcvval    = nullptr;
    QLineEdit* spdval     = nullptr;
    QLineEdit* pdval      = nullptr;
    QLineEdit* edval      = nullptr;
    QLineEdit* recval     = nullptr;
    QLineEdit* endval     = nullptr;
    QLineEdit* bodyval    = nullptr;
    QLineEdit* stunval    = nullptr;
    QLabel*    strpoints  = nullptr;
    QLabel*    dexpoints  = nullptr;
    QLabel*    conpoints  = nullptr;
    QLabel*    intpoints  = nullptr;
    QLabel*    egopoints  = nullptr;
    QLabel*    prepoints  = nullptr;
    QLabel*    ocvpoints  = nullptr;
    QLabel*    dcvpoints  = nullptr;
    QLabel*    omcvpoints = nullptr;
    QLabel*    dmcvpoints = nullptr;
    QLabel*    spdpoints  = nullptr;
    QLabel*    pdpoints   = nullptr;
    QLabel*    edpoints   = nullptr;
    QLabel*    recpoints  = nullptr;
    QLabel*    endpoints  = nullptr;
    QLabel*    bodypoints = nullptr;
    QLabel*    stunpoints = nullptr;
    QLabel*    strroll    = nullptr;
    QLabel*    dexroll    = nullptr;
    QLabel*    conroll    = nullptr;
    QLabel*    introll    = nullptr;
    QLabel*    egoroll    = nullptr;
    QLabel*    preroll    = nullptr;
    QLabel*    totalcost  = nullptr;

    QLabel*    maximumend  = nullptr;
    QLabel*    maximumbody = nullptr;
    QLabel*    maximumstun = nullptr;
    QLineEdit* currentend  = nullptr;
    QLineEdit* currentbody = nullptr;
    QLineEdit* currentstun = nullptr;

    QLabel*        hthdamage         = nullptr;
    QLabel*        lift              = nullptr;
    QLabel*        strendcost        = nullptr;
    QList<QLabel*> phases            = { };
    QLabel*        baseocv           = nullptr;
    QLabel*        basedcv           = nullptr;
    QLabel*        baseomcv          = nullptr;
    QLabel*        basedmcv          = nullptr;
    QTextEdit*     combatskilllevels = nullptr;
    QLabel*        presenceattack    = nullptr;

    QTableWidget* movement    = nullptr;
    QLabel*       movementsfx = nullptr;

    QTableWidget* complications        = nullptr;
    QLabel*       totalcomplicationpts = nullptr;
    QMenu*        complicationsMenu    = nullptr;
    QAction*      newComplication      = nullptr;
    QAction*      editComplication     = nullptr;
    QAction*      deleteComplication   = nullptr;
    QAction*      cutComplication      = nullptr;
    QAction*      copyComplication     = nullptr;
    QAction*      pasteComplication    = nullptr;
    QAction*      moveComplicationUp   = nullptr;
    QAction*      moveComplicationDown = nullptr;

    QTableWidget* attacksandmaneuvers = nullptr;

    QTableWidget* defenses = nullptr;

    QLabel*    perceptionroll           = nullptr;
    QTextEdit* enhancedandunusualsenses = nullptr;

    QLabel*    totalpoints           = nullptr;
    QLineEdit* totalexperienceearned = nullptr;
    QLabel*    experiencespent       = nullptr;
    QLabel*    experienceunspent     = nullptr;

    QLabel*    charactername2 = nullptr;
    QLabel*    height         = nullptr;
    QLabel*    weight         = nullptr;
    QLineEdit* haircolor      = nullptr;
    QLineEdit* eyecolor       = nullptr;

    QLineEdit* campaignname = nullptr;
    QLineEdit* genre        = nullptr;
    QLineEdit* gamemaster   = nullptr;

    QTableWidget* skillstalentsandperks          = nullptr;
    QLabel*       totalskillstalentsandperkscost = nullptr;
    QMenu*        skillstalentsandperksMenu      = nullptr;
    QAction*      newSkillTalentOrPerk           = nullptr;
    QAction*      editSkillTalentOrPerk          = nullptr;
    QAction*      deleteSkillTalentOrPerk        = nullptr;
    QAction*      cutSkillTalentOrPerk           = nullptr;
    QAction*      copySkillTalentOrPerk          = nullptr;
    QAction*      pasteSkillTalentOrPerk         = nullptr;
    QAction*      moveSkillTalentOrPerkUp        = nullptr;
    QAction*      moveSkillTalentOrPerkDown      = nullptr;

    QTableWidget* powersandequipment          = nullptr;
    QLabel*       totalpowersandequipmentcost = nullptr;

    Sheet_UI() { }

    void setupUi(QWidget *widget)
    {
        auto* layout = new QGridLayout();
        widget->setLayout(layout);

        QFont largeBoldFont = widget->font();
        largeBoldFont.setPointSize(16);
        largeBoldFont.setBold(true);
        QFont largeFont = widget->font();
        largeFont.setPointSize(16);
        QFont font = widget->font();
        QFont tableFont = font;
        tableFont.setPointSize(12);
        QFont narrow = font;
        narrow.setStretch(QFont::Stretch::SemiCondensed);
        QFont smallfont = widget->font();
        smallfont.setPointSize(8);
        smallfont.setStretch(QFont::Stretch::SemiCondensed);

        charactername = createLineEdit(widget, largeBoldFont, 197,  75, 431, 27, "Characters superhero name");
        alternateids  = createLineEdit(widget, largeFont,     210, 104, 418, 27, "Characters secret id, typically");
        playername    = createLineEdit(widget, largeFont,     162, 134, 466, 27, "The players name");

        strval     = createLineEdit(widget, font, "10", 79, 250, 45, 20, "Strength: 1 point");
        dexval     = createLineEdit(widget, font, "10", 79, 275, 45, 20, "Dexterity: 2 points");
        conval     = createLineEdit(widget, font, "10", 79, 299, 45, 20, "Constitution: 1 point");
        intval     = createLineEdit(widget, font, "10", 79, 324, 45, 20, "Intellegence: 1 point");
        egoval     = createLineEdit(widget, font, "10", 79, 348, 45, 20, "Ego: 1 point");
        preval     = createLineEdit(widget, font, "10", 79, 373, 45, 20, "Presence: 1 point");
        ocvval     = createLineEdit(widget, font,  "3", 79, 404, 45, 20, "Offensive Combat Value: 5 points");
        dcvval     = createLineEdit(widget, font,  "3", 79, 429, 45, 20, "Defensive Combat Value: 5 points");
        omcvval    = createLineEdit(widget, font,  "3", 79, 454, 45, 20, "Offensive Mental Combat Value: 3 points");
        dmcvval    = createLineEdit(widget, font,  "3", 79, 479, 45, 20, "Defensive Mental Combat Value: 3 points");
        spdval     = createLineEdit(widget, font,  "2", 79, 504, 45, 20, "Speed: 10 points");
        pdval      = createLineEdit(widget, font,  "2", 79, 534, 45, 20, "Physical Defense: 1 point");
        edval      = createLineEdit(widget, font,  "2", 79, 559, 45, 20, "Energy Defense: 1 point");
        recval     = createLineEdit(widget, font,  "4", 79, 584, 45, 20, "Recovery: 1 point");
        endval     = createLineEdit(widget, font, "20", 79, 609, 45, 20, "Endurance: 1 point gets 5 points of END");
        bodyval    = createLineEdit(widget, font, "10", 79, 634, 45, 20, "Body: 1 point");
        stunval    = createLineEdit(widget, font, "20", 79, 659, 45, 20, "Stun: 1 point gets 2 points of STUN");
        strpoints  = createLabel(widget, font,   "0", 199, 250, 20);
        dexpoints  = createLabel(widget, font,   "0", 199, 275, 20);
        conpoints  = createLabel(widget, font,   "0", 199, 299, 20);
        intpoints  = createLabel(widget, font,   "0", 199, 324, 20);
        egopoints  = createLabel(widget, font,   "0", 199, 348, 20);
        prepoints  = createLabel(widget, font,   "0", 199, 373, 20);
        ocvpoints  = createLabel(widget, font,   "0", 199, 404, 20);
        dcvpoints  = createLabel(widget, font,   "0", 199, 429, 20);
        omcvpoints = createLabel(widget, font,   "0", 199, 454, 20);
        dmcvpoints = createLabel(widget, font,   "0", 199, 479, 20);
        spdpoints  = createLabel(widget, font,   "0", 199, 504, 20);
        pdpoints   = createLabel(widget, font,   "0", 199, 534, 20);
        edpoints   = createLabel(widget, font,   "0", 199, 559, 20);
        recpoints  = createLabel(widget, font,   "0", 199, 584, 20);
        endpoints  = createLabel(widget, font,   "0", 199, 609, 20);
        bodypoints = createLabel(widget, font,   "0", 199, 634, 20);
        stunpoints = createLabel(widget, font,   "0", 199, 659, 20);
        strroll    = createLabel(widget, font, "11-", 269, 250, 20);
        dexroll    = createLabel(widget, font, "11-", 269, 275, 20);
        conroll    = createLabel(widget, font, "11-", 269, 299, 20);
        introll    = createLabel(widget, font, "11-", 269, 324, 20);
        egoroll    = createLabel(widget, font, "11-", 269, 348, 20);
        preroll    = createLabel(widget, font, "11-", 269, 373, 20);
        totalcost  = createLabel(widget, font,   "0", 276, 659, 20);

        QString style = "QLineEdit { background: palegreen;"
                                 "   border-style: none;"
                                 " }";

        maximumend  = createLabel(widget, font, "20", 454, 247, 20);
        maximumbody = createLabel(widget, font, "10", 454, 272, 20);
        maximumstun = createLabel(widget, font, "20", 454, 296, 20);
        currentend  = createLineEdit(widget, font, style, "20", 542, 247, 97, 20, "You can keep track of your current END here");
        currentbody = createLineEdit(widget, font, style, "10", 542, 272, 97, 20, "You can keep track of your current BODY here");
        currentstun = createLineEdit(widget, font, style, "20", 542, 296, 97, 20, "You can keep track of your current STUN here");

        hthdamage         = createLabel(widget, font,   "2d6", 550, 376, 20);
        lift              = createLabel(widget, font, "100kg", 420, 401, 20);
        strendcost        = createLabel(widget, font,     "1", 603, 401, 20);
        phases.append(      createLabel(widget, font,      "", 451, 444, 20));
        phases.append(      createLabel(widget, font,      "", 464, 444, 20));
        phases.append(      createLabel(widget, font,      "", 478, 444, 20));
        phases.append(      createLabel(widget, font,      "", 492, 444, 20));
        phases.append(      createLabel(widget, font,      "", 506, 444, 20));
        phases.append(      createLabel(widget, font,     "X", 520, 444, 20));
        phases.append(      createLabel(widget, font,      "", 533, 444, 20));
        phases.append(      createLabel(widget, font,      "", 547, 444, 20));
        phases.append(      createLabel(widget, font,      "", 561, 444, 20));
        phases.append(      createLabel(widget, font,      "", 580, 444, 20));
        phases.append(      createLabel(widget, font,      "", 602, 444, 20));
        phases.append(      createLabel(widget, font,     "X", 624, 444, 20));
        baseocv           = createLabel(widget, font,     "3", 469, 471, 20);
        basedcv           = createLabel(widget, font,     "3", 589, 471, 20);
        baseomcv          = createLabel(widget, font,     "3", 483, 496, 20);
        basedmcv          = createLabel(widget, font,     "3", 603, 496, 20);
        combatskilllevels = createTextEdit(widget, narrow, "<b>Combat Skill Levels</b> ", 392, 520, 244, 145);
        presenceattack    = createLabel(widget, font,   "2d6", 573, 663, 20);

        movement    = createTableWidget(widget, font,
                                        {   "Type",           "Combat ", "Non-Combat" },
                                        { { "Run (12m)  ",    "12m",     "24m" },
                                          { "Swim (4m)  ",    "4m",       "8m" },
                                          { "H. Leap (4m)  ", "4m",       "8m" },
                                          { "V. Leap (2m)  ", "2m",       "4m" } }, 675, 225, 260, 195);
        movementsfx = createLabel(widget, font, "", 775, 423, 20);

        complications        = createTableWidget(widget, tableFont, { "Pts   ", "Complication", },
                                                 { }, 675, 590, 260, 455, "The things that make life difficult for your character", Selectable);
        totalcomplicationpts = createLabel(widget, font, "0/75", 675, 1045, 20);
        complicationsMenu    = createMenu(complications, font, { { "New",       &newComplication },
                                                                 { "Edit",      &editComplication },
                                                                 { "Delete",    &deleteComplication },
                                                                 { "-",         },
                                                                 { "Cut",       &cutComplication },
                                                                 { "Copy",      &copyComplication },
                                                                 { "Paste",     &pasteComplication },
                                                                 { "-",         },
                                                                 { "Move Up",   &moveComplicationUp },
                                                                 { "Move Down", &moveComplicationDown } } );

        attacksandmaneuvers = createTableWidget(widget, smallfont,
                                                {   "Maneuver",        "Phase", "OCV",   "DCV", "Effects"                  },
                                                { { "Block",           "½",     "+0",    "+0",  "Block, abort"             },
                                                  { "Brace",           "0",     "+2",    "½",   "+2 OCV vs R Mod"          },
                                                  { "Disarm",          "½",     "-2",    "+0",  "Disarm, STR v. STR"       },
                                                  { "Dodge",           "½",     "——",    "+3",  "Abort vs. all attacks"    },
                                                  { "Grab",            "½",     "-1",    "-2",  "Grab 2 limbs"             },
                                                  { "Grab By",         "½†",    "-3",    "-4",  "Move&Grab;+(v/10) to STR" },
                                                  { "Haymaker",        "½*",    "+0",    "-5",  "+4 DCs to attack"         },
                                                  { "Move By",         "½†",    "-2",    "-2",  "STR/2+v/10; take ⅓"       },
                                                  { "Move Through",    "½†",    "-v/10", "-3",  "STR+v/6; take ½ or full"  },
                                                  { "Multiple Attack", "1",     "var",   "½",   "Attack multiple times"    },
                                                  { "Set",             "1",     "+1",    "+0",  "Ranged attacks only"      },
                                                  { "Shove",           "½",     "-1",    "-1",  "Push 1m per 5 STR"        },
                                                  { "Strike",          "½",     "+0",    "+0",  "STR or weapon"            },
                                                  { "Throw",           "½",     "+0",    "+0",  "Throw w/STR dmg"          },
                                                  { "Trip",            "½",     "-1",    "-2",  "Knock target prone"       }
                                                }, 69, 739, 295, 495);

        defenses = createTableWidget(widget, font,
                                     {   "Type", "Amount/Effect" },
                                     { { "Normal PD ",      "2" },
                                       { "Resistant PD ",   "0" },
                                       { "Normal ED ",      "2" },
                                       { "Resistant ED ",   "0" },
                                       { "Mental Defense ", "0" },
                                       { "Power Defense ",  "0" },
                                       { "Flash Defense ",  "0" } }, 392, 739, 249, 270);

        perceptionroll = createLabel(widget, font, "11-", 569, 1066, 20);
        enhancedandunusualsenses = createTextEdit(widget, font, "<b>Enhanced and Unusual Senses</b>", 390, 1083, 249, 150);

        totalpoints       = createLabel(widget, font, "0/325", 855, 1135, 20);
        totalexperienceearned = createLineEdit(widget, font, "0", 853, 1159, 80, 20, "How much experience your character has earned");
        experiencespent   = createLabel(widget, font,     "0", 855, 1184, 20);
        experienceunspent = createLabel(widget, font,   "325", 855, 1209, 20);

        charactername2 = createLabel(widget, font,      "", 184, 1391, 20);
        height         = createLabel(widget, font,    "2m", 124, 1416, 20);
        weight         = createLabel(widget, font, "100kg", 249, 1416, 20);
        haircolor = createLineEdit(widget, font, "", 139, 1441, 58, 20, "Your characters hair color");
        eyecolor  = createLineEdit(widget, font, "", 261, 1441, 65, 20, "Your characters eye color");

        campaignname = createLineEdit(widget, font, "", 788, 1391, 145, 20, "The campaign your character is playing in");
        genre        = createLineEdit(widget, font, "", 721, 1416, 213, 20, "The kind of game (street level, superhero, galactic, etc)");
        gamemaster   = createLineEdit(widget, font, "", 764, 1441, 170, 20, "Who is running the game for your character");

        skillstalentsandperks         = createTableWidget(widget, tableFont, { "Cost", "Name                                  ", "Roll" },
                                                          { }, 73, 1521, 265, 990, "Things your character is skilled at or has a gift for", Selectable);
        totalskillstalentsandperkscost = createLabel(widget, font, "0", 73, 2510, 20);
        skillstalentsandperksMenu      = createMenu(skillstalentsandperks, font, { { "New",       &newSkillTalentOrPerk },
                                                                                   { "Edit",      &editSkillTalentOrPerk },
                                                                                   { "Delete",    &deleteSkillTalentOrPerk },
                                                                                   { "-",         },
                                                                                   { "Cut",       &cutSkillTalentOrPerk },
                                                                                   { "Copy",      &copySkillTalentOrPerk },
                                                                                   { "Paste",     &pasteSkillTalentOrPerk },
                                                                                   { "-",         },
                                                                                   { "Move Up",   &moveSkillTalentOrPerkUp },
                                                                                   { "Move Down", &moveSkillTalentOrPerkDown } } );

        powersandequipment          = createTableWidget(widget, tableFont,
                                                        { "Cost", "Name                ",
                                                          "Power/Equipment                                                            ",
                                                          "END" },
                                                        { }, 367, 1521, 570, 991, "Special powers and equipment for your character", Selectable);
        totalpowersandequipmentcost = createLabel(widget, font, "0", 367, 2511, 20);

        QMetaObject::connectSlotsByName(widget);
    }
};
