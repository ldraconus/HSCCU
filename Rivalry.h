#ifndef RIVALRY_H
#define RIVALRY_H

#include "complication.h"

class Rivalry: public Complication
{
public:
    Rivalry(): Complication() { }
    Rivalry(const Rivalry& ac)
        : Complication()
        , v(ac.v) { }
    Rivalry(Rivalry&& ac)
        : Complication()
        , v(ac.v) { }
    Rivalry(const QJsonObject& json)
        : Complication()
        , v { json["intensity"].toInt(0), json["nature"].toInt(0), json["pc"].toBool(false), json["power"].toInt(0), json["unaware"].toBool(false), json["who"].toString("") } { }

    Rivalry& operator=(const Rivalry& ac) {
        if (this != &ac) v = ac.v;
        return *this;
    }
    Rivalry& operator=(Rivalry&& ac) {
        v = ac.v;;
        return *this;
    }

    QString description() override {
        static QList<QString> natr { "", "Professional", "Romantic", "Professional and Romantic" };
        static QList<QString> powr { "", "Less Powerful", "As Powerful", "More Powerful", "Significatly More Powerful" };
        static QList<QString> intn { "", "Outdo, Embaraass, or Humiliate", "Maim, or Kill" };
        if (v.mNature < 1 || v.mPower < 1 || v.mIntensity < 1 || v.mWho.isEmpty()) return "<incomplete>";
        QString result = QString("Rivalry: %1 (%2; %3; %4").arg(v.mWho, natr[v.mNature], powr[v.mPower], intn[v.mIntensity]);
        if (v.mUnaware) result += "; Unaware";
        if (v.mPC) result += "; is a PC";
        return result + ")";
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        who       = createLineEdit(parent, layout, "Who is the rival?");
        nature    = createComboBox(parent, layout, "Nature of the Rivalry", { "", "Professional", "Romantic", "Professional and Romantic" });
        unaware   = createCheckBox(parent, layout, "Rival is Unaware of Rivalry");
        pc        = createCheckBox(parent, layout, "Rival is a PC");
        power     = createComboBox(parent, layout, "Power of the Rival", { "", "Less Powerful", "As Powerful", "More Powerful", "Significatly More Powerful" });
        intensity = createComboBox(parent, layout, "Intesity of Rivalry", { "", "Outdo, Embaraass, or Humiliate", "Maim, or Kill" });
    }
    Points points(bool noStore = false) override {
        if (!noStore) store();
        return v.mIntensity * 5_cp + ((v.mNature < 3) ? 5_cp : 10_cp) + (v.mUnaware ? 5_cp : 0_cp) + (v.mPC ? 5_cp : 0_cp) + v.mPower * 5_cp + (v.mIntensity - 1) * 5_cp;
    }
    void restore() override {
        vars s = v;
        who->setText(s.mWho);
        intensity->setCurrentIndex(s.mIntensity);
        nature->setCurrentIndex(s.mNature);
        pc->setChecked(s.mPC);
        power->setCurrentIndex(s.mPower);
        unaware->setChecked(s.mUnaware);
        v = s;
    }
    void store() override {
        v.mWho       = who->text();
        v.mIntensity = intensity->currentIndex();
        v.mNature    = nature->currentIndex();
        v.mPC        = pc->isChecked();
        v.mPower     = power->currentIndex();
        v.mUnaware   = unaware->isChecked();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]      = "Rivalry";
        obj["intensity"] = v.mIntensity;
        obj["nature"]    = v.mNature;
        obj["pc"]        = v.mPC;
        obj["power"]     = v.mPower;
        obj["unaware"]   = v.mUnaware;
        obj["who"]       = v.mWho;
        return obj;
    }

private:
    struct vars {
        int     mIntensity = -1;
        int     mNature    = -1;
        bool    mPC        = false;
        int     mPower     = -1;
        bool    mUnaware   = false;
        QString mWho       = "";
    } v;

    QComboBox* intensity;
    QComboBox* nature;
    QCheckBox* pc;
    QComboBox* power;
    QCheckBox* unaware;
    QLineEdit* who;
};

#endif // RIVALRY_H
