#ifndef SKILLDIALOG_H
#define SKILLDIALOG_H

#include "skilltalentorperk.h"

#include <QDialog>
#include <QPushButton>

namespace Ui {
class SkillDialog;
}

class SkillDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SkillDialog(QWidget *parent = nullptr);
    ~SkillDialog();

    SkillTalentOrPerk* skilltalentorperk() { return _skilltalentorperk; }

    SkillDialog& skilltalentorperk(SkillTalentOrPerk* s);

private:
    Ui::SkillDialog *ui;

    static const bool WordWrap = true;

    QLabel*            _description;
    QLabel*            _points;
    QPushButton*       _ok;
    SkillTalentOrPerk* _skilltalentorperk = nullptr;
    bool               _skipUpdate = false;

    QLabel* createLabel(QVBoxLayout* parent, QString text, bool wordWrap);
    void    updateForm();

public slots:
    void currentIndexChanged(int) { updateForm(); }

    void pickOne(int);
    void pickType(int);
    void stateChanged(int state);
    void textChanged(QString);
};

#endif // SKILLDIALOG_H
