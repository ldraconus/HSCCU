#ifndef PRINTWINDOW_H
#define PRINTWINDOW_H

#include <QMainWindow>

namespace Ui {
class PrintWindow;
}

class PrintWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PrintWindow(QWidget *parent = nullptr);
    ~PrintWindow();
    Ui::PrintWindow *ui;
};

#endif // PRINTWINDOW_H
