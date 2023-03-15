#ifndef EDITMENUDIALOG_H
#define EDITMENUDIALOG_H

#include <QDialog>

#ifdef __wasm__
namespace Ui {
class EditMenuDialog;
}

class EditMenuDialog : public QDialog
{
    Q_OBJECT

public:
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
};
#endif

#endif // EDITMENUDIALOG_H
