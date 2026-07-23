#ifndef PRINTDIALOG_H
#define PRINTDIALOG_H

#include <QDialog>
#include <QPageLayout>
#include <QPrintPreviewWidget>
#include <QPrinterInfo>

#include "printer.h"

namespace Ui {
class PrintDialog;
}

class PrintDialog : public QDialog {
    Q_OBJECT

public:
    explicit PrintDialog(QWidget *parent = nullptr);
    ~PrintDialog();
    Ui::PrintDialog *ui;

    QPageLayout::Orientation orientation();
    void updatePreview();

private:
    QString              mDefPrinter;
    Printer*             mPrinter { nullptr };
    QPrintPreviewWidget* mPreview { nullptr };
    QPushButton*         mPrintButton { nullptr };
    QList<QPrinterInfo>  mPrinterList;
    bool                 mReady { false };
    QPageSize            getSizes(Printer *, QPrinterInfo& def);

    void printerChanged(int);
    void paperChanged(int);
    void orientationChanged(int);
};

#endif // PRINTDIALOG_H
