#ifndef MODIFIERSDIALOG_H
#define MODIFIERSDIALOG_H

#include "shared.h"

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

namespace Ui {
class ModifiersDialog;
}

class ModifiersDialog : public QDialog
{
    Q_OBJECT

public:
    static const bool Advantage = true;
    static const bool Limitation = false;

    explicit ModifiersDialog(bool, QWidget *parent = nullptr);
    ~ModifiersDialog();

    shared_ptr<class Modifier> modifier() { return _modifier; }
    ModifiersDialog&           modifier(shared_ptr<class Modifier>&);

    void updateForm();

    void showEvent(QShowEvent*) override {
    }

    static ModifiersDialog& ref() { return *_modifiersDialog; }

private:
    static ModifiersDialog* _modifiersDialog;
    Ui::ModifiersDialog *ui;

    shared_ptr<class Modifier>  _modifier;
    shared_ptr<class Modifiers> _modifiers;

    QLabel*              _mod;
    QLabel*              _description;
    QPushButton*         _ok;
    bool                 _skipUpdate = false;
    bool                 _accepted = false;

    QLabel* createLabel(QVBoxLayout*, QString, bool wrap = false);
    void    setModifiers(bool);


public slots:
    void pickOne(int);
    void stateChanged(int state);
    void textChanged(QString);
    void currentIndexChanged(int);
    void itemSelectionChanged();
    void itemChanged(QTreeWidgetItem*, int);

    void justAccept() { accept(); }
};

#endif // MODIFIERSDIALOG_H
