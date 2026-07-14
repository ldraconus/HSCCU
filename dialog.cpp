#include "dialog.h"
#include "resizenotifier.h"

#include <QScreen>
#include <QShowEvent>
#include <QTimer>

Dialog::Dialog(QWidget* parent, Qt::WindowFlags f)
    : QDialog(parent, f) {
    installEventFilter(this);
    connect(&ResizeNotifier::instance(), &ResizeNotifier::resized, this, &Dialog::onBrowserResized);
}

void Dialog::showEvent(QShowEvent *event) {
    QDialog::showEvent(event);
    adjustForAvailableSpace();
}

void Dialog::adjustForAvailableSpace() {
    const QRect avail = screen()->availableGeometry();
    if (sizeHint().height() > avail.height()) resize(width(), avail.height());
}

bool Dialog::eventFilter(QObject* watched, QEvent* event) {
    if (watched == this && event->type() == QEvent::Show) QTimer::singleShot(0, this, &Dialog::adjustForAvailableSpace);
    return QDialog::eventFilter(watched, event);
}

void Dialog::onBrowserResized() {
    adjustForAvailableSpace();
}
