#include "complication.h"

#include "AccidentalChange.h"
#include "Dependence.h"
#include "Dependent.h"
#include "DistinctiveFeature.h"
#include "Enraged.h"
#include "Hunted.h"
#include "Money.h"
#include "NegativeReputation.h"
#include "PhysicalComplication.h"
#include "PsychologicalComplication.h"
#include "Rivalry.h"
#include "SocialComplication.h"
#include "Susceptibility.h"
#include "Unluck.h"
#include "Vulnerability.h"

bool Complication::isNumber(QString txt) {
    bool ok;
    txt.toInt(&ok, 10);
    return ok;
}

void Complication::callback(QCheckBox* checkBox) {
    auto f = _callbacksCB.find(checkBox);
    if (f == _callbacksCB.end()) return;
    auto function = f.value();
    function(this, checkBox->isChecked());
}

void Complication::callback(QLineEdit* edit) {
    auto f = _callbacksEdit.find(edit);
    if (f == _callbacksEdit.end()) return;
    auto function = f.value();
    function(this, edit->text());
}

QCheckBox* Complication::createCheckBox(QWidget* parent, QVBoxLayout* layout, QString prompt) {
    QCheckBox* checkBox = new QCheckBox(layout->parentWidget());
    checkBox->setText(prompt);
    checkBox->setChecked(false);
    layout->addWidget(checkBox);
    parent->connect(checkBox, SIGNAL(stateChanged(int)), parent, SLOT(stateChanged(int)));
    return checkBox;
}

QCheckBox* Complication::createCheckBox(QWidget* parent, QVBoxLayout* layout, QString prompt, std::_Mem_fn<void (Complication::*)(bool)> callback) {
    QCheckBox* checkBox = new QCheckBox(layout->parentWidget());
    checkBox->setText(prompt);
    checkBox->setChecked(false);
    layout->addWidget(checkBox);
    parent->connect(checkBox, SIGNAL(stateChanged(int)), parent, SLOT(stateChanged(int)));
    _callbacksCB.insert(_callbacksCB.cend(), checkBox, callback);
    return checkBox;
}

QComboBox* Complication::createComboBox(QWidget* parent, QVBoxLayout* layout, QString prompt, QList<QString> options, std::_Mem_fn<void (Complication::*)(int)> callback) {
    QComboBox* comboBox = new QComboBox(layout->parentWidget());
    comboBox->addItems(options);
    comboBox->setPlaceholderText(prompt);
    comboBox->setCurrentIndex(-1);
    layout->addWidget(comboBox);
    parent->connect(comboBox, SIGNAL(currentIndexChanged(int)), parent, SLOT(currentIndexChanged(int)));
    _callbacksCBox.insert(_callbacksCBox.cend(), comboBox, callback);
    return comboBox;
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

QLineEdit* Complication::createLineEdit(QWidget* parent, QVBoxLayout* layout, QString prompt, std::_Mem_fn<void (Complication::*)(QString)> callback) {
    QLineEdit* lineEdit = new QLineEdit(layout->parentWidget());
    lineEdit->setPlaceholderText(prompt);
    layout->addWidget(lineEdit);
    parent->connect(lineEdit, SIGNAL(textChanged(QString)), parent, SLOT(textChanged(QString)));
    _callbacksEdit.insert(_callbacksEdit.cend(), lineEdit, callback);
    return lineEdit;
}

QLineEdit* Complication::createLineEdit(QWidget* parent, QVBoxLayout* layout, QString prompt) {
    QLineEdit* lineEdit = new QLineEdit(layout->parentWidget());
    lineEdit->setPlaceholderText(prompt);
    layout->addWidget(lineEdit);
    parent->connect(lineEdit, SIGNAL(textChanged(QString)), parent, SLOT(textChanged(QString)));
    return lineEdit;
}

void Complication::createForm(QWidget* parent, QVBoxLayout* layout) {
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != NULL) {
        delete item->widget();
        delete item;
    }
    form(parent, layout);
}

QList<QString> Complication::Available() {
    return { "Accidental Change",
             "Dependence",
             "Dependent NPC",
             "Distinctive Feature",
             "Enraged/Berserk",
             "Hunted",
             "Money",
             "Negative Reputation",
             "Physical Complication",
             "Psychological Complication",
             "Rivalry",
             "Social Complication",
             "Susceptibility",
             "Unluck",
             "Vulnerability" };
}

shared_ptr<Complication> Complication::ByIndex(int idx) {
    switch (idx) {
    case 0:  return make_shared<AccidentalChange>();
    case 1:  return make_shared<Dependence>();
    case 2:  return make_shared<Dependent>();
    case 3:  return make_shared<DistinctiveFeature>();
    case 4:  return make_shared<Enraged>();
    case 5:  return make_shared<Hunted>();
    case 6:  return make_shared<MoneyComp>();
    case 7:  return make_shared<NegativeReputation>();
    case 8:  return make_shared<PhysicalComplication>();
    case 9:  return make_shared<PsychologicalComplication>();
    case 10: return make_shared<Rivalry>();
    case 11: return make_shared<SocialComplication>();
    case 12: return make_shared<Susceptibility>();
    case 13: return make_shared<Unluck>();
    case 14: return make_shared<Vulnerability>();
    default: return nullptr;
    }
}

shared_ptr<Complication> Complication::FromJson(QString name, const QJsonObject& json) {
    auto complications = Available();
    switch (complications.indexOf(name)) {
    case 0:  return make_shared<AccidentalChange>(json);
    case 1:  return make_shared<Dependence>(json);
    case 2:  return make_shared<Dependent>(json);
    case 3:  return make_shared<DistinctiveFeature>(json);
    case 4:  return make_shared<Enraged>(json);
    case 5:  return make_shared<Hunted>(json);
    case 6:  return make_shared<MoneyComp>(json);
    case 7:  return make_shared<NegativeReputation>(json);
    case 8:  return make_shared<PhysicalComplication>(json);
    case 9:  return make_shared<PsychologicalComplication>(json);
    case 10: return make_shared<Rivalry>(json);
    case 11: return make_shared<SocialComplication>(json);
    case 12: return make_shared<Susceptibility>(json);
    case 13: return make_shared<Unluck>(json);
    case 14: return make_shared<Vulnerability>(json);
    default: return nullptr;
    }
}
