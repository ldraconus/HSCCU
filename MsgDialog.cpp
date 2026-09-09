#include "MsgDialog.h"
#include "ui_MsgDialog.h"

MsgDialog::MsgDialog(QWidget *parent)
    : QDialog(parent)
    , mUi(new Ui::MsgDialog) {
    mUi->setupUi(this);

    connect(mUi->buttonBox, &QDialogButtonBox::clicked, this, [this](QAbstractButton* btn) {
        emit buttonClicked(btn);
    });
}

MsgDialog::~MsgDialog() {
    delete mUi;
}

QMessageBox::StandardButton MsgDialog::standardButton(QAbstractButton *button) const {
    return QMessageBox::StandardButton(mUi->buttonBox->standardButton(button));
}

void MsgDialog::showEvent(QShowEvent* se) {
    QDialog::showEvent(se);

    mUi->text->setText(mText);
    mUi->informativeText->setText(mInformativeText);
    mUi->buttonBox->setStandardButtons(toStandardButtons(mButtons));
    int x = 0;
    QColor color("black");
    int pnt = 48;
    switch (mIcon) {
    case QMessageBox::Question: x = 1;                                  break;
    case QMessageBox::Warning:  x = 2; color = QColor("red");           break;
    case QMessageBox::Critical: x = 3; color = QColor("red"); pnt = 96; break;
    default:                    x = 0;                        pnt = 42; break;
    }

    mUi->icon->setText(mIcons[x]);
    QFont fnt = mUi->icon->font();
    fnt.setPointSize(pnt);
    mUi->icon->setFont(fnt);
    mUi->icon->setStyleSheet("QLabel: { color: " + color.name() + "; }");
}

QDialogButtonBox::StandardButton MsgDialog::toStandardButton(QMessageBox::StandardButton button) {
    if (button & QMessageBox::Ok)     return QDialogButtonBox::Ok;
    if (button & QMessageBox::Cancel) return QDialogButtonBox::Cancel;
    if (button & QMessageBox::Yes)    return QDialogButtonBox::Yes;
    if (button & QMessageBox::No)     return QDialogButtonBox::No;
    return QDialogButtonBox::NoButton;
}

QDialogButtonBox::StandardButtons MsgDialog::toStandardButtons(QMessageBox::StandardButtons buttons) {
    QDialogButtonBox::StandardButtons use = QDialogButtonBox::NoButton;
    if (buttons & QMessageBox::Ok)     use |= QDialogButtonBox::Ok;
    if (buttons & QMessageBox::Cancel) use |= QDialogButtonBox::Cancel;
    if (buttons & QMessageBox::Yes)    use |= QDialogButtonBox::Yes;
    if (buttons & QMessageBox::No)     use |= QDialogButtonBox::No;
    return use;
}
