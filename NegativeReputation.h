#ifndef NEGATIVEREPUTATION_H
#define NEGATIVEREPUTATION_H

#include "complication.h"
#include "sheet.h"

class NegativeReputation: public Complication {
public:
    NegativeReputation() = default;
    NegativeReputation(const QJsonObject& json)
        : Complication(json)
        , v { json["extreme"].toBool(false),
              json["frequency"].toInt(0),
              json["limited"].toBool(false),
              json["what"].toString("") } { }

    QString abbreviation() override { return str(true); }
    QString description() override  { return str(); }
    QString str(bool abbr = false) {
        static QList<QString> freq     { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)", "Always" };
        static QList<QString> freqSans { "Infrequently", "Frequently", "Very Frequently", "Always" };
        static QList<QString> freqAbbr     { "8-", "11-", "14-", "Always" };
        static QList<QString> freqSansAbbr { "Infreq.", "Freq.", "V. Freq.", "Always" };
        if (v.mFrequency < 0 || v.mWhat.isEmpty()) return "<incomplete>";
#ifndef ISHSC
        return QString(abbr ? "Neg. Rep.: %1 (%2%3%4)" : "Negative Reputation: %1 (%2%3%4)").arg(v.mWhat,
                                                               Sheet::ref().getOption().showFrequencyRolls()
                                                                    ? (abbr ? freqAbbr[v.mFrequency] : freq[v.mFrequency]) : (abbr ? freqSansAbbr[v.mFrequency] : freqSans[v.mFrequency]),
                                                                      v.mExtreme ? (abbr ? "; Xtreme" : "; Extreme") : "", v.mLimited ? (abbr ? "Lim. Grp" : "; Limited Group") : "");
#else
        return QString(abbr ? "Neg. Rep.: %1 (%2%3%4)" : "Negative Reputation: %1 (%2%3%4)").arg(v.mWhat,
                                                               abbr ? freqAbbr[v.mFrequency] : freq[v.mFrequency]),
                                                               v.mExtreme ? (abbr ? "; Xtreme" : "; Extreme") : "", v.mLimited ? (abbr ? "Lim. Grp" : "; Limited Group") : "");
#endif
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        what      = createLineEdit(parent, layout, "What is the reputation?");
#ifndef ISHSC
        if (Sheet::ref().getOption().showFrequencyRolls()) frequency = createComboBox(parent, layout, "How often is it recognized", { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)" });
        else frequency = createComboBox(parent, layout, "How often is it recognized", { "Infrequently", "Frequently", "Very Frequently" });
#else
        frequency = createComboBox(parent, layout, "How often is it recognized", { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)" });
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
        QJsonObject obj  = Complication::toJson();
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

    QCheckBox* extreme = nullptr;
    QComboBox* frequency = nullptr;
    QCheckBox* limited = nullptr;
    QLineEdit* what = nullptr;
};

#endif // NEGATIVEREPUTATION_H
