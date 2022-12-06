#ifndef MONEY_H
#define MONEY_H

#include "complication.h"

class Money: public Complication {
public:
    Money(): Complication() { }
    Money(const Money& d)
        : Complication()
        , v(d.v) { }
    Money(Money&& d)
        : Complication()
        , v(d.v) { }
    Money(const QJsonObject& json)
        : Complication()
        , v { json["amount"].toInt(0) } { }

    Money& operator=(const Money& d) {
        if (this != &d) {
            v = d.v;
        }
        return *this;
    }
    Money& operator=(Money&& d) {
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
    int points(bool noStore = false) override {
        if (!noStore) store();
        return (v._amount < 0) ? 0 : ((2 - v._amount) * 5);
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
