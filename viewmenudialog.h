#pragma once

#include <QDialog>
#include <QShowEvent>

namespace Ui {
class ViewMenuDialog;
}

class ViewMenuDialog : public QDialog {
    Q_OBJECT

public:
    explicit ViewMenuDialog(QWidget *parent = nullptr);
    ~ViewMenuDialog();

    void setPos(QPoint pos)  { mPos = pos; }

    void showEvent(QShowEvent*) override;

private:
    QPointF             mPos;
    Ui::ViewMenuDialog* mUi;

    void check(QPushButton* btn);
};
