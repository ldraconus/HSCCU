#include "powers.h"
#include "powerdialog.h"

#include "AdjustmentPowers.h"

#include <QHeaderView>
#include <QScrollBar>
#include <QWidget>

#define MAKE_ADJ(x)    Power::adjustmentPower<x> _##x
#define LINK(x)        { #x, &statics::_##x }
#define SPCS(x,y)      { x, &statics::_##y }

namespace statics {
    // Adjustment Powers
    MAKE_ADJ(Absorption);
}

QMap<QString, Power::adjustmentBase*> Power::_adjustmentPower {
    SPCS("Absorption▲", Absorption)
};

Power::Power() {
    ui = PowerDialog::ref().UI();
}

bool Power::isNumber(QString txt) {
    bool ok;
    txt.toInt(&ok, 10);
    return ok;
}

void Power::callback(QCheckBox* checkBox) {
    _sender = checkBox;
    auto f = _callbacksCB.find(checkBox);
    if (f == _callbacksCB.end()) return;
    auto function = f.value();
    function(this, checkBox->isChecked());
}

void Power::callback(QLineEdit* edit) {
    _sender = edit;
    auto f = _callbacksEdit.find(edit);
    if (f == _callbacksEdit.end()) return;
    auto function = f.value();
    function(this, edit->text());
}

QCheckBox* Power::createCheckBox(QWidget* parent, QVBoxLayout* layout, QString prompt) {
    QCheckBox* checkBox = new QCheckBox(layout->parentWidget());
    checkBox->setText(prompt);
    checkBox->setChecked(false);
    layout->addWidget(checkBox);
    parent->connect(checkBox, SIGNAL(stateChanged(int)), parent, SLOT(stateChanged(int)));
    return checkBox;
}

QCheckBox* Power::createCheckBox(QWidget* parent, QVBoxLayout* layout, QString prompt, std::_Mem_fn<void (Power::*)(bool)> callback) {
    QCheckBox* checkBox = new QCheckBox(layout->parentWidget());
    checkBox->setText(prompt);
    checkBox->setChecked(false);
    layout->addWidget(checkBox);
    parent->connect(checkBox, SIGNAL(stateChanged(int)), parent, SLOT(stateChanged(int)));
    _callbacksCB.insert(_callbacksCB.cend(), checkBox, callback);
    return checkBox;
}

QComboBox* Power::createComboBox(QWidget* parent, QVBoxLayout* layout, QString prompt, QList<QString> options, std::_Mem_fn<void (Power::*)(int)> callback) {
    QComboBox* comboBox = new QComboBox(layout->parentWidget());
    comboBox->addItems(options);
    comboBox->setPlaceholderText(prompt);
    comboBox->setToolTip(prompt);
    comboBox->setCurrentIndex(-1);
    layout->addWidget(comboBox);
    parent->connect(comboBox, SIGNAL(currentIndexChanged(int)), parent, SLOT(currentIndexChanged(int)));
    _callbacksCBox.insert(_callbacksCBox.cend(), comboBox, callback);
    return comboBox;
}

QComboBox* Power::createComboBox(QWidget* parent, QVBoxLayout* layout, QString prompt, QList<QString> options) {
    QComboBox* comboBox = new QComboBox(layout->parentWidget());
    comboBox->addItems(options);
    comboBox->setPlaceholderText(prompt);
    comboBox->setToolTip(prompt);
    comboBox->setCurrentIndex(-1);
    layout->addWidget(comboBox);
    parent->connect(comboBox, SIGNAL(currentIndexChanged(int)), parent, SLOT(currentIndexChanged(int)));
    return comboBox;
}

QLabel* Power::createLabel(QWidget*, QVBoxLayout* layout, QString text) {
    QLabel* label = new QLabel(layout->parentWidget());
    label->setText(text);
    layout->addWidget(label);
    return label;
}

QLineEdit* Power::createLineEdit(QWidget* parent, QVBoxLayout* layout, QString prompt, std::_Mem_fn<void (Power::*)(QString)> callback) {
    QLineEdit* lineEdit = new QLineEdit(layout->parentWidget());
    lineEdit->setPlaceholderText(prompt);
    lineEdit->setToolTip(prompt);
    lineEdit->setText("");
    layout->addWidget(lineEdit);
    parent->connect(lineEdit, SIGNAL(textChanged(QString)), parent, SLOT(textChanged(QString)));
    _callbacksEdit.insert(_callbacksEdit.cend(), lineEdit, callback);
    return lineEdit;
}

QLineEdit* Power::createLineEdit(QWidget* parent, QVBoxLayout* layout, QString prompt) {
    QLineEdit* lineEdit = new QLineEdit(layout->parentWidget());
    lineEdit->setPlaceholderText(prompt);
    lineEdit->setToolTip(prompt);
    layout->addWidget(lineEdit);
    parent->connect(lineEdit, SIGNAL(textChanged(QString)), parent, SLOT(textChanged(QString)));
    return lineEdit;
}

void Power::ClearForm(QVBoxLayout* layout) {
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != NULL) {
        delete item->widget();
        delete item;
    }
}

void Power::createForm(QWidget* parent, QVBoxLayout* layout) {
    ClearForm(layout);
    form(parent, layout);
}

QList<QString> Power::Available() {
    return AdjustmentPowers();
}

QList<QString> Power::AdjustmentPowers() {
    return _adjustmentPower.keys();
}

Power* Power::ByName(QString name) {
    if (_adjustmentPower.find(name) != _adjustmentPower.end()) return _adjustmentPower[name]->create();
    else return nullptr;
}

Power* Power::FromJson(QString name, const QJsonObject& json) {
    if (_adjustmentPower.find(name) != _adjustmentPower.end()) return _adjustmentPower[name]->create(json);
    else return nullptr;
}

