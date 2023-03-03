#pragma once

#include <QAction>
#include <QLineEdit>
#include <QGridLayout>
#include <QFontDatabase>
#include <QFrame>
#include <QHeaderView>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QPlainTextEdit>
#include <QScreen>
#include <QTableView>
#include <QTableWidget>
#include <QTextEdit>

#include "shared.h"

class Sheet_UI
{
private:
    void moveTo(QWidget* w, At p, Size s = { }) {
        QRect r = w->geometry();
        r.moveTo({ p.x, p.y });
        if (s.l != -1) r.setWidth(s.l);
        if (s.h != -1) r.setHeight(s.h);
        w->setGeometry(r);
    }

    QLabel* createLabel(QWidget* parent, QFont& font, QString val, At p, Size s) {
        QLabel* label = new QLabel(parent);
        label->setFont(font);
        label->setText(val);
        if (s.h == -1) moveTo(label, p, { s.h, s.l });
        else moveTo(label, p, s);
        widgets.append(label);
        return label;
    }

    QLabel* createImage(QWidget* parent, At p, Size s, bool selectable = false) {
        QLabel* label = new QLabel(parent);
        QString style = "QLabel { background: cyan;"
                              "   border-style: none;"
                              " }";
        if (selectable) label->setContextMenuPolicy(Qt::CustomContextMenu);
        label->setStyleSheet(style);
        if (s.h == -1) moveTo(label, p, { s.h, s.l });
        else moveTo(label, p, s);
        widgets.append(label);
        return label;
    }

    QLineEdit* createLineEdit(QWidget* parent, QFont& font, At p, Size s, QString w = "") {
        return createLineEdit(parent, font, "", p, s, w);
    }

    QLineEdit* createLineEdit(QWidget* parent, QFont& font, QString val, At p, Size s, QString w = "") {
        QString style = "QLineEdit { background: cyan;"
                                 "   border-style: none;"
                                 " }";
        return createLineEdit(parent, font, style, val, p, s, w);
    }

    QLineEdit* createLineEdit(QWidget* parent, QFont& font, QString style, QString val, At p, Size s, QString w = "") {
        QLineEdit* lineedit = new QLineEdit(parent);
        lineedit->setFont(font);
        lineedit->setText(val);
        lineedit->setStyleSheet(style);
        lineedit->setToolTip(w);
        moveTo(lineedit, p, s);
        widgets.append(lineedit);
        QFrame* line = new QFrame(parent);
        line->setLineWidth(1);
        line->setFrameShape(QFrame::HLine);
        QFont f = lineedit->font();
        QFontMetrics metrics(f);
        moveTo(line, { p.x, p.y + s.h - metrics.descent() + 2 }, { s.l, 1 });
        return lineedit;
    }

    QLineEdit* createNumEdit(QWidget* parent, QFont& font, At p, Size s, QString w = "") {
        return createNumEdit(parent, font, "", p, s, w);
    }

    QLineEdit* createNumEdit(QWidget* parent, QFont& font, QString val, At p, Size s, QString w = "") {
        QString style = "QLineEdit { background: cyan;"
                                 "   border-style: none;"
                                 " }";
        return createNumEdit(parent, font, style, val, p, s, w);
    }

    QLineEdit* createNumEdit(QWidget* parent, QFont& font, QString style, QString val, At p, Size s, QString w = "") {
        QLineEdit* lineedit = new QLineEdit(parent);
        lineedit->setFont(font);
        lineedit->setText(val);
        lineedit->setStyleSheet(style);
        lineedit->setToolTip(w);
        lineedit->setAlignment(Qt::AlignCenter);
        moveTo(lineedit, p, s);
        widgets.append(lineedit);
        QFrame* line = new QFrame(parent);
        line->setLineWidth(1);
        line->setFrameShape(QFrame::HLine);
        QFont f = lineedit->font();
        QFontMetrics metrics(f);
        moveTo(line, { p.x, p.y + s.h - metrics.descent() + 2 }, { s.l, 1 });
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

    QTableWidget* createTableWidget(QWidget* parent, QFont& font, QStringList headers, QList<QStringList> vals, At p, Size s,
                                    bool selectable = false) {
        return createTableWidget(parent, font, headers, vals, p, s, "", selectable);
    }

    QTableWidget* createTableWidget(QWidget* parent, QFont& font, QStringList headers, QList<QStringList> vals, At p, Size s,
                                    QString w, bool selectable = false) {
        QTableWidget* tablewidget = new QTableWidget(parent);
        tablewidget->setContextMenuPolicy(Qt::CustomContextMenu);
        tablewidget->setWordWrap(true);
        tablewidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        tablewidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        auto verticalHeader = tablewidget->verticalHeader();
        int pnt = font.pointSize();
        double dpiy = parent->screen()->physicalDotsPerInchY();
        double pntY = (pnt * dpiy) / 72.0;
        int sz = (pntY + 0.5);
        verticalHeader->setVisible(false);
        verticalHeader->setMinimumSectionSize(1);
        verticalHeader->setMaximumSectionSize(selectable ? s.l : sz);
        verticalHeader->setDefaultSectionSize(1);
        verticalHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
        auto horizontalHeader = tablewidget->horizontalHeader();
        horizontalHeader->setStretchLastSection(true);
        horizontalHeader->setMaximumSectionSize(s.l);
        horizontalHeader->setDefaultSectionSize(10);
        horizontalHeader->setDefaultAlignment(Qt::AlignLeft);
        horizontalHeader->setMaximumSize(s.l, sz);
        tablewidget->setSelectionMode(selectable ? QAbstractItemView::SingleSelection : QAbstractItemView::NoSelection);
        tablewidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        tablewidget->setFont(font);
        QString family = font.family();
        if (selectable)
            tablewidget->setStyleSheet("QTableWidget { selection-color: white;"
                                                   "   selection-background-color: DarkCyan;"
                                                   "   gridline-color: cyan;"
                                                   "   background-color: cyan;"
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
                                           QString("   font: %2pt \"%1\";").arg(family).arg((pnt * 8 + 5) / 10) +
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
        moveTo(tablewidget, p, s);
        int total = 0;
        for (i = 0; i < tablewidget->rowCount(); ++i) tablewidget->resizeRowToContents(i);
        for (i = 1; i < tablewidget->columnCount(); ++i) {
            tablewidget->resizeColumnToContents(i - 1);
            total += tablewidget->columnWidth(i - 1);
        }
        tablewidget->setColumnWidth(headers.size() - 1, s.l - total);
        widgets.append(tablewidget);

        return tablewidget;
    }

    QTextEdit* createTextEdit(QWidget* parent, QFont& font, QString val, At p, Size s) {
        return createTextEdit(parent, font, val, p, s, "");
    }

    QTextEdit* createTextEdit(QWidget* parent, QFont& font, QString val, At p, Size s, QString w) {
        QTextEdit* textedit = new QTextEdit(parent);
        textedit->setFont(font);
        textedit->setHtml(val);
        textedit->setStyleSheet("QTextEdit { border-style: none; }");
        textedit->setReadOnly(true);
        textedit->setToolTip(w);
        moveTo(textedit, p, s);
        widgets.append(textedit);
        return textedit;
    }

    QPlainTextEdit* createTextEditor(QWidget* parent, QFont& font, At p, Size s, QString w) {
        QPlainTextEdit* editwidget = new QPlainTextEdit(parent);
        editwidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        editwidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        int pnt = font.pointSize();
        editwidget->setFont(font);
        QString family = font.family();
        editwidget->setStyleSheet("QPlainTextEdit { selection-color: white;"
                                                "   selection-background-color: DarkCyan;"
                                                "   gridline-color: cyan;"
                                                "   background-color: cyan;"
                                                "   border-style: none;"
                                      + QString("   font: %2pt \"%1\";").arg(family).arg((pnt * 8 + 5) / 10) +
                                                "   color: black;"
                                                " }");
        editwidget->setToolTip(w);
        moveTo(editwidget, p, s);
        widgets.append(editwidget);

        return editwidget;
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

    QLabel*  image      = nullptr;
    QMenu*   imageMenu  = nullptr;
    QAction* newImage   = nullptr;
    QAction* clearImage = nullptr;

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
    QAction*      newSkillEnhancer               = nullptr;
    QAction*      editSkillTalentOrPerk          = nullptr;
    QAction*      deleteSkillTalentOrPerk        = nullptr;
    QAction*      cutSkillTalentOrPerk           = nullptr;
    QAction*      copySkillTalentOrPerk          = nullptr;
    QAction*      pasteSkillTalentOrPerk         = nullptr;
    QAction*      moveSkillTalentOrPerkUp        = nullptr;
    QAction*      moveSkillTalentOrPerkDown      = nullptr;

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

    QTableWidget* powersandequipment          = nullptr;
    QLabel*       totalpowersandequipmentcost = nullptr;
    QMenu*        powersandequipmentMenu      = nullptr;
    QAction*      newPowerOrEquipment         = nullptr;
    QAction*      editPowerOrEquipment        = nullptr;
    QAction*      deletePowerOrEquipment      = nullptr;
    QAction*      cutPowerOrEquipment         = nullptr;
    QAction*      copyPowerOrEquipment        = nullptr;
    QAction*      pastePowerOrEquipment       = nullptr;
    QAction*      movePowerOrEquipmentUp      = nullptr;
    QAction*      movePowerOrEquipmentDown    = nullptr;
    QGridLayout*  layout                      = nullptr;

    QPlainTextEdit* notes = nullptr;

    QList<QWidget*> widgets;
    QFont smallfont;

    Sheet_UI() { }

    void setupUi(QWidget* widget, QWidget* hidden)
    {
        layout = new QGridLayout();
        widget->setLayout(layout);

        int id = QFontDatabase::addApplicationFont(":/font/SegoeUIHS.ttf");
        QString family = QFontDatabase::applicationFontFamilies(id).at(0);
        QFont font({ family });
        font.setPointSize(12);

        QFont largeBoldFont = font;
        largeBoldFont.setPointSize(16);
        largeBoldFont.setBold(true);
        QFont largeNarrowBoldFont = largeBoldFont;
        largeNarrowBoldFont.setStretch(QFont::Stretch::Condensed);
        QFont largeFont = font;
        largeFont.setPointSize(16);
        QFont largeNarrowFont = largeFont;
        largeNarrowFont.setStretch(QFont::Stretch::SemiCondensed);
        QFont tableFont = font;
        tableFont.setPointSize(12);
        QFont narrow = font;
        narrow.setStretch(QFont::Stretch::SemiCondensed);
        QFont narrowTableFont = narrow;
        narrowTableFont.setPointSize(12);
        smallfont = font;
        smallfont.setPointSize(8);
        smallfont.setStretch(QFont::Stretch::SemiCondensed);
        QFont smallBoldFont = smallfont;
        smallBoldFont.setPointSize(12);
        smallBoldFont.setBold(true);
        smallBoldFont.setStretch(QFont::Stretch::Unstretched);
        QFont smallBoldNarrowFont = smallBoldFont;
        smallBoldNarrowFont.setStretch(QFont::Stretch::SemiCondensed);
        QFont smallNarrowFont = smallBoldNarrowFont;
        smallNarrowFont.setBold(false);
        QFont smallBoldWideFont = smallBoldFont;
        smallBoldWideFont.setPointSize(10);
        smallBoldWideFont.setStretch(QFont::Stretch::Expanded);
        QFont tinyFont = smallfont;
        tinyFont.setPointSize(11);
        tinyFont.setStretch(QFont::Stretch::ExtraCondensed);
        QFont tinyBoldFont = tinyFont;
        tinyBoldFont.setBold(true);

        createLabel(widget, largeNarrowBoldFont, "Character Name",       { 61, 76 },  { 175, 27 });
        createLabel(widget, largeNarrowFont,     "Alternate Identities", { 61, 105 }, { 175, 27 });
        createLabel(widget, largeNarrowFont,     "Player Name",          { 61, 135 }, { 175, 27 });

        charactername = createLineEdit(widget, largeBoldFont, { 190,  75 }, { 438, 27 }, "Characters superhero name");
        alternateids  = createLineEdit(widget, largeFont,     { 210, 104 }, { 418, 27 }, "Characters secret id, typically");
        playername    = createLineEdit(widget, largeFont,     { 162, 134 }, { 466, 27 }, "The players name");

        createLabel(widget, smallBoldWideFont, "CHARACTERISTICS", { 129, 196 }, { 175, 20 });
        createLabel(widget, smallBoldWideFont, "CHARACTERISTICS", { 128, 196 }, { 175, 20 });
        createLabel(widget, smallBoldFont, "Val",    { 91, 223 },  { 100, 22 });
        createLabel(widget, smallBoldFont, "Char",   { 139, 223 }, { 100, 22 });
        createLabel(widget, smallBoldFont, "Points", { 199, 223 }, { 100, 22 });
        createLabel(widget, smallBoldFont, "Roll",   { 267, 223 }, { 100, 22 });
        createLabel(widget, smallBoldFont, "STR",  { 139, 247 }, { 100, 22 });
        createLabel(widget, smallBoldFont, "DEX",  { 139, 272 }, { 100, 22 });
        createLabel(widget, smallBoldFont, "CON",  { 139, 296 }, { 100, 22 });
        createLabel(widget, smallBoldFont, "INT",  { 139, 321 }, { 100, 22 });
        createLabel(widget, smallBoldFont, "EGO",  { 139, 345 }, { 100, 22 });
        createLabel(widget, smallBoldFont, "PRE",  { 139, 370 }, { 100, 22 });
        createLabel(widget, smallBoldFont, "OCV",  { 139, 401 }, { 100, 22 });
        createLabel(widget, smallBoldFont, "DCV",  { 139, 426 }, { 100, 22 });
        createLabel(widget, smallBoldFont, "OMCV", { 139, 451 }, { 100, 22 });
        createLabel(widget, smallBoldFont, "DMCV", { 139, 476 }, { 100, 22 });
        createLabel(widget, smallBoldFont, "SPD",  { 139, 501 }, { 100, 22 });
        createLabel(widget, smallBoldFont, "PD",   { 139, 531 }, { 100, 22 });
        createLabel(widget, smallBoldFont, "ED",   { 139, 556 }, { 100, 22 });
        createLabel(widget, smallBoldFont, "REC",  { 139, 581 }, { 100, 22 });
        createLabel(widget, smallBoldFont, "END",  { 139, 606 }, { 100, 22 });
        createLabel(widget, smallBoldFont, "BODY", { 139, 631 }, { 100, 22 });
        createLabel(widget, smallBoldFont, "STUN", { 139, 656 }, { 100, 22 });

        strval     = createNumEdit(widget, font, "10", { 79, 248 }, { 45, 20 }, "Strength: 1 point");
        dexval     = createNumEdit(widget, font, "10", { 79, 273 }, { 45, 20 }, "Dexterity: 2 points");
        conval     = createNumEdit(widget, font, "10", { 79, 297 }, { 45, 20 }, "Constitution: 1 point");
        intval     = createNumEdit(widget, font, "10", { 79, 322 }, { 45, 20 }, "Intellegence: 1 point");
        egoval     = createNumEdit(widget, font, "10", { 79, 346 }, { 45, 20 }, "Ego: 1 point");
        preval     = createNumEdit(widget, font, "10", { 79, 371 }, { 45, 20 }, "Presence: 1 point");
        ocvval     = createNumEdit(widget, font,  "3", { 79, 402 }, { 45, 20 }, "Offensive Combat Value: 5 points");
        dcvval     = createNumEdit(widget, font,  "3", { 79, 427 }, { 45, 20 }, "Defensive Combat Value: 5 points");
        omcvval    = createNumEdit(widget, font,  "3", { 79, 452 }, { 45, 20 }, "Offensive Mental Combat Value: 3 points");
        dmcvval    = createNumEdit(widget, font,  "3", { 79, 477 }, { 45, 20 }, "Defensive Mental Combat Value: 3 points");
        spdval     = createNumEdit(widget, font,  "2", { 79, 502 }, { 45, 20 }, "Speed: 10 points");
        pdval      = createNumEdit(widget, font,  "2", { 79, 532 }, { 45, 20 }, "Physical Defense: 1 point");
        edval      = createNumEdit(widget, font,  "2", { 79, 557 }, { 45, 20 }, "Energy Defense: 1 point");
        recval     = createNumEdit(widget, font,  "4", { 79, 582 }, { 45, 20 }, "Recovery: 1 point");
        endval     = createNumEdit(widget, font, "20", { 79, 607 }, { 45, 20 }, "Endurance: 1 point gets 5 points of END");
        bodyval    = createNumEdit(widget, font, "10", { 79, 632 }, { 45, 20 }, "Body: 1 point");
        stunval    = createNumEdit(widget, font, "20", { 79, 657 }, { 45, 20 }, "Stun: 1 point gets 2 points of STUN");
        strpoints  = createLabel(widget, font,   "0", { 199, 248 }, 20);
        dexpoints  = createLabel(widget, font,   "0", { 199, 273 }, 20);
        conpoints  = createLabel(widget, font,   "0", { 199, 297 }, 20);
        intpoints  = createLabel(widget, font,   "0", { 199, 322 }, 20);
        egopoints  = createLabel(widget, font,   "0", { 199, 346 }, 20);
        prepoints  = createLabel(widget, font,   "0", { 199, 371 }, 20);
        ocvpoints  = createLabel(widget, font,   "0", { 199, 402 }, 20);
        dcvpoints  = createLabel(widget, font,   "0", { 199, 427 }, 20);
        omcvpoints = createLabel(widget, font,   "0", { 199, 452 }, 20);
        dmcvpoints = createLabel(widget, font,   "0", { 199, 477 }, 20);
        spdpoints  = createLabel(widget, font,   "0", { 199, 502 }, 20);
        pdpoints   = createLabel(widget, font,   "0", { 199, 532 }, 20);
        edpoints   = createLabel(widget, font,   "0", { 199, 557 }, 20);
        recpoints  = createLabel(widget, font,   "0", { 199, 582 }, 20);
        endpoints  = createLabel(widget, font,   "0", { 199, 607 }, 20);
        bodypoints = createLabel(widget, font,   "0", { 199, 632 }, 20);
        stunpoints = createLabel(widget, font,   "0", { 199, 657 }, 20);
        strroll    = createLabel(widget, font, "11-", { 269, 248 }, 20);
        dexroll    = createLabel(widget, font, "11-", { 269, 273 }, 20);
        conroll    = createLabel(widget, font, "11-", { 269, 297 }, 20);
        introll    = createLabel(widget, font, "11-", { 269, 322 }, 20);
        egoroll    = createLabel(widget, font, "11-", { 269, 347 }, 20);
        preroll    = createLabel(widget, font, "11-", { 269, 372 }, 20);

        createLabel(widget, smallBoldFont, "Total Cost", { 276, 631 }, { 100, 22 });
        totalcost  = createLabel(widget, font,   "0", { 276, 657 }, 20);

        createLabel(widget, smallBoldWideFont, "CURRENT STATUS", { 435, 196 }, { 175, 20 });
        createLabel(widget, smallBoldWideFont, "CURRENT STATUS", { 434, 196 }, { 175, 20 });
        createLabel(widget, smallBoldFont, "Maximum", { 454, 222 }, { 100, 22 });
        createLabel(widget, smallBoldFont, "Current", { 542, 222 }, { 100, 22 });
        createLabel(widget, smallBoldFont, "END",  { 394, 245 }, { 100, 22 });
        createLabel(widget, smallBoldFont, "BODY", { 394, 270 }, { 100, 22 });
        createLabel(widget, smallBoldFont, "STUN", { 394, 294 }, { 100, 22 });

        QString style = "QLineEdit { background: palegreen;"
                                 "   border-style: none;"
                                 " }";

        maximumend  = createLabel(widget, font, "20", { 454, 247 }, 20);
        maximumbody = createLabel(widget, font, "10", { 454, 272 }, 20);
        maximumstun = createLabel(widget, font, "20", { 454, 296 }, 20);
        currentend  = createLineEdit(widget, font, style, "20", { 542, 247 }, { 97, 20 }, "You can keep track of your current END here");
        currentbody = createLineEdit(widget, font, style, "10", { 542, 272 }, { 97, 20 }, "You can keep track of your current BODY here");
        currentstun = createLineEdit(widget, font, style, "20", { 542, 296 }, { 97, 20 }, "You can keep track of your current STUN here");

        createLabel(widget, smallBoldWideFont, "VITAL INFORMATION", { 420, 347 }, { 200, 20 });
        createLabel(widget, smallBoldWideFont, "VITAL INFORMATION", { 419, 347 }, { 200, 20 });
        createLabel(widget, smallBoldNarrowFont, "HTH Damage",      { 397, 374 }, { 100, 22 });
        createLabel(widget, smallNarrowFont,     "(STR/5)d6",       { 492, 374 }, { 100, 22 });
        createLabel(widget, smallNarrowFont,     "Lift",            { 397, 399 }, { 100, 22 });
        createLabel(widget, smallNarrowFont,     "STR END Cost",    { 520, 399 }, { 100, 22 });
        createLabel(widget, smallNarrowFont,     "1   2   3   4  5   6   7  8   9   10   11   12", { 450, 425 }, { 200, 22 });
        createLabel(widget, smallBoldNarrowFont, "Phases",          { 397, 442 }, { 100, 22 });
        createLabel(widget, smallBoldNarrowFont, "Base OCV",        { 395, 469 }, { 100, 22 });
        createLabel(widget, smallBoldNarrowFont, "Base DCV",        { 515, 469 }, { 100, 22 });
        createLabel(widget, smallBoldNarrowFont, "Base OMCV",       { 395, 494 }, { 100, 22 });
        createLabel(widget, smallBoldNarrowFont, "Base DMCV",       { 515, 494 }, { 100, 22 });
        createLabel(widget, smallBoldNarrowFont, "Presence Attack", { 395, 661 }, { 150, 22 });
        createLabel(widget, smallNarrowFont,     "(PRE/5)d6",       { 510, 661 }, { 100, 22 });

        hthdamage         = createLabel(widget, font,   "2d6", { 550, 376 }, 20);
        lift              = createLabel(widget, font, "100kg", { 420, 401 }, 20);
        strendcost        = createLabel(widget, font,     "1", { 603, 401 }, 20);
        phases.append(      createLabel(widget, font,      "", { 451, 444 }, 20));
        phases.append(      createLabel(widget, font,      "", { 464, 444 }, 20));
        phases.append(      createLabel(widget, font,      "", { 478, 444 }, 20));
        phases.append(      createLabel(widget, font,      "", { 492, 444 }, 20));
        phases.append(      createLabel(widget, font,      "", { 506, 444 }, 20));
        phases.append(      createLabel(widget, font,     "X", { 520, 444 }, 20));
        phases.append(      createLabel(widget, font,      "", { 533, 444 }, 20));
        phases.append(      createLabel(widget, font,      "", { 547, 444 }, 20));
        phases.append(      createLabel(widget, font,      "", { 561, 444 }, 20));
        phases.append(      createLabel(widget, font,      "", { 580, 444 }, 20));
        phases.append(      createLabel(widget, font,      "", { 602, 444 }, 20));
        phases.append(      createLabel(widget, font,     "X", { 624, 444 }, 20));
        baseocv           = createLabel(widget, font,     "3", { 469, 471 }, 20);
        basedcv           = createLabel(widget, font,     "3", { 589, 471 }, 20);
        baseomcv          = createLabel(widget, font,     "3", { 483, 496 }, 20);
        basedmcv          = createLabel(widget, font,     "3", { 603, 496 }, 20);
        combatskilllevels = createTextEdit(widget, narrow, "<b>Combat Skill Levels</b> ", { 392, 520 }, { 244, 145 });
        presenceattack    = createLabel(widget, font,   "2d6", { 573, 663 }, 20);

        createLabel(widget, smallBoldWideFont, "MOVEMENT", { 748, 196 }, { 175, 20 });
        createLabel(widget, smallBoldWideFont, "MOVEMENT", { 747, 196 }, { 175, 20 });
        createLabel(widget, smallNarrowFont, "Movement SFX", { 678, 420 }, { 100, 22 });

        movement    = createTableWidget(widget, narrowTableFont,
                                        {   "Type",           "Combat ", "Non-Combat" },
                                        { { "Run (12m)  ",    "12m",     "24m" },
                                          { "Swim (4m)  ",    "4m",       "8m" },
                                          { "H. Leap (4m)  ", "4m",       "8m" },
                                          { "V. Leap (2m)  ", "2m",       "4m" } }, { 675, 225 }, { 260, 195 });
        movementsfx = createLabel(widget, font, "", { 775, 423 }, 20);

        createLabel(widget, smallBoldWideFont, "RANGE MODIFIERS", { 718, 474 }, { 175, 20 });
        createLabel(widget, smallBoldWideFont, "RANGE MODIFIERS", { 717, 474 }, { 175, 20 });
        createLabel(widget, tinyBoldFont, "Range(m)", { 678, 497 }, { 100, 22 });
        createLabel(widget, tinyFont,     "0-8",      { 737, 497 }, { 100, 22 });
        createLabel(widget, tinyFont,     "9-16",     { 760, 497 }, { 100, 22 });
        createLabel(widget, tinyFont,     "17-32",    { 788, 497 }, { 100, 22 });
        createLabel(widget, tinyFont,     "33-64",    { 822, 497 }, { 100, 22 });
        createLabel(widget, tinyFont,     "65-128",   { 858, 497 }, { 100, 22 });
        createLabel(widget, tinyFont,     "128-256",  { 898, 497 }, { 100, 22 });
        createLabel(widget, tinyBoldFont, "OCV Mod",  { 680, 514 }, { 100, 22 });
        createLabel(widget, tinyFont,     "-0",       { 741, 514 }, { 100, 22 });
        createLabel(widget, tinyFont,     "-2",       { 766, 514 }, { 100, 22 });
        createLabel(widget, tinyFont,     "-4",       { 796, 514 }, { 100, 22 });
        createLabel(widget, tinyFont,     "-6",       { 830, 514 }, { 100, 22 });
        createLabel(widget, tinyFont,     "-8",       { 868, 514 }, { 100, 22 });
        createLabel(widget, tinyFont,     "-10",      { 909, 514 }, { 100, 22 });

        image     = createImage(widget, { 663, 555 }, { 285, 533 }, Selectable);
        imageMenu = createMenu(image, font, { { "New Image",   &newImage   },
                                              { "Clear Image", &clearImage } } );

        createLabel(widget, smallBoldWideFont, "ATTACKS & MANEUVERS", { 104, 712 }, { 230, 20 });
        createLabel(widget, smallBoldWideFont, "ATTACKS & MANEUVERS", { 103, 712 }, { 230, 20 });

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
                                                }, { 69, 739 }, { 295, 495 });

        createLabel(widget, smallBoldWideFont, "DEFENSES", { 468, 712 }, { 225, 20 });
        createLabel(widget, smallBoldWideFont, "DEFENSES", { 467, 712 }, { 225, 20 });

        defenses = createTableWidget(widget, font,
                                     {   "Type", "Amount/Effect" },
                                     { { "Normal PD ",      "2" },
                                       { "Resistant PD ",   "0" },
                                       { "Normal ED ",      "2" },
                                       { "Resistant ED ",   "0" },
                                       { "Mental Defense ", "0" },
                                       { "Power Defense ",  "0" },
                                       { "Flash Defense ",  "0" } }, { 392, 739 }, { 249, 270 });

        createLabel(widget, smallBoldWideFont, "SENSES", { 480, 1038 }, { 225, 20 });
        createLabel(widget, smallBoldWideFont, "SENSES", { 479, 1038 }, { 225, 20 });
        createLabel(widget, smallBoldNarrowFont, "Perception Roll", { 395, 1063 }, { 150, 22 });
        createLabel(widget, smallNarrowFont,     "(9+INT/5)",       { 506, 1063 }, { 100, 22 });

        perceptionroll = createLabel(widget, font, "11-", { 569, 1066 }, 20);
        enhancedandunusualsenses = createTextEdit(widget, font, "<b>Enhanced and Unusual Senses</b>", { 390, 1083 }, { 249, 150 });

        createLabel(widget, smallBoldWideFont, "EXPERIENCE POINTS", { 713, 1106 }, { 225, 20 });
        createLabel(widget, smallBoldWideFont, "EXPERIENCE POINTS", { 712, 1106 }, { 225, 20 });
        createLabel(widget, smallBoldNarrowFont, "Total Points",            { 675, 1133 }, { 200, 22 });
        createLabel(widget, smallBoldNarrowFont, "Total Experience Earned", { 675, 1156 }, { 300, 22 });
        createLabel(widget, smallNarrowFont,     "Experience Spent",        { 675, 1181 }, { 300, 22 });
        createLabel(widget, smallNarrowFont,     "Experience Unspent",      { 675, 1206 }, { 300, 22 });

        totalpoints       = createLabel(widget, font, "0/325", { 855, 1135 }, 20);
        totalexperienceearned = createLineEdit(widget, font, "0", { 853, 1159 }, { 80, 20 }, "How much experience your character has earned");
        experiencespent   = createLabel(widget, font,     "0", { 855, 1184 }, 20);
        experienceunspent = createLabel(widget, font,   "325", { 855, 1209 }, 20);

        createLabel(widget, smallBoldWideFont, "CHARACTER INFORMATION", { 78, 1362 }, { 250, 20 });
        createLabel(widget, smallBoldWideFont, "CHARACTER INFORMATION", { 77, 1362 }, { 250, 20 });
        createLabel(widget, smallBoldNarrowFont, "Character Name", {  66, 1388 }, { 200, 22 });
        createLabel(widget, smallBoldNarrowFont, "Height",         {  66, 1413 }, { 200, 22 });
        createLabel(widget, smallBoldNarrowFont, "Weight",         { 196, 1413 }, { 200, 22 });
        createLabel(widget, smallBoldNarrowFont, "Hair Color",     {  66, 1439 }, { 200, 22 });
        createLabel(widget, smallBoldNarrowFont, "Eye Color",      { 196, 1439 }, { 200, 22 });

        charactername2 = createLabel(widget, font,      "", { 184, 1391 }, 20);
        height         = createLabel(widget, font,    "2m", { 124, 1416 }, 20);
        weight         = createLabel(widget, font, "100kg", { 249, 1416 }, 20);
        haircolor = createLineEdit(widget, font, "", { 139, 1441 }, { 58, 20 }, "Your characters hair color");
        eyecolor  = createLineEdit(widget, font, "", { 262, 1441 }, { 64, 20 }, "Your characters eye color");

        createLabel(widget, smallBoldWideFont, "CAMPAIGN INFORMATION", { 690, 1362 }, { 250, 20 });
        createLabel(widget, smallBoldWideFont, "CAMPAIGN INFORMATION", { 689, 1362 }, { 250, 20 });
        createLabel(widget, smallBoldNarrowFont, "Campaign Name", { 672, 1389 }, { 200, 22 });
        createLabel(widget, smallBoldNarrowFont, "Genre",         { 672, 1413 }, { 200, 22 });
        createLabel(widget, smallBoldNarrowFont, "Gamemaster",    { 672, 1439 }, { 200, 22 });

        campaignname = createLineEdit(widget, font, "", { 788, 1391 }, { 145, 20 }, "The campaign your character is playing in");
        genre        = createLineEdit(widget, font, "", { 721, 1416 }, { 213, 20 }, "The kind of game (street level, superhero, galactic, etc)");
        gamemaster   = createLineEdit(widget, font, "", { 764, 1441 }, { 170, 20 }, "Who is running the game for your character");

        createLabel(widget, smallBoldWideFont, "SKILLS, PERKS, & TALENTS", { 82, 1495 }, { 250, 20 });
        createLabel(widget, smallBoldWideFont, "SKILLS, PERKS, & TALENTS", { 83, 1495 }, { 250, 20 });
        createLabel(widget, smallBoldNarrowFont, "Total Skills,Perks,& Talents Cost", { 112, 2057 }, { 300, 22 });

        skillstalentsandperks         = createTableWidget(widget, tableFont, { "Cost", "Name                        ", "Roll" },
                                                          { }, { 73, 1521 }, { 265, 535 }, "Things your character is skilled at or has a gift for", Selectable);
        totalskillstalentsandperkscost = createLabel(widget, font, "0", { 73, 2058 }, 20);
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
        createLabel(widget, smallBoldWideFont, "COMPLICATIONS", { 122, 2103 }, { 175, 20 });
        createLabel(widget, smallBoldWideFont, "COMPLICATIONS", { 123, 2103 }, { 175, 20 });
        createLabel(widget, smallBoldNarrowFont, "Total Complications Points", { 117, 2512 }, { 200, 22 });

        complications        = createTableWidget(widget, tableFont, { "Pts   ", "Complication" },
                                                 { }, { 73, 2130 }, { 260, 383 }, "The things that make life difficult for your character", Selectable);
        totalcomplicationpts = createLabel(widget, font, "0/75", { 73, 2513 }, 20);
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

        createLabel(widget, smallBoldWideFont, "POWERS AND EQUIPMENT", { 537, 1496 }, { 250, 20 });
        createLabel(widget, smallBoldWideFont, "POWERS AND EQUIPMENT", { 536, 1496 }, { 250, 20 });
        createLabel(widget, smallBoldNarrowFont, "Total Powers/Equipment Cost", { 410, 2510 }, { 300, 22 });

        powersandequipment          = createTableWidget(widget, tableFont,
                                                        { "Cost", "Name              ",
                                                          "Power/Equipment                                                 ",
                                                          "END" },
                                                        { }, { 367, 1521 }, { 570, 991 }, "Special powers and equipment for your character", Selectable);
        totalpowersandequipmentcost = createLabel(widget, font, "0", { 367, 2511 }, 20);
        powersandequipmentMenu      = createMenu(powersandequipment, font, { { "New",       &newPowerOrEquipment },
                                                                             { "Edit",      &editPowerOrEquipment },
                                                                             { "Delete",    &deletePowerOrEquipment },
                                                                             { "-",         },
                                                                             { "Cut",       &cutPowerOrEquipment },
                                                                             { "Copy",      &copyPowerOrEquipment },
                                                                             { "Paste",     &pastePowerOrEquipment },
                                                                             { "-",         },
                                                                             { "Move Up",   &movePowerOrEquipmentUp },
                                                                             { "Move Down", &movePowerOrEquipmentDown } } );

        QMetaObject::connectSlotsByName(widget);

        // title all of the boxes
        layout = new QGridLayout();
        hidden->setLayout(layout);
        createLabel(hidden, smallBoldWideFont, "KNOCKBACK MODIFIERS", { 90, 82 }, { 275, 20 });
        createLabel(hidden, smallBoldWideFont, "KNOCKBACK MODIFIERS", { 89, 82 }, { 275, 20 });

        createLabel(hidden, smallBoldWideFont, "WALL BODY", { 750, 80 }, { 275, 20 });
        createLabel(hidden, smallBoldWideFont, "WALL BODY", { 749, 80 }, { 275, 20 });

        createLabel(hidden, smallBoldWideFont, "NOTES", { 625, 200 }, { 275, 20 });
        createLabel(hidden, smallBoldWideFont, "NOTES", { 624, 200 }, { 275, 20 });
        notes = createTextEditor(hidden, font, { 365, 229 }, { 575, 1008 }, "Game notes");

        createLabel(hidden, smallBoldWideFont, "HIT LOCATION CHART", { 95, 245 }, { 275, 20 });
        createLabel(hidden, smallBoldWideFont, "HIT LOCATION CHART", { 94, 245 }, { 275, 20 });

        createLabel(hidden, smallBoldWideFont, "COMBAT MODIFIERS", { 105, 669 }, { 275, 20 });
        createLabel(hidden, smallBoldWideFont, "COMBAT MODIFIERS", { 104, 669 }, { 275, 20 });

        createLabel(hidden, smallBoldWideFont, "SKILL MODIFIERS", { 115, 882 }, { 275, 20 });
        createLabel(hidden, smallBoldWideFont, "SKILL MODIFIERS", { 114, 882 }, { 275, 20 });
        // put PlainText editor in the biiig box

        QMetaObject::connectSlotsByName(hidden);
        hidden->setVisible(false);
    }
};
