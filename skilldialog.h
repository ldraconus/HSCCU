#ifndef SKILLDIALOG_H
#define SKILLDIALOG_H

#include <QDialog>

namespace Ui {
class SkillDialog;
}

class SkillDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SkillDialog(QWidget *parent = nullptr);
    ~SkillDialog();

private:
    Ui::SkillDialog *ui;
};

#endif // SKILLDIALOG_H
