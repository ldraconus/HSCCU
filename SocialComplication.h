#ifndef SOCIALCOMPLICATION_H
#define SOCIALCOMPLICATION_H

#include "complication.h"
#ifndef ISHSC
#include "sheet.h"
#endif

class SocialComplication: public Complication {
public:
    SocialComplication(): Complication() { }
    SocialComplication(const QJsonObject& json)
        : Complication(json)
        , v { json["effects"].toInt(0),
              json["frequency"].toInt(0),
              json["notRestrictive"].toBool(false),
              json["what"].toString("") } { }

    QString abbreviation() override { return str(); }
    QString description() override  { return str(); }
    QString str(bool abbr = false) {
        static QList<QString> freq     { "Infrequently (8-)", "Frequently (11-)",  "Very Frequently (14-)" };
        static QList<QString> freqSans { "Infrequently", "Frequently",  "Very Frequently" };
        static QList<QString> effc     { "Minor", "Major", "Severe" };
        static QList<QString> freqAbbr     { "8-", "11-",  "14-" };
        static QList<QString> freqSansAbbr { "Infreq.", "Freq.",  "V. Freq." };
        static QList<QString> effcAbbr     { "Min.", "Maj.", "Sev." };
        if (v.mWhat.isEmpty() || v.mFrequency < 0 || v.mEffects < 0) return "<incomplete>";
        return (abbr ? "Soc. Comp." : "Social Complication: ") + v.mWhat + " (" + (
#ifndef ISHSC
                   Sheet::ref().getOption().showFrequencyRolls()
#else
                   true
#endif
                       ? (abbr ? freqAbbr[v.mFrequency] : freq[v.mFrequency]) : (abbr ? freqSansAbbr[v.mFrequency] : freqSans[v.mFrequency])) + "); " +
                (abbr ? effcAbbr[v.mEffects] : effc[v.mEffects]) +
                (v.mNotRestrictive ? (abbr ? "; Not in Some Cult." : "; Not Resrtictive in Some Cultures") : "") + ")";
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        what           = createLineEdit(parent, layout, "What is the complication?");
#ifndef ISHSC
        if (Sheet::ref().getOption().showFrequencyRolls())
#endif
            frequency  = createComboBox(parent, layout, "How often is it a problem?", { "Infrequently (8-)", "Frequently (11-)",  "Very Frequently (14-)" });
#ifndef ISHSC
        else
            frequency  = createComboBox(parent, layout, "How often is it a problem?", { "Infrequently", "Frequently",  "Very Frequently" });
#endif
        effects        = createComboBox(parent, layout, "Effects of the Restriction?", { "Minor", "Major", "Severe" });
        notRestrictive = createCheckBox(parent, layout, "Complication is Not Restictive in Some Cultures or Societies");
    }
    Points points(bool noStore = false) override {
        if (!noStore) store();
        auto pnts = 5_cp * (v.mFrequency + 1) + (v.mEffects < 0 ? 0 : v.mEffects) * 5_cp - (v.mNotRestrictive ? 5_cp : 0_cp);
        return pnts;
    }
    void restore() override {
        vars s = v;
        effects->setCurrentIndex(s.mEffects);
        frequency->setCurrentIndex(s.mFrequency);
        notRestrictive->setChecked(s.mNotRestrictive);
        what->setText(s.mWhat);
        v = s;
    }
    void store() override {
        v.mEffects        = effects->currentIndex();
        v.mFrequency      = frequency->currentIndex();
        v.mNotRestrictive = notRestrictive->isChecked();
        v.mWhat           = what->text();
    }
    QJsonObject toJson() override {
        QJsonObject obj       = Complication::toJson();
        obj["name"]           = "Social Complication";
        obj["effects"]        = v.mEffects;
        obj["frequency"]      = v.mFrequency;
        obj["notRestrictive"] = v.mNotRestrictive;
        obj["what"]           = v.mWhat;
        return obj;
    }

private:
    struct vars {
        int     mEffects        = -1;
        int     mFrequency      = -1;
        bool    mNotRestrictive = false;
        QString mWhat           = "";
    } v;

    QComboBox* effects;
    QComboBox* frequency;
    QCheckBox* notRestrictive;
    QLineEdit* what;
};

#endif // SOCIALCOMPLICATION_H
