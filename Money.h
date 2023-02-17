#ifndef MONEY_H
#define MONEY_H

#include "complication.h"

class MoneyComp: public Complication {
public:
    MoneyComp(): Complication() { }
    MoneyComp(const MoneyComp& d)
        : Complication()
        , v(d.v) { }
    MoneyComp(MoneyComp&& d)
        : Complication()
        , v(d.v) { }
    MoneyComp(const QJsonObject& json)
        : Complication()
        , v { json["amount"].toInt(0) } { }

    MoneyComp& operator=(const MoneyComp& d) {
        if (this != &d) {
            v = d.v;
        }
        return *this;
    }
    MoneyComp& operator=(MoneyComp&& d) {
        v = d.v;
        return *this;
    }

    QString description() override {
        static QList<QString> amount { "Destitute ($3,000 or less)",
                                       "Poor ($10,000 or less)" };
        if (v._amount < 0) return "<incomplete>";
        return amount[v._amount];
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        amount = createComboBox(parent, layout, "How Poor is the PC?", { "Destitute ($3,000 or less)", "Poor ($10,000 or less)" });
    }
    Points<> points(bool noStore = false) override {
        if (!noStore) store();
        return (v._amount < 0) ? 0_cp : ((2 - v._amount) * 5_cp);
    }
    void restore() override {
        vars s = v;
        amount->setCurrentIndex(s._amount);
        v = s;
    }
    void store() override {
        v._amount = amount->currentIndex();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]   = "Money";
        obj["amount"] = v._amount;
        return obj;
    }

private:
    struct vars {
        int _amount = -1;
    } v;

    QComboBox* amount;
};

#endif // MONEY_H
