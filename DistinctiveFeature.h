#ifndef DISTINCTIVEFEATURE_H
#define DISTINCTIVEFEATURE_H

#include "complication.h"

class DistinctiveFeature: public Complication
{
public:
    DistinctiveFeature(): Complication() { }
    DistinctiveFeature(const DistinctiveFeature& ac)
        : Complication()
        , v(ac.v) { }
    DistinctiveFeature(DistinctiveFeature&& ac)
        : Complication()
        , v(ac.v) { }
    DistinctiveFeature(const QJsonObject& json)
        : Complication()
        , v { json["concealability"].toInt(0)
            , json["detectable"].toInt(0)
            , json["notDistinctive"].toBool(false)
            , json["reaction"].toInt(0)
            , json["what"].toString("") } { }

    DistinctiveFeature& operator=(const DistinctiveFeature& ac) {
        if (this != &ac) v = ac.v;
        return *this;
    }
    DistinctiveFeature& operator=(DistinctiveFeature&& ac) {
        v = ac.v;
        return *this;
    }

    QString description() override {
        static QList<QString> conc { "Easily Concealed", "Concealable (with Disguise Skill or major effort)",
                                     "Not Concealable" };
        static QList<QString> dtct {  "Commonly-Used Senses", "Uncommonly-Used Senses and/or by a Large Group and/or By Simple Tests",
                                      "Unusual Senses and/or by a Small Group and/or Only By Technology or Major Effort" };
        static QList<QString> rctn { "Noticed and Recognizable", "Always Noticed and Causes Major Reaction or Prejudice",
                                     "Causes Extreme Reaction" };
        if (v._what.isEmpty() || v._reaction == -1 || v._detectable == -1 || v._concealability == -1) return "<incomplete>";
        return "Distinctive Feature: " + v._what + " (" + conc[v._concealability] + "; " + rctn[v._reaction] + "; " + dtct[v._detectable] +
                (v._notDistinctive ? "; Not Distictive in Some Cultures" : "") + ")";
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
        return 5 * (v._concealability + 1) + v._reaction * 5 - v._detectable * 5 - (v._notDistinctive ? 5 : 0);
    }
    void restore() override {
        vars s = v;
        what->setText(s._what);
        concealability->setCurrentIndex(s._concealability);
        detectable->setCurrentIndex(s._detectable);
        notDistinctive->setChecked(s._notDistinctive);
        reaction->setCurrentIndex(s._reaction);
        v = s;
    }
    void store() override {
        v._what           = what->text();
        v._concealability = concealability->currentIndex();
        v._detectable     = detectable->currentIndex();
        v._notDistinctive = notDistinctive->isChecked();
        v._reaction       = reaction->currentIndex();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]           = "Distinctive Feature";
        obj["concealability"] = v._concealability;
        obj["detectable"]     = v._detectable;
        obj["notDistinctive"] = v._notDistinctive;
        obj["reaction"]       = v._reaction;
        obj["what"]           = v._what;
        return obj;
    }

private:
    struct vars {
        int     _concealability = -1;
        int     _detectable = -1;
        bool    _notDistinctive = false;
        int     _reaction = -1;
        QString _what = "";
    } v;

    QComboBox* concealability;
    QComboBox* detectable;
    QCheckBox* notDistinctive;
    QComboBox* reaction;
    QLineEdit* what;
};

#endif // DISTINCTIVEFEATURE_H
