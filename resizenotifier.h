#pragma once

#include <QObject>

class ResizeNotifier: public QObject {
    Q_OBJECT

public:
    static ResizeNotifier &instance();

signals:
    void resized();

private:
    explicit ResizeNotifier(QObject *parent = nullptr);
};
