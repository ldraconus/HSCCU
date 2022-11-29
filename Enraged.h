#ifndef ENRAGED_H
#define ENRAGED_H

#include "complication.h"

class Enraged: public Complication
{
public:
    Enraged(): Complication() { }
    Enraged(const Enraged& ac)
        : Complication()
        , _chance(ac._chance)
        , _frequency(ac._frequency)
        , _regain(ac._regain)
        , _type(ac._type)
        , _what(ac._what) { }
    Enraged(Enraged&& ac)
        : Complication()
        , _chance(ac._chance)
        , _frequency(ac._frequency)
        , _regain(ac._regain)
        , _type(ac._type)
        , _what(ac._what) { }
    Enraged(const QJsonObject& json)
        : Complication()
        , _chance(json["chance"].toInt(0))
        , _frequency(json["frequency"].toInt(0))
        , _regain(json["regain"].toInt(0))
        , _type(json["type"].toBool(false))
        , _what(json["what"].toString("")) { }

    Enraged& operator=(const Enraged& ac) {
        if (this != &ac) {
            _chance    = ac._chance;
            _frequency = ac._frequency;
            _regain    = ac._regain;
            _type      = ac._type;
            _what      = ac._what;
        }
        return *this;
    }
    Enraged& operator=(Enraged&& ac) {
        _chance    = ac._chance;
        _frequency = ac._frequency;
        _regain    = ac._regain;
        _type      = ac._type;
        _what      = ac._what;
        return *this;
    }

    QString description() override {
        static QList<QString> freq { "Uncommon", "Common", "Very Common" };
        static QList<QString> chnc { "8-", "11-", "14-" };
        static QList<QString> regn { "14-", "11-", "8-" };
        if (_frequency == -1 || _chance == -1 || _regain == -1 || _what.isEmpty()) return "<incomplete>";
        return QString("%1: %2 (%3 Go: %4; Recover: %5)").arg(_type ? "Berserk" : "Enraged", _what, freq[_frequency], chnc[_chance], regn[_regain]);
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        what      = createLineEdit(parent, layout, "What sets you off?");
        frequency = createComboBox(parent, layout, "How commonly does it happen", { "Uncommon", "Common", "Very Common" });
        chance    = createComboBox(parent, layout, "How easily do you go?", { "On an 8-", "On an 11-", "On a 14-" });
        regain    = createComboBox(parent, layout, "How easily do you recover?", { "On a 14-", "On an 11-", "On an 8-" });
        type      = createCheckBox(parent, layout, "Berserk");
    }
    int points(bool noStore = false) override {
        if (!noStore) store();
        return (_frequency + 1) * 5 + _chance * 5 + _regain * 5 + (_type ? 10 : 0);
    }
    void restore() override {
        what->setText(_what);
        chance->setCurrentIndex(_chance);
        frequency->setCurrentIndex(_frequency);
        regain->setCurrentIndex(_regain);
        type->setChecked(_type);
    }
    void store() override {
        _what      = what->text();
        _chance    = chance->currentIndex();
        _frequency = frequency->currentIndex();
        _regain    = regain->currentIndex();
        _type      = type->isChecked();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]      = "Enraged/Berserk";
        obj["chance"]    = _chance;
        obj["frequency"] = _frequency;
        obj["regain"]    = _regain;
        obj["type"]      = _type;
        obj["what"]      = _what;
        return obj;
    }

private:
    int     _chance = -1;
    int     _frequency = -1;
    int     _regain = -1;
    bool    _type;
    QString _what = "";

    QComboBox* chance;
    QComboBox* frequency;
    QComboBox* regain;
    QCheckBox* type;
    QLineEdit* what;
};

#endif // ENRAGED_H
