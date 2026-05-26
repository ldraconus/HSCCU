#ifndef COMPLICATIONSDIALOG_H
#define COMPLICATIONSDIALOG_H

#include "complication.h"

#include <QDialog>
#include <QPushButton>

namespace Ui {
class ComplicationsDialog;
}

class ComplicationsDialog : public QDialog {
    Q_OBJECT

public:
    ComplicationsDialog() = delete;
    ComplicationsDialog(const ComplicationsDialog&) = delete;
    ComplicationsDialog(ComplicationsDialog &&) = delete;
    explicit ComplicationsDialog(QWidget *parent = nullptr);
    ~ComplicationsDialog() override;

    ComplicationsDialog& operator=(const ComplicationsDialog&) = delete;
    ComplicationsDialog& operator=(ComplicationsDialog&&) = delete;

    shared_ptr<Complication> complication() { return mComplication; }
    ComplicationsDialog& complication(shared_ptr<Complication>& c);

private:
    Ui::ComplicationsDialog *ui;

    static const bool WordWrap = true;

    shared_ptr<Complication> mComplication = nullptr;
    gsl::owner<QLabel*>      mDescription = nullptr;
    gsl::owner<QPushButton*> mOk = nullptr;
    gsl::owner<QLabel*>      mPoints = nullptr;
    bool                     mSkipUpdate = false;

    gsl::owner<QLabel*> createLabel(QVBoxLayout*, QString, bool wordWrap = false);
    void                updateForm();

public slots:
    void currentIndexChanged(int) { updateForm(); }

    void pickComplication(int);
    void stateChanged(int state);
    void textChanged(QString);
};

#endif // COMPLICATIONSDIALOG_H
