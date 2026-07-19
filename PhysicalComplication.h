#ifndef PHYSICALCOMPLICATION_H
#define PHYSICALCOMPLICATION_H

#include "complication.h"

class PhysicalComplication: public Complication {
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
    ~PhysicalComplication() override { }

    PhysicalComplication& operator=(const PhysicalComplication& ac) = delete;
    PhysicalComplication& operator=(PhysicalComplication&& ac) = delete;

    QString description() override {
        static QList<QString> impr { "Barely", "Slightly", "Greatly", "Fully" };
        static QList<QString> freq { "Infrequently", "Frequently", "Very Frequently", "Always" };
        if (v.mFrequency < 0 || v.mImpairs < 0 || v.mWhat.isEmpty()) return "<incomplete>";
        return QString("Physical Complication: %1 (%2; %3 imparing)").arg(v.mWhat, freq[v.mFrequency], impr[v.mImpairs]);
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        what         = createLineEdit(parent, layout, "What is the complication?");
        frequency    = createComboBox(parent, layout, "How often does it affect you?", { "Infrequently", "Frequently", "Very Frequently", "All The Time" });
        impairs      = createComboBox(parent, layout, "How badly does it affect you?", { "Barely", "Slightly", "Greatly", "Fully" });
    }
    Points points(bool noStore = false) override {
        if (!noStore) store();
        return (v.mFrequency + 1) * 5_cp + (v.mImpairs < 0 ? 0 : v.mImpairs) * 5_cp; // NOLINT
    }
    void restore() override {
        vars s = v;
        what->setText(s.mWhat);
        frequency->setCurrentIndex(s.mFrequency);
        impairs->setCurrentIndex(s.mImpairs);
        v = s;
    }
    void store() override {
        v.mWhat      = what->text();
        v.mFrequency = frequency->currentIndex();
        v.mImpairs   = impairs->currentIndex();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]      = "Physical Complication";
        obj["frequency"] = v.mFrequency;
        obj["impairs"]   = v.mImpairs;
        obj["what"]      = v.mWhat;
        return obj;
    }

private:
    struct vars {
        int     mFrequency = -1;
        int     mImpairs = -1;
        QString mWhat = "";
    } v;

    QComboBox* frequency = nullptr;
    QComboBox* impairs = nullptr;
    QLineEdit* what = nullptr;
};

#endif // PHYSICALCOMPLICATION_H
