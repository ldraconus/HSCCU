#ifndef ADJUSTMENTPOWERS_H
#define ADJUSTMENTPOWERS_H

#include "powers.h"

class AdjustmentPowers: public Power {
public:
    AdjustmentPowers(): Power() { }
    AdjustmentPowers(QString name)
        : Power()
        , v { name, "" } { }
    AdjustmentPowers(const AdjustmentPowers& s)
        : Power()
        , v(s.v) { }
    AdjustmentPowers(AdjustmentPowers&& s)
        : Power()
        , v(s.v) { }
    AdjustmentPowers(const QJsonObject& json)
        : Power()
        , v { json["name"].toString(), json["powerName"].toString("") } { }

    virtual AdjustmentPowers& operator=(const AdjustmentPowers& s) {
        if (this != &s) v = s.v;
        return *this;
    }
    virtual AdjustmentPowers& operator=(AdjustmentPowers&& s) {
        v = s.v;
        return *this;
    }

    QString     description(bool roll = false) override = 0;
    int         points(bool noStore = false) override   = 0;

    Fraction adv() override            { return Fraction(0); }
    void     checked(bool) override    { }
    QString  end() override            { return ""; }
    Fraction lim() override            { return Fraction(0); }
    void     numeric(QString) override { }

    void        form(QWidget* widget, QVBoxLayout* layout) override { powerName = createLineEdit(widget, layout, "Nickname of power");
                                                                    }
    QString     name() override                                     { return v._name;
                                                                    }
    QString     nickname() override                                 { return v._powerName;
                                                                    }
    void        restore() override                                  { vars s = v;
                                                                      powerName->setText(s._powerName);
                                                                      v = s;
                                                                    }
    void        store() override                                    { v._powerName = powerName->text();
                                                                    }
    QJsonObject toJson() override                                   { QJsonObject obj;
                                                                      obj["name"]      = v._name;
                                                                      obj["powerName"] = v._powerName;
                                                                      return obj;
                                                                    }

private:
    struct vars {
        QString _name;
        QString _powerName;
    } v;

    QLineEdit* powerName;
};

class Absorption: public AdjustmentPowers {
public:
    Absorption(): AdjustmentPowers("Absorption")                { }
    Absorption(const Absorption& s): AdjustmentPowers(s)        { }
    Absorption(Absorption&& s): AdjustmentPowers(s)             { }
    Absorption(const QJsonObject& json): AdjustmentPowers(json) { v._body      = json["body"].toInt(0);
                                                                  v._to        = json["to"].toString("");
                                                                  v._defensive = json["defensive"].toInt(0);
                                                                  v._varying   = json["varying"].toBool(false);
                                                                }

    Fraction adv() override                                      { return def(); }
    QString  description(bool showEND = false) override          { return (showEND ? "" : "") + optOut(showEND); }
    QString  end() override                                      { return ""; }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AdjustmentPowers::form(parent, layout);
                                                                   body      = createLineEdit(parent, layout, "BODY to Absorb", std::mem_fn(&Power::numeric));
                                                                   to        = createLineEdit(parent, layout, "Points transfered to?");
                                                                   defensive = createComboBox(parent, layout, "Defensive Absorption?", { "None",
                                                                                                                                         "Normal Defense",
                                                                                                                                         "Resistant Defense" });
                                                                   varying   = createCheckBox(parent, layout, "Varying Effect");
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    int      points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v._body; }
    void     restore() override                                  { AdjustmentPowers::restore();
                                                                   vars s = v;
                                                                   body->setText(QString("%1").arg(s._body));
                                                                   to->setText(s._to);
                                                                   defensive->setCurrentIndex(s._defensive);
                                                                   varying->setChecked(s._varying);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AdjustmentPowers::store();
                                                                   v._body      = body->text().toInt(0);
                                                                   v._to        = to->text();
                                                                   v._defensive = defensive->currentIndex();
                                                                   v._varying   = varying->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AdjustmentPowers::toJson();
                                                                   obj["body"]      = v._body;
                                                                   obj["to"]        = v._to;
                                                                   obj["defensive"] = v._defensive;
                                                                   obj["varying"]   = v._varying;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     _body = 0;
        QString _to = "";
        int     _defensive = -1;
        bool    _varying = false;
    } v;

    QLineEdit* body;
    QLineEdit* to;
    QComboBox* defensive;
    QCheckBox* varying;

    QString optOut(bool showEND) {
        if (v._defensive == -1 || v._to.isEmpty()) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " - ";
        res += "Absorption▲: " + QString("+%1").arg(v._body) + " to " + v._to;
        if (v._defensive == 1) {
            Fraction f(1, 2);
            res += "; (+" + f.toString() + ") Normal Defensive";
        }
        if (v._defensive == 2) res += "; (+1) Resistant Defensive";
        if (v._varying) {
            Fraction f(3, 4);
            res += "; (+" + f.toString() + ") Varying";
        }
        return res;
    }

    Fraction def() {
        Fraction var(0, 1);
        if (v._varying) var = Fraction(3, 4);
        switch (v._defensive) {
        case 1: return var + Fraction(1, 2);
        case 2: return var + Fraction(1);
        }
        return var;
    }

    void numeric(QString) override {
        QString txt = body->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        body->undo();
    }
};

// ▲○

#endif // ADJUSTMENTPOWERS_H
