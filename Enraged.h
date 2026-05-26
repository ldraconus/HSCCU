#ifndef ENRAGED_H
#define ENRAGED_H

#include "complication.h"

class Enraged: public Complication {
public:
    Enraged(): Complication() { }
    Enraged(const Enraged& ac)
        : Complication()
        , v(ac.v) { }
    Enraged(Enraged&& ac)
        : Complication()
        , v(ac.v) { }
    Enraged(const QJsonObject& json)
        : Complication()
        , v { json["chance"].toInt(0)
            , json["frequency"].toInt(0)
            , json["regain"].toInt(0)
            , json["type"].toBool(false)
            , json["what"].toString("") } { }
    ~Enraged() override { }

    Enraged& operator=(const Enraged& ac) {
        if (this != &ac) v = ac.v;
        return *this;
    }
    Enraged& operator=(Enraged&& ac) {
        v = ac.v;
        return *this;
    }

    QString description() override {
        static QList<QString> freq { "Uncommon", "Common", "Very Common" };
        static QList<QString> chnc { "8-", "11-", "14-" };
        static QList<QString> regn { "14-", "11-", "8-" };
        if (v.mRrequency < 0 || v.mChance < 0 || v.mRegain < 0 || v.mWhat.isEmpty()) return "<incomplete>";
        return QString("%1: %2 (%3 Go: %4; Recover: %5)").arg(v.mType ? "Berserk" : "Enraged", v.mWhat, freq[v.mRrequency], chnc[v.mChance], regn[v.mRegain]);
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        what      = createLineEdit(parent, layout, "What sets you off?");
        frequency = createComboBox(parent, layout, "How commonly does it happen", { "Uncommon", "Common", "Very Common" });
        chance    = createComboBox(parent, layout, "How easily do you go?", { "On an 8-", "On an 11-", "On a 14-" });
        regain    = createComboBox(parent, layout, "How easily do you recover?", { "On a 14-", "On an 11-", "On an 8-" });
        type      = createCheckBox(parent, layout, "Berserk");
    }
    Points points(bool noStore = false) override {
        if (!noStore) store();
        return (v.mRrequency + 1) * 5_cp + v.mChance * 5_cp + v.mRegain * 5_cp + (v.mType ? 10_cp : 0_cp); // NOLINT
    }
    void restore() override {
        vars s = v;
        what->setText(s.mWhat);
        chance->setCurrentIndex(s.mChance);
        frequency->setCurrentIndex(s.mRrequency);
        regain->setCurrentIndex(s.mRegain);
        type->setChecked(s.mType);
        v = s;
    }
    void store() override {
        v.mWhat      = what->text();
        v.mChance    = chance->currentIndex();
        v.mRrequency = frequency->currentIndex();
        v.mRegain    = regain->currentIndex();
        v.mType      = type->isChecked();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]      = "Enraged/Berserk";
        obj["chance"]    = v.mChance;
        obj["frequency"] = v.mRrequency;
        obj["regain"]    = v.mRegain;
        obj["type"]      = v.mType;
        obj["what"]      = v.mWhat;
        return obj;
    }

private:
    struct vars {
        int     mChance = -1;
        int     mRrequency = -1;
        int     mRegain = -1;
        bool    mType = false;
        QString mWhat = "";
    } v;

    gsl::owner<QComboBox*> chance = nullptr;
    gsl::owner<QComboBox*> frequency = nullptr;
    gsl::owner<QComboBox*> regain = nullptr;
    gsl::owner<QCheckBox*> type = nullptr;
    gsl::owner<QLineEdit*> what = nullptr;
};

#endif // ENRAGED_H
