#ifndef SHEET_H
#define SHEET_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class Sheet; }
QT_END_NAMESPACE

class Sheet : public QMainWindow
{
    Q_OBJECT

public:
    Sheet(QWidget *parent = nullptr);
    ~Sheet();

private:
    Ui::Sheet *ui;
};
#endif // SHEET_H
