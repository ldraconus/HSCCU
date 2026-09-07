#pragma once

#include <QComboBox>
#include <QProxyStyle>
#include <QWidget>

class ComboBoxStyle : public QProxyStyle {
public:
    using QProxyStyle::QProxyStyle;

    int styleHint(StyleHint hint,
                  const QStyleOption* option = nullptr,
                  const QWidget* widget = nullptr,
                  QStyleHintReturn* returnData = nullptr) const override {
        if (hint == QStyle::SH_ComboBox_Popup) return false;

        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
};

class ComboBox: public QComboBox {
    Q_OBJECT
public:
    ComboBox(QWidget* parent = nullptr);

    void showPopup() override;
};
