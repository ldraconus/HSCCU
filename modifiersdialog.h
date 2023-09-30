#ifndef MODIFIERSDIALOG_H
#define MODIFIERSDIALOG_H

#include <QDialog>
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

class ModifiersDialog : public QDialog
{
    Q_OBJECT

public:
    static constexpr bool Advantage = true;
    static constexpr bool Limitation = false;

    explicit ModifiersDialog(bool, QWidget *parent = nullptr);
    ~ModifiersDialog();

    std::shared_ptr<class Modifier> modifier() { return _modifier; }
    bool modifier(std::shared_ptr<class Modifier>&);

    void updateForm();

    /* Per the code, we need to wait until the dialog is full ready before we can just accept it.  With WASM,
     * we can't reliably run timers because of event loop issues, so we're trying this */
    void showEvent(QShowEvent*) override {
        if (_justAccept) accept();
    }

    static ModifiersDialog& ref() { return *_modifiersDialog; }

private:
    static ModifiersDialog* _modifiersDialog;
    Ui::ModifiersDialog *ui;

    shared_ptr<class Modifier>  _modifier = nullptr;
    shared_ptr<class Modifiers> _modifiers = nullptr;

    bool                 _accepted = false;
    bool                 _add = false;
    QLabel*              _description;
    bool                 _justAccept = false;
    QLabel*              _mod;
    QPushButton*         _ok;
    bool                 _skipUpdate = false;

    QLabel* createLabel(QVBoxLayout*, QString, bool wrap = false);
    void    setModifiers(bool);

public slots:
    void doAccepted();
    void currentIndexChanged(int);
    void itemChanged(QTreeWidgetItem*, int);
    void itemSelectionChanged();
    void justAccept() { accept(); }
    void pickOne(int);
    void stateChanged(int state);
    void textChanged(QString);
};

#endif // MODIFIERSDIALOG_H
