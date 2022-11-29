#ifndef UNLUCK_H
#define UNLUCK_H

#include "complication.h"

class Unluck: public Complication
{
public:
    Unluck(): Complication() { }
    Unluck(const Unluck& ac)
        : Complication()
        , _dice(ac._dice) { }
    Unluck(Unluck&& ac)
        : Complication()
        , _dice(ac._dice) { }
    Unluck(const QJsonObject& json)
        : Complication()
        , _dice(json["dice"].toInt(0)) { }

    Unluck& operator=(const Unluck& ac) {
        if (this != &ac) _dice = ac._dice;
        return *this;
    }
    Unluck& operator=(Unluck&& ac) {
        _dice = ac._dice;
        return *this;
    }

    QString description() override {
        if (_dice < 1) return "<incomplete>";
        return QString("Unluck: %1d6").arg(_dice);
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        dice = createLineEdit(parent, layout, "How many dice of unluck?", std::mem_fn(&Complication::numeric));
    }
    int points(bool noStore = false) override {
        if (!noStore) store();
        return _dice * 5;
    }
    void restore() override {
        dice->setText(QString("%1").arg(_dice));
    }
    void store() override {
        _dice = dice->text().toInt(0);
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"] = "Unluck";
        obj["dice"] = _dice;
        return obj;
    }

private:
    int     _dice = 0;

    QLineEdit* dice;

    void numeric(QString) override {
        QString txt = dice->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        dice->undo();
    }
};

#endif // UNLUCK_H
