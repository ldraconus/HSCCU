#pragma once

#include "fraction.h"
#include <QAction>
#include <QLineEdit>
#include <QGridLayout>
#if defined(__wasm__) || defined(Q_OS_ANDROID)
#include <QFile>
#endif
#include <QApplication>
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

class ClickableLabel: public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget* parent = Q_NULLPTR)
        : QLabel(parent) {
    }

signals:
    void clicked();

protected:
#if defined(__wasm__)
    void mousePressEvent(QMouseEvent* me) override {
        if (me->button() == Qt::RightButton) emit customContextMenuRequested(me->pos());
        else                                 emit clicked();
    }
#elif defined(Q_OS_ANDROID)
    void mousePressEvent(QMouseEvent* me) override { if (me->button()== Qt::LeftButton) mPnt = me->pos(); }
    void mouseReleaseEvent(QMouseEvent* me) override {
        if (me->button() == Qt::LeftButton
            && (me->pos() - mPnt).manhattanLength() < QApplication::startDragDistance()) emit customContextMenuRequested(me->globalPosition().toPoint());
        else                                                                             QLabel::mouseReleaseEvent(me);
    }

    QPoint mPnt;
#else
    void mousePressEvent(QMouseEvent* me) override {
        if (me->button() == Qt::RightButton) emit customContextMenuRequested(me->globalPosition().toPoint());
        else                                 emit clicked();
    }
#endif
};

class ClickableTable: public QTableWidget {
    Q_OBJECT

public:
    explicit ClickableTable(QWidget* parent = Q_NULLPTR)
        : QTableWidget(parent) {
#ifdef __wasm__
//        this->setAttribute(Qt::WA_AcceptTouchEvents);
#endif
    }

signals:
    void showmenu();

protected:
#if defined(__wasm__)
    void mousePressEvent(QMouseEvent* me) override {
        if (me->button() == Qt::RightButton)     emit customContextMenuRequested(me->pos());
        else if (me->button() == Qt::LeftButton) emit clicked(indexAt(me->pos()));
        else                                     QTableWidget::mousePressEvent(me);
    }
#elif defined(Q_OS_ANDROID)
    void mousePressEvent(QMouseEvent* me) override { if (me->button()== Qt::LeftButton) mPnt = me->pos(); }
    void mouseReleaseEvent(QMouseEvent* me) override {
        if (me->button() == Qt::LeftButton
            && (me->pos() - mPnt).manhattanLength() < QApplication::startDragDistance()) emit customContextMenuRequested(me->globalPosition().toPoint());
        else                                                                             QTableWidget::mouseReleaseEvent(me);
    }

    QPoint mPnt;
#else
    void mousePressEvent(QMouseEvent* me) override {
        if (me->button() == Qt::RightButton) emit customContextMenuRequested(me->globalPosition().toPoint());
        else                                 QTableWidget::mousePressEvent(me);
    }
#endif
};

class Sheet_UI {
public:
    ClickableTable* createTableWidget(QWidget* parent, QFont& fontIn, QStringList headers, QList<QStringList> vals, At p, Size s,
                                                  bool selectable = false, bool label = true) {
        return createTableWidget(parent, fontIn, headers, vals, p, s, "", selectable, label);
    }

    void rebuildTable(QTableWidget* tablewidget, QStringList headers, QList<QStringList> vals, bool selectable = false, bool label = false) {
        font = QFont("Segoe UI", StandardFontSize);
        QFont narrow = font;
        narrow.setStretch(QFont::Stretch::SemiCondensed);
        QFont narrowTblFont = narrow;
        narrowTableFont.setPointSize(TinyFontSize);

        tablewidget->setColumnCount(int(headers.size()));
        tablewidget->setRowCount(int(vals.size()));
        tablewidget->setHorizontalHeaderLabels(headers);
#ifdef __wasm__
        int pnt = narrowTblFont.pointSize();
        QFont temp = font;
        temp.setPointSize(pnt * 8 + 0.5); // NOLINT
        tablewidget->setFont(temp);
#else
        tablewidget->setFont(narrowTblFont);
#endif
        tablewidget->setHorizontalHeaderLabels(headers);
        int i = 0;
        for (i = 0; i < vals.size(); ++i) {
            for (int j = 0; j < vals[i].size(); ++j) {
                if (label) {
                    QLabel* cell = new QLabel(vals[i][j]);
                    cell->setFont(narrowTableFont);
                    cell->setStyleSheet("color: #000;");
                    tablewidget->setCellWidget(i, j, cell);
                } else {
                    QTableWidgetItem* lbl = new QTableWidgetItem(vals[i][j]);
                    lbl->setFont(narrowTableFont);
                    lbl->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
                    if (selectable) lbl->setFlags(Qt::ItemIsSelectable);
                    else lbl->setFlags(Qt::NoItemFlags);
                    tablewidget->setItem(i, j, lbl);
                }
            }
        }
        for (i = 0; i < tablewidget->rowCount(); ++i) tablewidget->resizeRowToContents(i);
#ifdef __wasm__
        for (i = 0; i < tablewidget->columnCount(); ++i) tablewidget->resizeColumnToContents(i);
#else
        int total = 0;
        for (i = 1; i < tablewidget->columnCount(); ++i) {
            tablewidget->resizeColumnToContents(i - 1);
            total += tablewidget->columnWidth(i - 1);
        }
        tablewidget->setColumnWidth(int(headers.size()) - 1, tablewidget->geometry().size().width() - total);
#endif
    }

private:
    void moveTo(QWidget* w, At p, Size s = { }) {
        QRect r = w->geometry();
        r.moveTo({ p.x(), p.y() });
        if (s.l() != -1) r.setWidth(s.l());
        if (s.h() != -1) r.setHeight(s.h());
        w->setGeometry(r);
    }

    QLabel* createLabel(QWidget* parent, QFont& fontIn, QString val, At p, Size s, bool header = false) {
        QLabel* label = new QLabel(parent);
        label->setFont(fontIn);
        label->setText(val);
        label->setStyleSheet("color: #000; background: transparent;");
        if (s.h() == -1) moveTo(label, p, { s.h(), s.l() });
        else moveTo(label, p, s);
        if (parent != page3) widgets.append(label);
        else hiddenWidgets.append(label);
        if (header) headerWidgets.append(label);
        return label;
    }

    QLabel* createLabel(QWidget* parent, QFont& fontIn, QString val, At p, QString max, bool header = false) {
        QFontMetrics headerMetrics(fontIn);
        int w = headerMetrics.horizontalAdvance(max);
        int h = headerMetrics.height();
        return createLabel(parent, fontIn, val, p, { w, h }, header);
    }

    QLabel* createLabel(QWidget* parent, QFont& fontIn, QString val, At p, bool header = false) {
        QFontMetrics headerMetrics(fontIn);
        int w = headerMetrics.horizontalAdvance(val);
        int h = headerMetrics.height();
        return createLabel(parent, fontIn, val, p, { w, h }, header);
    }

    void createBlockHeader(QWidget* parent, QFont& fontIn, int blockX, int y, int width, const QString& str) {
        QFontMetrics headerMetrics(fontIn);
        int w = headerMetrics.horizontalAdvance(str);
        int h = headerMetrics.ascent();
        int x = (width - w) / 2 + blockX;
        createLabel(parent, fontIn, str, { x, y }, { w, h }); // NOLINT
#if !defined(unix)
        createLabel(parent, fontIn, str, { x - 1, y }, { w, h }); // NOLINT
#endif
    }

    ClickableLabel* createImage(QWidget* parent, At p, Size s, const QString imageName, bool selectable = false) {
        ClickableLabel* label = createImage(parent, p, s);
        QPixmap pixmap(imageName);
        pixmap = pixmap.scaled(s.l(), s.h(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        label->setPixmap(pixmap);
        label->setAlignment(Qt::AlignCenter);
        return label;
    }

    ClickableLabel* createImage(QWidget* parent, At p, Size s, bool selectable = false) {
        ClickableLabel* label = new ClickableLabel(parent);
        QString style = "QLabel { background: cyan;"
                              "   border-style: none;"
                              " }";
#ifndef __wasm__
        if (selectable) label->setContextMenuPolicy(Qt::CustomContextMenu);
#endif
        if (selectable) label->setStyleSheet(style);
        if (s.h() == -1) moveTo(label, p, { s.h(), s.l() });
        else moveTo(label, p, s);
        if (parent != page3) widgets.append(label);
        else hiddenWidgets.append(label);
        return label;
    }

    QLineEdit* createLineEdit(QWidget* parent, QFont& fontIn, At p, Size s, QString w = "") {
        return createLineEdit(parent, fontIn, "", p, s, w);
    }

    QLineEdit* createLineEdit(QWidget* parent, QFont& fontIn, QString val, At p, Size s, QString w = "") {
        QString style = "QLineEdit { background: cyan;"
                                 "   color: #000;"
                                 "   border: 1px cyan;"
                                 "   border-style: none;"
                                 " }"
                        "QToolTip { border: 1px solid #555555;"
                        "           padding: 3px;"
                        "           background-color: #333333;"
                        "           color: #ffffff;"
                        "}";
        return createLineEdit(parent, fontIn, style, val, p, s, w);
    }

    QLineEdit* createLineEdit(QWidget* parent, QFont& fontIn, QString style, QString val, At p, Size s, QString w = "") {
        QLineEdit* lineedit = new QLineEdit(parent);
        lineedit->setFont(fontIn);
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
        moveTo(line, { p.x(), p.y() + s.h() - metrics.descent() + 2 }, { s.l(), 1 });
        return lineedit;
    }

    QLineEdit* createNumEdit(QWidget* parent, QFont& fontIn, At p, Size s, QString w = "") {
        return createNumEdit(parent, fontIn, "", p, s, w);
    }

    QLineEdit* createNumEdit(QWidget* parent, QFont& fontIn, QString val, At p, Size s, QString w = "") {
        QString style = "QLineEdit { background: cyan;"
                                 "   color: #000;"
                                 "   border: 1px cyan;"
                                 "   border-style: none;"
                                 " }"
                        "QToolTip { border: 1px solid #555555;"
                        "           padding: 3px;"
                        "           background-color: #333333;"
                        "           color: #ffffff;"
                        "}";
        return createNumEdit(parent, fontIn, style, val, p, s, w);
    }

    QLineEdit* createNumEdit(QWidget* parent, QFont& fontIn, QString style, QString val, At p, Size s, QString w = "") {
        QLineEdit* lineedit = new QLineEdit(parent);
        lineedit->setFont(fontIn);
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
        moveTo(line, { p.x(), p.y() + s.h() - metrics.descent() + 2 }, { s.l(), 1 });
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
        QString text;
        QAction** action;
    };

    QMenu* createMenu(QWidget* parent, QFont& fontIn, QList<menuItems> items) {
        QMenu* menu = new QMenu(parent);
        menu->setStyleSheet("QMenu { color: #000; background: #bbb; }");
        menu->setFont(fontIn);
        for (auto& item: items) {
            if (item.action == nullptr) {
                menu->addSeparator();
                continue;
            }
            QAction*& action = *item.action;
            action = new QAction(item.text);
            action->setFont(fontIn);
            menu->addAction(action);
        }
        return menu;
    }

    static constexpr bool Selectable = true;
    static constexpr bool NoSelectable = false;
    static constexpr bool NoLabel = false;

    static constexpr double Points = 72.0;
    static constexpr double Half   = 0.5;

    struct ColumnHeader {
        int width;
        QString header;
    };

    ClickableTable* createTableWidget(QWidget* parent, QFont& fontIn, const QList<ColumnHeader>& headers, QList<QStringList> vals, At p, Size s,
                                      QString w = "", bool selectable = false, bool label = true) {
        ClickableTable* tablewidget = new ClickableTable(parent);
        tablewidget->setWordWrap(true);
        tablewidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        tablewidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        QFontMetrics metrics(fontIn);
        int pnt = fontIn.pointSize();
        int sz = metrics.height();
#ifdef __wasm__
        QFont temp = fontIn;
        temp.setPointSize(pnt * 8 + 0.5); // NOLINT
        tablewidget->setFont(temp);
#else
        tablewidget->setFont(fontIn);
#endif
        auto verticalHeader = tablewidget->verticalHeader();
        verticalHeader->setVisible(false);
        verticalHeader->setMinimumSectionSize(sz);
        verticalHeader->setMaximumSectionSize(selectable ? s.l() : sz);
        verticalHeader->setDefaultSectionSize(sz);
        verticalHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
        auto horizontalHeader = tablewidget->horizontalHeader();
        horizontalHeader->setStretchLastSection(true);
        horizontalHeader->setMaximumSectionSize(s.l());
        horizontalHeader->setDefaultSectionSize(10); // NOLINT
        horizontalHeader->setDefaultAlignment(Qt::AlignLeft);
        horizontalHeader->setMaximumSize(s.l(), sz);
        tablewidget->setSelectionMode(selectable ? QAbstractItemView::SingleSelection : QAbstractItemView::NoSelection);
        tablewidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        QString family = fontIn.family();
        if (selectable)
            tablewidget->setStyleSheet("QTableWidget { selection-color: black;"
                                       "   selection-background-color: darkcyan;"
                                       "   gridline-color: cyan;"
                                       "   background-color: cyan;"
                                       "   border: 1px cyan;"
                                       "   border-style: none;"
                                       + QString("   font: %2pt \"%1\";").arg(family).arg(pnt) + // NOLINT
                                       "   color: black;"
                                       " } "
                                       "QHeaderView::section { background-color: white;"
                                       "   border-style: none;"
                                       "   color: black;" +
                                       QString("   font: bold %2pt \"%1\";").arg(family).arg(pnt) + // NOLINT
                                       " } "
                                       "QTableWidget::item:selected { background: darkcyan; "
                                       "   color: black; "
                                       "   border: 1px darkcyan; "
                                       "   border-style: none; "
                                       "} "
                                       "QToolTip { border: 1px solid #555555;"
                                       "           padding: 3px;"
                                       "           background-color: #333333;"
                                       "           color: #ffffff;"
                                       "}");
        else
            tablewidget->setStyleSheet("QTableWidget { selection-color: transparent;"
                                       "   selection-background-color: transparent;"
                                       "   gridline-color: transparent;"
                                       "   border: 1px transparent;;"
                                       "   border-style: none;"
                                       "   background-color: transparent;"
                                       "   color: black;" +
                                       QString("   font: %2pt \"%1\";").arg(family).arg(pnt) + // NOLINT
                                       " } "
                                       "QHeaderView::section { background-color: white;"
                                       "   border-style: none;"
                                       "   color: black;" +
                                       QString("   font: bold %2pt \"%1\";").arg(family).arg(pnt) +
                                       " }"
                                       "QToolTip { border: 1px solid #555555;"
                                       "           padding: 3px;"
                                       "           background-color: #333333;"
                                       "           color: #ffffff;"
                                       "}");
        tablewidget->setColumnCount(int(headers.size()));
        tablewidget->setRowCount(int(vals.size()));
        QStringList textHeaders;
        int i = 0;
        for (const auto& hdr: std::as_const(headers)) {
            textHeaders.append(hdr.header.trimmed());
            tablewidget->setColumnWidth(i, hdr.width);
            ++i;
        }
        tablewidget->setHorizontalHeaderLabels(textHeaders);
        for (i = 0; i < vals.size(); ++i) {
            for (int j = 0; j < vals[i].size(); ++j) {
                QTableWidgetItem* lbl = new QTableWidgetItem(vals[i][j]);
                lbl->setFont(font);
                lbl->setBackground(QBrush(Qt::transparent));
                lbl->setForeground(Qt::black);
                lbl->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
                if (selectable) lbl->setFlags(Qt::ItemIsSelectable);
                else lbl->setFlags(Qt::NoItemFlags);
                tablewidget->setItem(i, j, lbl);
            }
        }
        tablewidget->setToolTip(w);
        moveTo(tablewidget, p, s);
        for (i = 0; i < tablewidget->rowCount(); ++i) tablewidget->resizeRowToContents(i);
#ifdef __wasm__
        for (i = 0; i < tablewidget->columnCount(); ++i) tablewidget->resizeColumnToContents(i);
#else
        int total = 0;
        for (i = 1; i < tablewidget->columnCount(); ++i) total += tablewidget->columnWidth(i - 1);
        tablewidget->setColumnWidth(int(headers.size()) - 1, s.l() - total);
#endif
        widgets.append(tablewidget);

        return tablewidget;
    }

    ClickableTable* createTableWidget(QWidget* parent, QFont& fontIn, QStringList headers, QList<QStringList> vals, At p, Size s,
                                      QString w, bool selectable = false, bool label = true) {
        QList<ColumnHeader> tableHeaders;
        QFontMetrics metrics(fontIn);
        int col = 0;
        for (const auto& hdr: std::as_const(headers)) {
            col = metrics.horizontalAdvance(hdr);
            tableHeaders.append({ col, hdr.trimmed() });
        }
        return createTableWidget(parent, fontIn, tableHeaders, vals, p, s, w, selectable, label);
    }

    QTextEdit* createTextEdit(QWidget* parent, QFont& fontIn, QString val, At p, Size s) {
        return createTextEdit(parent, font, val, p, s, "");
    }

    QTextEdit* createTextEdit(QWidget* parent, QFont& fontIn, QString val, At p, Size s, QString w) {
        QTextEdit* textedit = new QTextEdit(parent);
        textedit->setFont(fontIn);
        textedit->setHtml(val);
        textedit->setStyleSheet("QTextEdit { "
                                             "  border: 1px transparent;"
                                             "  border-style: none; "
                                             "  background: #fff;"
                                             "  color: #000; "
                                             "}"
                                "QToolTip { "
                                "              border: 1px solid #555555;"
                                "              padding: 3px;"
                                "              background-color: #333333;"
                                "              color: #ffffff; "
                                "         }");
        textedit->setReadOnly(true);
        textedit->setToolTip(w);
        moveTo(textedit, p, s);
        widgets.append(textedit);
        return textedit;
    }

    QPlainTextEdit* createTextEditor(QWidget* parent, QFont& fontIn, At p, Size s, QString w) {
        QPlainTextEdit* editwidget = new QPlainTextEdit(parent);
        editwidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        editwidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        editwidget->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        editwidget->setFont(fontIn);
        editwidget->setStyleSheet("QPlainTextEdit { selection-color: white;"
                                                "   selection-background-color: darkcyan;"
                                                "   gridline-color: cyan;"
                                                "   background-color: cyan;"
                                                "   border: 1px cyan;"
                                                "   border-style: none;"
                                                "   color: black;"
                                                " }"
                                  "QToolTip { border: 1px solid #555555;"
                                  "           padding: 3px;"
                                  "           background-color: #333333;"
                                  "           color: #ffffff;"
                                  "}");
        editwidget->setToolTip(w);
        moveTo(editwidget, p, s);
        hiddenWidgets.append(editwidget);

        return editwidget;
    }


public:
    QLineEdit* alternateids  = nullptr;
    QLineEdit* charactername = nullptr;
    QLineEdit* playername    = nullptr;

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

    QLabel* banner1 = nullptr;
    QLabel* banner2 = nullptr;
    QLabel* banner3 = nullptr;

    QTableWidget* attacksandmaneuvers = nullptr;

    QTableWidget* defenses = nullptr;

    QLabel*    perceptionroll           = nullptr;
    QTextEdit* enhancedandunusualsenses = nullptr;

    QLabel*    totalpoints           = nullptr;
    QLineEdit* totalexperienceearned = nullptr;
    QLabel*    experiencespent       = nullptr;
    QLabel*    experienceunspent     = nullptr;

    QLabel*    charactername2 = nullptr;
    QLineEdit* height         = nullptr;
    QLineEdit* weight         = nullptr;
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

    QPlainTextEdit* notes      = nullptr;
    QWidget*        page3      = nullptr;
    QLabel*         head       = nullptr;
    QLabel*         hands      = nullptr;
    QLabel*         arms       = nullptr;
    QLabel*         shoulders  = nullptr;
    QLabel*         chest      = nullptr;
    QLabel*         stomach    = nullptr;
    QLabel*         vitals     = nullptr;
    QLabel*         thighs     = nullptr;
    QLabel*         legs       = nullptr;
    QLabel*         feet       = nullptr;
    QLabel*         averageDEF = nullptr;
    QLabel*         DCVmod     = nullptr;
    QLabel*         armorNotes = nullptr;

    QList<QWidget*> widgets;
    QList<QWidget*> hiddenWidgets;
    QList<QWidget*> headerWidgets;

    QFont font;
    QFont smallfont;
    QFont largeBoldFont;
    QFont smallBoldWideFont;
    QFont headerFont;
    QFont narrowTableFont;

    Sheet_UI() = default;
    Sheet_UI(const Sheet_UI&) = default;
    Sheet_UI(Sheet_UI&&) = default;
    ~Sheet_UI() { }

    Sheet_UI& operator=(const Sheet_UI&) = default;
    Sheet_UI& operator=(Sheet_UI&&) = default;

    static constexpr int TinyFontSize = 10;
#if defined(Q_OS_ANDROID)
    static constexpr int StandardFontSize   = 11;
    static constexpr int SmallFontPointSize = 8;
    static constexpr int LargeBoldFontSize  = 16;
    static constexpr int HeaderFontSize     = 14;
    static constexpr int TableFontSize      = 13;
#elif defined(unix)
    static constexpr int StandardFontSize   = 11;
    static constexpr int SmallFontPointSize = 7;
    static constexpr int LargeBoldFontSize  = 16;
    static constexpr int HeaderFontSize     = 14;
    static constexpr int TableFontSize      = StandardFontSize;
#else
    static constexpr int StandardFontSize   = 11;
    static constexpr int SmallFontPointSize = 8;
    static constexpr int LargeBoldFontSize  = 15;
    static constexpr int HeaderFontSize     = 14;
    static constexpr int TableFontSize      = StandardFontSize;
#endif

    void setupUi(QWidget* widget, QWidget* hidden) {
        layout = new QGridLayout();
        widget->setLayout(layout);

#if defined(__wasm__) || defined(Q_OS_ANDROID)
        QFile fontResource(":/font/SegoeUIHS.ttf");
        (void) fontResource.open(QIODevice::ReadOnly);
        QByteArray data = fontResource.readAll();
        int id = QFontDatabase::addApplicationFontFromData(data);
        fontResource.close();
#else
        int id = QFontDatabase::addApplicationFont(":/font/SegoeUIHS.ttf");
#endif
        QString family = QFontDatabase::applicationFontFamilies(id).at(0);
        font = QFont({ family });
        font.setPointSize(StandardFontSize);

        largeBoldFont = font;
        largeBoldFont.setPointSize(LargeBoldFontSize);
        largeBoldFont.setBold(true);

        QFont largeNarrowBoldFont = largeBoldFont;
        largeNarrowBoldFont.setStretch(QFont::Stretch::Condensed);

        QFont largeFont = font;
        largeFont.setPointSize(LargeBoldFontSize);

        QFont largeNarrowFont = largeFont;
        largeNarrowFont.setStretch(QFont::Stretch::SemiCondensed);

        QFont tableFont = font;
        tableFont.setPointSize(TableFontSize);
        tableFont.setStretch(QFont::Stretch::SemiCondensed);

        QFont narrow = font;
        narrow.setStretch(QFont::Stretch::SemiCondensed);

        narrowTableFont = narrow;
        narrowTableFont.setPointSize(TinyFontSize);

        smallfont = font;
        smallfont.setPointSize(SmallFontPointSize);
        smallfont.setStretch(QFont::Stretch::SemiCondensed);
        QFontMetrics smallFontMetrics(smallfont);

        QFont smallBoldFont = smallfont;
        smallBoldFont.setPointSize(StandardFontSize);
        smallBoldFont.setBold(true);
        smallBoldFont.setStretch(QFont::Stretch::Unstretched);

        QFont smallBoldNarrowFont = smallBoldFont;
        smallBoldNarrowFont.setStretch(QFont::Stretch::SemiCondensed);

        QFont smallNarrowFont = smallBoldNarrowFont;
        smallNarrowFont.setBold(false);

        smallBoldWideFont = smallBoldFont;
        smallBoldWideFont.setPointSize(SmallFontPointSize);
        smallBoldWideFont.setStretch(QFont::Stretch::Expanded);

#if defined(unix) && !defined(Q_OS_ANDROID)
        headerFont = smallBoldWideFont;
#else
        headerFont = largeBoldFont;
        headerFont.setPointSize(HeaderFontSize);
        headerFont.setStretch(QFont::Stretch::SemiCondensed);
#endif

        QFont tinyFont = smallfont;
        tinyFont.setPointSize(TinyFontSize);
        tinyFont.setStretch(QFont::Stretch::ExtraCondensed);

        QFont tinyBoldFont = tinyFont;
        tinyBoldFont.setBold(true);

        createLabel(widget, largeNarrowBoldFont, "Character Name",       { 61, 76 }); // NOLINT
        createLabel(widget, largeNarrowFont,     "Alternate Identities", { 61, 105 }); // NOLINT
        createLabel(widget, largeNarrowFont,     "Player Name",          { 61, 135 }); // NOLINT

        charactername = createLineEdit(widget, largeBoldFont, { 190,  75 }, { 438, 27 }, "Characters superhero name"); // NOLINT
#if (defined(__wasm__) || defined(unix)) && !defined(Q_OS_ANDROID)
        alternateids  = createLineEdit(widget, largeFont,     { 225, 104 }, { 403, 27 }, "Characters secret id, typically"); // NOLINT
        playername    = createLineEdit(widget, largeFont,     { 170, 134 }, { 458, 27 }, "The players name"); // NOLINT
#else
        alternateids  = createLineEdit(widget, largeFont,     { 215, 104 }, { 413, 27 }, "Characters secret id, typically"); // NOLINT
        playername    = createLineEdit(widget, largeFont,     { 167, 134 }, { 461, 27 }, "The players name"); // NOLINT
#endif

        banner1 = createImage(widget, { 654, 76 } , { 293, 109 }, ":/gfx/HeroSystem-Banner.png", false); // NOLINT

#if defined(unix) && !defined(Q_OS_ANDROID)
        QFontMetrics headerMetrics(smallBoldWideFont);
#else
        QFontMetrics headerMetrics(headerFont);
#endif

        createBlockHeader(widget, headerFont, 72, 197, 295, "CHARACTERISTICS");
        createLabel(widget, smallBoldFont, "Val",    { 91,  223 }); // NOLINT
        createLabel(widget, smallBoldFont, "Char",   { 139, 223 }); // NOLINT
        createLabel(widget, smallBoldFont, "Points", { 199, 223 }); // NOLINT
        createLabel(widget, smallBoldFont, "Roll",   { 267, 223 }); // NOLINT
        createLabel(widget, smallBoldFont, "STR",    { 139, 247 }); // NOLINT
        createLabel(widget, smallBoldFont, "DEX",    { 139, 272 }); // NOLINT
        createLabel(widget, smallBoldFont, "CON",    { 139, 296 }); // NOLINT
        createLabel(widget, smallBoldFont, "INT",    { 139, 321 }); // NOLINT
        createLabel(widget, smallBoldFont, "EGO",    { 139, 345 }); // NOLINT
        createLabel(widget, smallBoldFont, "PRE",    { 139, 370 }); // NOLINT
        createLabel(widget, smallBoldFont, "OCV",    { 139, 401 }); // NOLINT
        createLabel(widget, smallBoldFont, "DCV",    { 139, 426 }); // NOLINT
        createLabel(widget, smallBoldFont, "OMCV",   { 139, 451 }); // NOLINT
        createLabel(widget, smallBoldFont, "DMCV",   { 139, 476 }); // NOLINT
        createLabel(widget, smallBoldFont, "SPD",    { 139, 501 }); // NOLINT
        createLabel(widget, smallBoldFont, "PD",     { 139, 531 }); // NOLINT
        createLabel(widget, smallBoldFont, "ED",     { 139, 556 }); // NOLINT
        createLabel(widget, smallBoldFont, "REC",    { 139, 581 }); // NOLINT
        createLabel(widget, smallBoldFont, "END",    { 139, 606 }); // NOLINT
        createLabel(widget, smallBoldFont, "BODY",   { 139, 631 }); // NOLINT
        createLabel(widget, smallBoldFont, "STUN",   { 139, 656 }); // NOLINT

        strval     = createNumEdit(widget, font, "10", { 79, 248 }, { 45, 20 }, "Strength: 1 point"); // NOLINT
        dexval     = createNumEdit(widget, font, "10", { 79, 273 }, { 45, 20 }, "Dexterity: 2 points"); // NOLINT
        conval     = createNumEdit(widget, font, "10", { 79, 297 }, { 45, 20 }, "Constitution: 1 point"); // NOLINT
        intval     = createNumEdit(widget, font, "10", { 79, 322 }, { 45, 20 }, "Intellegence: 1 point"); // NOLINT
        egoval     = createNumEdit(widget, font, "10", { 79, 346 }, { 45, 20 }, "Ego: 1 point"); // NOLINT
        preval     = createNumEdit(widget, font, "10", { 79, 371 }, { 45, 20 }, "Presence: 1 point"); // NOLINT
        ocvval     = createNumEdit(widget, font,  "3", { 79, 402 }, { 45, 20 }, "Offensive Combat Value: 5 points"); // NOLINT
        dcvval     = createNumEdit(widget, font,  "3", { 79, 427 }, { 45, 20 }, "Defensive Combat Value: 5 points"); // NOLINT
        omcvval    = createNumEdit(widget, font,  "3", { 79, 452 }, { 45, 20 }, "Offensive Mental Combat Value: 3 points"); // NOLINT
        dmcvval    = createNumEdit(widget, font,  "3", { 79, 477 }, { 45, 20 }, "Defensive Mental Combat Value: 3 points"); // NOLINT
        spdval     = createNumEdit(widget, font,  "2", { 79, 502 }, { 45, 20 }, "Speed: 10 points"); // NOLINT
        pdval      = createNumEdit(widget, font,  "2", { 79, 532 }, { 45, 20 }, "Physical Defense: 1 point"); // NOLINT
        edval      = createNumEdit(widget, font,  "2", { 79, 557 }, { 45, 20 }, "Energy Defense: 1 point"); // NOLINT
        recval     = createNumEdit(widget, font,  "4", { 79, 582 }, { 45, 20 }, "Recovery: 1 point"); // NOLINT
        endval     = createNumEdit(widget, font, "20", { 79, 607 }, { 45, 20 }, "Endurance: 1 point gets 5 points of END"); // NOLINT
        bodyval    = createNumEdit(widget, font, "10", { 79, 632 }, { 45, 20 }, "Body: 1 point"); // NOLINT
        stunval    = createNumEdit(widget, font, "20", { 79, 657 }, { 45, 20 }, "Stun: 1 point gets 2 points of STUN"); // NOLINT
        strpoints  = createLabel(widget, font,   "0", { 199, 248 }, "000"); // NOLINT
        dexpoints  = createLabel(widget, font,   "0", { 199, 273 }, "000"); // NOLINT
        conpoints  = createLabel(widget, font,   "0", { 199, 297 }, "000"); // NOLINT
        intpoints  = createLabel(widget, font,   "0", { 199, 322 }, "000"); // NOLINT
        egopoints  = createLabel(widget, font,   "0", { 199, 346 }, "000"); // NOLINT
        prepoints  = createLabel(widget, font,   "0", { 199, 371 }, "000"); // NOLINT
        ocvpoints  = createLabel(widget, font,   "0", { 199, 402 }, "000"); // NOLINT
        dcvpoints  = createLabel(widget, font,   "0", { 199, 427 }, "000"); // NOLINT
        omcvpoints = createLabel(widget, font,   "0", { 199, 452 }, "000"); // NOLINT
        dmcvpoints = createLabel(widget, font,   "0", { 199, 477 }, "000"); // NOLINT
        spdpoints  = createLabel(widget, font,   "0", { 199, 502 }, "000"); // NOLINT
        pdpoints   = createLabel(widget, font,   "0", { 199, 532 }, "000"); // NOLINT
        edpoints   = createLabel(widget, font,   "0", { 199, 557 }, "000"); // NOLINT
        recpoints  = createLabel(widget, font,   "0", { 199, 582 }, "000"); // NOLINT
        endpoints  = createLabel(widget, font,   "0", { 199, 607 }, "0000"); // NOLINT
        bodypoints = createLabel(widget, font,   "0", { 199, 632 }, "000"); // NOLINT
        stunpoints = createLabel(widget, font,   "0", { 199, 657 }, "0000"); // NOLINT
        strroll    = createLabel(widget, font, "11-", { 269, 248 }, "00-/00-"); // NOLINT
        dexroll    = createLabel(widget, font, "11-", { 269, 273 }, "00-/00-"); // NOLINT
        conroll    = createLabel(widget, font, "11-", { 269, 297 }, "00-/00-"); // NOLINT
        introll    = createLabel(widget, font, "11-", { 269, 322 }, "00-/00-"); // NOLINT
        egoroll    = createLabel(widget, font, "11-", { 269, 347 }, "00-/00-"); // NOLINT
        preroll    = createLabel(widget, font, "11-", { 269, 372 }, "00-/00-"); // NOLINT

        createLabel(widget, smallBoldFont, "Total Cost", { 276, 631 }); // NOLINT
        totalcost  = createLabel(widget, font,   "0", { 276, 657 }, "0000"); // NOLINT

        createBlockHeader(widget, headerFont, 394, 197, 243, "CURRENT STATUS");
        createLabel(widget, smallBoldFont, "Maximum", { 454, 222 }); // NOLINT
        createLabel(widget, smallBoldFont, "Current", { 542, 222 }); // NOLINT
        createLabel(widget, smallBoldFont, "END",  { 394, 245 }); // NOLINT
        createLabel(widget, smallBoldFont, "BODY", { 394, 270 }); // NOLINT
        createLabel(widget, smallBoldFont, "STUN", { 394, 294 }); // NOLINT

        QString style = "QLineEdit { background: palegreen;"
                                 "   color: #000;"
                                 "   border-style: none;"
                                 " }"
                        "QToolTip { border: 1px solid #555555;"
                        "           padding: 3px;"
                        "           background-color: #333333;"
                        "           color: #ffffff;"
                        "}";

        maximumend  = createLabel(widget, font, "20", { 454, 247 }, "0000"); // NOLINT
        maximumbody = createLabel(widget, font, "10", { 454, 272 }, "000"); // NOLINT
        maximumstun = createLabel(widget, font, "20", { 454, 296 }, "0000"); // NOLINT
        currentend  = createLineEdit(widget, font, style, "20", { 542, 247 }, { 97, 20 }, "You can keep track of your current END here"); // NOLINT
        currentbody = createLineEdit(widget, font, style, "10", { 542, 272 }, { 97, 20 }, "You can keep track of your current BODY here"); // NOLINT
        currentstun = createLineEdit(widget, font, style, "20", { 542, 296 }, { 97, 20 }, "You can keep track of your current STUN here"); // NOLINT

        createBlockHeader(widget, headerFont, 394, 348, 243, "VITAL INFORMATION");
        createLabel(widget, smallBoldNarrowFont, "HTH Damage",      { 397, 375 }); // NOLINT
#ifdef __wasm__
        createLabel(widget, smallNarrowFont,     "(STR/5)d6",       { 486, 375 }); // NOLINT
#else
        createLabel(widget, smallNarrowFont,     "(STR/5)d6",       { 496, 375 }); // NOLINT
#endif
        createLabel(widget, smallNarrowFont,     "Lift",            { 397, 399 }); // NOLINT
        createLabel(widget, smallNarrowFont,     "STR END Cost",    { 510, 399 }); // NOLINT
        createLabel(widget, font,                "1",               { 451, 425 }); // NOLINT
        createLabel(widget, font,                "2",               { 464, 425 }); // NOLINT
        createLabel(widget, font,                "3",               { 478, 425 }); // NOLINT
        createLabel(widget, font,                "4",               { 492, 425 }); // NOLINT
        createLabel(widget, font,                "5",               { 506, 425 }); // NOLINT
        createLabel(widget, font,                "6",               { 520, 425 }); // NOLINT
        createLabel(widget, font,                "7",               { 533, 425 }); // NOLINT
        createLabel(widget, font,                "8",               { 547, 425 }); // NOLINT
        createLabel(widget, font,                "9",               { 561, 425 }); // NOLINT
        createLabel(widget, font,               "10",               { 575, 425 }); // NOLINT
        createLabel(widget, font,               "11",               { 597, 425 }); // NOLINT
        createLabel(widget, font,               "12",               { 619, 425 }); // NOLINT
        createLabel(widget, smallBoldNarrowFont, "Phases",          { 397, 442 }); // NOLINT
        createLabel(widget, smallBoldNarrowFont, "Base OCV",        { 395, 469 }); // NOLINT
        createLabel(widget, smallBoldNarrowFont, "Base DCV",        { 515, 469 }); // NOLINT
        createLabel(widget, smallBoldNarrowFont, "Base OMCV",       { 395, 494 }); // NOLINT
        createLabel(widget, smallBoldNarrowFont, "Base DMCV",       { 515, 494 }); // NOLINT
        createLabel(widget, smallBoldNarrowFont, "Presence Attack", { 395, 661 }); // NOLINT
#ifdef __wasm__
        createLabel(widget, smallNarrowFont,     "(PRE/5)d6",       { 500, 661 }); // NOLINT
#else
        createLabel(widget, smallNarrowFont,     "(PRE/5)d6",       { 510, 661 }); // NOLINT
#endif

        hthdamage         = createLabel(widget, font,   "2d6", { 551, 376 }, "00d6+0"); // NOLINT
        lift              = createLabel(widget, font, "100kg", { 420, 401 }, "00000000"); // NOLINT
        strendcost        = createLabel(widget, font,     "1", { 603, 401 }, "00"); // NOLINT
        phases.append(      createLabel(widget, font,      "", { 451, 444 }, "X")); // NOLINT
        phases.append(      createLabel(widget, font,      "", { 464, 444 }, "X")); // NOLINT
        phases.append(      createLabel(widget, font,      "", { 478, 444 }, "X")); // NOLINT
        phases.append(      createLabel(widget, font,      "", { 492, 444 }, "X")); // NOLINT
        phases.append(      createLabel(widget, font,      "", { 506, 444 }, "X")); // NOLINT
        phases.append(      createLabel(widget, font,     "X", { 520, 444 }, "X")); // NOLINT
        phases.append(      createLabel(widget, font,      "", { 533, 444 }, "X")); // NOLINT
        phases.append(      createLabel(widget, font,      "", { 547, 444 }, "X")); // NOLINT
        phases.append(      createLabel(widget, font,      "", { 561, 444 }, "X")); // NOLINT
        phases.append(      createLabel(widget, font,      "", { 580, 444 }, "X")); // NOLINT
        phases.append(      createLabel(widget, font,      "", { 602, 444 }, "X")); // NOLINT
        phases.append(      createLabel(widget, font,     "X", { 624, 444 }, "X")); // NOLINT
        baseocv           = createLabel(widget, font,     "3", { 469, 471 }, "00"); // NOLINT
        basedcv           = createLabel(widget, font,     "3", { 589, 471 }, "00"); // NOLINT
        baseomcv          = createLabel(widget, font,     "3", { 483, 496 }, "00"); // NOLINT
        basedmcv          = createLabel(widget, font,     "3", { 603, 496 }, "00"); // NOLINT
        combatskilllevels = createTextEdit(widget, narrow, "<b>Combat Skill Levels</b> ", { 392, 520 }, { 244, 145 }); // NOLINT
        presenceattack    = createLabel(widget, font,   "2d6", { 573, 663 }, "00s6+0"); // NOLINT

        createBlockHeader(widget, headerFont, 679, 198, 251, "MOVEMENT");
        createLabel(widget, smallNarrowFont, "Movement SFX", { 678, 420 }, { 100, 22 }); // NOLINT

        movement    = createTableWidget(widget, narrowTableFont,
                                        { { 97, "Type" }, { 73, "Combat" }, { 89, "Non-Combat" } },
                                        { { "Run (12m)",    "12m",           "24m" },
                                          { "Swim (4m)",    "4m",            "8m" },
                                          { "H. Leap (4m)", "4m",            "8m" },
                                          { "V. Leap (2m)", "2m",            "4m" } }, { 675, 225 }, { 260, 195 }); // NOLINT
        movementsfx = createLabel(widget, font, "", { 775, 423 }, 20); // NOLINT

        createBlockHeader(widget, headerFont, 679, 475, 243, "RANGE MODIFIERS");
        createLabel(widget, tinyBoldFont, "Range(m)", { 678, 502 }, { 52, 20 }); // NOLINT
        createLabel(widget, tinyFont,     "0-8",      { 737, 502 }, { 40, 20 }); // NOLINT
        createLabel(widget, tinyFont,     "9-16",     { 760, 502 }, { 40, 20 }); // NOLINT
        createLabel(widget, tinyFont,     "17-32",    { 788, 502 }, { 40, 20 }); // NOLINT
        createLabel(widget, tinyFont,     "33-64",    { 822, 502 }, { 40, 20 }); // NOLINT
        createLabel(widget, tinyFont,     "65-128",   { 858, 502 }, { 40, 20 }); // NOLINT
        createLabel(widget, tinyFont,     "128-256",  { 898, 502 }, { 40, 20 }); // NOLINT
        createLabel(widget, tinyBoldFont, "OCV Mod",  { 680, 522 }, { 52, 20 }); // NOLINT
        createLabel(widget, tinyFont,     "-0",       { 741, 522 }, { 30, 20 }); // NOLINT
        createLabel(widget, tinyFont,     "-2",       { 766, 522 }, { 30, 20 }); // NOLINT
        createLabel(widget, tinyFont,     "-4",       { 796, 522 }, { 30, 20 }); // NOLINT
        createLabel(widget, tinyFont,     "-6",       { 830, 522 }, { 30, 20 }); // NOLINT
        createLabel(widget, tinyFont,     "-8",       { 868, 522 }, { 30, 20 }); // NOLINT
        createLabel(widget, tinyFont,     "-10",      { 909, 522 }, { 30, 20 }); // NOLINT

        image     = createImage(widget, { 663, 555 }, { 285, 533 }, Selectable); // NOLINT

        imageMenu = createMenu(image, font, { { "New Image",   &newImage   },
                                              { "Clear Image", &clearImage } } );

        createBlockHeader(widget, headerFont, 72, 714, 295, "ATTACKS & MANEUVERS");
        attacksandmaneuvers = createTableWidget(widget, narrowTableFont,
                                                { { 65, "Maneuver" }, { 34, "Phase" }, { 28, "OCV" }, { 26, "DCV" }, { 135, "Effects" } },
                                                { { "Block",            "½",             "+0",          "+0",          "Block, abort"              },
                                                  { "Brace",            "0",             "+2",          "½",           "+2 OCV vs R Mod"           },
                                                  { "Disarm",           "½",             "-2",          "+0",          "Disarm, 10 v. STR"         },
                                                  { "Dodge",            "½",             "——",          "+3",          "Abort vs. all attacks"     },
                                                  { "Grab",             "½",             "-1",          "-2",          "Grab 2 limbs"              },
                                                  { "Grab By",          "½†",            "-3",          "-4",          "Move&Grab;+(ͮ⁄₁₀) STR"     },
                                                  { "Haymaker",         "½*",            "+0",          "-5",          "+4 DCs to attack"          },
                                                  { "Move By",          "½†",            "-2",          "-2",          "(1+ͮ⁄₁₀)d6; take ⅓"        },
#if (defined(__wasm__) || defined(unix)) && !defined(Q_OS_ANDROID)
                                                  { "Move Thru",        "½†",            "-ͮ⁄₁₀",       "-3",          "(2+ͮ⁄₆)d6; take ½ or all"  },
                                                  { "Mult.Attx",        "1",             "var",         "½",           "Attack multiple times"     },
#else
                                                  { "Move Through",     "½†",            "-ͮ⁄₁₀",       "-3",         "(2+ͮ⁄₆)d6; take ½ or all"  },
                                                  { "Multiple Attacks", "1",             "var",         "½",          "Attack multiple times"     },
#endif
                                                  { "Set",              "1",             "+1",          "+0",          "Ranged attacks only"       },
                                                  { "Shove",            "½",             "-1",          "-1",          "Push 2m"                   },
                                                  { "Strike",           "½",             "+0",          "+0",          "2d6 or weapon"             },
                                                  { "Throw",            "½",             "+0",          "+0",          "Throw w/2d6 dmg"           },
                                                  { "Trip",             "½",             "-1",          "-2",          "Knock target prone"        }
                                                }, { 69, 739 }, { 295, 495 }); // NOLINT

        createBlockHeader(widget, headerFont, 394, 714, 243, "DEFENSES");
        defenses = createTableWidget(widget, narrowTableFont,
                                     { { 108, "Type" },   { 142, "Amount/Effect" } },
                                     { { "Normal PD",      "2" },
                                       { "Resistant PD",   "0" },
                                       { "Normal ED",      "2" },
                                       { "Resistant ED",   "0" },
                                       { "Mental Defense", "0" },
                                       { "Power Defense",  "0" },
                                       { "Flash Defense",  "0" } }, { 392, 739 }, { 249, 270 }); // NOLINT

        createBlockHeader(widget, headerFont, 394, 1040, 243, "SENSES");
        createLabel(widget, smallBoldNarrowFont, "Perception Roll", { 395, 1065 }, "-00"); // NOLINT
        createLabel(widget, smallNarrowFont,     "(9+INT/5)",       { 496, 1065 }, "00"); // NOLINT
        perceptionroll = createLabel(widget, font, "11-", { 569, 1066 }, "-00"); // NOLINT
        enhancedandunusualsenses = createTextEdit(widget, font, "<b>Enhanced and Unusual Senses</b>", { 390, 1083 }, { 249, 150 }); // NOLINT

        createBlockHeader(widget, headerFont, 679, 1108, 243, "EXPERIENCE POINTS");
        createLabel(widget, smallBoldNarrowFont, "Total Points",            { 675, 1133 }); // NOLINT
        createLabel(widget, smallBoldNarrowFont, "Total Experience Earned", { 675, 1156 }); // NOLINT
        createLabel(widget, smallNarrowFont,     "Experience Spent",        { 675, 1181 }); // NOLINT
        createLabel(widget, smallNarrowFont,     "Experience Unspent",      { 675, 1206 }); // NOLINT

        totalpoints           = createLabel(widget, font, "0/325", { 855, 1135 }, "0000"); // NOLINT
        totalexperienceearned = createLineEdit(widget, font,  "0", { 853, 1158 }, { 80, 20 }, "How much experience your character has earned"); // NOLINT
        experiencespent       = createLabel(widget, font,     "0", { 855, 1183 }, "000"); // NOLINT
        experienceunspent     = createLabel(widget, font,   "325", { 855, 1207 }, "0000"); // NOLINT

        createBlockHeader(widget, headerFont, 72, 1363, 259, "CHARACTER INFORMATION");
        createLabel(widget, smallBoldNarrowFont, "Character Name", {  66, 1388 }); // NOLINT
        createLabel(widget, smallBoldNarrowFont, "Height",         {  66, 1413 }); // NOLINT
        createLabel(widget, smallBoldNarrowFont, "Weight",         { 196, 1413 }); // NOLINT
        createLabel(widget, smallBoldNarrowFont, "Hair Color",     {  66, 1439 }); // NOLINT
        createLabel(widget, smallBoldNarrowFont, "Eye Color",      { 196, 1439 }); // NOLINT

        charactername2 = createLabel(widget, font, "", { 184, 1388 }, 20); // NOLINT
        height    = createLineEdit(widget, font,    "2m", { 124, 1414 }, { 72, 20 }, "Your characters height (certain powers may override)"); // NOLINT
        weight    = createLineEdit(widget, font, "100kg", { 249, 1414 }, { 76, 20 }, "Your characters weight (certain powers may override)"); // NOLINT
        haircolor = createLineEdit(widget, font,      "", { 139, 1440 }, { 58, 20 }, "Your characters hair color"); // NOLINT
        eyecolor  = createLineEdit(widget, font,      "", { 262, 1440 }, { 64, 20 }, "Your characters eye color"); // NOLINT

        banner2 = createImage(widget, { 360, 1376 } , { 293, 109 }, ":/gfx/HeroSystem-Banner.png", false); // NOLINT

        createBlockHeader(widget, headerFont, 678, 1363, 257, "CAMPAIGN INFORMATION");
        createLabel(widget, smallBoldNarrowFont, "Campaign Name", { 672, 1389 }); // NOLINT
        createLabel(widget, smallBoldNarrowFont, "Genre",         { 672, 1413 }); // NOLINT
        createLabel(widget, smallBoldNarrowFont, "Gamemaster",    { 672, 1439 }); // NOLINT
        campaignname = createLineEdit(widget, font, "", { 788, 1391 }, { 145, 20 }, "The campaign your character is playing in"); // NOLINT
        genre        = createLineEdit(widget, font, "", { 721, 1416 }, { 213, 20 }, "The kind of game (street level, superhero, galactic, etc)"); // NOLINT
        gamemaster   = createLineEdit(widget, font, "", { 764, 1441 }, { 170, 20 }, "Who is running the game for your character"); // NOLINT

        createBlockHeader(widget, headerFont, 72, 1496, 259, "SKILLS, PERKS, & TALENTS");
        createLabel(widget, smallBoldNarrowFont, "Total Skills,Perks, & Talents Cost", { 112, 2057 }); // NOLINT
        skillstalentsandperks         = createTableWidget(widget, tableFont, { { 42, "Cost" }, { 179, "Name" }, { 38, "Roll" } },
                                                  { }, { 73, 1521 }, { 265, 535 }, "Things your character is skilled at or has a gift for", Selectable); // NOLINT
        totalskillstalentsandperkscost = createLabel(widget, font, "0", { 73, 2058 }, "000"); // NOLINT
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

        createBlockHeader(widget, headerFont, 72, 2104, 259, "COMPLICATIONS");
        createLabel(widget, smallBoldNarrowFont, "Total Complications Points", { 117, 2512 }); // NOLINT
        complications        = createTableWidget(widget, tableFont, { { 41, "Pts" }, { 221, "Complication" } },
                                                 { }, { 73, 2130 }, { 265, 383 }, "The things that make life difficult for your character", Selectable); // NOLINT
        totalcomplicationpts = createLabel(widget, font, "0/75", { 73, 2513 }, "000/000"); // NOLINT
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

        createBlockHeader(widget, headerFont, 369, 1498, 564, "POWERS AND EQUIPMENT");
        createLabel(widget, smallBoldNarrowFont, "Total Powers/Equipment Cost", { 410, 2510 }); // NOLINT

        QFontMetrics metrics(tableFont);
        powersandequipment          = createTableWidget(widget, tableFont,
                                                        { { 43, "Cost" }, { 107, "Name" }, { 378, "Power/Equipment" }, { 42, "END" } },
                                                        { }, { 367, 1522 }, { 570, 991 }, "Special powers and equipment for your character", Selectable); // NOLINT
        totalpowersandequipmentcost = createLabel(widget, font, "0", { 367, 2511 }, "0000"); // NOLINT
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

        page3 = hidden;
        layout = new QGridLayout();
        hidden->setLayout(layout);

        banner3 = createImage(hidden, { 360, 76 } , { 293, 109 }, ":/gfx/HeroSystem-Banner.png", false); // NOLINT
        notes = createTextEditor(hidden, tableFont, { 365, 229 }, { 575, 1008 }, "Game notes"); // NOLINT

        head      = createLabel(hidden, font, "0", { 281, 310 }, "00"); // NOLINT
        hands     = createLabel(hidden, font, "0", { 281, 332 }, "00"); // NOLINT
        arms      = createLabel(hidden, font, "0", { 281, 355 }, "00"); // NOLINT
        shoulders = createLabel(hidden, font, "0", { 281, 378 }, "00"); // NOLINT
        chest     = createLabel(hidden, font, "0", { 281, 399 }, "00"); // NOLINT
        stomach   = createLabel(hidden, font, "0", { 281, 422 }, "00"); // NOLINT
        vitals    = createLabel(hidden, font, "0", { 281, 444 }, "00"); // NOLINT
        thighs    = createLabel(hidden, font, "0", { 281, 466 }, "00"); // NOLINT
        legs      = createLabel(hidden, font, "0", { 281, 488 }, "00"); // NOLINT
        feet      = createLabel(hidden, font, "0", { 281, 510 }, "00"); // NOLINT

        averageDEF = createLabel(hidden, font, "0",  { 281, 540 }, "00"); // NOLINT
        DCVmod     = createLabel(hidden, font, "+0", { 158, 565 }, "+00"); // NOLINT
        armorNotes = createLabel(hidden, font, "",   { 158, 590 }, "MMMMMMMMMMMMMMMMMMMMMMMMMMMM"); // NOLINT

        createBlockHeader(hidden, headerFont, 75,  85,  249, "KNOCKBACK MODIFIERS");
        createBlockHeader(hidden, headerFont, 680, 82,  256, "WALL BODY");
        createBlockHeader(hidden, headerFont, 366, 202, 569, "NOTES");
        createBlockHeader(hidden, headerFont, 75,  248, 249, "HIT LOCATION CHART");
        createBlockHeader(hidden, headerFont, 75,  671, 249, "COMBAT MODIFIERS");
        createBlockHeader(hidden, headerFont, 75,  884, 249, "SKILL MODIFIERS");

        hidden->setVisible(false);
    }
};
