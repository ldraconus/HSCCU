#ifndef POWERDIALOG_H
#define POWERDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QTableWidget>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QVBoxLayout>

#include "powers.h"
#include "shared.h"

namespace Ui {
class PowerDialog;
}

class PowerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PowerDialog(QWidget *parent = nullptr, shared_ptr<Power>& save = _dummy);
    PowerDialog(const PowerDialog&) = delete;
    PowerDialog(PowerDialog&&) = delete;
    ~PowerDialog() override;

    PowerDialog& operator=(const PowerDialog&) = delete;
    PowerDialog& operator=(PowerDialog&&) = delete;

    QTableWidget* createAdvantages(QWidget* parent, QVBoxLayout* layout);
    QTableWidget* createLimitations(QWidget* parent, QVBoxLayout* layout);

    PowerDialog&  powerorequipment(shared_ptr<class Power> s);

    Ui::PowerDialog* UI() { return ui; }

    shared_ptr<class Power> powerorequipment() { return _power; }
    void                    multipower()       { _inMultipower = true; }
    bool                    isAccepted()       { return _done && _accepted; }
    bool                    isCanceled()       { return _done && !_accepted; }
    bool                    isDone()           { return _done; }

    void setCellLabel(QTableWidget*, int, int, QString, QFont&);
    void setColumns(QTableWidget* tablewidget);
    void updateForm();

    shared_ptr<Power> saved() { return _saved; }
    void              save(shared_ptr<Power> s) { _saved = s; }

    static PowerDialog& ref() { return *_ptr; }

    static PowerDialog* _ptr; // NOLINT

    void setEquipment() { _equipment = true; }
private:
    Ui::PowerDialog *ui;

    static shared_ptr<Power> _dummy; // NOLINT
    static const bool WordWrap = true;

    bool                        _accepted = false;
    QTableWidget*               _advantages = nullptr;
    QMenu*                      _advantagesMenu = nullptr;
    QPushButton*                _cancel = nullptr;
    QLabel*                     _description = nullptr;
    bool                        _done = false;
    bool                        _equipment = false;
    QLabel*                     _errorMsg = nullptr;
    bool                        _inMultipower = false;
    QTableWidget*               _limitations = nullptr;
    QMenu*                      _limitationsMenu = nullptr;
    shared_ptr<ModifiersDialog> _mod = nullptr;
    QPushButton*                _ok = nullptr;
    QLabel*                     _points = nullptr;
    shared_ptr<Power>           _power = nullptr;
    shared_ptr<Power>&          _saved; // NOLINT
    bool                        _skipUpdate = false;

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
        virtual ~menuItems() { }

        menuItems& operator=(const menuItems&) = delete;
        menuItems& operator=(menuItems&&) = delete;

        QString text = "";
        QAction** action = nullptr;
    };


    QMenu*        createMenu(QWidget*, const QFont&, QList<menuItems>);
    QLabel*       createLabel(QVBoxLayout* parent, QString text, bool wordWrap = false);
    QWidget*      createEditButton(QWidget*, QVBoxLayout*, const QList<const char*>&);
    QTableWidget* createTableWidget(QWidget*, QVBoxLayout*, QList<int>, QString, int);
    void          setupPower(shared_ptr<Power>&);

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
    void doUpdate()                                 { updateForm(); }

    void aboutToShowAdvantagesMenu();
    void aboutToShowLimitationsMenu();
    void activated(int);
    void advantagesMenu(QPoint);
    void buttonPressed(bool);
    void cancel();
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
    void ok();
    void pasteAdvantage();
    void pasteLimitation();
    void pickOne(int);
    void pickType(int);
    void stateChanged(int state);
    void textChanged(QString);
};

#endif // POWERDIALOG_H
