#pragma once

#include <QDialog>

namespace Ui {
class PowerMenuDialog;
}

class PowerMenuDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PowerMenuDialog(QWidget *parent = nullptr);
    ~PowerMenuDialog();

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
    Ui::PowerMenuDialog *ui;
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

