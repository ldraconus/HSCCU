#pragma once

#include <QDialog>

namespace Ui {
class ViewMenuDialog;
}

class ViewMenuDialog : public QDialog {
    Q_OBJECT

public:
    explicit ViewMenuDialog(QWidget *parent = nullptr);
    ~ViewMenuDialog();

private:
    Ui::ViewMenuDialog *ui;
};
