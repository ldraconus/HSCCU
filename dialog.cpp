#include "dialog.h"
#include "resizenotifier.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QScroller>
#include <QScreen>
#include <QScrollArea>
#include <QScroller>
#include <QShowEvent>
#include <QTableWidget>
#include <QTimer>

Dialog::Dialog(QWidget* parent, Qt::WindowFlags f)
    : QDialog(parent, f) {
    installEventFilter(this);
    connect(&ResizeNotifier::instance(), &ResizeNotifier::resized, this, &Dialog::onBrowserResized);
}

void Dialog::showEvent(QShowEvent *event) {
    QDialog::showEvent(event);

    adjustForAvailableSpace();
#ifdef Q_OS_ANDROID
    for (auto* combo: findChildren<QComboBox*>()) {
        auto* view = combo->view();
        QScroller::grabGesture(view->viewport(), QScroller::LeftMouseButtonGesture);
        view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    }
    for (auto* scroll: findChildren<QScrollArea*>()) QScroller::grabGesture(scroll->viewport(), QScroller::TouchGesture);
    for (auto* table: findChildren<QTableWidget*>()) {
        table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        QScroller::grabGesture(table->viewport(), QScroller::TouchGesture);
    }
#endif
}

void Dialog::adjustForAvailableSpace() {
    const QRect avail = screen()->availableGeometry();
#ifdef Q_OS_ANDROID
    const int margin = 8;
    QSize wanted(avail.width()  - margin * 2,
                     avail.height() - margin * 2);

    setMinimumSize(wanted);
    resize(wanted);

    move(avail.left() + margin,
         avail.top()  + margin);
#else
    if (sizeHint().height() > avail.height()) {
        resize(width(), avail.height());
        move(pos().x(), 0);
        return;
    }
    if (pos().y() + sizeHint().height() > avail.height()) move(pos().x(), (avail.height() - sizeHint().height()) / 2);
#endif
}

bool Dialog::eventFilter(QObject* watched, QEvent* event) {
    if (watched == this && event->type() == QEvent::Show) QTimer::singleShot(0, this, &Dialog::adjustForAvailableSpace);
    return QDialog::eventFilter(watched, event);
}

void Dialog::onBrowserResized() {
    adjustForAvailableSpace();
}
