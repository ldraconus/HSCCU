#ifndef DEPENDENT_H
#define DEPENDENT_H

#include <cmath>

#include "complication.h"

class Dependent: public Complication
{
public:
    Dependent(): Complication() { }
    Dependent(const Dependent& ac)
        : Complication()
        , _competence(ac._competence)
        , _frequency(ac._frequency)
        , _multiples(ac._multiples)
        , _unaware(ac._unaware)
        , _useful(ac._useful)
        , _who(ac._who) { }
    Dependent(Dependent&& ac)
        : Complication()
        , _competence(ac._competence)
        , _frequency(ac._frequency)
        , _multiples(ac._multiples)
        , _unaware(ac._unaware)
        , _useful(ac._useful)
        , _who(ac._who) { }
    Dependent(const QJsonObject& json)
        : Complication()
        , _competence(json["competence"].toInt(0))
        , _frequency(json["frequency"].toInt(0))
        , _multiples(json["multiples"].toInt(0))
        , _unaware(json["unaware"].toBool(false))
        , _useful(json["useful"].toBool(false))
        , _who(json["who"].toString("")) { }

    Dependent& operator=(const Dependent& ac) {
        if (this != &ac) {
            _who        = ac._who;
            _competence = ac._competence;
            _frequency  = ac._frequency;
            _multiples  = ac._multiples;
            _unaware    = ac._unaware;
            _useful     = ac._useful;
        }
        return *this;
    }
    Dependent& operator=(Dependent&& ac) {
        _who        = ac._who;
        _competence = ac._competence;
        _frequency  = ac._frequency;
        _multiples  = ac._multiples;
        _unaware    = ac._unaware;
        _useful     = ac._useful;
        return *this;
    }

    QString description() override {
        static QList<QString> comp { "Incompetent", "Normal", "Slightly Less Powerful Then The PC", "As Powerful As The PC" };
        static QList<QString> freq { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)" };
        if (_frequency == -1 || _competence == -1 || _who.isEmpty()) return "<incomplete>";
        QString result = "Dependent NPC: " + _who + " (" + freq[_frequency];
        if (_competence != -1) result += "; " + comp[_competence];
        if (_unaware) result += "; DNPC is unaware of character's adventuring";
        if (_useful) result += "; DNPC has useful noncombat position or skills";
        if (_multiples > 0) result += QString("; %1x people").arg(std::pow(2, _multiples));
        return result + ")";
    }
    void form(QWidget* parent, QVBoxLayout* layout) override {
        who        = createLineEdit(parent, layout, "Who is it?");
        competence = createComboBox(parent, layout, "How compentent is the DNPC?", { "Incompetent", "Normal", "Slightly Less Powerful Then The PC",
                                                                                     "As Powerful As The PC" });
        useful     = createCheckBox(parent, layout, "DNPC has useful noncombat position or skills");
        frequency  = createComboBox(parent, layout, "How often do they Appear?", { "Infrequently (8-)", "Frequently (11-)", "Very Frequently (14-)" });
        unaware    = createCheckBox(parent, layout, "DNPC is unaware of character's adventuring");
        multiples  = createLineEdit(parent, layout, "How many multiples of 2?", std::mem_fn(&Complication::numeric));
    }
    int points(bool noStore = false) override {
        if (!noStore) store();
        return 10 - _competence * 5 + (_useful ? 5 : 0) + 5 * (_frequency + 1) + (_unaware ? 5 : 0) + _multiples * 5;
    }
    void restore() override {
        who->setText(_who);
        competence->setCurrentIndex(_competence);
        frequency->setCurrentIndex(_frequency);
        multiples->setText(QString("%1").arg(_multiples));
        unaware->setChecked(_unaware);
        useful->setChecked(_useful);
    }
    void store() override {
        _who        = who->text();
        _competence = competence->currentIndex();
        _frequency  = frequency->currentIndex();
        _multiples  = multiples->text().toInt(0);
        _unaware    = unaware->isChecked();
        _useful     = useful->isChecked();
    }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]       = "Dependent NPC";
        obj["competence"] = _competence;
        obj["frequency"]  = _frequency;
        obj["multiples"]  = _multiples;
        obj["unaware"]    = _unaware;
        obj["useful"]     = _useful;
        obj["who"]        = _who;
        return obj;
    }

private:
    int     _competence = -1;
    int     _frequency = -1;
    int     _multiples = 0;
    bool    _unaware = false;
    bool    _useful = false;
    QString _who = "";

    QComboBox* competence;
    QComboBox* frequency;
    QLineEdit* multiples;
    QCheckBox* unaware;
    QCheckBox* useful;
    QLineEdit* who;

    void numeric(QString) override {
        QString txt = multiples->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        multiples->undo();
    }
};

#endif // DEPENDENT_H
