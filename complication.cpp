#include "complication.h"

#include "AccidentalChange.h"
#include "Dependence.h"

void Complication::callback(QCheckBox* checkBox) {
    auto f = _callbacks.find(checkBox);
    if (f == _callbacks.end()) return;
    auto function = f.value();
    function(this, checkBox->isChecked());
}

QCheckBox* Complication::createCheckBox(QWidget* parent, QVBoxLayout* layout, QString prompt, std::_Mem_fn<void (Complication::*)(bool)> callback) {
    QCheckBox* checkBox = new QCheckBox(layout->parentWidget());
    checkBox->setText(prompt);
    checkBox->setChecked(false);
    layout->addWidget(checkBox);
    parent->connect(checkBox, SIGNAL(stateChanged(int)), parent, SLOT(stateChanged(int)));
    _callbacks.insert(_callbacks.cend(), checkBox, callback);
    return checkBox;
}

QComboBox* Complication::createComboBox(QWidget* parent, QVBoxLayout* layout, QString prompt, QList<QString> options) {
    QComboBox* comboBox = new QComboBox(layout->parentWidget());
    comboBox->addItems(options);
    comboBox->setPlaceholderText(prompt);
    comboBox->setCurrentIndex(-1);
    layout->addWidget(comboBox);
    parent->connect(comboBox, SIGNAL(currentIndexChanged(int)), parent, SLOT(currentIndexChanged(int)));
    return comboBox;
}

QLabel* Complication::createLabel(QWidget*, QVBoxLayout* layout, QString text) {
    QLabel* label = new QLabel(layout->parentWidget());
    label->setText(text);
    layout->addWidget(label);
    return label;
}

QLineEdit* Complication::createLineEdit(QWidget* parent, QVBoxLayout* layout, QString prompt) {
    QLineEdit* lineEdit = new QLineEdit(layout->parentWidget());
    lineEdit->setPlaceholderText(prompt);
    layout->addWidget(lineEdit);
    parent->connect(lineEdit, SIGNAL(textChanged(QString)), parent, SLOT(textChanged(QString)));
    return lineEdit;
}

void Complication::createForm(QWidget* parent, QVBoxLayout* layout) {
    for (const auto& child: layout->children()) delete child;
    form(parent, layout);
}

QList<QString> Complication::Available() {
    return { "Accidental Change",
             "Dependence" };
}

Complication* Complication::ByIndex(int idx) {
    switch (idx) {
    case 0:  return new AccidentalChange();
    case 1:  return new Dependence();
    default: return nullptr;
    }
}

Complication* Complication::FromJson(QString name, const QJsonObject& json) {
    auto complications = Available();
    switch (complications.indexOf(name)) {
    case 0:  return new AccidentalChange(json);
    case 1:  return new Dependence(json);
    default: return nullptr;
    }
}
