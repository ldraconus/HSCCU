#ifndef ENRAGED_H
#define ENRAGED_H

#include "complication.h"

class Enraged: public Complication
{
public:
    Enraged(): Complication() { }
    Enraged(const Enraged& ac)
        : Complication()
        , v(ac.v) { }
    Enraged(Enraged&& ac)
        : Complication()
        , v(ac.v) { }
    Enraged(const QJsonObject& json)
        : Complication()
        , v { json["chance"].toInt(0)
            , json["frequency"].toInt(0)
            , json["regain"].toInt(0)
            , json["type"].toBool(false)
            , json["what"].toString("") } { }

    Enraged& operator=(const Enraged& ac) {
        if (this != &ac) v = ac.v;
        return *this;
    }
    Enraged& operator=(Enraged&& ac) {
        v = ac.v;
        return *this;
    }

    QString description() override {
        static QList<QString> freq { "Uncommon", "Common", "Very Common" };
        static QList<QString> chnc { "8-", "11-", "14-" };
        static QList<QString> regn { "14-", "11-", "8-" };
        if (v._frequency < 0 || v._chance < 0 || v._regain < 0 || v._what.isEmpty()) return "<incomplete>";
        return QString("%1: %2 (%3 Go: %4; Recover: %5)").arg(v._type ? "Berserk" : "Enraged", v._what, freq[v._frequency], chnc[v._chance], regn[v._regain]);
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        what      = createLineEdit(parent, layout, "What sets you off?");
        frequency = createComboBox(parent, layout, "How commonly does it happen", { "Uncommon", "Common", "Very Common" });
        chance    = createComboBox(parent, layout, "How easily do you go?", { "On an 8-", "On an 11-", "On a 14-" });
        regain    = createComboBox(parent, layout, "How easily do you recover?", { "On a 14-", "On an 11-", "On an 8-" });
        type      = createCheckBox(parent, layout, "Berserk");
    }
    Points<> points(bool noStore = false) override {
        if (!noStore) store();
        return (v._frequency + 1) * 5_cp + v._chance * 5_cp + v._regain * 5_cp + (v._type ? 10_cp : 0_cp);
    }
    void restore() override {
        vars s = v;
        what->setText(s._what);
        chance->setCurrentIndex(s._chance);
        frequency->setCurrentIndex(s._frequency);
        regain->setCurrentIndex(s._regain);
        type->setChecked(s._type);
        v = s;
    }
    void store() override {
        v._what      = what->text();
        v._chance    = chance->currentIndex();
        v._frequency = frequency->currentIndex();
        v._regain    = regain->currentIndex();
        v._type      = type->isChecked();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]      = "Enraged/Berserk";
        obj["chance"]    = v._chance;
        obj["frequency"] = v._frequency;
        obj["regain"]    = v._regain;
        obj["type"]      = v._type;
        obj["what"]      = v._what;
        return obj;
    }

private:
    struct vars {
        int     _chance = -1;
        int     _frequency = -1;
        int     _regain = -1;
        bool    _type;
        QString _what = "";
    } v;

    QComboBox* chance;
    QComboBox* frequency;
    QComboBox* regain;
    QCheckBox* type;
    QLineEdit* what;
};

#endif // ENRAGED_H
