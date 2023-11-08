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
    bool ok = false;
    constexpr int base10 = 10;
    txt.toInt(&ok, base10);
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

gsl::owner<QCheckBox*> Complication::createCheckBox(QWidget* parent, QVBoxLayout* layout, QString prompt) {
    gsl::owner<QCheckBox*> checkBox = new QCheckBox(layout->parentWidget());
    checkBox->setText(prompt);
    checkBox->setChecked(false);
#ifdef unix
    QString style = "color: #000; background: #fff;";
    checkBox->setStyleSheet(style);
#endif
    layout->addWidget(checkBox);
    parent->connect(checkBox, SIGNAL(stateChanged(int)), parent, SLOT(stateChanged(int)));
    return checkBox;
}

gsl::owner<QCheckBox*> Complication::createCheckBox(QWidget* parent, QVBoxLayout* layout, QString prompt, std::_Mem_fn<void (Complication::*)(bool)> callback) {
    gsl::owner<QCheckBox*> checkBox = createCheckBox(parent, layout, prompt);
    _callbacksCB.insert(_callbacksCB.cend(), checkBox, callback);
    return checkBox;
}

gsl::owner<QComboBox*> Complication::createComboBox(QWidget* parent, QVBoxLayout* layout, QString prompt, QList<QString> options, std::_Mem_fn<void (Complication::*)(int)> callback) {
    gsl::owner<QComboBox*> comboBox = createComboBox(parent, layout, prompt, options);
    _callbacksCBox.insert(_callbacksCBox.cend(), comboBox, callback);
    return comboBox;
}

gsl::owner<QComboBox*> Complication::createComboBox(QWidget* parent, QVBoxLayout* layout, QString prompt, QList<QString> options) {
    gsl::owner<QComboBox*> comboBox = new QComboBox(layout->parentWidget());
    comboBox->addItems(options);
    comboBox->setPlaceholderText(prompt);
    comboBox->setCurrentIndex(-1);
#ifdef unix
    QString style = "QComboBox { color: #000; background: #fff; } QComboBox QAbstractItemView { color: #000; background-color: #fff; }";
    comboBox->setStyleSheet(style);
#endif
    layout->addWidget(comboBox);
    parent->connect(comboBox, SIGNAL(currentIndexChanged(int)), parent, SLOT(currentIndexChanged(int)));
    return comboBox;
}

gsl::owner<QLabel*> Complication::createLabel(QWidget*, QVBoxLayout* layout, QString text) {
    gsl::owner<QLabel*> label = new QLabel(layout->parentWidget());
    label->setText(text);
#ifdef unix
    QString style = "color: #000; background: #fff;";
    label->setStyleSheet(style);
#endif
    layout->addWidget(label);
    return label;
}

gsl::owner<QLineEdit*> Complication::createLineEdit(QWidget* parent, QVBoxLayout* layout, QString prompt, std::_Mem_fn<void (Complication::*)(QString)> callback) {
    gsl::owner<QLineEdit*> lineEdit = createLineEdit(parent, layout, prompt);
    _callbacksEdit.insert(_callbacksEdit.cend(), lineEdit, callback);
    return lineEdit;
}

gsl::owner<QLineEdit*> Complication::createLineEdit(QWidget* parent, QVBoxLayout* layout, QString prompt) {
    gsl::owner<QLineEdit*> lineEdit = new QLineEdit(layout->parentWidget());
    lineEdit->setPlaceholderText(prompt);
#ifdef unix
    QString style = "color: #000; background: #fff;";
    lineEdit->setStyleSheet(style);
#endif
    layout->addWidget(lineEdit);
    parent->connect(lineEdit, SIGNAL(textChanged(QString)), parent, SLOT(textChanged(QString)));
    return lineEdit;
}

void Complication::createForm(QWidget* parent, QVBoxLayout* layout) {
    gsl::owner<QLayoutItem*> item = nullptr;
    while ((item = layout->takeAt(0)) != NULL) {
        delete item->widget();
        delete item;
    }
    form(parent, layout);
}

QList<QString> Complication::Available() {
    return { "Accidental Change",
             "Blank Line",
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
    case 1:  return make_shared<BlankComp>();
    case 2:  return make_shared<Dependence>();
    case 3:  return make_shared<Dependent>();
    case 4:  return make_shared<DistinctiveFeature>();
    case 5:  return make_shared<Enraged>();
    case 6:  return make_shared<Hunted>();
    case 7:  return make_shared<MoneyComp>();
    case 8:  return make_shared<NegativeReputation>();
    case 9:  return make_shared<PhysicalComplication>();
    case 10: return make_shared<PsychologicalComplication>();
    case 11: return make_shared<Rivalry>();
    case 12: return make_shared<SocialComplication>();
    case 13: return make_shared<Susceptibility>();
    case 14: return make_shared<Unluck>();
    case 15: return make_shared<Vulnerability>();
    default: return nullptr;
    }
}

shared_ptr<Complication> Complication::FromJson(QString name, const QJsonObject& json) {
    auto complications = Available();
    switch (complications.indexOf(name)) {
    case 0:  return make_shared<AccidentalChange>(json);
    case 1:  return make_shared<BlankComp>(json);
    case 2:  return make_shared<Dependence>(json);
    case 3:  return make_shared<Dependent>(json);
    case 4:  return make_shared<DistinctiveFeature>(json);
    case 5:  return make_shared<Enraged>(json);
    case 6:  return make_shared<Hunted>(json);
    case 7:  return make_shared<MoneyComp>(json);
    case 8:  return make_shared<NegativeReputation>(json);
    case 9:  return make_shared<PhysicalComplication>(json);
    case 10: return make_shared<PsychologicalComplication>(json);
    case 11: return make_shared<Rivalry>(json);
    case 12: return make_shared<SocialComplication>(json);
    case 13: return make_shared<Susceptibility>(json);
    case 14: return make_shared<Unluck>(json);
    case 15: return make_shared<Vulnerability>(json);
    default: return nullptr;
    }
}
