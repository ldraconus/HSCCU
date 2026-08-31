#pragma once

#include <QDialog>
#include <QJsonObject>
#include <QObject>

class Dialog: public QDialog {
    Q_OBJECT

public:
    explicit Dialog(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    virtual void restore(const QJsonObject& json) { }
    virtual QJsonObject save() { return { }; }

protected:
    void showEvent(QShowEvent *event) override;

    virtual void adjustForAvailableSpace();
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void onBrowserResized();
};
