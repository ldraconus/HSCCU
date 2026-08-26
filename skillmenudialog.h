#ifndef SKILLMENUDIALOG_H
#define SKILLMENUDIALOG_H

#include <QDialog>

namespace Ui {
class SkillMenuDialog;
}

#if defined(__wasm__) || defined(Q_OS_ANDROID)
class SkillMenuDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SkillMenuDialog(QWidget *parent = nullptr);
    ~SkillMenuDialog();

    void setEdit(bool x)     { mShowEdit     = x; }
    void setDelete(bool x)   { mShowDelete   = x; }
    void setCut(bool x)      { mShowCut      = x; }
    void setCopy(bool x)     { mShowCopy     = x; }
    void setPaste(bool x)    { mShowPaste    = x; }
    void setMoveUp(bool x)   { mShowMoveUp   = x; }
    void setMoveDown(bool x) { mShowMoveDown = x; }
    void setPos(QPoint pos)  { mPos = pos; }

    void showEvent(QShowEvent*) override;

private:
    Ui::SkillMenuDialog* mUi;
    QPointF              mPos;

    bool mShowEdit     = false;
    bool mShowDelete   = false;
    bool mShowCut      = false;
    bool mShowCopy     = false;
    bool mShowPaste    = false;
    bool mShowMoveUp   = false;
    bool mShowMoveDown = false;

public slots:
    void newButton();
    void editButton();
    void deleteButton();
    void cutButton();
    void copyButton();
    void pasteButton();
    void moveUpButton();
    void moveDownButton();
};
#else
class SkillMenuDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SkillMenuDialog(QWidget *parent = nullptr);
    ~SkillMenuDialog();
};
#endif

#endif // SKILLMENUDIALOG_H
