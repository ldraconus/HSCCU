#ifndef IMGMENUDIALOG_H
#define IMGMENUDIALOG_H
#include <QDialog>

namespace Ui {
class ImgMenuDialog;
}

class ImgMenuDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImgMenuDialog(QWidget *parent = nullptr);
    ~ImgMenuDialog();

    void setPos(QPoint pos) { _pos = pos; }

    void showEvent(QShowEvent* se);

private:
    Ui::ImgMenuDialog *ui;
    QPoint _pos;

public slots:
    void clearImage();
    void newImage();
};
#endif // IMGMENUDIALOG_H
