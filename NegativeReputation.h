#ifndef NEGATIVEREPUTATION_H
#define NEGATIVEREPUTATION_H

#include "complication.h"

class NegativeReputation: public Complication
{
public:
    NegativeReputation(): Complication() { }
    NegativeReputation(const NegativeReputation& ac)
        : Complication()
        , _extreme(ac._extreme)
        , _frequency(ac._frequency)
        , _limited(ac._limited)
        , _what(ac._what) { }
    NegativeReputation(NegativeReputation&& ac)
        : Complication()
        , _extreme(ac._extreme)
        , _frequency(ac._frequency)
        , _limited(ac._limited)
        , _what(ac._what) { }
    NegativeReputation(const QJsonObject& json)
        : Complication()
        , _extreme(json["extreme"].toBool(false))
        , _frequency(json["frequency"].toInt(0))
        , _limited(json["limited"].toBool(false))
        , _what(json["what"].toString("")) { }

    NegativeReputation& operator=(const NegativeReputation& ac) {
        if (this != &ac) {
            _extreme   = ac._extreme;
            _frequency = ac._frequency;
            _limited   = ac._limited;
            _what      = ac._what;
        }
        return *this;
    }
    NegativeReputation& operator=(NegativeReputation&& ac) {
        _extreme   = ac._extreme;
        _frequency = ac._frequency;
        _limited   = ac._limited;
        _what      = ac._what;
        return *this;
    }

    QString description() override {
        static QList<QString> freq { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)", "Always" };
        if (_frequency == -1 || _what.isEmpty()) return "<incomplete>";
        return QString("Negative Reputation: %1 (%2%3%4)").arg(_what, freq[_frequency], _extreme ? "; Extreme" : "", _limited ? "; Limited Group" : "");
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        what      = createLineEdit(parent, layout, "What is the reputation?");
        frequency = createComboBox(parent, layout, "How often is it recognized", { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)" });
        extreme   = createCheckBox(parent, layout, "Extreme Reputation");
        limited   = createCheckBox(parent, layout, "Known only to a limited group");
    }
    int points(bool noStore = false) override {
        if (!noStore) store();
        return (_extreme ? 5 : 0) + (_frequency + 1) * 5 - (_limited ? 5 : 0);
    }
    void restore() override {
        what->setText(_what);
        extreme->setChecked(_extreme);
        frequency->setCurrentIndex(_frequency);
        limited->setChecked(_limited);
    }
    void store() override {
        _what      = what->text();
        _extreme   = extreme->isChecked();
        _frequency = frequency->currentIndex();
        _limited   = limited->isChecked();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]      = "Negative Reputation";
        obj["extreme"]   = _extreme;
        obj["frequency"] = _frequency;
        obj["limited"]   = _limited;
        obj["what"]      = _what;
        return obj;
    }

private:
    bool    _extreme = false;
    int     _frequency = -1;
    bool    _limited = false;
    QString _what = "";

    QCheckBox* extreme;
    QComboBox* frequency;
    QCheckBox* limited;
    QLineEdit* what;
};

#endif // NEGATIVEREPUTATION_H
