#pragma once

#include <QDialog>

namespace Ui { class HelpMenuDialog; }

class HelpMenuDialog: public QDialog {
    Q_OBJECT

public:
#ifdef __wasm__
    explicit HelpMenuDialog(QWidget *parent = nullptr);
    ~HelpMenuDialog();

    void mousePressEvent(QMouseEvent*);
    void showEvent(QShowEvent*);

private:
    Ui::HelpMenuDialog *ui;

public slots:
    void doAbout();

#else
    explicit HelpMenuDialog(QWidget *parent = nullptr) { }
    ~HelpMenuDialog() { }
#endif
};
