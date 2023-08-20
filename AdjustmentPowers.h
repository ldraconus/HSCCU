#ifndef ADJUSTMENTPOWERS_H
#define ADJUSTMENTPOWERS_H

#include "powers.h"

class Absorption: public AllPowers {
public:
    Absorption(): AllPowers("Absorption▲")               { }
    Absorption(const Absorption& s): AllPowers(s)        { }
    Absorption(Absorption&& s): AllPowers(s)             { }
    Absorption(const QJsonObject& json): AllPowers(json) { v._body      = json["body"].toInt(0);
                                                           v._to        = json["to"].toString("");
                                                           v._defensive = json["defensive"].toInt(0);
                                                           v._varying   = json["varying"].toBool(false);
                                                         }
    virtual Absorption& operator=(const Absorption& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Absorption& operator=(Absorption&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return def(); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   body      = createLineEdit(parent, layout, "BODY to Absorb", std::mem_fn(&Power::numeric));
                                                                   to        = createLineEdit(parent, layout, "Points transfered to?");
                                                                   defensive = createComboBox(parent, layout, "Defensive Absorption?", { "Non-Defensive",
                                                                                                                                         "Normal Defense",
                                                                                                                                         "Resistant Defense" },
                                                                                              std::mem_fn(&Power::index));
                                                                   varying   = createCheckBox(parent, layout, "Varying Effect");
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points   points(bool noStore = false) override               { if (!noStore) store();
                                                                   return Points(v._body); }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   body->setText(QString("%1").arg(s._body));
                                                                   to->setText(s._to);
                                                                   defensive->setCurrentIndex(s._defensive);
                                                                   varying->setChecked(s._varying);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._body      = body->text().toInt(0);
                                                                   v._to        = to->text();
                                                                   v._defensive = defensive->currentIndex();
                                                                   v._varying   = varying->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
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
        if (v._defensive < 1 && v._to.isEmpty()) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
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
        Fraction threeQuarter(3, 4);
        Fraction oneHalf(1, 2);
        if (v._varying) var = threeQuarter;
        switch (v._defensive) {
        case 1: return var + oneHalf;
        case 2: return var + 1;
        }
        return var;
    }

    void index(int) override {
        store();
#ifndef ISHSC
        PowerDialog::ref().updateForm();
#endif
    }

    void numeric(int) override {
        QString txt = body->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        body->undo();
    }
};

class Aid: public AllPowers {
public:
    Aid(): AllPowers("Aid")                       { }
    Aid(const Aid& s): AllPowers(s)               { }
    Aid(Aid&& s): AllPowers(s)                    { }
    Aid(const QJsonObject& json): AllPowers(json) { v._dice  = json["dice"].toInt(0);
                                                    v._to    = json["to"].toString("");
                                                    v._boost = json["boost"].toBool(false);
                                                    v._who   = json["who"].toInt(0);
                                                  }
    virtual Aid& operator=(const Aid& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Aid& operator=(Aid&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   dice  = createLineEdit(parent, layout, "Dice of Aid", std::mem_fn(&Power::numeric));
                                                                   to    = createLineEdit(parent, layout, "Points transfered to?");
                                                                   boost = createCheckBox(parent, layout, "Boost");
                                                                   who   = createComboBox(parent, layout, "Who to aid?", { "Aid Anyone", "Only Others", "Only Self" });
                                                                 }
    Fraction lim() override                                      { return def(); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v._dice * 6_cp; }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dice->setText(QString("%1").arg(s._dice));
                                                                   to->setText(s._to);
                                                                   who->setCurrentIndex(s._who);
                                                                   boost->setChecked(s._boost);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._dice  = dice->text().toInt();
                                                                   v._to    = to->text();
                                                                   v._who   = who->currentIndex();
                                                                   v._boost = boost->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dice"]  = v._dice;
                                                                   obj["to"]    = v._to;
                                                                   obj["who"]   = v._who;
                                                                   obj["boost"] = v._boost;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     _dice = 0;
        QString _to = "";
        bool    _boost = false;
        int     _who = -1;
    } v;

    QLineEdit* dice;
    QLineEdit* to;
    QCheckBox* boost;
    QComboBox* who;

    QString optOut(bool showEND) {
        if (v._dice < 1 || v._to.isEmpty()) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += (v._boost ? "Boost: " : "Aid: " ) + QString("+%1").arg(v._dice) + "d6 to " + v._to;
        if (v._who == 1) {
            Fraction f(1, 2);
            res += "; (-" + f.toString() + ") Only Aid Others";
        }
        if (v._who == 2) res += "; (-1) Only Aid Self";
        return res;
    }

    Fraction def() {
        Fraction oneHalf(1, 2);
        switch (v._who) {
        case 1: return oneHalf;
        case 2: return Fraction(1);
        }
        return Fraction(0);
    }

    void index(int) override {
        store();
#ifndef ISHSC
        PowerDialog::ref().updateForm();
#endif
    }

    void numeric(int) override {
        QString txt = dice->text();
#ifndef ISHSC
        PowerDialog::ref().updateForm();
#endif
        if (txt.isEmpty() || isNumber(txt)) return;
        dice->undo();
    }
};

class Drain: public AllPowers {
public:
    Drain(): AllPowers("Drain")                     { }
    Drain(const Drain& s): AllPowers(s)             { }
    Drain(Drain&& s): AllPowers(s)                  { }
    Drain(const QJsonObject& json): AllPowers(json) { v._dice     = json["body"].toInt(0);
                                                      v._from     = json["from"].toString("");
                                                      v._suppress = json["suppress"].toBool(false);
                                                    }
    virtual Drain& operator=(const Drain& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Drain& operator=(Drain&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   dice     = createLineEdit(parent, layout, "Dice of Drain", std::mem_fn(&Power::numeric));
                                                                   from     = createLineEdit(parent, layout, "Points drained from?");
                                                                   suppress = createCheckBox(parent, layout, "Suppress");
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return Points(v._dice * 10); }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dice->setText(QString("%1").arg(s._dice));
                                                                   from->setText(s._from);
                                                                   suppress->setChecked(s._suppress);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._dice     = dice->text().toInt();
                                                                   v._from     = from->text();
                                                                   v._suppress = suppress->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dice"]     = v._dice;
                                                                   obj["from"]     = v._from;
                                                                   obj["suppress"] = v._suppress;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     _dice = 0;
        QString _from = "";
        bool    _suppress = false;
    } v;

    QLineEdit* dice;
    QLineEdit* from;
    QCheckBox* suppress;

    QString optOut(bool showEND) {
        if (v._dice < 1 || v._from.isEmpty()) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += (v._suppress ? "Suppress: " : "Drain: " ) + QString("+%1").arg(v._dice) + "d6 from " + v._from;
        return res;
    }

    void numeric(int) override {
        QString txt = dice->text();
#ifndef ISHSC
        PowerDialog::ref().updateForm();
#endif
        if (txt.isEmpty() || isNumber(txt)) return;
        dice->undo();
    }
};

class Healing: public AllPowers {
public:
    Healing(): AllPowers("Healing")                   { }
    Healing(const Healing& s): AllPowers(s)           { }
    Healing(Healing&& s): AllPowers(s)                { }
    Healing(const QJsonObject& json): AllPowers(json) { v._dice = json["dice"].toInt(0);
                                                        v._to   = json["to"].toString("");
                                                        v._rate = json["rate"].toInt(-1);
                                                      }
    virtual Healing& operator=(const Healing& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Healing& operator=(Healing&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return def(); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   dice = createLineEdit(parent, layout, "Dice of Healing?", std::mem_fn(&Power::numeric));
                                                                   to   = createLineEdit(parent, layout, "Points healed to?");
                                                                   rate = createComboBox(parent, layout, "Reduced Re-use Delay?", { "1 Day", "6 Hours", "1 Hour", "20 Minutes",
                                                                                                                                    "5 Minutes", "1 Minute", "Turn" });
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return Points(v._dice * 10); }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dice->setText(QString("%1").arg(s._dice));
                                                                   to->setText(s._to);
                                                                   rate->setCurrentIndex(s._rate);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._dice = dice->text().toInt();
                                                                   v._to   = to->text();
                                                                   v._rate = rate->currentIndex();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dice"] = v._dice;
                                                                   obj["to"]   = v._to;
                                                                   obj["rate"] = v._rate;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     _dice = 0;
        QString _to = "";
        int     _rate = -1;
    } v;

    QLineEdit* dice;
    QLineEdit* to;
    QComboBox* rate;

    Fraction def() {
        if (v._rate >= 1) return v._rate * Fraction(1, 4);
        return Fraction(0);
    }

    QString optOut(bool showEND) {
        if (v._dice < 1 || v._to.isEmpty()) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "Healing: " + QString("+%1").arg(v._dice) + "d6 to " + v._to;
        QStringList rate { "", "6 Hours", "Hour", "20 Minutes",
                           "5 Minutes", "Minute", "Turn" };
        if (v._rate >= 1) res += " (Usable Every " + rate[v._rate] + "On Same Target)";
        return res;
    }

    void numeric(int) override {
        QString txt = dice->text();
#ifndef ISHSC
        PowerDialog::ref().updateForm();
#endif
        if (txt.isEmpty() || isNumber(txt)) return;
        dice->undo();
    }
};

#endif // ADJUSTMENTPOWERS_H
