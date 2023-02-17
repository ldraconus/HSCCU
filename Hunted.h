#ifndef HUNTED_H
#define HUNTED_H

#include "complication.h"

class Hunted: public Complication
{
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

    Hunted& operator=(const Hunted& ac) {
        if (this != &ac) v = ac.v;
        return *this;
    }
    Hunted& operator=(Hunted&& ac) {
        v = ac.v;
        return *this;
    }

    QString description() override {
        static QList<QString> capa { "Less Powerful", "As Powerful", "More Powerful" };
        static QList<QString> freq { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)" };
        static QList<QString> mtvn { "Watching", "Mildly Punish", "Harsly Punish" };
        if (v._frequency < 0 || v._capabilities < 0 || v._motivation  < 0 || v._who.isEmpty()) return "<incomplete>";
        QString result = QString("Hunted: %1 (%2; %3").arg(v._who, capa[v._capabilities], freq[v._frequency]);
        if (v._nci) result += "; NCI";
        if (v._limited) result += "; Limited geographical area";
        return result + "; " + mtvn[v._motivation] + ")";
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        who          = createLineEdit(parent, layout, "Who is hunting you?");
        capabilities = createComboBox(parent, layout, "Hunter's Capabilities", { "Less Powerful than PC", "As Powerful as PC", "More Powerful than PC" });
        frequency    = createComboBox(parent, layout, "How often do they show up", { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)" });
        easy         = createCheckBox(parent, layout, "PC is easy to find");
        nci          = createCheckBox(parent, layout, "Hunter has extensive Non-combat Influence (NCI)");
        limited      = createCheckBox(parent, layout, "Limited to a certain geographical area");
        motivation   = createComboBox(parent, layout, "Hunter's motivation", { "Watching", "Mildly punish", "Harshly punish" });
    }
    Points<> points(bool noStore = false) override {
        if (!noStore) store();
        return (v._capabilities + 1) * 5_cp + v._frequency * 5_cp + (v._easy ? 5_cp : 0_cp) + (v._nci ? 5_cp : 0_cp) - (v._limited ? 5_cp : 0_cp) - (2 - v._motivation) * 5_cp;
    }
    void restore() override {
        vars s = v;
        who->setText(s._who);
        capabilities->setCurrentIndex(s._capabilities);
        easy->setChecked(s._easy);
        frequency->setCurrentIndex(s._frequency);
        limited->setChecked(s._limited);
        motivation->setCurrentIndex(s._motivation);
        nci->setChecked(s._nci);
        v = s;
    }
    void store() override {
        v._who          = who->text();
        v._capabilities = capabilities->currentIndex();
        v._easy         = easy->isChecked();
        v._frequency    = frequency->currentIndex();
        v._limited      = limited->isChecked();
        v._motivation   = motivation->currentIndex();
        v._nci          = nci->isChecked();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]         = "Hunted";
        obj["capabilities"] = v._capabilities;
        obj["easy"]         = v._easy;
        obj["frequency"]    = v._frequency;
        obj["limited"]      = v._limited;
        obj["motivation"]   = v._motivation;
        obj["nci"]          = v._nci;
        obj["who"]          = v._who;
        return obj;
    }

private:
    struct vars {
        int     _capabilities = -1;
        bool    _easy = false;
        int     _frequency = -1;
        bool    _limited = false;
        int     _motivation = -1;
        bool    _nci = false;
        QString _who = "";
    } v;

    QComboBox* capabilities;
    QComboBox* frequency;
    QCheckBox* easy;
    QCheckBox* limited;
    QComboBox* motivation;
    QCheckBox* nci;
    QLineEdit* who;
};

#endif // HUNTED_H
