#ifndef COMPLICATIONSDIALOG_H
#define COMPLICATIONSDIALOG_H

#include "complication.h"
#include "dialog.h"

#include <QPushButton>

namespace Ui {
class ComplicationsDialog;
}

class ComplicationsDialog: public Dialog {
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
    QLabel*      mDescription = nullptr;
    QPushButton* mOk = nullptr;
    QLabel*      mPoints = nullptr;
    bool                     mSkipUpdate = false;

    QLabel* createLabel(QVBoxLayout*, QString, bool wordWrap = false);
    void                updateForm();

public slots:
    void currentIndexChanged(int) { updateForm(); }

    void pickComplication(int);
    void stateChanged(int state);
    void textChanged(QString);
};

#endif // COMPLICATIONSDIALOG_H
