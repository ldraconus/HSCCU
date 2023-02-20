#ifndef PHYSICALCOMPLICATION_H
#define PHYSICALCOMPLICATION_H

#include "complication.h"

class PhysicalComplication: public Complication
{
public:
    PhysicalComplication(): Complication() { }
    PhysicalComplication(const PhysicalComplication& ac)
        : Complication()
        , v(ac.v) { }
    PhysicalComplication(PhysicalComplication&& ac)
        : Complication()
        , v(ac.v) { }
    PhysicalComplication(const QJsonObject& json)
        : Complication()
        , v { json["frequency"].toInt(0), json["impairs"].toInt(0), json["what"].toString("") } { }

    PhysicalComplication& operator=(const PhysicalComplication& ac) {
        if (this != &ac) v = ac.v;
        return *this;
    }
    PhysicalComplication& operator=(PhysicalComplication&& ac) {
        v = ac.v;
        return *this;
    }

    QString description() override {
        static QList<QString> impr { "Barely", "Slightly", "Greatly", "Fully" };
        static QList<QString> freq { "Infrequently", "Frequently", "Very Frequently", "Always" };
        if (v._frequency < 0 || v._impairs < 0 || v._what.isEmpty()) return "<incomplete>";
        return QString("Physical Complication: %1 (%2; %3 imparing)").arg(v._what, freq[v._frequency], impr[v._impairs]);
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        what         = createLineEdit(parent, layout, "What is the complication?");
        frequency    = createComboBox(parent, layout, "How often does it affect you?", { "Infrequently", "Frequently", "Very Frequently", "All The Time" });
        impairs      = createComboBox(parent, layout, "How badly does it affect you?", { "Barely", "Slightly", "Greatly", "Fully" });
    }
    Points<> points(bool noStore = false) override {
        if (!noStore) store();
        return (v._frequency + 1) * 5_cp + (v._impairs < 0 ? 0 : v._impairs) * 5_cp;
    }
    void restore() override {
        vars s = v;
        what->setText(s._what);
        frequency->setCurrentIndex(s._frequency);
        impairs->setCurrentIndex(s._impairs);
        v = s;
    }
    void store() override {
        v._what      = what->text();
        v._frequency = frequency->currentIndex();
        v._impairs   = impairs->currentIndex();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]      = "Physical Complication";
        obj["frequency"] = v._frequency;
        obj["impairs"]   = v._impairs;
        obj["what"]      = v._what;
        return obj;
    }

private:
    struct vars {
        int     _frequency = -1;
        int     _impairs = -1;
        QString _what = "";
    } v;

    QComboBox* frequency;
    QComboBox* impairs;
    QLineEdit* what;
};

#endif // PHYSICALCOMPLICATION_H
