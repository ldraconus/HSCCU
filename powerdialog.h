#ifndef POWERDIALOG_H
#define POWERDIALOG_H

#include "dialog.h"
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

class PowerDialog : public Dialog {
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

    shared_ptr<class Power> powerorequipment() { return mPower; }
    void                    multipower()       { mInMultipower = true; }
    bool                    isAccepted()       { return mDone && mAccepted; }
    bool                    isCanceled()       { return mDone && !mAccepted; }
    bool                    isDone()           { return mDone; }

    void setCellLabel(QTableWidget*, int, int, QString, QFont&);
    void setColumns(QTableWidget* tablewidget);
    void updateForm();

    shared_ptr<Power> saved() { return mSaved; }
    void              save(shared_ptr<Power> s) { mSaved = s; }

    static PowerDialog& ref() { return *mPtr; }

    static PowerDialog* mPtr; // NOLINT

    void setEquipment() { mEquipment = true; }

protected:
    void showEvent(QShowEvent* event) override { Dialog::showEvent(event); mOk->setEnabled(false); }

private:
    Ui::PowerDialog *ui;

    static shared_ptr<Power> _dummy; // NOLINT
    static const bool WordWrap = true;

    bool                        mAccepted = false;
    QTableWidget*               mAdvantages = nullptr;
    QMenu*                      mAdvantagesMenu = nullptr;
    QPushButton*                mCancel = nullptr;
    QLabel*                     mDescription = nullptr;
    bool                        mDone = false;
    bool                        mEquipment = false;
    QLabel*                     mErrorMsg = nullptr;
    bool                        mInMultipower = false;
    QTableWidget*               mLimitations = nullptr;
    QMenu*                      mLimitationsMenu = nullptr;
    shared_ptr<ModifiersDialog> mMod = nullptr;
    QPushButton*                mOk = nullptr;
    QLabel*                     mPoints = nullptr;
    shared_ptr<Power>           mPower = nullptr;
    shared_ptr<Power>&          mSaved; // NOLINT
    bool                        mSkipUpdate = false;

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
    QWidget*      createEditButton(QWidget*, QVBoxLayout*, const QList<std::function<void(bool)>>&);
    QTableWidget* createTableWidget(QWidget*, QVBoxLayout*, QList<int>, QString, int);
    void          setupPower(shared_ptr<Power>&);

    QAction* mNewAdvantage      = nullptr;
    QAction* mEditAdvantage     = nullptr;
    QAction* mDeleteAdvantage   = nullptr;
    QAction* mCutAdvantage      = nullptr;
    QAction* mCopyAdvantage     = nullptr;
    QAction* mPasteAdvantage    = nullptr;
    QAction* mMoveAdvantageUp   = nullptr;
    QAction* mMoveAdvantageDown = nullptr;

    QAction* mNewLimitation      = nullptr;
    QAction* mEditLimitation     = nullptr;
    QAction* mDeleteLimitation   = nullptr;
    QAction* mCutLimitation      = nullptr;
    QAction* mCopyLimitation     = nullptr;
    QAction* mPasteLimitation    = nullptr;
    QAction* mMoveLimitationUp   = nullptr;
    QAction* mMoveLimitationDown = nullptr;


public slots:
    void advantageDoubleClicked(QTableWidgetItem*)  { editAdvantage(true); }
    void limitationDoubleClicked(QTableWidgetItem*) { editLimitation(true); }
    void doUpdate()                                 { updateForm(); }

    void aboutToShowAdvantagesMenu();
    void aboutToShowLimitationsMenu();
    void activated(int);
    void advantagesMenu(QPoint);
    void buttonPressed(bool);
    void cancel();
    void copyAdvantage(bool);
    void copyLimitation(bool);
    void currentIndexChanged(int);
    void cutAdvantage(bool);
    void cutLimitation(bool);
    void deleteAdvantage(bool);
    void deleteLimitation(bool);
    void editAdvantage(bool);
    void editLimitation(bool);
    void itemChanged(QTreeWidgetItem*,int);
    void itemSelectionChanged();
    void limitationsMenu(QPoint);
    void moveAdvantageDown(bool);
    void moveLimitationDown(bool);
    void moveAdvantageUp(bool);
    void moveLimitationUp(bool);
    void newAdvantage(bool);
    void newLimitation(bool);
    void ok();
    void pasteAdvantage(bool);
    void pasteLimitation(bool);
    void pickOne(int);
    void pickType(int);
    void stateChanged(bool state);
    void textChanged(QString);
};

#endif // POWERDIALOG_H
