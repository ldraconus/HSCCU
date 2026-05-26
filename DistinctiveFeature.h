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
    ~DistinctiveFeature() override { }

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
        if (v.mWhat.isEmpty() || v.mReaction < 0 || v.mDetectable < 0 || v.mConcealability < 0) return "<incomplete>";
        return "Distinctive Feature: " + v.mWhat + " (" + conc[v.mConcealability] + "; " + rctn[v.mReaction] + "; " + dtct[v.mDetectable] +
                (v.mNotDistinctive ? "; Not Distictive in Some Cultures" : "") + ")";
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        what           = createLineEdit(parent, layout, "What is distinctive?");
        concealability = createComboBox(parent, layout, "How concealable is it?", { "Easily Concealed", "Concealable (with Disguise Skill or major effort)",
                                                                                    "Not Concealable" });
        reaction       = createComboBox(parent, layout, "Reaction to Feature", {"Noticed and Recognizable", "Always Noticed and Causes Major Reaction or Prejudice",
                                                                                 "Causes Extreme Reaction (e.g., terror, lust, disgust)" });
        detectable     = createComboBox(parent, layout, "How detectable is it?", { "Commonly-Used Senses", "Uncommonly-Used Senses and/or by a Large Group and/or By Simple Tests",
                                                                                   "Unusual Senses and/or by a Small Group and/or Only By Technology or Major Effort" });
        notDistinctive = createCheckBox(parent, layout, "Feature is Not Distinctive in Some Cultures or Societies");
    }
    Points points(bool noStore = false) override {
        if (!noStore) store();
        return 5_cp * (v.mConcealability + 1) + v.mReaction * 5_cp - v.mDetectable * 5_cp - (v.mNotDistinctive ? 5_cp : 0_cp); // NOLINT
    }
    void restore() override {
        vars s = v;
        what->setText(s.mWhat);
        concealability->setCurrentIndex(s.mConcealability);
        detectable->setCurrentIndex(s.mDetectable);
        notDistinctive->setChecked(s.mNotDistinctive);
        reaction->setCurrentIndex(s.mReaction);
        v = s;
    }
    void store() override {
        v.mWhat           = what->text();
        v.mConcealability = concealability->currentIndex();
        v.mDetectable     = detectable->currentIndex();
        v.mNotDistinctive = notDistinctive->isChecked();
        v.mReaction       = reaction->currentIndex();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]           = "Distinctive Feature";
        obj["concealability"] = v.mConcealability;
        obj["detectable"]     = v.mDetectable;
        obj["notDistinctive"] = v.mNotDistinctive;
        obj["reaction"]       = v.mReaction;
        obj["what"]           = v.mWhat;
        return obj;
    }

private:
    struct vars {
        int     mConcealability = -1;
        int     mDetectable = -1;
        bool    mNotDistinctive = false;
        int     mReaction = -1;
        QString mWhat = "";
    } v;

    gsl::owner<QComboBox*> concealability = nullptr;
    gsl::owner<QComboBox*> detectable = nullptr;
    gsl::owner<QCheckBox*> notDistinctive = nullptr;
    gsl::owner<QComboBox*> reaction = nullptr;
    gsl::owner<QLineEdit*> what = nullptr;
};

#endif // DISTINCTIVEFEATURE_H
