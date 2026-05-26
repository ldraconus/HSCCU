#ifndef UNLUCK_H
#define UNLUCK_H

#include "complication.h"

class Unluck: public Complication {
public:
    Unluck(): Complication() { }
    Unluck(const Unluck& ac)
        : Complication()
        , v(ac.v) { }
    Unluck(Unluck&& ac)
        : Complication()
        , v(ac.v) { }
    Unluck(const QJsonObject& json)
        : Complication()
        , v { json["dice"].toInt(0) } { }

    Unluck& operator=(const Unluck& ac) {
        if (this != &ac) v = ac.v;
        return *this;
    }
    Unluck& operator=(Unluck&& ac) {
        v = ac.v;
        return *this;
    }

    QString description() override {
        if (v.mDice < 1) return "<incomplete>";
        return QString("Unluck: %1d6").arg(v.mDice);
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        dice = createLineEdit(parent, layout, "How many dice of unluck?", std::mem_fn(&Complication::numeric));
    }
    Points points(bool noStore = false) override {
        if (!noStore) store();
        return v.mDice * 5_cp;
    }
    void restore() override {
        vars s = v;
        dice->setText(QString("%1").arg(s.mDice));
        v = s;
    }
    void store() override {
        v.mDice = dice->text().toInt(0);
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"] = "Unluck";
        obj["dice"] = v.mDice;
        return obj;
    }

private:
    struct vars {
        int mDice = 0;
    } v;

    QLineEdit* dice;

    void numeric(QString) override {
        QString txt = dice->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        dice->undo();
    }
};

#endif // UNLUCK_H
