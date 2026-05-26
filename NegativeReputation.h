#ifndef NEGATIVEREPUTATION_H
#define NEGATIVEREPUTATION_H

#include "complication.h"
#include "sheet.h"

class NegativeReputation: public Complication
{
public:
    NegativeReputation(): Complication() { }
    NegativeReputation(const NegativeReputation& ac)
        : Complication()
        , v(ac.v) { }
    NegativeReputation(NegativeReputation&& ac)
        : Complication()
        , v(ac.v) { }
    NegativeReputation(const QJsonObject& json)
        : Complication()
        , v { json["extreme"].toBool(false), json["frequency"].toInt(0), json["limited"].toBool(false), json["what"].toString("") } { }
    ~NegativeReputation() override { }

    NegativeReputation& operator=(const NegativeReputation& ac) = delete;
    NegativeReputation& operator=(NegativeReputation&& ac) = delete;

    QString description() override {
        static QList<QString> freq     { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)", "Always" };
        static QList<QString> freqSans { "Infrequently", "Frequently", "Very Frequently", "Always" };
        if (v.mFrequency < 0 || v.mWhat.isEmpty()) return "<incomplete>";
        return QString("Negative Reputation: %1 (%2%3%4)").arg(v.mWhat,
#ifndef ISHSC
                                                               Sheet::ref().getOption().showFrequencyRolls()
#else
                                                               true
#endif
                                                                   ? freq[v.mFrequency] : freqSans[v.mFrequency],
                                                               v.mExtreme ? "; Extreme" : "", v.mLimited ? "; Limited Group" : "");
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        what      = createLineEdit(parent, layout, "What is the reputation?");
#ifndef ISHSC
        if (Sheet::ref().getOption().showFrequencyRolls())
#endif
            frequency = createComboBox(parent, layout, "How often is it recognized", { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)" });
#ifndef ISHSC
        else
            frequency = createComboBox(parent, layout, "How often is it recognized", { "Infrequently", "Frequently", "Very Frequently" });
#endif
        extreme   = createCheckBox(parent, layout, "Extreme Reputation");
        limited   = createCheckBox(parent, layout, "Known only to a limited group");
    }
    Points points(bool noStore = false) override {
        if (!noStore) store();
        return (v.mExtreme ? 5_cp : 0_cp) + (v.mFrequency + 1) * 5_cp - (v.mLimited ? 5_cp : 0_cp); // NOLINT
    }
    void restore() override {
        vars s = v;
        what->setText(s.mWhat);
        extreme->setChecked(s.mExtreme);
        frequency->setCurrentIndex(s.mFrequency);
        limited->setChecked(s.mLimited);
        v = s;
    }
    void store() override {
        v.mWhat      = what->text();
        v.mExtreme   = extreme->isChecked();
        v.mFrequency = frequency->currentIndex();
        v.mLimited   = limited->isChecked();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]      = "Negative Reputation";
        obj["extreme"]   = v.mExtreme;
        obj["frequency"] = v.mFrequency;
        obj["limited"]   = v.mLimited;
        obj["what"]      = v.mWhat;
        return obj;
    }

private:
    struct vars {
        bool    mExtreme = false;
        int     mFrequency = -1;
        bool    mLimited = false;
        QString mWhat = "";
    } v;

    gsl::owner<QCheckBox*> extreme = nullptr;
    gsl::owner<QComboBox*> frequency = nullptr;
    gsl::owner<QCheckBox*> limited = nullptr;
    gsl::owner<QLineEdit*> what = nullptr;
};

#endif // NEGATIVEREPUTATION_H
