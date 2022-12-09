#ifndef SOCIALCOMPLICATION_H
#define SOCIALCOMPLICATION_H

#include "complication.h"

class SocialComplication: public Complication
{
public:
    SocialComplication(): Complication() { }
    SocialComplication(const SocialComplication& ac)
        : Complication()
        , v(ac.v) { }
    SocialComplication(SocialComplication&& ac)
        : Complication()
        , v(ac.v) { }
    SocialComplication(const QJsonObject& json)
        : Complication()
        , v { json["effects"].toInt(0), json["frequency"].toInt(0), json["notRestrictive"].toBool(false), json["what"].toString("") } { }

    SocialComplication& operator=(const SocialComplication& ac) {
        if (this != &ac) v = ac.v;
        return *this;
    }
    SocialComplication& operator=(SocialComplication&& ac) {
        v = ac.v;
        return *this;
    }

    QString description() override {
        static QList<QString> freq { "Infrequently (8-)", "Frequently (11-)",  "Very Frequently (14-)" };
        static QList<QString> effc { "Minor", "Major", "Severe" };
        if (v._what.isEmpty() || v._frequency == -1 || v._effects == -1) return "<incomplete>";
        return "Social Complication: " + v._what + " (" + freq[v._frequency] + "; " + effc[v._effects] +
                (v._notRestrictive ? "; Not Resrtictive in Some Cultures" : "") + ")";
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        what           = createLineEdit(parent, layout, "What is the complication?");
        frequency      = createComboBox(parent, layout, "How often is it a problem?", { "Infrequently (8-)", "Frequently (11-)",  "Very Frequently (14-)" });
        effects        = createComboBox(parent, layout, "Effects of the Restriction?", { "Minor", "Major", "Severe" });
        notRestrictive = createCheckBox(parent, layout, "Complication is Not Restictive in Some Cultures or Societies");
    }
    int points(bool noStore = false) override {
        if (!noStore) store();
        return 5 * (v._frequency + 1) + v._effects * 5 - (v._notRestrictive ? 5 : 0);
    }
    void restore() override {
        vars s = v;
        effects->setCurrentIndex(s._effects);
        frequency->setCurrentIndex(s._frequency);
        notRestrictive->setChecked(s._notRestrictive);
        what->setText(s._what);
        v = s;
    }
    void store() override {
        v._effects        = effects->currentIndex();
        v._frequency      = frequency->currentIndex();
        v._notRestrictive = notRestrictive->isChecked();
        v._what           = what->text();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]           = "Social Complication";
        obj["effects"]        = v._effects;
        obj["frequency"]      = v._frequency;
        obj["notRestrictive"] = v._notRestrictive;
        obj["what"]           = v._what;
        return obj;
    }

private:
    struct vars {
        int     _effects        = -1;
        int     _frequency      = -1;
        bool    _notRestrictive = false;
        QString _what           = "";
    } v;

    QComboBox* effects;
    QComboBox* frequency;
    QCheckBox* notRestrictive;
    QLineEdit* what;
};

#endif // SOCIALCOMPLICATION_H