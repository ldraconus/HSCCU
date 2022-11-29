#ifndef HUNTED_H
#define HUNTED_H

#include "complication.h"

class Hunted: public Complication
{
public:
    Hunted(): Complication() { }
    Hunted(const Hunted& ac)
        : Complication()
        , _capabilities(ac._capabilities)
        , _easy(ac._easy)
        , _frequency(ac._frequency)
        , _limited(ac._limited)
        , _motivation(ac._motivation)
        , _nci(ac._nci)
        , _who(ac._who) { }
    Hunted(Hunted&& ac)
        : Complication()
        , _capabilities(ac._capabilities)
        , _easy(ac._easy)
        , _frequency(ac._frequency)
        , _limited(ac._limited)
        , _motivation(ac._motivation)
        , _nci(ac._nci)
        , _who(ac._who) { }
    Hunted(const QJsonObject& json)
        : Complication()
        , _capabilities(json["capabilities"].toInt(0))
        , _easy(json["easy"].toBool(false))
        , _frequency(json["frequency"].toInt(0))
        , _limited(json["limited"].toBool(false))
        , _motivation(json["motivation"].toInt(0))
        , _nci(json["nci"].toBool(false))
        , _who(json["who"].toString("")) { }

    Hunted& operator=(const Hunted& ac) {
        if (this != &ac) {
            _capabilities = ac._capabilities;
            _easy         = ac._easy;
            _frequency    = ac._frequency;
            _limited      = ac._limited;
            _motivation   = ac._motivation;
            _nci          = ac._nci;
            _who          = ac._who;
        }
        return *this;
    }
    Hunted& operator=(Hunted&& ac) {
        _capabilities = ac._capabilities;
        _easy         = ac._easy;
        _frequency    = ac._frequency;
        _limited      = ac._limited;
        _motivation   = ac._motivation;
        _nci          = ac._nci;
        _who          = ac._who;
        return *this;
    }

    QString description() override {
        static QList<QString> capa { "Less Powerful", "As Powerful", "More Powerful" };
        static QList<QString> freq { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)" };
        static QList<QString> mtvn { "Watching", "Mildly Punish", "Harsly Punish" };
        if (_frequency == -1 || _capabilities == -1 || _motivation == -1 || _who.isEmpty()) return "<incomplete>";
        QString result = QString("Hunted: %1 (%2; %3").arg(_who, capa[_capabilities], freq[_frequency]);
        if (_nci) result += "; NCI";
        if (_limited) result += "; Limited geographical area";
        return result + "; " + mtvn[_motivation] + ")";
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
    int points(bool noStore = false) override {
        if (!noStore) store();
        return (_capabilities + 1) * 5 + _frequency * 5 + (_easy ? 5 : 0) + (_nci ? 5 : 0) - (_limited ? 5 : 0) - (2 - _motivation) * 5;
    }
    void restore() override {
        who->setText(_who);
        capabilities->setCurrentIndex(_capabilities);
        easy->setChecked(_easy);
        frequency->setCurrentIndex(_frequency);
        limited->setChecked(_limited);
        motivation->setCurrentIndex(_motivation);
        nci->setChecked(_nci);
    }
    void store() override {
        _who          = who->text();
        _capabilities = capabilities->currentIndex();
        _easy         = easy->isChecked();
        _frequency    = frequency->currentIndex();
        _limited      = limited->isChecked();
        _motivation   = motivation->currentIndex();
        _nci          = nci->isChecked();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]         = "Hunted";
        obj["capabilities"] = _capabilities;
        obj["easy"]         = _easy;
        obj["frequency"]    = _frequency;
        obj["limited"]      = _limited;
        obj["motivation"]   = _motivation;
        obj["nci"]          = _nci;
        obj["who"]          = _who;
        return obj;
    }

private:
    int     _capabilities = -1;
    bool    _easy = false;
    int     _frequency = -1;
    bool    _limited = false;
    int     _motivation = -1;
    bool    _nci = false;
    QString _who = "";

    QComboBox* capabilities;
    QComboBox* frequency;
    QCheckBox* easy;
    QCheckBox* limited;
    QComboBox* motivation;
    QCheckBox* nci;
    QLineEdit* who;
};

#endif // HUNTED_H
