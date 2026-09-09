#pragma once

#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QDialog>
#include <QMessageBox>

namespace Ui { class MsgDialog; }

class MsgDialog: public QDialog {
    Q_OBJECT

public:
    explicit MsgDialog(QWidget *parent = nullptr);
    ~MsgDialog();

    QMessageBox::Icon icon() const                    { return mIcon; }
    void              setIcon(QMessageBox::Icon icon) { mIcon = icon; }

    QString text() const                 { return mText; }
    void    setText(const QString& text) { mText = text; }

    QString informativeText() const                 { return mInformativeText; }
    void    setInformativeText(const QString& text) { mInformativeText = text; }

    void                         setStandardButtons(QMessageBox::StandardButtons buttons) { mButtons = buttons; }
    QMessageBox::StandardButtons standardButtons() const                                  { return mButtons; }

    void                        setDefaultButton(QMessageBox::StandardButton button) { mDefaultButton = button; }
    QMessageBox::StandardButton defaultButton() const                                { return mDefaultButton; }

    QMessageBox::StandardButton standardButton(QAbstractButton* button) const;

signals:
    void buttonClicked(QAbstractButton*);

public slots:
private:
    QMessageBox::StandardButtons mButtons       { QMessageBox::Ok };
    QMessageBox::StandardButton  mDefaultButton { QMessageBox::NoButton };
    QMessageBox::Icon            mIcon          { QMessageBox::Icon::NoIcon };
    QString                      mIcons         { "Ⓘ?!∅" };
    QString                      mInformativeText;
    QString                      mText;
    Ui::MsgDialog*               mUi            { nullptr };

    void showEvent(QShowEvent*) override;

    QDialogButtonBox::StandardButton  toStandardButton(QMessageBox::StandardButton button);
    QDialogButtonBox::StandardButtons toStandardButtons(QMessageBox::StandardButtons buttons);
};
