#pragma once

#include <QDialog>
#include <QStack>

#include "5th.h"

namespace Ui {
    class DebugDialog;
}

class DebugDialog: public QDialog {
    Q_OBJECT

public:
    explicit DebugDialog(fifth::vm*, fifth::exe, QWidget* parent = nullptr);
    ~DebugDialog();

    void in();
    void out();
    void over();

private:
    struct pos {
        pos() { }
        pos(int p, fifth::exe w)
            : pc(p)
            , word(w) { }
        pos(const pos&) = default;
        pos(pos&&) = default;

        pos& operator=(const pos&) = default;
        pos& operator=(pos&&) = default;

        int        pc   { -1 };
        fifth::exe word { nullptr };
    };
    \
    int              pc    { 0 };
    QStack<pos>      stack { };
    Ui::DebugDialog* ui    { nullptr };
    fifth::vm*       vm    { nullptr };
    fifth::exe       word  { nullptr };

    void updateCode();
    void updateStack();
};
