#ifndef FILEMENUDIALOG_H
#define FILEMENUDIALOG_H
#ifdef __wasm__
#include <QDialog>

namespace Ui {
class FileMenuDialog;
}

class FileMenuDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileMenuDialog(QWidget *parent = nullptr);
    ~FileMenuDialog();

    void setSave(bool enabled);

    void showEvent(QShowEvent* se);

private:
    Ui::FileMenuDialog *ui;

public slots:
    void doNew();
    void doOpen();
    void doSave();
};
#endif

#endif // FILEMENUDIALOG_H
