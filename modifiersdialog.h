#ifndef MODIFIERSDIALOG_H
#define MODIFIERSDIALOG_H

#include "dialog.h"

#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

#include "shared.h"

namespace Ui {
class ModifiersDialog;
}

using std::shared_ptr;

class ModifiersDialog: public Dialog {
    Q_OBJECT

public:
    static constexpr bool Advantage = true;
    static constexpr bool Limitation = false;

    explicit ModifiersDialog(bool, QWidget *parent = nullptr);
    ModifiersDialog(const ModifiersDialog&) = delete;
    ModifiersDialog(ModifiersDialog&&) = delete;
    ~ModifiersDialog() override;

    ModifiersDialog& operator=(const ModifiersDialog&) = delete;
    ModifiersDialog& operator=(ModifiersDialog&&) = delete;

    std::shared_ptr<class Modifier> modifier() { return mModifier; }
    bool modifier(std::shared_ptr<class Modifier>&);

    void updateForm();

    /* Per the code, we need to wait until the dialog is full ready before we can just accept it.  With WASM,
     * we can't reliably run timers because of event loop issues, so we're trying this */
    void showEvent(QShowEvent*) override {
        if (mJustAccept) accept();
    }

    static ModifiersDialog& ref() { return *mModifiersDialog; }

private:
    static ModifiersDialog* mModifiersDialog; // NOLINT
    Ui::ModifiersDialog *ui;

    shared_ptr<class Modifier>  mModifier = nullptr;
    shared_ptr<class Modifiers> mModifiers = nullptr;

    bool                 mAccepted = false;
    bool                 mAdd = false;
    QLabel*  mDescription = nullptr;
    bool                 mJustAccept = false;
    QPushButton*         mOk = nullptr;
    bool                 mSkipUpdate = false;

    QLabel* createLabel(QVBoxLayout*, QString, bool wrap = false);
    void                setModifiers(bool);

public slots:
    void doAccepted();
    void currentIndexChanged(int);
    void itemChanged(QTreeWidgetItem*, int);
    void itemSelectionChanged();
    void justAccept() { accept(); }
    void pickOne(int);
    void stateChanged(bool state);
    void textChanged(QString);
};

#endif // MODIFIERSDIALOG_H
