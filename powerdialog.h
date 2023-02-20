#ifndef POWERDIALOG_H
#define POWERDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QTableWidget>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QVBoxLayout>

#include "shared.h"

namespace Ui {
class PowerDialog;
}

class PowerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PowerDialog(QWidget *parent = nullptr);
    ~PowerDialog();

    QTableWidget* createAdvantages(QWidget* parent, QVBoxLayout* layout);
    QTableWidget* createLimitations(QWidget* parent, QVBoxLayout* layout);

    PowerDialog&  powerorequipment(shared_ptr<class Power> s);

    static Ui::PowerDialog* UI() { return ui; }

    shared_ptr<class Power> powerorequipment() { return _power; }
    void                    multipower()       { _inMultipower = true; }

    void setCellLabel(QTableWidget*, int, int, QString, QFont&);
    void setColumns(QTableWidget* tablewidget);
    void updateForm();

    static PowerDialog& ref() { return *_ptr; }

    static PowerDialog* _ptr;

private:
    static Ui::PowerDialog *ui;

    static const bool WordWrap = true;

    QTableWidget*     _advantages;
    QMenu*            _advantagesMenu;
    QLabel*           _description;
    bool              _inMultipower;
    QTableWidget*     _limitations;
    QMenu*            _limitationsMenu;
    QLabel*           _points;
    QPushButton*      _ok;
    shared_ptr<Power> _power = nullptr;
    bool              _skipUpdate = false;

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

    QMenu*        createMenu(QWidget*, const QFont&, QList<menuItems>);
    QLabel*       createLabel(QVBoxLayout* parent, QString text, bool wordWrap = false);
    QTableWidget* createTableWidget(QWidget*, QVBoxLayout*, QList<int>, QString, int);
    QAction* _newAdvantage      = nullptr;
    QAction* _editAdvantage     = nullptr;
    QAction* _deleteAdvantage   = nullptr;
    QAction* _cutAdvantage      = nullptr;
    QAction* _copyAdvantage     = nullptr;
    QAction* _pasteAdvantage    = nullptr;
    QAction* _moveAdvantageUp   = nullptr;
    QAction* _moveAdvantageDown = nullptr;

    QAction* _newLimitation      = nullptr;
    QAction* _editLimitation     = nullptr;
    QAction* _deleteLimitation   = nullptr;
    QAction* _cutLimitation      = nullptr;
    QAction* _copyLimitation     = nullptr;
    QAction* _pasteLimitation    = nullptr;
    QAction* _moveLimitationUp   = nullptr;
    QAction* _moveLimitationDown = nullptr;


public slots:
    void advantageDoubleClicked(QTableWidgetItem*)  { editAdvantage(); }
    void limitationDoubleClicked(QTableWidgetItem*) { editLimitation(); }

    void aboutToShowAdvantagesMenu();
    void aboutToShowLimitationsMenu();
    void advantagesMenu(QPoint);
    void copyAdvantage();
    void copyLimitation();
    void currentIndexChanged(int);
    void cutAdvantage();
    void cutLimitation();
    void deleteAdvantage();
    void deleteLimitation();
    void editAdvantage();
    void editLimitation();
    void itemChanged(QTreeWidgetItem*,int);
    void itemSelectionChanged();
    void limitationsMenu(QPoint);
    void moveAdvantageDown();
    void moveLimitationDown();
    void moveAdvantageUp();
    void moveLimitationUp();
    void newAdvantage();
    void newLimitation();
    void pasteAdvantage();
    void pasteLimitation();
    void pickOne(int);
    void pickType(int);
    void stateChanged(int state);
    void textChanged(QString);
};

#endif // POWERDIALOG_H
