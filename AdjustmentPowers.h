#ifndef ADJUSTMENTPOWERS_H
#define ADJUSTMENTPOWERS_H

#include "powers.h"
#include "powerdialog.h"

class Absorption: public AllPowers {
public:
    Absorption(): AllPowers("Absorption▲")               { }
    Absorption(const Absorption& s): AllPowers(s)        { }
    Absorption(Absorption&& s): AllPowers(s)             { }
    Absorption(const QJsonObject& json): AllPowers(json) { v.mBody      = json["body"].toInt(0);
                                                           v.mTo        = json["to"].toString("");
                                                           v.mDefensive = json["defensive"].toInt(0);
                                                           v.mVarying   = json["varying"].toBool(false);
                                                         }
    ~Absorption() override { }

    Absorption& operator=(const Absorption& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    Absorption& operator=(Absorption&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return def(); }
    QString  abbreviation(bool showEnd = false) override         { return optOut(showEnd, true); }
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
                                                                   return Points(v.mBody); }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   body->setText(QString("%1").arg(s.mBody));
                                                                   to->setText(s.mTo);
                                                                   defensive->setCurrentIndex(s.mDefensive);
                                                                   varying->setChecked(s.mVarying);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mBody      = body->text().toInt(0);
                                                                   v.mTo        = to->text();
                                                                   v.mDefensive = defensive->currentIndex();
                                                                   v.mVarying   = varying->isChecked();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["body"]      = v.mBody;
                                                                   obj["to"]        = v.mTo;
                                                                   obj["defensive"] = v.mDefensive;
                                                                   obj["varying"]   = v.mVarying;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     mBody = 0;
        QString mTo = "";
        int     mDefensive = -1;
        bool    mVarying = false;
    } v;

    QLineEdit* body = nullptr;
    QLineEdit* to = nullptr;
    QComboBox* defensive = nullptr;
    QCheckBox* varying = nullptr;

    QString optOut(bool showEND, bool abbr = false) {
        if (v.mDefensive < 1 && v.mTo.isEmpty()) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "Absorption▲: " + QString("+%1").arg(v.mBody) + " to " + v.mTo;
        if (v.mDefensive == 1) {
            Fraction f(1, 2);
            res += "; (+" + f.toString();
            if (abbr) res += ") Def";
            else res = +") Normal Defensive";
        }
        if (v.mDefensive == 2) {
            res += "; (+1) ";
            if (abbr) res += "RDef ";
            else res += "Resistant Def ";
        }
        if (v.mVarying) {
            Fraction f(3, 4);
            res += "; (+" + f.toString() + ") Varying";
        }
        return res;
    }

    Fraction def() {
        Fraction var(0, 1);
        Fraction threeQuarter(3, 4);
        Fraction oneHalf(1, 2);
        if (v.mVarying) var = threeQuarter;
        switch (v.mDefensive) {
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
    Aid(const QJsonObject& json): AllPowers(json) { v.mDice  = json["dice"].toInt(0);
                                                    v.mTo    = json["to"].toString("");
                                                    v.mBoost = json["boost"].toBool(false);
                                                    v.mWho   = json["who"].toInt(0);
                                                  }
    ~Aid() override { }

    Aid& operator=(const Aid& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    Aid& operator=(Aid&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  abbreviation(bool showEND = false) override         { return optOut(showEND, true); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   dice  = createLineEdit(parent, layout, "Dice of Aid", std::mem_fn(&Power::numeric));
                                                                   to    = createLineEdit(parent, layout, "Points transfered to?");
                                                                   boost = createCheckBox(parent, layout, "Boost");
                                                                   who   = createComboBox(parent, layout, "Who to aid?", { "Aid Anyone", "Only Others", "Only Self" });
                                                                 }
    Fraction lim() override                                      { return def(); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v.mDice * 6_cp; } // NOLINT
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dice->setText(QString("%1").arg(s.mDice));
                                                                   to->setText(s.mTo);
                                                                   who->setCurrentIndex(s.mWho);
                                                                   boost->setChecked(s.mBoost);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mDice  = dice->text().toInt();
                                                                   v.mTo    = to->text();
                                                                   v.mWho   = who->currentIndex();
                                                                   v.mBoost = boost->isChecked();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dice"]  = v.mDice;
                                                                   obj["to"]    = v.mTo;
                                                                   obj["who"]   = v.mWho;
                                                                   obj["boost"] = v.mBoost;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     mDice = 0;
        QString mTo = "";
        bool    mBoost = false;
        int     mWho = -1;
    } v;

    QLineEdit* dice = nullptr;
    QLineEdit* to = nullptr;
    QCheckBox* boost = nullptr;
    QComboBox* who = nullptr;

    QString optOut(bool showEND, bool abbr = false) {
        if (v.mDice < 1 || v.mTo.isEmpty()) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += (v.mBoost ? "Boost: " : "Aid: " ) + QString("+%1").arg(v.mDice) + "d6 to " + v.mTo;
        if (v.mWho == 1) {
            Fraction f(1, 2);
            if (abbr) res += "; (-" + f.toString() + ") Others Only";
            else res += "; (-" + f.toString() + ") Only Aid Others";
        }
        if (v.mWho == 2) {
            if (abbr) res += "; (-1) Self Only";
            else res += "; (-1) Only Aid Self";
        }
        return res;
    }

    Fraction def() {
        Fraction oneHalf(1, 2);
        switch (v.mWho) {
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
    Drain(const QJsonObject& json): AllPowers(json) { v.mDice     = json["body"].toInt(0);
                                                      v.mFrom     = json["from"].toString("");
                                                      v.mSuppress = json["suppress"].toBool(false);
                                                    }
    ~Drain() override { }

    Drain& operator=(const Drain& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    Drain& operator=(Drain&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  abbreviation(bool showEND = false) override         { return optOut(showEND, true); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   dice     = createLineEdit(parent, layout, "Dice of Drain", std::mem_fn(&Power::numeric));
                                                                   from     = createLineEdit(parent, layout, "Points drained from?");
                                                                   suppress = createCheckBox(parent, layout, "Suppress");
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override                 { if (!noStore) store();
                                                                   return Points(v.mDice * 10); } // NOLINT
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dice->setText(QString("%1").arg(s.mDice));
                                                                   from->setText(s.mFrom);
                                                                   suppress->setChecked(s.mSuppress);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mDice     = dice->text().toInt();
                                                                   v.mFrom     = from->text();
                                                                   v.mSuppress = suppress->isChecked();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dice"]     = v.mDice;
                                                                   obj["from"]     = v.mFrom;
                                                                   obj["suppress"] = v.mSuppress;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     mDice = 0;
        QString mFrom = "";
        bool    mSuppress = false;
    } v;

    QLineEdit* dice = nullptr;
    QLineEdit* from = nullptr;
    QCheckBox* suppress = nullptr;

    QString optOut(bool showEND, bool abbr = false) {
        if (v.mDice < 1 || v.mFrom.isEmpty()) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += (v.mSuppress ? "Suppress: " : "Drain: ") + QString("+%1").arg(v.mDice);
        if (abbr) res += +"d6 of " + v.mFrom;
        else res += +"d6 from " + v.mFrom;
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
    Healing(const QJsonObject& json): AllPowers(json) { v.mDice = json["dice"].toInt(0);
                                                        v.mTo   = json["to"].toString("");
                                                        v.mRate = json["rate"].toInt(-1);
                                                      }
    ~Healing() override { }

    Healing& operator=(const Healing& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    Healing& operator=(Healing&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return def(); }
    QString  abbreviation(bool showEND = false) override         { return optOut(showEND, true); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   dice = createLineEdit(parent, layout, "Dice of Healing?", std::mem_fn(&Power::numeric));
                                                                   to   = createLineEdit(parent, layout, "Points healed to?");
                                                                   rate = createComboBox(parent, layout, "Reduced Re-use Delay?", { "1 Day", "6 Hours", "1 Hour", "20 Minutes",
                                                                                                                                    "5 Minutes", "1 Minute", "Turn" });
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return Points(v.mDice * 10); } // NOLINT
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dice->setText(QString("%1").arg(s.mDice));
                                                                   to->setText(s.mTo);
                                                                   rate->setCurrentIndex(s.mRate);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mDice = dice->text().toInt();
                                                                   v.mTo   = to->text();
                                                                   v.mRate = rate->currentIndex();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dice"] = v.mDice;
                                                                   obj["to"]   = v.mTo;
                                                                   obj["rate"] = v.mRate;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     mDice = 0;
        QString mTo = "";
        int     mRate = -1;
    } v;

    QLineEdit* dice = nullptr;
    QLineEdit* to = nullptr;
    QComboBox* rate = nullptr;

    Fraction def() {
        if (v.mRate >= 1) return v.mRate * Fraction(1, 4);
        return Fraction(0);
    }

    QString optOut(bool showEND, bool abbr = false) {
        if (v.mDice < 1 || v.mTo.isEmpty()) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "Healing: " + QString("+%1").arg(v.mDice) + "d6 to " + v.mTo;
        if (abbr) {
            QStringList rate { "", "6 Hrs", "Hr", "20 Mins",
                             "5 Mins", "Min", "Turn" };
            if (v.mRate >= 1) res += " (Rpt " + rate[v.mRate] + " On A Tgt)";
        } else {
            QStringList rate { "", "6 Hours", "Hour", "20 Minutes",
                               "5 Minutes", "Minute", "Turn" };
            if (v.mRate >= 1) res += " (Usable Every " + rate[v.mRate] + " On Same Target)";
        }
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
