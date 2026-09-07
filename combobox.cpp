#include "combobox.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QPointer>
#include <QSignalBlocker>
#include <QTimer>

ComboBox::ComboBox(QWidget *parent)
    : QComboBox(parent) {
#ifdef Q_OS_ANDROID
    auto proxy = new ComboBoxStyle(QApplication::style()->name());
    proxy->setParent(this);
    setStyle(proxy);
#endif
}

void ComboBox::showPopup() {

#ifdef Q_OS_ANDROID
    int index = currentIndex();
    {
        QSignalBlocker block(this);
        setCurrentIndex(-1);
    }

    QComboBox::showPopup();

    QPointer<ComboBox> self(this);

    QTimer::singleShot(0, this, [self, index]() {
        QSignalBlocker blocker(self);
        self->setCurrentIndex(index);
    });

#else
    QComboBox::showPopup();
#endif
}
