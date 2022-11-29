#ifndef PHYSICALCOMPLICATION_H
#define PHYSICALCOMPLICATION_H

#include "complication.h"

class PhysicalComplication: public Complication
{
public:
    PhysicalComplication(): Complication() { }
    PhysicalComplication(const PhysicalComplication& ac)
        : Complication()
        , _frequency(ac._frequency)
        , _impairs(ac._impairs)
        , _what(ac._what) { }
    PhysicalComplication(PhysicalComplication&& ac)
        : Complication()
        , _frequency(ac._frequency)
        , _impairs(ac._impairs)
        , _what(ac._what) { }
    PhysicalComplication(const QJsonObject& json)
        : Complication()
        , _frequency(json["frequency"].toInt(0))
        , _impairs(json["impairs"].toInt(0))
        , _what(json["what"].toString("")) { }

    PhysicalComplication& operator=(const PhysicalComplication& ac) {
        if (this != &ac) {
            _frequency = ac._frequency;
            _impairs   = ac._impairs;
            _what      = ac._what;
        }
        return *this;
    }
    PhysicalComplication& operator=(PhysicalComplication&& ac) {
        _frequency = ac._frequency;
        _impairs   = ac._impairs;
        _what      = ac._what;
        return *this;
    }

    QString description() override {
        static QList<QString> impr { "Barely", "Slightly", "Greatly", "Fully" };
        static QList<QString> freq { "Infrequently", "Frequently", "Very Frequently", "Always" };
        if (_frequency == -1 || _impairs == -1 || _what.isEmpty()) return "<incomplete>";
        return QString("Physical Complication: %1 (%2; %3 imparing)").arg(_what, freq[_frequency], impr[_impairs]);
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        what         = createLineEdit(parent, layout, "What is the complication?");
        frequency    = createComboBox(parent, layout, "How often does it affect you?", { "Infrequently", "Frequently", "Very Frequently", "All The Time" });
        impairs      = createComboBox(parent, layout, "How badly does it affect you?", { "Barely", "Slightly", "Greatly", "Fully" });
    }
    int points(bool noStore = false) override {
        if (!noStore) store();
        return (_frequency + 1) * 5 + _impairs * 5;
    }
    void restore() override {
        what->setText(_what);
        frequency->setCurrentIndex(_frequency);
        impairs->setCurrentIndex(_impairs);
    }
    void store() override {
        _what      = what->text();
        _frequency = frequency->currentIndex();
        _impairs   = impairs->currentIndex();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]         = "Physical Complication";
        obj["frequency"]    = _frequency;
        obj["impairs"]      = _impairs;
        obj["what"]         = _what;
        return obj;
    }

private:
    int     _frequency = -1;
    int     _impairs = -1;
    QString _what = "";

    QComboBox* frequency;
    QComboBox* impairs;
    QLineEdit* what;
};

#endif // PHYSICALCOMPLICATION_H
