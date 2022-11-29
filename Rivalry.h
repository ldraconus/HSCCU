#ifndef RIVALRY_H
#define RIVALRY_H

#include "complication.h"

class Rivalry: public Complication
{
public:
    Rivalry(): Complication() { }
    Rivalry(const Rivalry& ac)
        : Complication()
        , _intensity(ac._intensity)
        , _nature(ac._nature)
        , _pc(ac._pc)
        , _power(ac._power)
        , _unaware(ac._unaware)
        , _who(ac._who) { }
    Rivalry(Rivalry&& ac)
        : Complication()
        , _intensity(ac._intensity)
        , _nature(ac._nature)
        , _pc(ac._pc)
        , _power(ac._power)
        , _unaware(ac._unaware)
        , _who(ac._who) { }
    Rivalry(const QJsonObject& json)
        : Complication()
        , _intensity(json["intensity"].toInt(0))
        , _nature(json["nature"].toInt(0))
        , _pc(json["pc"].toInt(0))
        , _power(json["power"].toInt(0))
        , _unaware(json["unaware"].toBool(false))
        , _who(json["who"].toString("")) { }

    Rivalry& operator=(const Rivalry& ac) {
        if (this != &ac) {
            _intensity = ac._intensity;
            _nature    = ac._nature;
            _pc        = ac._pc;
            _power     = ac._power;
            _unaware   = ac._unaware;
            _who       = ac._who;
        }
        return *this;
    }
    Rivalry& operator=(Rivalry&& ac) {
        _intensity = ac._intensity;
        _nature    = ac._nature;
        _pc        = ac._pc;
        _power     = ac._power;
        _unaware   = ac._unaware;
        _who       = ac._who;
        return *this;
    }

    QString description() override {
        static QList<QString> natr { "Professional", "Romantic", "Professional and Romantic" };
        static QList<QString> powr { "Less Powerful", "As Powerful", "More Powerful", "Significatly More Powerful" };
        static QList<QString> intn { "Outdo, Embaraass, or Humiliate", "Maim, or Kill" };
        if (_nature == -1 || _power == -1 || _intensity == -1 || _who.isEmpty()) return "<incomplete>";
        QString result = QString("Rivalry: %1 (%2; %3; %4").arg(_who, natr[_nature], powr[_power], intn[_intensity]);
        if (_unaware) result += "; Unaware";
        if (_pc) result += "; is a PC";
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
        return _intensity * 5 + ((_nature < 2) ? 5 : 10) + (_unaware ? 5 : 0) + (_pc ? 5 : 0) + (_power * 5 - 5) + _intensity * 5;
    }
    void restore() override {
        who->setText(_who);
        intensity->setCurrentIndex(_intensity);
        nature->setCurrentIndex(_nature);
        pc->setChecked(_pc);
        power->setCurrentIndex(_power);
        unaware->setChecked(_unaware);
    }
    void store() override {
        _who       = who->text();
        _intensity = intensity->currentIndex();
        _nature    = nature->currentIndex();
        _pc        = pc->isChecked();
        _power     = power->currentIndex();
        _unaware   = unaware->isChecked();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]      = "Rivalry";
        obj["intensity"] = _intensity;
        obj["nature"]    = _nature;
        obj["pc"]        = _pc;
        obj["power"]     = _power;
        obj["unaware"]   = _unaware;
        obj["who"]       = _who;
        return obj;
    }

private:
    int     _intensity = -1;
    int     _nature    = -1;
    bool    _pc        = false;
    int     _power     = -1;
    bool    _unaware   = false;
    QString _who       = "";

    QComboBox* intensity;
    QComboBox* nature;
    QCheckBox* pc;
    QComboBox* power;
    QCheckBox* unaware;
    QLineEdit* who;
};

#endif // RIVALRY_H
