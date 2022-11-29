#ifndef SOCIALCOMPLICATION_H
#define SOCIALCOMPLICATION_H

#include "complication.h"

class SocialComplication: public Complication
{
public:
    SocialComplication(): Complication() { }
    SocialComplication(const SocialComplication& ac)
        : Complication()
        , _effects(ac._effects)
        , _frequency(ac._frequency)
        , _notRestrictive(ac._notRestrictive)
        , _what(ac._what) { }
    SocialComplication(SocialComplication&& ac)
        : Complication()
        , _effects(ac._effects)
        , _frequency(ac._frequency)
        , _notRestrictive(ac._notRestrictive)
        , _what(ac._what) { }
    SocialComplication(const QJsonObject& json)
        : Complication()
        , _effects(json["effects"].toInt(0))
        , _frequency(json["frequency"].toInt(0))
        , _notRestrictive(json["notRestrictive"].toBool(false))
        , _what(json["what"].toString("")) { }

    SocialComplication& operator=(const SocialComplication& ac) {
        if (this != &ac) {
            _frequency      = ac._frequency;
            _effects        = ac._effects;
            _notRestrictive = ac._notRestrictive;
            _what           = ac._what;
        }
        return *this;
    }
    SocialComplication& operator=(SocialComplication&& ac) {
        _frequency      = ac._frequency;
        _effects        = ac._effects;
        _notRestrictive = ac._notRestrictive;
        _what           = ac._what;
        return *this;
    }

    QString description() override {
        static QList<QString> freq { "Infrequently (8-)", "Frequently (11-)",  "Very Frequently (14-)" };
        static QList<QString> effc { "Minor", "Major", "Severe" };
        if (_what.isEmpty() || _frequency == -1 || _effects == -1) return "<incomplete>";
        return "Social Complication: " + _what + " (" + freq[_frequency] + "; " + effc[_effects] +
                (_notRestrictive ? "; Not Resrtictive in Some Cultures" : "") + ")";
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        what           = createLineEdit(parent, layout, "What is the complication?");
        frequency      = createComboBox(parent, layout, "How often is it a problem?", { "Infrequently (8-)", "Frequently (11-)",  "Very Frequently (14-)" });
        effects        = createComboBox(parent, layout, "Effects of the Restriction?", { "Minor", "Major", "Severe" });
        notRestrictive = createCheckBox(parent, layout, "Complication is Not Restictive in Some Cultures or Societies");
    }
    int points(bool noStore = false) override {
        if (!noStore) store();
        return 5 * (_frequency + 1) + _effects * 5 - (_notRestrictive ? 5 : 0);
    }
    void restore() override {
        effects->setCurrentIndex(_effects);
        frequency->setCurrentIndex(_frequency);
        notRestrictive->setChecked(_notRestrictive);
        what->setText(_what);
    }
    void store() override {
        _effects        = effects->currentIndex();
        _frequency      = frequency->currentIndex();
        _notRestrictive = notRestrictive->isChecked();
        _what           = what->text();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]           = "Social Complication";
        obj["effects"]        = _effects;
        obj["frequency"]      = _frequency;
        obj["notRestrictive"] = _notRestrictive;
        obj["what"]           = _what;
        return obj;
    }

private:
    int     _effects        = -1;
    int     _frequency      = -1;
    bool    _notRestrictive = false;
    QString _what           = "";

    QComboBox* effects;
    QComboBox* frequency;
    QCheckBox* notRestrictive;
    QLineEdit* what;
};

#endif // SOCIALCOMPLICATION_H
