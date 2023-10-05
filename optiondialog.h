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
    optionDialog(const optionDialog&) = delete;
    optionDialog(optionDialog&&) = delete;
    ~optionDialog() override;

    optionDialog& operator=(const optionDialog&) = delete;
    optionDialog& operator=(optionDialog&&) = delete;

    int  complications()      { return ui->complicationsLineEdit->text().toInt(); }
    bool equipmentFree()      { return ui->equipmentFree->isChecked(); }
    bool showFrequencyRolls() { return ui->showFrequencyRolls->isChecked(); }
    bool showNotesPage()      { return ui->notePageShown->isChecked(); }
    bool normalHumanMaxima()  { return ui->normalHumanMaxima->isChecked(); }
    int  activePointsPerEND() { return ui->activePointsPerEND->text().toInt(); }
    int  totalPoints()        { return ui->totalPointsLineEdit->text().toInt(); }

    void setComplications(int x)       { ui->complicationsLineEdit->setText(QString("%1").arg(x)); }
    void setEquipmentFree(bool f)      { ui->equipmentFree->setChecked(f); }
    void setShowFrequencyRolls(bool f) { ui->showFrequencyRolls->setChecked(f); }
    void setShowNotesPage(bool f)      { ui->notePageShown->setChecked(f); }
    void setNormalHumanMaxima(bool f)  { ui->normalHumanMaxima->setChecked(f); }
    void setActivePointsPerEND(int x)  { ui->activePointsPerEND->setText(QString("%1").arg(x)); }
    void setTotalPoints(int x)         { ui->totalPointsLineEdit->setText(QString("%1").arg(x)); }

private:
    Ui::optionDialog *ui;

    bool isNumber(QString& txt) {
        bool ok {};
        txt.toInt(&ok);
        return ok;
    }

public slots:
    void accept() override;
    void numeric(QString);
    void pickSomething(int);
};

#endif // OPTIONDIALOG_H
