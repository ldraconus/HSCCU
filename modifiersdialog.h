#ifndef MODIFIERSDIALOG_H
#define MODIFIERSDIALOG_H

#include <QDialog>

namespace Ui {
class ModifiersDialog;
}

class ModifiersDialog : public QDialog
{
    Q_OBJECT

public:
    static const bool Advantage = true;
    static const bool Limitation = false;

    explicit ModifiersDialog(bool advantage, QString type, QString which, QWidget *parent = nullptr);
    ~ModifiersDialog();

private:
    Ui::ModifiersDialog *ui;
};

#endif // MODIFIERSDIALOG_H
