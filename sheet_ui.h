#pragma once

#include "fraction.h"
#include <QAction>
#include <QLineEdit>
#include <QGridLayout>
#ifdef __wasm__
#include <QFile>
#endif
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

#include<gsl/gsl>

#include "shared.h"

class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(gsl::owner<QWidget*> parent = Q_NULLPTR)
        : QLabel(parent) {
#ifdef __wasm__
        this->setAttribute(Qt::WA_AcceptTouchEvents);
#endif
    }

signals:
    void clicked();

protected:
#ifdef __wasm__
    void mousePressEvent(QMouseEvent* me) override {
        if (me->buttons() == Qt::RightButton)
            emit customContextMenuRequested(me->pos());
        else
            emit clicked();
    }
    bool event(QEvent* e) override {
        QTouchEvent* te = dynamic_cast<QTouchEvent*>(e);
        if (te == nullptr) return QLabel::event(e);
        if (te->type() == QEvent::TouchEnd) {
            te->accept();
            if (te->pointCount() == 2) {
                QPoint pnt = te->point(0).position().toPoint();
                emit customContextMenuRequested(pnt);
            }
        }
        return true;
    }
#else
    void mousePressEvent(QMouseEvent*) override {
        emit clicked();
    }
#endif

};

class ClickableTable : public QTableWidget {
    Q_OBJECT

public:
    explicit ClickableTable(QWidget* parent = Q_NULLPTR)
        : QTableWidget(parent) {
#ifdef __wasm__
        this->setAttribute(Qt::WA_AcceptTouchEvents);
#endif
    }

signals:
    void showmenu();

protected:
#ifdef __wasm__
    void mousePressEvent(QMouseEvent* me) override {
        if (me->buttons() == Qt::RightButton)
            emit customContextMenuRequested(me->pos());
        else
            emit showmenu();
    }
    bool event(QEvent* e) override {
        QTouchEvent* te = dynamic_cast<QTouchEvent*>(e);
        if (te == nullptr) return QTableWidget::event(e);
        if (te->type() == QEvent::TouchEnd) {
            te->accept();
            if (te->pointCount() == 2) {
                emit showmenu();
                emit customContextMenuRequested(te->point(0).position().toPoint());
            }
        }
        return true;
    }
#else
    void mousePressEvent(QMouseEvent* me) override {
        emit showmenu();
        QTableWidget::mousePressEvent(me);
    }
#endif
};

class Sheet_UI
{
private:    
    void moveTo(QWidget* w, At p, Size s = { }) {
        QRect r = w->geometry();
        r.moveTo({ p.x(), p.y() });
        if (s.l() != -1) r.setWidth(s.l());
        if (s.h() != -1) r.setHeight(s.h());
        w->setGeometry(r);
    }

    gsl::owner<QLabel*> createLabel(QWidget* parent, QFont& font, QString val, At p, Size s, bool header = false) {
        gsl::owner<QLabel*> label = new QLabel(parent);
        label->setFont(font);
        label->setText(val);
#ifdef unix
        label->setStyleSheet("color: #000;");
#endif
        if (s.h() == -1) moveTo(label, p, { s.h(), s.l() });
        else moveTo(label, p, s);
        if (parent != page3) widgets.append(label);
        else hiddenWidgets.append(label);
        if (header) headerWidgets.append(label);
        return label;
    }

#ifndef __wasm__
    gsl::owner<QLabel*> createImage(QWidget* parent, At p, Size s, const QString& image, bool selectable = false) {
        gsl::owner<QLabel*> label = createImage(parent, p, s, selectable);
#else
    gsl::owner<ClickableLabel*> createImage(QWidget* parent, At p, Size s, bool) {
        gsl::owner<ClickableLabel*> label = createImage((parent, p, s);
#endif
        QPixmap pixmap(image);
        pixmap = pixmap.scaled(s.l(), s.h(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        label->setPixmap(pixmap);
        label->setAlignment(Qt::AlignCenter);
        return label;
    }

#ifndef __wasm__
    gsl::owner<QLabel*> createImage(QWidget* parent, At p, Size s, bool selectable = false) {
        gsl::owner<QLabel*> label = new QLabel(parent);
#else
    gsl::owner<ClickableLabel*> createImage(QWidget* parent, At p, Size s, bool) {
        gsl::owner<ClickableLabel*> label = new ClickableLabel(parent);
#endif
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

    gsl::owner<QLineEdit*> createLineEdit(QWidget* parent, QFont& font, At p, Size s, QString w = "") {
        return createLineEdit(parent, font, "", p, s, w);
    }

    gsl::owner<QLineEdit*> createLineEdit(QWidget* parent, QFont& font, QString val, At p, Size s, QString w = "") {
        QString style = "QLineEdit { background: cyan;"
#ifdef unix
                                 "   color: #000;"
#endif
                                 "   border-style: none;"
                                 " }";
        return createLineEdit(parent, font, style, val, p, s, w);
    }

    gsl::owner<QLineEdit*> createLineEdit(QWidget* parent, QFont& font, QString style, QString val, At p, Size s, QString w = "") {
        gsl::owner<QLineEdit*> lineedit = new QLineEdit(parent);
        lineedit->setFont(font);
        lineedit->setText(val);
        lineedit->setStyleSheet(style);
        lineedit->setToolTip(w);
        moveTo(lineedit, p, s);
        widgets.append(lineedit);
        gsl::owner<QFrame*> line = new QFrame(parent);
        line->setLineWidth(1);
        line->setFrameShape(QFrame::HLine);
        QFont f = lineedit->font();
        QFontMetrics metrics(f);
        moveTo(line, { p.x(), p.y() + s.h() - metrics.descent() + 2 }, { s.l(), 1 });
        return lineedit;
    }

    gsl::owner<QLineEdit*> createNumEdit(QWidget* parent, QFont& font, At p, Size s, QString w = "") {
        return createNumEdit(parent, font, "", p, s, w);
    }

    gsl::owner<QLineEdit*> createNumEdit(QWidget* parent, QFont& font, QString val, At p, Size s, QString w = "") {
        QString style = "QLineEdit { background: cyan;"
#ifdef unix
                                 "   color: #000;"
#endif
                                 "   border-style: none;"
                                 " }";
        return createNumEdit(parent, font, style, val, p, s, w);
    }

    gsl::owner<QLineEdit*> createNumEdit(QWidget* parent, QFont& font, QString style, QString val, At p, Size s, QString w = "") {
        gsl::owner<QLineEdit*> lineedit = new QLineEdit(parent);
        lineedit->setFont(font);
        lineedit->setText(val);
        lineedit->setStyleSheet(style);
        lineedit->setToolTip(w);
        lineedit->setAlignment(Qt::AlignCenter);
        moveTo(lineedit, p, s);
        widgets.append(lineedit);
        gsl::owner<QFrame*> line = new QFrame(parent);
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
        menuItems(QString x, gsl::owner<QAction*>* y)
            : text(x)
            , action(y) { }
        QString text;
        gsl::owner<QAction*>* action;
    };

    gsl::owner<QMenu*> createMenu(QWidget* parent, QFont& font, QList<menuItems> items) {
        gsl::owner<QMenu*> menu = new QMenu(parent);
#ifdef unix
        menu->setStyleSheet("QMenu { color: #000; background: #fff; }");
#endif
        menu->setFont(font);
        for (auto& item: items) {
            if (item.action == nullptr) {
                menu->addSeparator();
                continue;
            }
            gsl::owner<QAction*>& action = *item.action;
            action = new QAction(item.text);
            action->setFont(font);
            menu->addAction(action);
        }
        return menu;
    }

    static constexpr bool Selectable = true;
    static constexpr bool NoSelectable = false;
    static constexpr bool NoLabel = false;

    static constexpr double Points = 72.0;
    static constexpr double Half   = 0.5;

#ifdef __wasm__
    gsl::owner<ClickableTable*> createTableWidget(QWidget* parent, QFont& font, QStringList headers, QList<QStringList> vals, At p, Size s,
                                      bool selectable = false, bool label = true) {
#else
    gsl::owner<QTableWidget*> createTableWidget(QWidget* parent, QFont& font, QStringList headers, QList<QStringList> vals, At p, Size s,
                                    bool selectable = false, bool label = true) {
#endif
        return createTableWidget(parent, font, headers, vals, p, s, "", selectable, label);
    }

#ifdef __wasm__
    gsl::owner<ClickableTable*> createTableWidget(QWidget* parent, QFont& font, QStringList headers, QList<QStringList> vals, At p, Size s,
                                      QString w, bool selectable = false, bool label = true) {
        gsl::owner<ClickableTable*> tablewidget = new ClickableTable(parent);
#else
    gsl::owner<QTableWidget*> createTableWidget(QWidget* parent, QFont& font, QStringList headers, QList<QStringList> vals, At p, Size s,
                                                QString w, bool selectable = false, bool label = true) {
        gsl::owner<QTableWidget*> tablewidget = new QTableWidget(parent);
        tablewidget->setContextMenuPolicy(Qt::CustomContextMenu);
#endif
        tablewidget->setWordWrap(true);
        tablewidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        tablewidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        int pnt = font.pointSize();
        auto dpiy = parent->screen()->physicalDotsPerInchY();
        int sz = (pnt * Fraction(static_cast<long>(dpiy), Points)).toInt();
#ifdef __wasm__
        QFont temp = font;
        temp.setPointSize(pnt * 8 + 0.5); // NOLINT
        tablewidget->setFont(temp);
#else
        tablewidget->setFont(font);
#endif
        auto verticalHeader = tablewidget->verticalHeader();
        verticalHeader->setVisible(false);
        verticalHeader->setMinimumSectionSize(sz + 2);
        verticalHeader->setMaximumSectionSize(selectable ? s.l() : sz + 2);
        verticalHeader->setDefaultSectionSize(sz + 2);
        verticalHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
        auto horizontalHeader = tablewidget->horizontalHeader();
        horizontalHeader->setStretchLastSection(true);
        horizontalHeader->setMaximumSectionSize(s.l());
        horizontalHeader->setDefaultSectionSize(10); // NOLINT
        horizontalHeader->setDefaultAlignment(Qt::AlignLeft);
#ifdef __wasm__
        horizontalHeader->setMaximumSize(s.l(), sz + 2);
#else
        horizontalHeader->setMaximumSize(s.l(), sz * 2);
#endif
        tablewidget->setSelectionMode(selectable ? QAbstractItemView::SingleSelection : QAbstractItemView::NoSelection);
        tablewidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        QString family = font.family();
        if (selectable)
            tablewidget->setStyleSheet("QTableWidget { selection-color: white;"
                                                   "   selection-background-color: DarkCyan;"
                                                   "   gridline-color: cyan;"
                                                   "   background-color: cyan;"
                                                   "   border-style: none;"
                                         + QString("   font: %2pt \"%1\";").arg(family).arg(pnt) + // NOLINT
                                                   "   color: black;"
                                                   " } "
                                       "QHeaderView::section { background-color: white;"
                                                           "   border-style: none;"
                                                           "   color: black;" +
                                                   QString("   font: bold %2pt \"%1\";").arg(family).arg(pnt) + // NOLINT
                                                           " }");
        else
            tablewidget->setStyleSheet("QTableWidget { selection-color: transparent;"
                                                   "   selection-background-color: transparent;"
                                                   "   gridline-color: transparent;"
                                                   "   border-style: none;"
                                                   "   background-color: transparent;"
                                                   "   color: black;" +
                                           QString("   font: %2pt \"%1\";").arg(family).arg(pnt) + // NOLINT
                                                   " } "
                                       "QHeaderView::section { background-color: white;"
                                                           "   border-style: none;"
                                                           "   color: black;" +
                                                   QString("   font: bold %2pt \"%1\";").arg(family).arg(pnt) +
                                                           " }");
        tablewidget->setColumnCount(gsl::narrow<int>(headers.size()));
        tablewidget->setRowCount(gsl::narrow<int>(vals.size()));
        tablewidget->setHorizontalHeaderLabels(headers);
        int i = 0;
        for (i = 0; i < vals.size(); ++i) {
            for (int j = 0; j < vals[i].size(); ++j) {
                if (label) {
                    gsl::owner<QLabel*> cell = new QLabel(vals[i][j]);
                    cell->setFont(font);
#ifdef unix
                    cell->setStyleSheet("color: #000;");
#endif
                    tablewidget->setCellWidget(i, j, cell);
                } else {
                    gsl::owner<QTableWidgetItem*> lbl = new QTableWidgetItem(vals[i][j]);
                    lbl->setFont(font);
                    lbl->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
                    if (selectable) lbl->setFlags(Qt::ItemIsSelectable);
                    else lbl->setFlags(Qt::NoItemFlags);
                    tablewidget->setItem(i, j, lbl);
                }
            }
        }
        tablewidget->setToolTip(w);
        moveTo(tablewidget, p, s);
        for (i = 0; i < tablewidget->rowCount(); ++i) tablewidget->resizeRowToContents(i);
#ifdef __wasm__
        for (i = 0; i < tablewidget->columnCount(); ++i) tablewidget->resizeColumnToContents(i);
#else
        int total = 0;
        for (i = 1; i < tablewidget->columnCount(); ++i) {
            tablewidget->resizeColumnToContents(i - 1);
            total += tablewidget->columnWidth(i - 1);
        }
        tablewidget->setColumnWidth(gsl::narrow<int>(headers.size()) - 1, s.l() - total);
#endif
        widgets.append(tablewidget);

        return tablewidget;
    }

    gsl::owner<QTextEdit*> createTextEdit(QWidget* parent, QFont& font, QString val, At p, Size s) {
        return createTextEdit(parent, font, val, p, s, "");
    }

    gsl::owner<QTextEdit*> createTextEdit(QWidget* parent, QFont& font, QString val, At p, Size s, QString w) {
        gsl::owner<QTextEdit*> textedit = new QTextEdit(parent);
        textedit->setFont(font);
        textedit->setHtml(val);
        textedit->setStyleSheet("QTextEdit { "
                                "  border-style: none; "
#ifdef unix
                                "  background: #fff;"
                                "  color: #000;"
#endif
                                "}");
        textedit->setReadOnly(true);
        textedit->setToolTip(w);
        moveTo(textedit, p, s);
        widgets.append(textedit);
        return textedit;
    }

    gsl::owner<QPlainTextEdit*> createTextEditor(QWidget* parent, QFont& font, At p, Size s, QString w) {
        gsl::owner<QPlainTextEdit*> editwidget = new QPlainTextEdit(parent);
        editwidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        editwidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        editwidget->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        editwidget->setFont(font);
        editwidget->setStyleSheet("QPlainTextEdit { selection-color: white;"
                                                "   selection-background-color: DarkCyan;"
                                                "   gridline-color: cyan;"
                                                "   background-color: cyan;"
                                                "   border-style: none;"
                                                "   color: black;"
                                                " }");
        editwidget->setToolTip(w);
        moveTo(editwidget, p, s);
        hiddenWidgets.append(editwidget);

        return editwidget;
    }


public:
    gsl::owner<QLineEdit*> alternateids  = nullptr;
    gsl::owner<QLineEdit*> charactername = nullptr;
    gsl::owner<QLineEdit*> playername    = nullptr;

    gsl::owner<QLineEdit*> strval     = nullptr;
    gsl::owner<QLineEdit*> dexval     = nullptr;
    gsl::owner<QLineEdit*> conval     = nullptr;
    gsl::owner<QLineEdit*> intval     = nullptr;
    gsl::owner<QLineEdit*> egoval     = nullptr;
    gsl::owner<QLineEdit*> preval     = nullptr;
    gsl::owner<QLineEdit*> ocvval     = nullptr;
    gsl::owner<QLineEdit*> dcvval     = nullptr;
    gsl::owner<QLineEdit*> omcvval    = nullptr;
    gsl::owner<QLineEdit*> dmcvval    = nullptr;
    gsl::owner<QLineEdit*> spdval     = nullptr;
    gsl::owner<QLineEdit*> pdval      = nullptr;
    gsl::owner<QLineEdit*> edval      = nullptr;
    gsl::owner<QLineEdit*> recval     = nullptr;
    gsl::owner<QLineEdit*> endval     = nullptr;
    gsl::owner<QLineEdit*> bodyval    = nullptr;
    gsl::owner<QLineEdit*> stunval    = nullptr;
    gsl::owner<QLabel*>    strpoints  = nullptr;
    gsl::owner<QLabel*>    dexpoints  = nullptr;
    gsl::owner<QLabel*>    conpoints  = nullptr;
    gsl::owner<QLabel*>    intpoints  = nullptr;
    gsl::owner<QLabel*>    egopoints  = nullptr;
    gsl::owner<QLabel*>    prepoints  = nullptr;
    gsl::owner<QLabel*>    ocvpoints  = nullptr;
    gsl::owner<QLabel*>    dcvpoints  = nullptr;
    gsl::owner<QLabel*>    omcvpoints = nullptr;
    gsl::owner<QLabel*>    dmcvpoints = nullptr;
    gsl::owner<QLabel*>    spdpoints  = nullptr;
    gsl::owner<QLabel*>    pdpoints   = nullptr;
    gsl::owner<QLabel*>    edpoints   = nullptr;
    gsl::owner<QLabel*>    recpoints  = nullptr;
    gsl::owner<QLabel*>    endpoints  = nullptr;
    gsl::owner<QLabel*>    bodypoints = nullptr;
    gsl::owner<QLabel*>    stunpoints = nullptr;
    gsl::owner<QLabel*>    strroll    = nullptr;
    gsl::owner<QLabel*>    dexroll    = nullptr;
    gsl::owner<QLabel*>    conroll    = nullptr;
    gsl::owner<QLabel*>    introll    = nullptr;
    gsl::owner<QLabel*>    egoroll    = nullptr;
    gsl::owner<QLabel*>    preroll    = nullptr;
    gsl::owner<QLabel*>    totalcost  = nullptr;

    gsl::owner<QLabel*>    maximumend  = nullptr;
    gsl::owner<QLabel*>    maximumbody = nullptr;
    gsl::owner<QLabel*>    maximumstun = nullptr;
    gsl::owner<QLineEdit*> currentend  = nullptr;
    gsl::owner<QLineEdit*> currentbody = nullptr;
    gsl::owner<QLineEdit*> currentstun = nullptr;

    gsl::owner<QLabel*>        hthdamage         = nullptr;
    gsl::owner<QLabel*>        lift              = nullptr;
    gsl::owner<QLabel*>        strendcost        = nullptr;
    QList<gsl::owner<QLabel*>> phases            = { };
    gsl::owner<QLabel*>        baseocv           = nullptr;
    gsl::owner<QLabel*>        basedcv           = nullptr;
    gsl::owner<QLabel*>        baseomcv          = nullptr;
    gsl::owner<QLabel*>        basedmcv          = nullptr;
    gsl::owner<QTextEdit*>     combatskilllevels = nullptr;
    gsl::owner<QLabel*>        presenceattack    = nullptr;

    gsl::owner<QTableWidget*> movement    = nullptr;
    gsl::owner<QLabel*>       movementsfx = nullptr;

    gsl::owner<QLabel*>  image      = nullptr;
    gsl::owner<QMenu*>   imageMenu  = nullptr;
    gsl::owner<QAction*> newImage   = nullptr;
    gsl::owner<QAction*> clearImage = nullptr;

    gsl::owner<QLabel*> banner1 = nullptr;
    gsl::owner<QLabel*> banner2 = nullptr;
    gsl::owner<QLabel*> banner3 = nullptr;

    gsl::owner<QTableWidget*> attacksandmaneuvers = nullptr;

    gsl::owner<QTableWidget*> defenses = nullptr;

    gsl::owner<QLabel*>    perceptionroll           = nullptr;
    gsl::owner<QTextEdit*> enhancedandunusualsenses = nullptr;

    gsl::owner<QLabel*>    totalpoints           = nullptr;
    gsl::owner<QLineEdit*> totalexperienceearned = nullptr;
    gsl::owner<QLabel*>    experiencespent       = nullptr;
    gsl::owner<QLabel*>    experienceunspent     = nullptr;

    gsl::owner<QLabel*>    charactername2 = nullptr;
    gsl::owner<QLineEdit*> height         = nullptr;
    gsl::owner<QLineEdit*> weight         = nullptr;
    gsl::owner<QLineEdit*> haircolor      = nullptr;
    gsl::owner<QLineEdit*> eyecolor       = nullptr;

    gsl::owner<QLineEdit*> campaignname = nullptr;
    gsl::owner<QLineEdit*> genre        = nullptr;
    gsl::owner<QLineEdit*> gamemaster   = nullptr;

    gsl::owner<QTableWidget*> skillstalentsandperks          = nullptr;
    gsl::owner<QLabel*>       totalskillstalentsandperkscost = nullptr;
    gsl::owner<QMenu*>        skillstalentsandperksMenu      = nullptr;
    gsl::owner<QAction*>      newSkillTalentOrPerk           = nullptr;
    gsl::owner<QAction*>      editSkillTalentOrPerk          = nullptr;
    gsl::owner<QAction*>      deleteSkillTalentOrPerk        = nullptr;
    gsl::owner<QAction*>      cutSkillTalentOrPerk           = nullptr;
    gsl::owner<QAction*>      copySkillTalentOrPerk          = nullptr;
    gsl::owner<QAction*>      pasteSkillTalentOrPerk         = nullptr;
    gsl::owner<QAction*>      moveSkillTalentOrPerkUp        = nullptr;
    gsl::owner<QAction*>      moveSkillTalentOrPerkDown      = nullptr;

    gsl::owner<QTableWidget*> complications        = nullptr;
    gsl::owner<QLabel*>       totalcomplicationpts = nullptr;
    gsl::owner<QMenu*>        complicationsMenu    = nullptr;
    gsl::owner<QAction*>      newComplication      = nullptr;
    gsl::owner<QAction*>      editComplication     = nullptr;
    gsl::owner<QAction*>      deleteComplication   = nullptr;
    gsl::owner<QAction*>      cutComplication      = nullptr;
    gsl::owner<QAction*>      copyComplication     = nullptr;
    gsl::owner<QAction*>      pasteComplication    = nullptr;
    gsl::owner<QAction*>      moveComplicationUp   = nullptr;
    gsl::owner<QAction*>      moveComplicationDown = nullptr;

    gsl::owner<QTableWidget*> powersandequipment          = nullptr;
    gsl::owner<QLabel*>       totalpowersandequipmentcost = nullptr;
    gsl::owner<QMenu*>        powersandequipmentMenu      = nullptr;
    gsl::owner<QAction*>      newPowerOrEquipment         = nullptr;
    gsl::owner<QAction*>      editPowerOrEquipment        = nullptr;
    gsl::owner<QAction*>      deletePowerOrEquipment      = nullptr;
    gsl::owner<QAction*>      cutPowerOrEquipment         = nullptr;
    gsl::owner<QAction*>      copyPowerOrEquipment        = nullptr;
    gsl::owner<QAction*>      pastePowerOrEquipment       = nullptr;
    gsl::owner<QAction*>      movePowerOrEquipmentUp      = nullptr;
    gsl::owner<QAction*>      movePowerOrEquipmentDown    = nullptr;
    gsl::owner<QGridLayout*>  layout                      = nullptr;

    gsl::owner<QPlainTextEdit*> notes      = nullptr;
    gsl::owner<QWidget*>        page3      = nullptr;
    gsl::owner<QLabel*>         head       = nullptr;
    gsl::owner<QLabel*>         hands      = nullptr;
    gsl::owner<QLabel*>         arms       = nullptr;
    gsl::owner<QLabel*>         shoulders  = nullptr;
    gsl::owner<QLabel*>         chest      = nullptr;
    gsl::owner<QLabel*>         stomach    = nullptr;
    gsl::owner<QLabel*>         vitals     = nullptr;
    gsl::owner<QLabel*>         thighs     = nullptr;
    gsl::owner<QLabel*>         legs       = nullptr;
    gsl::owner<QLabel*>         feet       = nullptr;
    gsl::owner<QLabel*>         averageDEF = nullptr;
    gsl::owner<QLabel*>         DCVmod     = nullptr;
    gsl::owner<QLabel*>         armorNotes = nullptr;

    QList<gsl::owner<QWidget*>> widgets;
    QList<gsl::owner<QWidget*>> hiddenWidgets;
    QList<gsl::owner<QWidget*>> headerWidgets;

    QFont font;
    QFont smallfont;
    QFont largeBoldFont;
    QFont smallBoldWideFont;
    QFont headerFont;

    Sheet_UI() = default;
    Sheet_UI(const Sheet_UI&) = default;
    Sheet_UI(Sheet_UI&&) = default;
    ~Sheet_UI() { }

    Sheet_UI& operator=(const Sheet_UI&) = default;
    Sheet_UI& operator=(Sheet_UI&&) = default;

    static constexpr int StandardFontSize = 12;
    static constexpr int TinyFontSize = 11;
#ifdef unix
    static constexpr int SmallFontPointSize = 7;
    static constexpr int LargeBoldFontSize = 16;
#else
    static constexpr int SmallFontPointSize = 8;
    static constexpr int LargeBoldFontSize = 16;
    static constexpr int HeaderFontSize    = 14;
#endif

    void setupUi(gsl::owner<QWidget*> widget, gsl::owner<QWidget*> hidden)
    {
        layout = new QGridLayout();
        widget->setLayout(layout);

#ifdef __wasm__
        QFile fontResource(":/font/SegoeUIHS.ttf");
        fontResource.open(QIODevice::ReadOnly);
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

#ifndef unix
        headerFont = largeBoldFont;
        headerFont.setPointSize(HeaderFontSize);
        headerFont.setStretch(QFont::Stretch::SemiCondensed);
#endif

        QFont tableFont = font;
        tableFont.setPointSize(StandardFontSize);

        QFont narrow = font;
        narrow.setStretch(QFont::Stretch::SemiCondensed);

        QFont narrowTableFont = narrow;
        narrowTableFont.setPointSize(TinyFontSize);

        smallfont = font;
        smallfont.setPointSize(SmallFontPointSize);
        smallfont.setStretch(QFont::Stretch::SemiCondensed);

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

        QFont tinyFont = smallfont;
        tinyFont.setPointSize(TinyFontSize);
        tinyFont.setStretch(QFont::Stretch::ExtraCondensed);

        QFont tinyBoldFont = tinyFont;
        tinyBoldFont.setBold(true);

        createLabel(widget, largeNarrowBoldFont, "Character Name",       { 61, 76 },  { 175, 27 }); // NOLINT
        createLabel(widget, largeNarrowFont,     "Alternate Identities", { 61, 105 }, { 175, 27 }); // NOLINT
        createLabel(widget, largeNarrowFont,     "Player Name",          { 61, 135 }, { 175, 27 }); // NOLINT

        charactername = createLineEdit(widget, largeBoldFont, { 190,  75 }, { 438, 27 }, "Characters superhero name"); // NOLINT
#if defined(__wasm__) || defined(unix)
        alternateids  = createLineEdit(widget, largeFont,     { 225, 104 }, { 403, 27 }, "Characters secret id, typically"); // NOLINT
        playername    = createLineEdit(widget, largeFont,     { 170, 134 }, { 458, 27 }, "The players name"); // NOLINT
#else
        alternateids  = createLineEdit(widget, largeFont,     { 210, 104 }, { 418, 27 }, "Characters secret id, typically"); // NOLINT
        playername    = createLineEdit(widget, largeFont,     { 162, 134 }, { 466, 27 }, "The players name"); // NOLINT
#endif

        banner1 = createImage(widget, { 654, 76 } , { 293, 109 }, ":/gfx/HeroSystem-Banner.png", false); // NOLINT

#ifdef unix
        createLabel(widget, smallBoldWideFont, "CHARACTERISTICS", { 149, 197 }, { 175, 17 }); // NOLINT
#else
        createLabel(widget, headerFont, "CHARACTERISTICS", { 129, 195 }, { 175, 20 }); // NOLINT
        createLabel(widget, headerFont, "CHARACTERISTICS", { 128, 195 }, { 175, 20 }); // NOLINT
#endif
        createLabel(widget, smallBoldFont, "Val",    { 91, 223 },  { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldFont, "Char",   { 139, 223 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldFont, "Points", { 199, 223 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldFont, "Roll",   { 267, 223 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldFont, "STR",  { 139, 247 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldFont, "DEX",  { 139, 272 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldFont, "CON",  { 139, 296 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldFont, "INT",  { 139, 321 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldFont, "EGO",  { 139, 345 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldFont, "PRE",  { 139, 370 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldFont, "OCV",  { 139, 401 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldFont, "DCV",  { 139, 426 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldFont, "OMCV", { 139, 451 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldFont, "DMCV", { 139, 476 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldFont, "SPD",  { 139, 501 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldFont, "PD",   { 139, 531 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldFont, "ED",   { 139, 556 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldFont, "REC",  { 139, 581 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldFont, "END",  { 139, 606 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldFont, "BODY", { 139, 631 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldFont, "STUN", { 139, 656 }, { 100, 22 }); // NOLINT

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
        strpoints  = createLabel(widget, font,   "0", { 199, 248 }, 20); // NOLINT
        dexpoints  = createLabel(widget, font,   "0", { 199, 273 }, 20); // NOLINT
        conpoints  = createLabel(widget, font,   "0", { 199, 297 }, 20); // NOLINT
        intpoints  = createLabel(widget, font,   "0", { 199, 322 }, 20); // NOLINT
        egopoints  = createLabel(widget, font,   "0", { 199, 346 }, 20); // NOLINT
        prepoints  = createLabel(widget, font,   "0", { 199, 371 }, 20); // NOLINT
        ocvpoints  = createLabel(widget, font,   "0", { 199, 402 }, 20); // NOLINT
        dcvpoints  = createLabel(widget, font,   "0", { 199, 427 }, 20); // NOLINT
        omcvpoints = createLabel(widget, font,   "0", { 199, 452 }, 20); // NOLINT
        dmcvpoints = createLabel(widget, font,   "0", { 199, 477 }, 20); // NOLINT
        spdpoints  = createLabel(widget, font,   "0", { 199, 502 }, 20); // NOLINT
        pdpoints   = createLabel(widget, font,   "0", { 199, 532 }, 20); // NOLINT
        edpoints   = createLabel(widget, font,   "0", { 199, 557 }, 20); // NOLINT
        recpoints  = createLabel(widget, font,   "0", { 199, 582 }, 20); // NOLINT
        endpoints  = createLabel(widget, font,   "0", { 199, 607 }, 20); // NOLINT
        bodypoints = createLabel(widget, font,   "0", { 199, 632 }, 20); // NOLINT
        stunpoints = createLabel(widget, font,   "0", { 199, 657 }, 20); // NOLINT
        strroll    = createLabel(widget, font, "11-", { 269, 248 }, { 75, 20 }); // NOLINT
        dexroll    = createLabel(widget, font, "11-", { 269, 273 }, { 75, 20 }); // NOLINT
        conroll    = createLabel(widget, font, "11-", { 269, 297 }, { 75, 20 }); // NOLINT
        introll    = createLabel(widget, font, "11-", { 269, 322 }, { 75, 20 }); // NOLINT
        egoroll    = createLabel(widget, font, "11-", { 269, 347 }, { 75, 20 }); // NOLINT
        preroll    = createLabel(widget, font, "11-", { 269, 372 }, { 75, 20 }); // NOLINT

        createLabel(widget, smallBoldFont, "Total Cost", { 276, 631 }, { 75, 22 }); // NOLINT
        totalcost  = createLabel(widget, font,   "0", { 276, 657 }, { 75, 20 }); // NOLINT

#ifdef unix
        createLabel(widget, smallBoldWideFont, "CURRENT STATUS", { 455, 197 }, { 135, 17 }); // NOLINT
#else
        createLabel(widget, headerFont, "CURRENT STATUS", { 435, 195 }, { 175, 20 }); // NOLINT
        createLabel(widget, headerFont, "CURRENT STATUS", { 434, 195 }, { 175, 20 }); // NOLINT
#endif
        createLabel(widget, smallBoldFont, "Maximum", { 454, 222 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldFont, "Current", { 542, 222 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldFont, "END",  { 394, 245 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldFont, "BODY", { 394, 270 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldFont, "STUN", { 394, 294 }, { 100, 22 }); // NOLINT

        QString style = "QLineEdit { background: palegreen;"
#ifdef unix
                                 "   color: #000;"
#endif
                                 "   border-style: none;"
                                 " }";

        maximumend  = createLabel(widget, font, "20", { 454, 247 }, 20); // NOLINT
        maximumbody = createLabel(widget, font, "10", { 454, 272 }, 20); // NOLINT
        maximumstun = createLabel(widget, font, "20", { 454, 296 }, 20); // NOLINT
        currentend  = createLineEdit(widget, font, style, "20", { 542, 247 }, { 97, 20 }, "You can keep track of your current END here"); // NOLINT
        currentbody = createLineEdit(widget, font, style, "10", { 542, 272 }, { 97, 20 }, "You can keep track of your current BODY here"); // NOLINT
        currentstun = createLineEdit(widget, font, style, "20", { 542, 296 }, { 97, 20 }, "You can keep track of your current STUN here"); // NOLINT

#ifdef unix
        createLabel(widget, smallBoldWideFont, "VITAL INFORMATION", { 440, 348 }, { 155, 17 }); // NOLINT
#else
        createLabel(widget, headerFont, "VITAL INFORMATION", { 420, 346 }, { 200, 20 }); // NOLINT
        createLabel(widget, headerFont, "VITAL INFORMATION", { 419, 346 }, { 200, 20 }); // NOLINT
#endif
        createLabel(widget, smallBoldNarrowFont, "HTH Damage",      { 397, 375 }, { 100, 22 }); // NOLINT
#ifdef __wasm__
        createLabel(widget, smallNarrowFont,     "(STR/5)d6",       { 486, 375 }, { 65, 22 }); // NOLINT
#else
        createLabel(widget, smallNarrowFont,     "(STR/5)d6",       { 496, 375 }, { 65, 22 }); // NOLINT
#endif
        createLabel(widget, smallNarrowFont,     "Lift",            { 397, 399 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallNarrowFont,     "STR END Cost",    { 510, 399 }, { 85, 22 }); // NOLINT
        createLabel(widget, font,                "1",               { 451, 425 }, { 20, 20}); // NOLINT
        createLabel(widget, font,                "2",               { 464, 425 }, { 20, 20}); // NOLINT
        createLabel(widget, font,                "3",               { 478, 425 }, { 20, 20}); // NOLINT
        createLabel(widget, font,                "4",               { 492, 425 }, { 20, 20}); // NOLINT
        createLabel(widget, font,                "5",               { 506, 425 }, { 20, 20}); // NOLINT
        createLabel(widget, font,                "6",               { 520, 425 }, { 20, 20}); // NOLINT
        createLabel(widget, font,                "7",               { 533, 425 }, { 20, 20}); // NOLINT
        createLabel(widget, font,                "8",               { 547, 425 }, { 20, 20}); // NOLINT
        createLabel(widget, font,                "9",               { 561, 425 }, { 20, 20}); // NOLINT
        createLabel(widget, font,               "10",               { 575, 425 }, { 20, 20}); // NOLINT
        createLabel(widget, font,               "11",               { 597, 425 }, { 20, 20}); // NOLINT
        createLabel(widget, font,               "12",               { 619, 425 }, { 20, 20}); // NOLINT
        createLabel(widget, smallBoldNarrowFont, "Phases",          { 397, 442 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldNarrowFont, "Base OCV",        { 395, 469 }, { 75, 22 }); // NOLINT
        createLabel(widget, smallBoldNarrowFont, "Base DCV",        { 515, 469 }, { 75, 22 }); // NOLINT
        createLabel(widget, smallBoldNarrowFont, "Base OMCV",       { 395, 494 }, { 80, 22 }); // NOLINT
        createLabel(widget, smallBoldNarrowFont, "Base DMCV",       { 515, 494 }, { 80, 22 }); // NOLINT
        createLabel(widget, smallBoldNarrowFont, "Presence Attack", { 395, 661 }, { 150, 22 }); // NOLINT
#ifdef __wasm__
        createLabel(widget, smallNarrowFont,     "(PRE/5)d6",       { 500, 661 }, { 65, 22 }); // NOLINT
#else
        createLabel(widget, smallNarrowFont,     "(PRE/5)d6",       { 510, 661 }, { 55, 22 }); // NOLINT
#endif

        hthdamage         = createLabel(widget, font,   "2d6", { 551, 376 }, { 70, 20 }); // NOLINT
        lift              = createLabel(widget, font, "100kg", { 420, 401 }, { 75, 20 }); // NOLINT
        strendcost        = createLabel(widget, font,     "1", { 603, 401 }, { 45, 20}); // NOLINT
        phases.append(      createLabel(widget, font,      "", { 451, 444 }, { 20, 20})); // NOLINT
        phases.append(      createLabel(widget, font,      "", { 464, 444 }, { 20, 20})); // NOLINT
        phases.append(      createLabel(widget, font,      "", { 478, 444 }, { 20, 20})); // NOLINT
        phases.append(      createLabel(widget, font,      "", { 492, 444 }, { 20, 20})); // NOLINT
        phases.append(      createLabel(widget, font,      "", { 506, 444 }, { 20, 20})); // NOLINT
        phases.append(      createLabel(widget, font,     "X", { 520, 444 }, { 20, 20})); // NOLINT
        phases.append(      createLabel(widget, font,      "", { 533, 444 }, { 20, 20})); // NOLINT
        phases.append(      createLabel(widget, font,      "", { 547, 444 }, { 20, 20})); // NOLINT
        phases.append(      createLabel(widget, font,      "", { 561, 444 }, { 20, 20})); // NOLINT
        phases.append(      createLabel(widget, font,      "", { 580, 444 }, { 20, 20})); // NOLINT
        phases.append(      createLabel(widget, font,      "", { 602, 444 }, { 20, 20})); // NOLINT
        phases.append(      createLabel(widget, font,     "X", { 624, 444 }, { 20, 20})); // NOLINT
        baseocv           = createLabel(widget, font,     "3", { 469, 471 }, { 35, 20}); // NOLINT
        basedcv           = createLabel(widget, font,     "3", { 589, 471 }, { 35, 20}); // NOLINT
        baseomcv          = createLabel(widget, font,     "3", { 483, 496 }, { 35, 20}); // NOLINT
        basedmcv          = createLabel(widget, font,     "3", { 603, 496 }, { 35, 20}); // NOLINT
        combatskilllevels = createTextEdit(widget, narrow, "<b>Combat Skill Levels</b> ", { 392, 520 }, { 244, 145 }); // NOLINT
        presenceattack    = createLabel(widget, font,   "2d6", { 573, 663 }, { 70, 20 }); // NOLINT

#ifdef unix
        createLabel(widget, smallBoldWideFont, "MOVEMENT", { 763, 197 }, { 105, 17 }); // NOLINT
#else
        createLabel(widget, headerFont, "MOVEMENT", { 748, 195 }, { 175, 20 }); // NOLINT
        createLabel(widget, headerFont, "MOVEMENT", { 747, 195 }, { 175, 20 }); // NOLINT
#endif
        createLabel(widget, smallNarrowFont, "Movement SFX", { 678, 420 }, { 100, 22 }); // NOLINT

        movement    = createTableWidget(widget, narrowTableFont,
                                        {   "Type",           "Combat ", "Non-Combat" },
                                        { { "Run (12m)  ",    "12m",     "24m" },
                                          { "Swim (4m)  ",    "4m",       "8m" },
                                          { "H. Leap (4m)  ", "4m",       "8m" },
                                          { "V. Leap (2m)  ", "2m",       "4m" } }, { 675, 225 }, { 260, 195 }); // NOLINT
        movementsfx = createLabel(widget, font, "", { 775, 423 }, 20); // NOLINT

#ifdef unix
        createLabel(widget, smallBoldWideFont, "RANGE MODIFIERS", { 733, 475 }, { 150, 17 }); // NOLINT
#else
        createLabel(widget, headerFont, "RANGE MODIFIERS", { 718, 473 }, { 175, 20 }); // NOLINT
        createLabel(widget, headerFont, "RANGE MODIFIERS", { 717, 473 }, { 175, 20 }); // NOLINT
#endif
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

#ifdef unix
        createLabel(widget, smallBoldWideFont, "ATTACKS & MANEUVERS", { 124, 714 }, { 190, 17 }); // NOLINT
#else
        createLabel(widget, headerFont, "ATTACKS & MANEUVERS", { 104, 711 }, { 230, 20 }); // NOLINT
        createLabel(widget, headerFont, "ATTACKS & MANEUVERS", { 103, 711 }, { 230, 20 }); // NOLINT
#endif

        attacksandmaneuvers = createTableWidget(widget, smallfont,
#if defined(__wasm__) || defined(unix)
                                                {   "Manuvr",       "Phase", "OCV",   "DCV", "Effects" },
#else
                                                {   "Maneuver",     "Phase", "OCV",   "DCV", "Effects" },
#endif
                                                { { "Block",        "½",     "+0",    "+0",  "Block, abort"              },
                                                  { "Brace",        "0",     "+2",    "½",   "+2 OCV vs R Mod"           },
                                                  { "Disarm",       "½",     "-2",    "+0",  "Disarm, STR v. STR"        },
                                                  { "Dodge",        "½",     "——",    "+3",  "Abort vs. all attacks"     },
                                                  { "Grab",         "½",     "-1",    "-2",  "Grab 2 limbs"              },
                                                  { "Grab By",      "½†",    "-3",    "-4",  "Move&Grab;+(ͮvel⁄₁₀) STR"   },
                                                  { "Haymaker",     "½*",    "+0",    "-5",  "+4 DCs to attack"          },
                                                  { "Move By",      "½†",    "-2",    "-2",  "(5+ͮvel⁄₁₀)d6; take ⅓"      },
#if defined(__wasm__) || defined(unix)
                                                  { "Move Thru",    "½†",    "-ͮ⁄₁₀", "-3",  "(10+ͮv⁄₆)d6; take ½ or full" },
                                                  { "Mult.Attx",    "1",     "var",   "½",   "Attack multiple times"     },
#else
                                                  { "Move Through",     "½†", "-ͮ⁄₁₀", "-3", "(10+ͮvel)⁄₆; take ½ or full" },
                                                  { "Multiple Attacks", "1",  "var",   "½",  "Attack multiple times"     },
#endif
                                                  { "Set",          "1",     "+1",    "+0",  "Ranged attacks only"       },
                                                  { "Shove",        "½",     "-1",    "-1",  "Push 2m"                   },
                                                  { "Strike",       "½",     "+0",    "+0",  "2 or weapon"               },
                                                  { "Throw",        "½",     "+0",    "+0",  "Throw w/2d6 dmg"           },
                                                  { "Trip",         "½",     "-1",    "-2",  "Knock target prone"        }
                                                }, { 69, 739 }, { 295, 495 }); // NOLINT

#ifdef unix
        createLabel(widget, smallBoldWideFont, "DEFENSES", { 473, 714 }, { 85, 17 }); // NOLINT
#else
        createLabel(widget, headerFont, "DEFENSES", { 468, 711 }, { 225, 20 }); // NOLINT
        createLabel(widget, headerFont, "DEFENSES", { 467, 711 }, { 225, 20 }); // NOLINT
#endif

        defenses = createTableWidget(widget, narrowTableFont,
                                     {   "Type", "Amount/Effect" },
                                     { { "Normal PD ",      "2" },
                                       { "Resistant PD ",   "0" },
                                       { "Normal ED ",      "2" },
                                       { "Resistant ED ",   "0" },
                                       { "Mental Defense ", "0" },
                                       { "Power Defense ",  "0" },
                                       { "Flash Defense ",  "0" } }, { 392, 739 }, { 249, 270 }); // NOLINT

#ifdef unix
        createLabel(widget, smallBoldWideFont, "SENSES", { 490, 1039 }, { 65, 17 }); // NOLINT
#else
        createLabel(widget, headerFont, "SENSES", { 480, 1037 }, { 225, 20 }); // NOLINT
        createLabel(widget, headerFont, "SENSES", { 479, 1037 }, { 225, 20 }); // NOLINT
#endif
        createLabel(widget, smallBoldNarrowFont, "Perception Roll", { 395, 1065 }, { 105, 20 }); // NOLINT
#ifdef __wasm__
        createLabel(widget, smallNarrowFont,     "(9+INT/5)",       { 496, 1065 }, {  60, 20 }); // NOLINT
#else
        createLabel(widget, smallNarrowFont,     "(9+INT/5)",       { 506, 1065 }, {  50, 20 }); // NOLINT
#endif
        perceptionroll = createLabel(widget, font, "11-", { 569, 1066 }, { 50, 20 }); // NOLINT
        enhancedandunusualsenses = createTextEdit(widget, font, "<b>Enhanced and Unusual Senses</b>", { 390, 1083 }, { 249, 150 }); // NOLINT

#ifdef unix
        createLabel(widget, smallBoldWideFont, "EXPERIENCE POINTS", { 733, 1107 }, { 165, 17 }); // NOLINT
#else
        createLabel(widget, headerFont, "EXPERIENCE POINTS", { 713, 1105 }, { 225, 20 }); // NOLINT
        createLabel(widget, headerFont, "EXPERIENCE POINTS", { 712, 1105 }, { 225, 20 }); // NOLINT
#endif
        createLabel(widget, smallBoldNarrowFont, "Total Points",            { 675, 1133 }, { 170, 22 }); // NOLINT
        createLabel(widget, smallBoldNarrowFont, "Total Experience Earned", { 675, 1156 }, { 170, 22 }); // NOLINT
        createLabel(widget, smallNarrowFont,     "Experience Spent",        { 675, 1181 }, { 170, 22 }); // NOLINT
        createLabel(widget, smallNarrowFont,     "Experience Unspent",      { 675, 1206 }, { 170, 22 }); // NOLINT

        totalpoints           = createLabel(widget, font, "0/325", { 855, 1135 }, { 80, 20 }); // NOLINT
        totalexperienceearned = createLineEdit(widget, font,  "0", { 853, 1158 }, { 80, 20 }, "How much experience your character has earned"); // NOLINT
        experiencespent       = createLabel(widget, font,     "0", { 855, 1183 }, { 80, 20 }); // NOLINT
        experienceunspent     = createLabel(widget, font,   "325", { 855, 1207 }, { 80, 20 }); // NOLINT

#ifdef unix
        createLabel(widget, smallBoldWideFont, "CHARACTER INFORMATION", { 98, 1363 }, { 230, 17 }); // NOLINT
#else
        createLabel(widget, headerFont, "CHARACTER INFORMATION", { 78, 1361 }, { 250, 20 }); // NOLINT
        createLabel(widget, headerFont, "CHARACTER INFORMATION", { 77, 1361 }, { 250, 20 }); // NOLINT
#endif
        createLabel(widget, smallBoldNarrowFont, "Character Name", {  66, 1388 }, { 150, 22 }); // NOLINT
        createLabel(widget, smallBoldNarrowFont, "Height",         {  66, 1413 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldNarrowFont, "Weight",         { 196, 1413 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldNarrowFont, "Hair Color",     {  66, 1439 }, { 100, 22 }); // NOLINT
        createLabel(widget, smallBoldNarrowFont, "Eye Color",      { 196, 1439 }, { 100, 22 }); // NOLINT

        charactername2 = createLabel(widget, font, "", { 184, 1388 }, 20); // NOLINT
        height    = createLineEdit(widget, font,    "2m", { 124, 1414 }, { 72, 20 }, "Your characters height (certain powers may override)"); // NOLINT
        weight    = createLineEdit(widget, font, "100kg", { 249, 1414 }, { 76, 20 }, "Your characters weight (certain powers may override)"); // NOLINT
        haircolor = createLineEdit(widget, font,      "", { 139, 1440 }, { 58, 20 }, "Your characters hair color"); // NOLINT
        eyecolor  = createLineEdit(widget, font,      "", { 262, 1440 }, { 64, 20 }, "Your characters eye color"); // NOLINT

        banner2 = createImage(widget, { 360, 1376 } , { 293, 109 }, ":/gfx/HeroSystem-Banner.png", false); // NOLINT

#ifdef unix
        createLabel(widget, smallBoldWideFont, "CAMPAIGN INFORMATION", { 710, 1363 }, { 230, 17 }); // NOLINT
#else
        createLabel(widget, headerFont, "CAMPAIGN INFORMATION", { 690, 1361 }, { 250, 20 }); // NOLINT
        createLabel(widget, headerFont, "CAMPAIGN INFORMATION", { 689, 1361 }, { 250, 20 }); // NOLINT
#endif
        createLabel(widget, smallBoldNarrowFont, "Campaign Name", { 672, 1389 }, { 200, 22 }); // NOLINT
        createLabel(widget, smallBoldNarrowFont, "Genre",         { 672, 1413 }, { 200, 22 }); // NOLINT
        createLabel(widget, smallBoldNarrowFont, "Gamemaster",    { 672, 1439 }, { 200, 22 }); // NOLINT
        campaignname = createLineEdit(widget, font, "", { 788, 1391 }, { 145, 20 }, "The campaign your character is playing in"); // NOLINT
        genre        = createLineEdit(widget, font, "", { 721, 1416 }, { 213, 20 }, "The kind of game (street level, superhero, galactic, etc)"); // NOLINT
        gamemaster   = createLineEdit(widget, font, "", { 764, 1441 }, { 170, 20 }, "Who is running the game for your character"); // NOLINT

#ifdef unix
        createLabel(widget, smallBoldWideFont, "SKILLS, PERKS, & TALENTS", { 107, 1497 }, { 195, 17 }); // NOLINT
#else
        createLabel(widget, headerFont, "SKILLS, PERKS, & TALENTS", { 82, 1494 }, { 250, 20 }); // NOLINT
        createLabel(widget, headerFont, "SKILLS, PERKS, & TALENTS", { 83, 1494 }, { 250, 20 }); // NOLINT
#endif
        createLabel(widget, smallBoldNarrowFont, "Total Skills,Perks,& Talents Cost", { 112, 2057 }, { 220, 22 }); // NOLINT
        skillstalentsandperks         = createTableWidget(widget, tableFont, { "Cost", "Name                        ", "Roll" },
                                                          { }, { 73, 1521 }, { 265, 535 }, "Things your character is skilled at or has a gift for", Selectable); // NOLINT
        totalskillstalentsandperkscost = createLabel(widget, font, "0", { 73, 2058 }, { 40, 20 }); // NOLINT
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

#ifdef unix
        createLabel(widget, smallBoldWideFont, "COMPLICATIONS", { 142, 2105 }, { 135, 17 }); // NOLINT
#else
        createLabel(widget, headerFont, "COMPLICATIONS", { 122, 2102 }, { 175, 20 }); // NOLINT
        createLabel(widget, headerFont, "COMPLICATIONS", { 123, 2102 }, { 175, 20 }); // NOLINT
#endif
        createLabel(widget, smallBoldNarrowFont, "Total Complications Points", { 117, 2512 }, { 200, 22 }); // NOLINT
        complications        = createTableWidget(widget, tableFont, { "Pts   ", "Complication" },
                                                 { }, { 73, 2130 }, { 260, 383 }, "The things that make life difficult for your character", Selectable); // NOLINT
        totalcomplicationpts = createLabel(widget, font, "0/75", { 73, 2513 }, { 45, 20 }); // NOLINT
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

#ifdef unix
        createLabel(widget, smallBoldWideFont, "POWERS AND EQUIPMENT", { 537, 1497 }, { 200, 17 }); // NOLINT
#else
        createLabel(widget, headerFont, "POWERS AND EQUIPMENT", { 537, 1495 }, { 250, 20 }); // NOLINT
        createLabel(widget, headerFont, "POWERS AND EQUIPMENT", { 536, 1495 }, { 250, 20 }); // NOLINT
#endif
        createLabel(widget, smallBoldNarrowFont, "Total Powers/Equipment Cost", { 410, 2510 }, { 300, 22 }); // NOLINT

        powersandequipment          = createTableWidget(widget, tableFont,
                                                        { "Cost", "Name              ",
                                                          "Power/Equipment                                           ",
                                                          "END" },
                                                        { }, { 367, 1522 }, { 570, 991 }, "Special powers and equipment for your character", Selectable); // NOLINT
        totalpowersandequipmentcost = createLabel(widget, font, "0", { 367, 2511 }, { 40, 20 }); // NOLINT
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

#ifdef unix
        createLabel(hidden, smallBoldWideFont, "KNOCKBACK MODIFIERS", { 110, 85 }, { 180, 17 });
#else
        createLabel(hidden, headerFont, "KNOCKBACK MODIFIERS", { 90, 82 }, { 275, 20 }); // NOLINT
        createLabel(hidden, headerFont, "KNOCKBACK MODIFIERS", { 89, 82 }, { 275, 20 }); // NOLINT
#endif

        banner3 = createImage(hidden, { 360, 76 } , { 293, 109 }, ":/gfx/HeroSystem-Banner.png", false); // NOLINT

#ifdef unix
        createLabel(hidden, smallBoldWideFont, "WALL BODY", { 770, 82 }, { 90, 17 });
#else
        createLabel(hidden, headerFont, "WALL BODY", { 750, 79 }, { 275, 20 }); // NOLINT
        createLabel(hidden, headerFont, "WALL BODY", { 749, 79 }, { 275, 20 }); // NOLINT
#endif

#ifdef unix
        createLabel(hidden, smallBoldWideFont, "NOTES", { 620, 202 }, { 50, 17 });
#else
        createLabel(hidden, headerFont, "NOTES", { 625, 200 }, { 275, 20 }); // NOLINT
        createLabel(hidden, headerFont, "NOTES", { 624, 200 }, { 275, 20 }); // NOLINT
#endif
        notes = createTextEditor(hidden, tableFont, { 365, 229 }, { 575, 1008 }, "Game notes"); // NOLINT

#ifdef unix
        createLabel(hidden, smallBoldWideFont, "HIT LOCATION CHART", { 125, 248 }, { 160, 17 }); // NOLINT
#else
        createLabel(hidden, headerFont, "HIT LOCATION CHART", { 95, 245 }, { 275, 20 }); // NOLINT
        createLabel(hidden, headerFont, "HIT LOCATION CHART", { 94, 245 }, { 275, 20 }); // NOLINT
#endif

        head      = createLabel(hidden, font, "0", { 281, 310 }, { 40, 20 }); // NOLINT
        hands     = createLabel(hidden, font, "0", { 281, 332 }, { 40, 20 }); // NOLINT
        arms      = createLabel(hidden, font, "0", { 281, 355 }, { 40, 20 }); // NOLINT
        shoulders = createLabel(hidden, font, "0", { 281, 378 }, { 40, 20 }); // NOLINT
        chest     = createLabel(hidden, font, "0", { 281, 399 }, { 40, 20 }); // NOLINT
        stomach   = createLabel(hidden, font, "0", { 281, 422 }, { 40, 20 }); // NOLINT
        vitals    = createLabel(hidden, font, "0", { 281, 444 }, { 40, 20 }); // NOLINT
        thighs    = createLabel(hidden, font, "0", { 281, 466 }, { 40, 20 }); // NOLINT
        legs      = createLabel(hidden, font, "0", { 281, 488 }, { 40, 20 }); // NOLINT
        feet      = createLabel(hidden, font, "0", { 281, 510 }, { 40, 20 }); // NOLINT

        averageDEF = createLabel(hidden, font, "0",  { 281, 540 }, { 40, 20 }); // NOLINT
        DCVmod     = createLabel(hidden, font, "+0", { 158, 565 }, { 40, 20 }); // NOLINT
        armorNotes = createLabel(hidden, font, "",   { 158, 590 }, { 40, 20 }); // NOLINT

#ifdef unix
        createLabel(hidden, smallBoldWideFont, "COMBAT MODIFIERS", { 130, 671 }, { 155, 17 }); // NOLINT
#else
        createLabel(hidden, headerFont, "COMBAT MODIFIERS", { 105, 669 }, { 275, 20 }); // NOLINT
        createLabel(hidden, headerFont, "COMBAT MODIFIERS", { 104, 669 }, { 275, 20 }); // NOLINT
#endif

#ifdef unix
        createLabel(hidden, smallBoldWideFont, "SKILL MODIFIERS", { 135, 884 }, { 125, 17 }); // NOLINT
#else
        createLabel(hidden, headerFont, "SKILL MODIFIERS", { 115, 882 }, { 275, 20 }); // NOLINT
        createLabel(hidden, headerFont, "SKILL MODIFIERS", { 114, 882 }, { 275, 20 }); // NOLINT
#endif

        hidden->setVisible(false);
    }
};
