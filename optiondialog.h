#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include "ui_optiondialog.h"
#include <QDialog>

namespace Ui {
class optionDialog;
}

class optionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit optionDialog(QWidget *parent = nullptr);
    ~optionDialog();

    int  complications()         { return ui->complicationsLineEdit->text().toInt(); }
    int  totalPoints()           { return ui->totalPointsLineEdit->text().toInt(); }

    void setTotalPoints(int x)   { ui->totalPointsLineEdit->setText(QString("%1").arg(x)); }
    void setComplications(int x) { ui->complicationsLineEdit->setText(QString("%1").arg(x)); }

private:
    Ui::optionDialog *ui;

    bool isNumber(QString& txt) { bool ok; txt.toInt(&ok, 10); return ok; }

public slots:
    void numeric(QString);
};

#endif // OPTIONDIALOG_H
