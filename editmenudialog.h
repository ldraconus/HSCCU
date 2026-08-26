#ifndef EDITMENUDIALOG_H
#define EDITMENUDIALOG_H

#include <QDialog>

namespace Ui {
class EditMenuDialog;
}

class EditMenuDialog : public QDialog {
    Q_OBJECT

public:
#ifdef __wasm__
    explicit EditMenuDialog(QWidget *parent = nullptr);
    ~EditMenuDialog();

    void mousePressEvent(QMouseEvent*);
    void showEvent(QShowEvent*);

private:
    Ui::EditMenuDialog *ui;

public slots:
    void doCut();
    void doCopy();
    void doPaste();
    void doOption();
#else
    explicit EditMenuDialog(QWidget *parent = nullptr) { }
    ~EditMenuDialog() { }
#endif
};

#endif // EDITMENUDIALOG_H
