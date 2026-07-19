#ifndef HUNTED_H
#define HUNTED_H

#include "complication.h"
#ifndef ISHSC
#include "sheet.h"
#endif

class Hunted: public Complication {
public:
    Hunted(): Complication() { }
    Hunted(const Hunted& ac)
        : Complication()
        , v(ac.v) { }
    Hunted(Hunted&& ac)
        : Complication()
        , v(ac.v) { }
    Hunted(const QJsonObject& json)
        : Complication()
        , v { json["capabilities"].toInt(0)
            , json["easy"].toBool(false)
            , json["frequency"].toInt(0)
            , json["limited"].toBool(false)
            , json["motivation"].toInt(0)
            , json["nci"].toBool(false)
            , json["who"].toString("") } { }
    ~Hunted() override { }

    Hunted& operator=(const Hunted& ac) {
        if (this != &ac) v = ac.v;
        return *this;
    }
    Hunted& operator=(Hunted&& ac) {
        v = ac.v;
        return *this;
    }

    QString description() override {
        static QList<QString> capa     { "Less Powerful", "As Powerful", "More Powerful" };
        static QList<QString> freq     { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)" };
        static QList<QString> freqSans { "Infrequently", "Frequently", "Very Frequently", "Always" };
        static QList<QString> mtvn     { "Watching", "Mildly Punish", "Harsly Punish" };
        if (v.mFrequency < 0 || v.mCapabilities < 0 || v.mMotivation  < 0 || v.mWho.isEmpty()) return "<incomplete>";
        QString result = QString("Hunted: %1 (%2; %3").arg(v.mWho, capa[v.mCapabilities],
#ifndef ISHSC
                                                           Sheet::ref().getOption().showFrequencyRolls()
#else
                                                           true
#endif
                                                               ? freq[v.mFrequency] : freqSans[v.mFrequency]);
        if (v.mNCI) result += "; NCI";
        if (v.mLimited) result += "; Limited geographical area";
        return result + "; " + mtvn[v.mMotivation] + ")";
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        who           = createLineEdit(parent, layout, "Who is hunting you?");
        capabilities  = createComboBox(parent, layout, "Hunter's Capabilities", { "Less Powerful than PC", "As Powerful as PC", "More Powerful than PC" });
#ifndef ISHSC
        if (Sheet::ref().getOption().showFrequencyRolls())
#endif
            frequency = createComboBox(parent, layout, "How often do they show up", { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)" });
#ifndef ISHSC
        else
            frequency = createComboBox(parent, layout, "How often do they show up", { "Infrequently", "Frequently", "Very Frequently" });
#endif
        easy          = createCheckBox(parent, layout, "PC is easy to find");
        nci           = createCheckBox(parent, layout, "Hunter has extensive Non-combat Influence (NCI)");
        limited       = createCheckBox(parent, layout, "Limited to a certain geographical area");
        motivation    = createComboBox(parent, layout, "Hunter's motivation", { "Watching", "Mildly punish", "Harshly punish" });
    }
    Points points(bool noStore = false) override {
        if (!noStore) store();
        return (v.mCapabilities + 1) * 5_cp + v.mFrequency * 5_cp + (v.mEasy ? 5_cp : 0_cp) + (v.mNCI ? 5_cp : 0_cp) - (v.mLimited ? 5_cp : 0_cp) - (2 - v.mMotivation) * 5_cp; // NOLINT
    }
    void restore() override {
        vars s = v;
        who->setText(s.mWho);
        capabilities->setCurrentIndex(s.mCapabilities);
        easy->setChecked(s.mEasy);
        frequency->setCurrentIndex(s.mFrequency);
        limited->setChecked(s.mLimited);
        motivation->setCurrentIndex(s.mMotivation);
        nci->setChecked(s.mNCI);
        v = s;
    }
    void store() override {
        v.mWho          = who->text();
        v.mCapabilities = capabilities->currentIndex();
        v.mEasy         = easy->isChecked();
        v.mFrequency    = frequency->currentIndex();
        v.mLimited      = limited->isChecked();
        v.mMotivation   = motivation->currentIndex();
        v.mNCI          = nci->isChecked();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]         = "Hunted";
        obj["capabilities"] = v.mCapabilities;
        obj["easy"]         = v.mEasy;
        obj["frequency"]    = v.mFrequency;
        obj["limited"]      = v.mLimited;
        obj["motivation"]   = v.mMotivation;
        obj["nci"]          = v.mNCI;
        obj["who"]          = v.mWho;
        return obj;
    }

private:
    struct vars {
        int     mCapabilities = -1;
        bool    mEasy = false;
        int     mFrequency = -1;
        bool    mLimited = false;
        int     mMotivation = -1;
        bool    mNCI = false;
        QString mWho = "";
    } v;

    QComboBox* capabilities = nullptr;
    QComboBox* frequency = nullptr;
    QCheckBox* easy = nullptr;
    QCheckBox* limited = nullptr;
    QComboBox* motivation = nullptr;
    QCheckBox* nci = nullptr;
    QLineEdit* who = nullptr;
};

#endif // HUNTED_H
