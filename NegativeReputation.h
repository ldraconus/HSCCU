#ifndef NEGATIVEREPUTATION_H
#define NEGATIVEREPUTATION_H

#include "complication.h"

class NegativeReputation: public Complication
{
public:
    NegativeReputation(): Complication() { }
    NegativeReputation(const NegativeReputation& ac)
        : Complication()
        , v(ac.v) { }
    NegativeReputation(NegativeReputation&& ac)
        : Complication()
        , v(ac.v) { }
    NegativeReputation(const QJsonObject& json)
        : Complication()
        , v { json["extreme"].toBool(false), json["frequency"].toInt(0), json["limited"].toBool(false), json["what"].toString("") } { }

    NegativeReputation& operator=(const NegativeReputation& ac) {
        if (this != &ac) v = ac.v;
        return *this;
    }
    NegativeReputation& operator=(NegativeReputation&& ac) {
        v = ac.v;
        return *this;
    }

    QString description() override {
        static QList<QString> freq { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)", "Always" };
        if (v._frequency == -1 || v._what.isEmpty()) return "<incomplete>";
        return QString("Negative Reputation: %1 (%2%3%4)").arg(v._what, freq[v._frequency], v._extreme ? "; Extreme" : "", v._limited ? "; Limited Group" : "");
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        what      = createLineEdit(parent, layout, "What is the reputation?");
        frequency = createComboBox(parent, layout, "How often is it recognized", { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)" });
        extreme   = createCheckBox(parent, layout, "Extreme Reputation");
        limited   = createCheckBox(parent, layout, "Known only to a limited group");
    }
    int points(bool noStore = false) override {
        if (!noStore) store();
        return (v._extreme ? 5 : 0) + (v._frequency + 1) * 5 - (v._limited ? 5 : 0);
    }
    void restore() override {
        vars s = v;
        what->setText(s._what);
        extreme->setChecked(s._extreme);
        frequency->setCurrentIndex(s._frequency);
        limited->setChecked(s._limited);
        v = s;
    }
    void store() override {
        v._what      = what->text();
        v._extreme   = extreme->isChecked();
        v._frequency = frequency->currentIndex();
        v._limited   = limited->isChecked();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]      = "Negative Reputation";
        obj["extreme"]   = v._extreme;
        obj["frequency"] = v._frequency;
        obj["limited"]   = v._limited;
        obj["what"]      = v._what;
        return obj;
    }

private:
    struct vars {
        bool    _extreme = false;
        int     _frequency = -1;
        bool    _limited = false;
        QString _what = "";
    } v;

    QCheckBox* extreme;
    QComboBox* frequency;
    QCheckBox* limited;
    QLineEdit* what;
};

#endif // NEGATIVEREPUTATION_H