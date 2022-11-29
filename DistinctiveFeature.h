#ifndef DISTINCTIVEFEATURE_H
#define DISTINCTIVEFEATURE_H

#include "complication.h"

class DistinctiveFeature: public Complication
{
public:
    DistinctiveFeature(): Complication() { }
    DistinctiveFeature(const DistinctiveFeature& ac)
        : Complication()
        , _concealability(ac._concealability)
        , _detectable(ac._detectable)
        , _notDistinctive(ac._notDistinctive)
        , _reaction(ac._reaction)
        , _what(ac._what) { }
    DistinctiveFeature(DistinctiveFeature&& ac)
        : Complication()
        , _concealability(ac._concealability)
        , _detectable(ac._detectable)
        , _notDistinctive(ac._notDistinctive)
        , _reaction(ac._reaction)
        , _what(ac._what) { }
    DistinctiveFeature(const QJsonObject& json)
        : Complication()
        , _concealability(json["concealability"].toInt(0))
        , _detectable(json["detectable"].toInt(0))
        , _notDistinctive(json["notDistinctive"].toBool(false))
        , _reaction(json["reaction"].toInt(0))
        , _what(json["what"].toString("")) { }

    DistinctiveFeature& operator=(const DistinctiveFeature& ac) {
        if (this != &ac) {
            _concealability = ac._concealability;
            _detectable     = ac._detectable;
            _notDistinctive = ac._notDistinctive;
            _reaction       = ac._reaction;
            _what           = ac._what;
        }
        return *this;
    }
    DistinctiveFeature& operator=(DistinctiveFeature&& ac) {
        _concealability = ac._concealability;
        _detectable     = ac._detectable;
        _notDistinctive = ac._notDistinctive;
        _reaction       = ac._reaction;
        _what           = ac._what;
        return *this;
    }

    QString description() override {
        static QList<QString> conc { "Easily Concealed", "Concealable (with Disguise Skill or major effort)",
                                     "Not Concealable" };
        static QList<QString> dtct {  "Commonly-Used Senses", "Uncommonly-Used Senses and/or by a Large Group and/or By Simple Tests",
                                      "Unusual Senses and/or by a Small Group and/or Only By Technology or Major Effort" };
        static QList<QString> rctn { "Noticed and Recognizable", "Always Noticed and Causes Major Reaction or Prejudice",
                                     "Causes Extreme Reaction" };
        if (_what.isEmpty() || _reaction == -1 || _detectable == -1 || _concealability == -1) return "<incomplete>";
        return "Distinctive Feature: " + _what + " (" + conc[_concealability] + "; " + rctn[_reaction] + "; " + dtct[_detectable] +
                (_notDistinctive ? "; Not Distictive in Some Cultures" : "") + ")";
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        what           = createLineEdit(parent, layout, "What is distinctive?");
        concealability = createComboBox(parent, layout, "How concealable is it?", { "Easily Concealed", "Concealable (with Disguise Skill or major effort)",
                                                                                    "Not Concealable" });
        reaction       = createComboBox(parent, layout, "Reaction to Feature", { "Noticed and Recognizable", "Always Noticed and Causes Major Reaction or Prejudice",
                                                                                 "Causes Extreme Reaction (e.g., terror, lust, disgust)" });
        detectable     = createComboBox(parent, layout, "How detectable is it?", { "Commonly-Used Senses", "Uncommonly-Used Senses and/or by a Large Group and/or By Simple Tests",
                                                                                   "Unusual Senses and/or by a Small Group and/or Only By Technology or Major Effort" });
        notDistinctive = createCheckBox(parent, layout, "Feature is Not Distinctive in Some Cultures or Societies");
    }
    int points(bool noStore = false) override {
        if (!noStore) store();
        return 5 * (_concealability + 1) + _reaction * 5 - _detectable * 5 - (_notDistinctive ? 5 : 0);
    }
    void restore() override {
        what->setText(_what);
        concealability->setCurrentIndex(_concealability);
        detectable->setCurrentIndex(_detectable);
        notDistinctive->setChecked(_notDistinctive);
        reaction->setCurrentIndex(_reaction);
    }
    void store() override {
        _what           = what->text();
        _concealability = concealability->currentIndex();
        _detectable     = detectable->currentIndex();
        _notDistinctive = notDistinctive->isChecked();
        _reaction       = reaction->currentIndex();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]           = "Distinctive Feature";
        obj["concealability"] = _concealability;
        obj["detectable"]     = _detectable;
        obj["notDistinctive"] = _notDistinctive;
        obj["reaction"]       = _reaction;
        obj["what"]           = _what;
        return obj;
    }

private:
    int     _concealability = -1;
    int     _detectable = -1;
    bool    _notDistinctive = false;
    int     _reaction = -1;
    QString _what = "";

    QComboBox* concealability;
    QComboBox* detectable;
    QCheckBox* notDistinctive;
    QComboBox* reaction;
    QLineEdit* what;
};

#endif // DISTINCTIVEFEATURE_H
