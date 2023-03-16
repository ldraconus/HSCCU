#ifndef SKILLMENUDIALOG_H
#define SKILLMENUDIALOG_H

#include <QDialog>

namespace Ui {
class SkillMenuDialog;
}

class SkillMenuDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SkillMenuDialog(QWidget *parent = nullptr);
    ~SkillMenuDialog();

    void setEdit(bool x)     { _showEdit     = x; }
    void setDelete(bool x)   { _showDelete   = x; }
    void setCut(bool x)      { _showCut      = x; }
    void setCopy(bool x)     { _showCopy     = x; }
    void setPaste(bool x)    { _showPaste    = x; }
    void setMoveUp(bool x)   { _showMoveUp   = x; }
    void setMoveDown(bool x) { _showMoveDown = x; }
    void setPos(QPoint pos)  { _pos = pos; }

    void showEvent(QShowEvent*) override;

private:
    Ui::SkillMenuDialog *ui;
    QPoint _pos;

    bool _showEdit     = false;
    bool _showDelete   = false;
    bool _showCut      = false;
    bool _showCopy     = false;
    bool _showPaste    = false;
    bool _showMoveUp   = false;
    bool _showMoveDown = false;

public slots:
    void newButton();
    void editButton();
    void deleteButton();
    void cutButton();
    void copyButton();
    void pasteButton();
    void moveUpButton();
    void moveDownButtons();
};

#endif // SKILLMENUDIALOG_H
