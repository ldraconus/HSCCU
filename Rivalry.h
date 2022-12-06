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
        static QList<QString> natr { "Professional", "Romantic", "Professional and Romantic" };
        static QList<QString> powr { "Less Powerful", "As Powerful", "More Powerful", "Significatly More Powerful" };
        static QList<QString> intn { "Outdo, Embaraass, or Humiliate", "Maim, or Kill" };
        if (v._nature == -1 || v._power == -1 || v._intensity == -1 || v._who.isEmpty()) return "<incomplete>";
        QString result = QString("Rivalry: %1 (%2; %3; %4").arg(v._who, natr[v._nature], powr[v._power], intn[v._intensity]);
        if (v._unaware) result += "; Unaware";
        if (v._pc) result += "; is a PC";
        return result + ")";
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        who       = createLineEdit(parent, layout, "Who is the rival?");
        nature    = createComboBox(parent, layout, "Nature of the Rivalry", { "Professional", "Romantic", "Professional and Romantic" });
        unaware   = createCheckBox(parent, layout, "Rival is Unaware of Rivalry");
        pc        = createCheckBox(parent, layout, "Rival is a PC");
        power     = createComboBox(parent, layout, "Power of the Rival", { "Less Powerful", "As Powerful", "More Powerful", "Significatly More Powerful" });
        intensity = createComboBox(parent, layout, "Intesity of Rivalry", { "Outdo, Embaraass, or Humiliate", "Maim, or Kill" });
    }
    int points(bool noStore = false) override {
        if (!noStore) store();
        return v._intensity * 5 + ((v._nature < 2) ? 5 : 10) + (v._unaware ? 5 : 0) + (v._pc ? 5 : 0) + (v._power * 5 - 5) + v._intensity * 5;
    }
    void restore() override {
        vars s = v;
        who->setText(s._who);
        intensity->setCurrentIndex(s._intensity);
        nature->setCurrentIndex(s._nature);
        pc->setChecked(s._pc);
        power->setCurrentIndex(s._power);
        unaware->setChecked(s._unaware);
        v = s;
    }
    void store() override {
        v._who       = who->text();
        v._intensity = intensity->currentIndex();
        v._nature    = nature->currentIndex();
        v._pc        = pc->isChecked();
        v._power     = power->currentIndex();
        v._unaware   = unaware->isChecked();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]      = "Rivalry";
        obj["intensity"] = v._intensity;
        obj["nature"]    = v._nature;
        obj["pc"]        = v._pc;
        obj["power"]     = v._power;
        obj["unaware"]   = v._unaware;
        obj["who"]       = v._who;
        return obj;
    }

private:
    struct vars {
        int     _intensity = -1;
        int     _nature    = -1;
        bool    _pc        = false;
        int     _power     = -1;
        bool    _unaware   = false;
        QString _who       = "";
    } v;

    QComboBox* intensity;
    QComboBox* nature;
    QCheckBox* pc;
    QComboBox* power;
    QCheckBox* unaware;
    QLineEdit* who;
};

#endif // RIVALRY_H
