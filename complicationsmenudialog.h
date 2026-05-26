#ifndef COMPLICATIONSMENUDIALOG_H
#define COMPLICATIONSMENUDIALOG_H

#include <QDialog>

namespace Ui {
class ComplicationsMenuDialog;
}

class ComplicationsMenuDialog : public QDialog {
    Q_OBJECT

public:
    explicit ComplicationsMenuDialog(QWidget *parent = nullptr);
    ~ComplicationsMenuDialog();

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
    Ui::ComplicationsMenuDialog *ui;

    QPoint mPos;

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
    void moveDownButtons();
};

#endif // COMPLICATIONSMENUDIALOG_H
