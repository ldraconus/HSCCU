#ifndef MONEY_H
#define MONEY_H

#include "complication.h"

class MoneyComp: public Complication {
public:
    MoneyComp() = default;
    MoneyComp(const QJsonObject& json)
        : Complication(json)
        , v { json["amount"].toInt(0) } { }

    QString abbreviation() override { return str(true); }
    QString description() override { return str(); }
    QString str(bool abbr = false) {
        static QList<QString> amountStr { "Destitute ($3,000 or less)",
                                       "Poor ($10,000 or less)" };
        static QList<QString> amountAbbr { "Destitute",
                                           "Poor" };
        if (v.mAmount < 0) return "<incomplete>";
        return abbr ? amountAbbr[v.mAmount] : amountStr[v.mAmount];
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        amount = createComboBox(parent, layout, "How Poor is the PC?", { "Destitute ($3,000 or less)", "Poor ($10,000 or less)" });
    }
    Points points(bool noStore = false) override {
        if (!noStore) store();
        return (v.mAmount < 0) ? 0_cp : ((2 - v.mAmount) * 5_cp); // NOLINT
    }
    void restore() override {
        vars s = v;
        amount->setCurrentIndex(s.mAmount);
        v = s;
    }
    void store() override {
        v.mAmount = amount->currentIndex();
    }
    QJsonObject toJson() override {
        QJsonObject obj = Complication::toJson();
        obj["name"]     = "Money";
        obj["amount"]   = v.mAmount;
        return obj;
    }

private:
    struct vars {
        int mAmount = -1;
    } v;

    QComboBox* amount = nullptr;
};

#endif // MONEY_H
