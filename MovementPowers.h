#ifndef MOVEMENT_POWERS_H
#define MOVEMENT_POWERS_H

#include "powers.h"

class ExtraDimensionalMovement: public AllPowers {
public:
    ExtraDimensionalMovement(): AllPowers("Extra-Dimensional Movementϴ")      { }
    ExtraDimensionalMovement(const ExtraDimensionalMovement& s): AllPowers(s) { }
    ExtraDimensionalMovement(ExtraDimensionalMovement&& s): AllPowers(s)      { }
    ExtraDimensionalMovement(const QJsonObject& json): AllPowers(json)        { v._time   = json["time"].toBool(false);
                                                                                     v._numDim = json["numDim"].toInt(0);
                                                                                     v._dim    = json["dim"].toString();
                                                                                     v._loc    = json["loc"].toInt(0);
                                                                                     v._where  = json["where"].toString();
                                                                                     v._when   = json["when"].toInt(0);
                                                                                     v._moment = json["moment"].toString();
                                                                                     v._span   = json["span"].toInt(0);
                                                                                     v._timLoc = json["timLoc"].toInt(0);
                                                                                     v._timWhr = json["timWhr"].toString();
                                                                                   }
    virtual ExtraDimensionalMovement& operator=(const ExtraDimensionalMovement& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual ExtraDimensionalMovement& operator=(ExtraDimensionalMovement&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return (showEND ? "" : "") + optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   time   = createCheckBox(parent, layout, "Time Travelϴ", std::mem_fn(&Power::checked));
                                                                   numDim = createComboBox(parent, layout, "Number of Dimensions Modifiers",
                                                                                           { "A Single Dimension",
                                                                                             "A Related Group of Dimensions",
                                                                                             "Any Dimension" });
                                                                   dim    = createLineEdit(parent, layout, "Related Group of Dimensions?");
                                                                   loc    = createComboBox(parent, layout, "Location in Dimension?",
                                                                                           { "Single Location",
                                                                                             "Any Physical Location",
                                                                                             "Corresponing To Current Location"
                                                                                           });
                                                                   where  = createLineEdit(parent, layout, "Where in target dimension(s)");
                                                                   when   = createComboBox(parent, layout, "When?",
                                                                                           { "Related Group of Discrete Moments",
                                                                                             "Forward in Time",
                                                                                             "Backwards in Time",
                                                                                             "Forward and Backwards in Time"});
                                                                   moment = createLineEdit(parent, layout, "Moments?");
                                                                   span   = createComboBox(parent, layout, "How far in time?",
                                                                                           { "1 Turn", "1 Minute", "5 Minutes", "20 Minutes", "1 Hour", "6 Hours",
                                                                                             "1 Day", "1 Week", "1 Month", "1 Season", "1 Year", "5 Years",
                                                                                             "25 Years", "1 Century", "5 Centuries", "25 Centuries" });
                                                                   timLoc = createComboBox(parent, layout, "Physical Location?",
                                                                                           { "No change in location",
                                                                                             "Any Single Location",
                                                                                             "A Limited Group Of Locations",
                                                                                             "Any Physical Location"
                                                                                           });
                                                                   timWhr = createLineEdit(parent, layout, "Where?");
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points<> points(bool noStore = false) override               { if (!noStore) store();
                                                                   auto pts = 20_cp;
                                                                   if (v._time) {
                                                                       pts += 20_cp;
                                                                       switch (v._when) {
                                                                       case 0: pts += 5_cp;                           break;
                                                                       case 1:
                                                                       case 2: pts += (1_cp * (v._span + 1) + 1) / 2; break;
                                                                       case 3: pts += 1_cp * (v._span + 1);           break;
                                                                       }
                                                                       switch (v._timLoc) {
                                                                       case 0:               break;
                                                                       case 1: pts += 2_cp;  break;
                                                                       case 2: pts += 5_cp;  break;
                                                                       case 3: pts += 10_cp; break;
                                                                       }
                                                                   } else {
                                                                       switch (v._loc) {
                                                                       case 0: switch (v._numDim) {
                                                                           case 1: pts += 5_cp;  break;
                                                                           case 2: pts += 10_cp; break;
                                                                           }
                                                                           break;
                                                                       case 1: switch (v._numDim) {
                                                                           case 0: pts += 5_cp;  break;
                                                                           case 1: pts += 10_cp; break;
                                                                           case 2: pts += 15_cp; break;
                                                                           }
                                                                           break;
                                                                       case 2: switch (v._numDim) {
                                                                           case 1: pts += 2_cp;  break;
                                                                           case 2: pts += 7_cp; break;
                                                                           }
                                                                           break;
                                                                       }
                                                                   }
                                                                   return pts; }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   time->setChecked(s._time);
                                                                   numDim->setCurrentIndex(s._numDim);
                                                                   dim->setText(s._dim);
                                                                   loc->setCurrentIndex(s._loc);
                                                                   where->setText(s._where);
                                                                   when->setCurrentIndex(s._when);
                                                                   moment->setText(s._moment);
                                                                   timLoc->setCurrentIndex(s._timLoc);
                                                                   timWhr->setText(s._timWhr);
                                                                   span->setCurrentIndex(s._span);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._time   = time->isChecked();
                                                                   v._numDim = numDim->currentIndex();
                                                                   v._dim    = dim->text();
                                                                   v._loc    = loc->currentIndex();
                                                                   v._where  = where->text();
                                                                   v._when   = when->currentIndex();
                                                                   v._moment = moment->text();
                                                                   v._timLoc = timLoc->currentIndex();
                                                                   v._timWhr = timWhr->text();
                                                                   v._span   = span->currentIndex();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["time"]   = v._time;
                                                                   obj["numDim"] = v._numDim;
                                                                   obj["dim"]    = v._dim;
                                                                   obj["loc"]    = v._loc;
                                                                   obj["where"]  = v._where;
                                                                   obj["when"]   = v._when;
                                                                   obj["moment"] = v._moment;
                                                                   obj["timLoc"] = v._timLoc;
                                                                   obj["timWhr"] = v._timWhr;
                                                                   obj["span"]   = v._span;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        bool    _time   = 0;
        int     _numDim = -1;
        QString _dim    = "";
        int     _loc    = -1;
        QString _where  = "";
        int     _when   = -1;
        QString _moment = "";
        int     _span   = -1;
        int     _timLoc = -1;
        QString _timWhr = "";
    } v;

    QCheckBox* time;
    QComboBox* numDim;
    QLineEdit* dim;
    QComboBox* loc;
    QLineEdit* where;
    QComboBox* when;
    QLineEdit* moment;
    QComboBox* span;
    QComboBox* timLoc;
    QLineEdit* timWhr;

    QString optOut(bool showEND) {
        if (!v._time && v._loc != 1 && v._numDim == 0) return "<incomplete>";
        if ((!v._time && ((v._numDim < 1 && v._dim.isEmpty())
                         || (v._loc < 1 && v._where.isEmpty()))) ||
            (v._time && ((v._when < 2 && v._moment.isEmpty()) ||
                         (v._when > 1 && v._span < 1) ||
                         (v._timLoc > 0 && v._timLoc < 3 && v._timWhr.isEmpty())))) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        if (v._time) {
            QStringList span
                { "1 Turn", "1 Minute", "5 Minutes", "20 Minutes", "1 Hour", "6 Hours",
                  "1 Day", "1 Week", "1 Month", "1 Season", "1 Year", "5 Years",
                  "25 Years", "1 Century", "5 Centuries", "25 Centuries" };
            res += "ϴTime Travelϴ ";
            if (v._when < 0) res += "To Fixed Time (" + v._moment + ")";
            else if (v._when == 1) res += "To Fixed Times (" + v._moment + ")";
            else {
                if (v._when == 2) res += "Forward In Time";
                else if (v._when == 3) res += "Backward In Time";
                else res += "Forwards And Backwards In Time";
                res += " (" + span[v._span] + ")";
            }
            if (v._timLoc > 0) {
                if (v._timLoc != 3) res += "; To " + v._timWhr;
                else res += "; To Any Location";
            }
        }
        else {
            res += "Extra-Dimensional Movementϴ";
            if (v._numDim < 1) res += "Single Dimenstion (" + v._dim + ")";
            else if (v._numDim == 1) res += "; Related Group of Dimensions (" + v._dim + ")";
            else res += "; Any Dimension";
            if (v._loc > 0) {
                res += "; Any Location";
                if (v._loc == 2) res += " Corresponding to Current Location";
            } else res += "; To " + v._where;
        }
        return res;
    }

    void numeric(int) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
        PowerDialog::ref().updateForm();
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }

    void checked(bool timeTravel) override {
        // don't care about sender
        numDim->setHidden(!timeTravel);
        dim->setHidden(!timeTravel);
        loc->setHidden(!timeTravel);
        where->setHidden(!timeTravel);
        when->setHidden(timeTravel);
        moment->setHidden(timeTravel);
        span->setHidden(timeTravel);
        timLoc->setHidden(timeTravel);
        timWhr->setHidden(timeTravel);
    }
};

class FTLTravel: public AllPowers {
public:
    FTLTravel(): AllPowers("FTL Travel")                { }
    FTLTravel(const FTLTravel& s): AllPowers(s)         { }
    FTLTravel(FTLTravel&& s): AllPowers(s)              { }
    FTLTravel(const QJsonObject& json): AllPowers(json) { v._speed   = json["speed"].toInt(0);
                                                          v._instant = json["instant"].toBool(false);
                                                        }
    virtual FTLTravel& operator=(const FTLTravel& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual FTLTravel& operator=(FTLTravel&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   speed   = createLineEdit(parent, layout, "Velocity?", std::mem_fn(&Power::numeric));
                                                                   instant = createCheckBox(parent, layout, "Instant Lightspeed");
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points<> points(bool noStore = false) override               { if (!noStore) store();
                                                                   Points<> pts = (v._instant ? 20_cp : 10_cp);
                                                                   int steps = (int) (log((double) v._speed) / log(2.0));
                                                                   pts = pts + steps * 2_cp;
                                                                   return pts;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   instant->setChecked(s._instant);
                                                                   speed->setText(QString("%1").arg(s._speed));
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._instant = instant->isChecked();
                                                                   v._speed   = speed->text().toInt();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["speed"]   = v._speed;
                                                                   obj["instant"] = v._instant;
                                                                   return obj;
                                                                 }

    int move() override      { return v._speed; }
    QString units() override { return "LY/Y"; }

private:
    struct vars {
        int     _speed   = -1;
        bool    _instant = false;
    } v;

    QLineEdit* speed;
    QCheckBox* instant;

    QString optOut(bool showEND) {
        if (v._speed < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1 LY/Y FTL Travel").arg(v._speed);
        if (v._instant) res += "; Instant Lightspeed";
        return res;
    }

    void numeric(int) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
        PowerDialog::ref().updateForm();
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }
};

class Flight: public AllPowers {
public:
    Flight(): AllPowers("Flight")                    { }
    Flight(const Flight& s): AllPowers(s)            { }
    Flight(Flight&& s): AllPowers(s)                 { }
    Flight(const QJsonObject& json): AllPowers(json) { v._speed   = json["speed"].toInt(0);
                                                       v._hover   = json["hover"].toInt(0);
                                                       v._glide   = json["glide"].toBool(false);
                                                       v._surface = json["surface"].toBool(false);
                                                     }
    virtual Flight& operator=(const Flight& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Flight& operator=(Flight&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   speed   = createLineEdit(parent, layout, "Velocity?", std::mem_fn(&Power::numeric));
                                                                   hover   = createComboBox(parent, layout, "Cannot Hover?", { "", "2m/Phase", "Half-Move/Phase" });
                                                                   glide   = createCheckBox(parent, layout, "Gliding");
                                                                   surface = createCheckBox(parent, layout, "Only In Contact With Surface");
                                                                 }
    Fraction lim() override                                      { return (v._glide     ? Fraction(1)               : Fraction(0)) +
                                                                          (v._surface   ? Fraction(1, 4)            : Fraction(0)) +
                                                                          (v._hover > 0 ? v._hover * Fraction(1, 4) : Fraction(0));
                                                                 }
    Points<> points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v._speed * 1_cp;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   glide->setChecked(s._glide);
                                                                   speed->setText(QString("%1").arg(s._speed));
                                                                   surface->setChecked(s._surface);
                                                                   hover->setCurrentIndex(v._hover);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._glide   = glide->isChecked();
                                                                   v._speed   = speed->text().toInt();
                                                                   v._surface = surface->isChecked();
                                                                   v._hover   = hover->currentIndex();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["speed"]   = v._speed;
                                                                   obj["glide"]   = v._glide;
                                                                   obj["surface"] = v._surface;
                                                                   obj["hover"]   = v._hover;
                                                                   return obj;
                                                                 }

    int move() override { return v._speed; }

private:
    struct vars {
        int  _speed   = -1;
        int  _hover   = -1;
        bool _glide   = false;
        bool _surface = false;
    } v;

    QLineEdit* speed;
    QComboBox* hover;
    QCheckBox* glide;
    QCheckBox* surface;

    QString optOut(bool showEND) {
        if (v._speed < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1m Flight").arg(v._speed);
        if (v._hover == 1) res += "Cannot Hover (2m/Phase Min)";
        if (v._hover == 2) res += QString("Cannot Hover (%1m/Phase Min)").arg((v._speed + 1) / 2);
        if (v._glide) res += "; Gliding";
        if (v._surface) res += "; Only In Contact With A Surface";
        return res;
    }

    void numeric(int) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
        PowerDialog::ref().updateForm();
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }
};

class Leaping: public AllPowers {
public:
    Leaping(): AllPowers("Leaping")                   { }
    Leaping(const Leaping& s): AllPowers(s)           { }
    Leaping(Leaping&& s): AllPowers(s)                { }
    Leaping(const QJsonObject& json): AllPowers(json) { v._speed    = json["speed"].toInt(0);
                                                        v._accurate = json["accurate"].toInt(0);
                                                        v._limit    = json["limit"].toInt(0);
                                                      }
    virtual Leaping& operator=(const Leaping& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Leaping& operator=(Leaping&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return (showEND ? "" : "") + optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   speed    = createLineEdit(parent, layout, "Leaping?", std::mem_fn(&Power::numeric));
                                                                   accurate = createCheckBox(parent, layout, "Accurate Leap?");
                                                                   limit   = createComboBox(parent, layout, "Limited Movement?", { "Unlimited", "Upward Only", "Forward Only" });
                                                                 }
    Fraction lim() override                                      { return (v._limit > 0 ? Fraction(1) : Fraction(0));
                                                                 }
    Points<> points(bool noStore = false) override               { if (!noStore) store();
                                                                   return (v._speed + 1) / 2 * 1_cp + v._accurate * 5_cp;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   limit->setCurrentIndex(s._limit);
                                                                   speed->setText(QString("%1").arg(s._speed));
                                                                   accurate->setChecked(v._accurate);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._limit    = limit->currentIndex();
                                                                   v._speed    = speed->text().toInt();
                                                                   v._accurate = accurate->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["speed"]    = v._speed;
                                                                   obj["accurate"] = v._accurate;
                                                                   obj["limit"]    = v._limit;
                                                                   return obj;
                                                                 }

    int move() override { return v._speed; }

private:
    struct vars {
        int  _speed    = 0;
        bool _accurate = false;
        int  _limit    = -1;
    } v;

    QLineEdit* speed;
    QCheckBox* accurate;
    QComboBox* limit;

    QString optOut(bool showEND) {
        if (v._speed < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1m Leap").arg(v._speed);
        if (v._accurate) res += "; Accurate Leap";
        if (v._limit == 1) res += "; Upward Movement Only";
        if (v._limit == 2) res += "; Forward Movement Only";
        return res;
    }

    void numeric(int) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
        PowerDialog::ref().updateForm();
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }
};

class Running: public AllPowers {
public:
    Running(): AllPowers("Running")                   { }
    Running(const Running& s): AllPowers(s)           { }
    Running(Running&& s): AllPowers(s)                { }
    Running(const QJsonObject& json): AllPowers(json) { v._speed   = json["speed"].toInt(0);
                                                        v._correct = json["hover"].toBool(false);
                                                        v._terrain = json["terrain"].toString();
                                                      }
    virtual Running& operator=(const Running& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Running& operator=(Running&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   speed   = createLineEdit(parent, layout, "Velocity?", std::mem_fn(&Power::numeric));
                                                                   correct = createCheckBox(parent, layout, "Only On Appropriate Terrain?");
                                                                   terrain = createLineEdit(parent, layout, "Terrain?");
                                                                 }
    Fraction lim() override                                      { return (v._correct ? Fraction(1, 2) : Fraction(0));
                                                                 }
    Points<> points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v._speed * 1_cp;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   speed->setText(QString("%1").arg(s._speed));
                                                                   correct->setChecked(v._correct);
                                                                   terrain->setText(v._terrain);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._speed   = speed->text().toInt();
                                                                   v._correct = correct->isChecked();
                                                                   v._terrain = terrain->text();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["speed"]   = v._speed;
                                                                   obj["correct"] = v._correct;
                                                                   obj["terrain"] = v._terrain;
                                                                   return obj;
                                                                 }

    int move() override { return v._speed; }

private:
    struct vars {
        int     _speed   = 0;
        bool    _correct = false;
        QString _terrain = "";
    } v;

    QLineEdit* speed;
    QCheckBox* correct;
    QLineEdit* terrain;

    QString optOut(bool showEND) {
        if (v._speed < 1 || (v._correct && v._terrain.isEmpty())) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1m Running").arg(v._speed);
        if (v._correct) res += "; Only On Appropriate Terrain (" + v._terrain + ")";
        return res;
    }

    void numeric(int) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
        PowerDialog::ref().updateForm();
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }
};

class Swimming: public AllPowers {
public:
    Swimming(): AllPowers("Swimming")                  { }
    Swimming(const Swimming& s): AllPowers(s)          { }
    Swimming(Swimming&& s): AllPowers(s)               { }
    Swimming(const QJsonObject& json): AllPowers(json) { v._speed   = json["speed"].toInt(0);
                                                         v._surface = json["surface"].toBool(false);
                                                       }
    virtual Swimming& operator=(const Swimming& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Swimming& operator=(Swimming&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   speed   = createLineEdit(parent, layout, "Velocity?", std::mem_fn(&Power::numeric));
                                                                   surface = createCheckBox(parent, layout, "Surface Only?");
                                                                 }
    Fraction lim() override                                      { return (v._surface ? Fraction(1, 2) : Fraction(0));
                                                                 }
    Points<> points(bool noStore = false) override               { if (!noStore) store();
                                                                   return (v._speed + 1) / 2 * 1_cp;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   speed->setText(QString("%1").arg(s._speed));
                                                                   surface->setChecked(v._surface);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._speed   = speed->text().toInt();
                                                                   v._surface = surface->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["speed"]   = v._speed;
                                                                   obj["surface"] = v._surface;
                                                                   return obj;
                                                                 }

    int move() override { return v._speed; }

private:
    struct vars {
        int     _speed   = 0;
        bool    _surface = false;
    } v;

    QLineEdit* speed;
    QCheckBox* surface;

    QString optOut(bool showEND) {
        if (v._speed < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1m Swimming").arg(v._speed);
        if (v._surface) res += "; Surface Only";
        return res;
    }

    void numeric(int) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
        PowerDialog::ref().updateForm();
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }
};

class Swinging: public AllPowers {
public:
    Swinging(): AllPowers("Swinging")                  { }
    Swinging(const Swinging& s): AllPowers(s)          { }
    Swinging(Swinging&& s): AllPowers(s)               { }
    Swinging(const QJsonObject& json): AllPowers(json) { v._speed   = json["speed"].toInt(0);
                                                            }
    virtual Swinging& operator=(const Swinging& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Swinging& operator=(Swinging&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   speed   = createLineEdit(parent, layout, "Velocity?", std::mem_fn(&Power::numeric));
                                                                 }
    Fraction lim() override                                      { return Fraction(0);
                                                                 }
    Points<> points(bool noStore = false) override               { if (!noStore) store();
                                                                   return (v._speed + 1) / 2 * 1_cp;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   speed->setText(QString("%1").arg(s._speed));
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._speed   = speed->text().toInt();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["speed"]   = v._speed;
                                                                   return obj;
                                                                 }

    int move() override { return v._speed; }

private:
    struct vars {
        int     _speed   = 0;
    } v;

    QLineEdit* speed;

    QString optOut(bool showEND) {
        if (v._speed < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1m Swinging").arg(v._speed);
        return res;
    }

    void numeric(int) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
        PowerDialog::ref().updateForm();
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }
};

class Teleportation: public AllPowers {
public:
    Teleportation(): AllPowers("Teleportation")             { }
    Teleportation(const Teleportation& s): AllPowers(s)     { }
    Teleportation(Teleportation&& s): AllPowers(s)          { }
    Teleportation(const QJsonObject& json): AllPowers(json) { v._speed = json["speed"].toInt(0);
                                                              v._veloc = json["veloc"].toBool(false);
                                                              v._safe  = json["safe"].toBool(false);
                                                              v._fixed = json["fixed"].toInt(0);
                                                              v._pass  = json["pass"].toBool(false);
                                                            }
    virtual Teleportation& operator=(const Teleportation& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Teleportation& operator=(Teleportation&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   speed = createLineEdit(parent, layout, "Distance?", std::mem_fn(&Power::numeric));
                                                                   veloc = createCheckBox(parent, layout, "No Relative Velocity");
                                                                   safe  = createCheckBox(parent, layout, "Safe Aquatic Teleport");
                                                                   fixed = createComboBox(parent, layout, "Only To?", { "Anywhere",
                                                                                                                        "Fixed Or Floating Locations",
                                                                                                                        "Fixed Locations" });
                                                                   pass  = createCheckBox(parent, layout, "Must Pass Through Intervening Space");
                                                                 }
    Fraction lim() override                                      { return ((v._fixed > -1) ? v._fixed * Fraction(1, 2) : Fraction(0)) + (v._pass ? Fraction(1, 4) : Fraction(0));
                                                                 }
    Points<> points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v._speed * 1_cp + (v._veloc ? 10_cp : 0_cp) + (v._safe ? 5_cp : 0_cp);
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   speed->setText(QString("%1").arg(s._speed));
                                                                   veloc->setChecked(v._veloc);
                                                                   safe->setChecked(v._safe);
                                                                   fixed->setCurrentIndex(v._fixed);
                                                                   pass->setChecked(v._pass);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._speed = speed->text().toInt();
                                                                   v._veloc = veloc->isChecked();
                                                                   v._safe  = safe->isChecked();
                                                                   v._fixed = fixed->currentIndex();
                                                                   v._pass  = pass->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["speed"] = v._speed;
                                                                   obj["veloc"] = v._veloc;
                                                                   obj["safe"]  = v._safe;
                                                                   obj["fixed"] = v._fixed;
                                                                   obj["pass"]  = v._pass;
                                                                   return obj;
                                                                 }

    int move() override { return v._speed; }

private:
    struct vars {
        int  _speed = 0;
        bool _veloc = false;
        bool _safe  = false;
        int  _fixed = -1;
        bool _pass  = false;
    } v;

    QLineEdit* speed;
    QCheckBox* veloc;
    QCheckBox* safe;
    QComboBox* fixed;
    QCheckBox* pass;

    QString optOut(bool showEND) {
        if (v._speed < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1m Telportation").arg(v._speed);
        if (v._veloc) res += "; No Relative Velocity";
        if (v._safe)  res += "; Safe Aquatic Teleport";
        if (v._fixed == 1) res += "; Can Only Teleport To Fixed Or Floating Locations";
        if (v._fixed == 2) res += "; Can Only Teleport To Fixed Locations";
        if (v._pass) res += "; Must Pass Through Intervening Space";
        return res;
    }

    void numeric(int) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
        PowerDialog::ref().updateForm();
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }
};

class Tunneling: public AllPowers {
public:
    Tunneling(): AllPowers("Tunneling")                 { }
    Tunneling(const Tunneling& s): AllPowers(s)         { }
    Tunneling(Tunneling&& s): AllPowers(s)              { }
    Tunneling(const QJsonObject& json): AllPowers(json) { v._speed  = json["speed"].toInt(0);
                                                               v._pd     = json["pd"].toInt(0);
                                                               v._fillin = json["fillin"].toBool(false);
                                                               v._limit  = json["limit"].toInt(0);
                                                               v._to     = json["to"].toString();
                                                             }
    virtual Tunneling& operator=(const Tunneling& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Tunneling& operator=(Tunneling&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   speed  = createLineEdit(parent, layout, "Distance?", std::mem_fn(&Power::numeric));
                                                                   pd     = createLineEdit(parent, layout, "PD?", std::mem_fn(&Power::numeric));
                                                                   fillin = createCheckBox(parent, layout, "Fill In");
                                                                   limit  = createComboBox(parent, layout, "Only Through?", { "Unlimited",
                                                                                                                              "Limited Medium",
                                                                                                                              "Very Limited Medium" });
                                                                   to     = createCheckBox(parent, layout, "Limited to?");
                                                                 }
    Fraction lim() override                                      { return (v._limit != -1) ? v._limit * Fraction(1, 2) : Fraction(0);
                                                                 }
    Points<> points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v._speed * 1_cp + v._pd * 2_cp + (v._fillin ? 10_cp : 0_cp);
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   speed->setText(QString("%1").arg(s._speed));
                                                                   pd->setText(QString("%1").arg(s._pd));
                                                                   fillin->setChecked(s._fillin);
                                                                   limit->setCurrentIndex(s._limit);
                                                                   to->setText(s._to);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._speed = speed->text().toInt();
                                                                   v._pd    = pd->text().toInt();
                                                                   v._fillin = fillin->isChecked();
                                                                   v._limit = limit->currentIndex();
                                                                   v._to  = to->text();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["speed"]  = v._speed;
                                                                   obj["pd"]     = v._pd;
                                                                   obj["fillin"] = v._fillin;
                                                                   obj["limit"]  = v._limit;
                                                                   obj["to"]     = v._to;
                                                                   return obj;
                                                                 }

    int move() override { return v._speed; }

private:
    struct vars {
        int     _speed  = 0;
        int     _pd     = 0;
        bool    _fillin = false;
        int     _limit  = -1;
        QString _to     = "";
    } v;

    QLineEdit* speed;
    QLineEdit* pd;
    QCheckBox* fillin;
    QComboBox* limit;
    QCheckBox* to;

    QString optOut(bool showEND) {
        if (v._speed < 1 || v._pd < 1 || (v._limit > 0 && v._to.isEmpty())) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1m Tunneling (%2 PD").arg(v._speed).arg(v._pd);
        if (v._fillin) res += "; Fill In";
        if (v._limit == 1) res += "; Limited Medium (" + v._to + ")";
        if (v._limit == 2) res += "; Very Limited Medium (" + v._to + ")";
        return res;
    }

    void numeric(int) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
        PowerDialog::ref().updateForm();
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }
};

#endif // AllPowers_H
