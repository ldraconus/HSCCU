#ifndef SKILLDIALOG_H
#define SKILLDIALOG_H

#include "shared.h"
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

    shared_ptr<SkillTalentOrPerk> skilltalentorperk() { return _skilltalentorperk; }

    SkillDialog& skilltalentorperk(shared_ptr<SkillTalentOrPerk> s);

private:
    Ui::SkillDialog *ui;

    static const bool WordWrap = true;

    QLabel*            _description;
    QLabel*            _points;
    QPushButton*       _ok;
    bool               _skipUpdate = false;

    shared_ptr<SkillTalentOrPerk> _skilltalentorperk = nullptr;

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
