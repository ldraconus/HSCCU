#ifndef COMPLICATIONSDIALOG_H
#define COMPLICATIONSDIALOG_H

#include "complication.h"

#include <QDialog>
#include <QPushButton>

namespace Ui {
class ComplicationsDialog;
}

class ComplicationsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ComplicationsDialog(QWidget *parent = nullptr);
    ~ComplicationsDialog();

    shared_ptr<Complication> complication() { return _complication; }
    ComplicationsDialog& complication(shared_ptr<Complication>& c);

private:
    Ui::ComplicationsDialog *ui;

    static const bool WordWrap = true;

    shared_ptr<Complication> _complication;
    QLabel*                  _description;
    QPushButton*             _ok;
    QLabel*                  _points;
    bool                     _skipUpdate = false;

    QLabel* createLabel(QVBoxLayout*, QString, bool wordWrap = false);
    void    updateForm();

public slots:
    void currentIndexChanged(int) { updateForm(); }

    void pickComplication(int);
    void stateChanged(int state);
    void textChanged(QString);
};

#endif // COMPLICATIONSDIALOG_H
