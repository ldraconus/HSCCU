#ifndef IMGMENUDIALOG_H
#define IMGMENUDIALOG_H
#include <QDialog>

#if defined(__wasm__) || defined(Q_OS_ANDROID)
namespace Ui {
class ImgMenuDialog;
}

class ImgMenuDialog : public QDialog {
    Q_OBJECT

public:
    explicit ImgMenuDialog(QWidget *parent = nullptr);
    ~ImgMenuDialog();

    void setPos(QPoint pos) { mPos = pos; }

    void showEvent(QShowEvent* se) override;

private:
    Ui::ImgMenuDialog* mUi;
    QPointF            mPos;

public slots:
    void clearImage();
    void newImage();
};
#else
class ImgMenuDialog : public QDialog {
    Q_OBJECT

public:
    explicit ImgMenuDialog(QWidget *parent = nullptr) { }
    ~ImgMenuDialog() { }
};
#endif
#endif // IMGMENUDIALOG_H
