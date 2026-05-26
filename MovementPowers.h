#ifndef MOVEMENT_POWERS_H
#define MOVEMENT_POWERS_H

#include "powers.h"
#include "powerdialog.h"

class ExtraDimensionalMovement: public AllPowers {
public:
    ExtraDimensionalMovement(): AllPowers("Extra-Dimensional Movementϴ")      { }
    ExtraDimensionalMovement(const ExtraDimensionalMovement& s): AllPowers(s) { }
    ExtraDimensionalMovement(ExtraDimensionalMovement&& s): AllPowers(s)      { }
    ExtraDimensionalMovement(const QJsonObject& json)
        : AllPowers(json) {
        v.mTime = json["time"].toBool(false);
        v.mNumDim = json["numDim"].toInt(0);
        v.mDim = json["dim"].toString();
        v.mLoc = json["loc"].toInt(0);
        v.mWhere = json["where"].toString();
        v.mWhen = json["when"].toInt(0);
        v.mMoment = json["moment"].toString();
        v.mSpan = json["span"].toInt(0);
        v.mTimLoc = json["timLoc"].toInt(0);
        v.mTimWhr = json["timWhr"].toString();
    }
    ~ExtraDimensionalMovement() override { }

    ExtraDimensionalMovement& operator=(const ExtraDimensionalMovement&) = delete;
    ExtraDimensionalMovement& operator=(ExtraDimensionalMovement&&) = delete;

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
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   auto pts = 20_cp; // NOLINT
                                                                   if (v.mTime) {
                                                                       pts += 20_cp; // NOLINT
                                                                       switch (v.mWhen) {
                                                                       case 0: pts += 5_cp;                           break; // NOLINT
                                                                       case 1:
                                                                       case 2: pts += (1_cp * (v.mSpan + 1) + 1) / 2; break;
                                                                       case 3: pts += 1_cp * (v.mSpan + 1);           break;
                                                                       }
                                                                       switch (v.mTimLoc) {
                                                                       case 0:               break;
                                                                       case 1: pts += 2_cp;  break;
                                                                       case 2: pts += 5_cp;  break; // NOLINT
                                                                       case 3: pts += 10_cp; break; // NOLINT
                                                                       }
                                                                   } else {
                                                                       switch (v.mLoc) {
                                                                       case 0: switch (v.mNumDim) {
                                                                           case 1: pts += 5_cp;  break; // NOLINT
                                                                           case 2: pts += 10_cp; break; // NOLINT
                                                                           }
                                                                           break;
                                                                       case 1: switch (v.mNumDim) {
                                                                           case 0: pts += 5_cp;  break; // NOLINT
                                                                           case 1: pts += 10_cp; break; // NOLINT
                                                                           case 2: pts += 15_cp; break; // NOLINT
                                                                           }
                                                                           break;
                                                                       case 2: switch (v.mNumDim) {
                                                                           case 1: pts += 2_cp;  break;
                                                                           case 2: pts += 7_cp; break; // NOLINT
                                                                           }
                                                                           break;
                                                                       }
                                                                   }
                                                                   return pts; }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   time->setChecked(s.mTime);
                                                                   numDim->setCurrentIndex(s.mNumDim);
                                                                   dim->setText(s.mDim);
                                                                   loc->setCurrentIndex(s.mLoc);
                                                                   where->setText(s.mWhere);
                                                                   when->setCurrentIndex(s.mWhen);
                                                                   moment->setText(s.mMoment);
                                                                   timLoc->setCurrentIndex(s.mTimLoc);
                                                                   timWhr->setText(s.mTimWhr);
                                                                   span->setCurrentIndex(s.mSpan);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mTime   = time->isChecked();
                                                                   v.mNumDim = numDim->currentIndex();
                                                                   v.mDim    = dim->text();
                                                                   v.mLoc    = loc->currentIndex();
                                                                   v.mWhere  = where->text();
                                                                   v.mWhen   = when->currentIndex();
                                                                   v.mMoment = moment->text();
                                                                   v.mTimLoc = timLoc->currentIndex();
                                                                   v.mTimWhr = timWhr->text();
                                                                   v.mSpan   = span->currentIndex();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["time"]   = v.mTime;
                                                                   obj["numDim"] = v.mNumDim;
                                                                   obj["dim"]    = v.mDim;
                                                                   obj["loc"]    = v.mLoc;
                                                                   obj["where"]  = v.mWhere;
                                                                   obj["when"]   = v.mWhen;
                                                                   obj["moment"] = v.mMoment;
                                                                   obj["timLoc"] = v.mTimLoc;
                                                                   obj["timWhr"] = v.mTimWhr;
                                                                   obj["span"]   = v.mSpan;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        bool    mTime   = 0;
        int     mNumDim = -1;
        QString mDim    = "";
        int     mLoc    = -1;
        QString mWhere  = "";
        int     mWhen   = -1;
        QString mMoment = "";
        int     mSpan   = -1;
        int     mTimLoc = -1;
        QString mTimWhr = "";
    } v;

    QCheckBox* time = nullptr;
    QComboBox* numDim = nullptr;
    QLineEdit* dim = nullptr;
    QComboBox* loc = nullptr;
    QLineEdit* where = nullptr;
    QComboBox* when = nullptr;
    QLineEdit* moment = nullptr;
    QComboBox* span = nullptr;
    QComboBox* timLoc = nullptr;
    QLineEdit* timWhr = nullptr;

    QString optOut(bool showEND) {
        if (!v.mTime && v.mLoc != 1 && v.mNumDim == 0) return "<incomplete>";
        if ((!v.mTime && ((v.mNumDim < 1 && v.mDim.isEmpty())
                         || (v.mLoc < 1 && v.mWhere.isEmpty()))) ||
            (v.mTime && ((v.mWhen < 2 && v.mMoment.isEmpty()) ||
                         (v.mWhen > 1 && v.mSpan < 1) ||
                         (v.mTimLoc > 0 && v.mTimLoc < 3 && v.mTimWhr.isEmpty())))) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        if (v.mTime) {
            QStringList span
                { "1 Turn", "1 Minute", "5 Minutes", "20 Minutes", "1 Hour", "6 Hours",
                  "1 Day", "1 Week", "1 Month", "1 Season", "1 Year", "5 Years",
                  "25 Years", "1 Century", "5 Centuries", "25 Centuries" };
            res += "ϴTime Travelϴ ";
            if (v.mWhen < 0) res += "To Fixed Time (" + v.mMoment + ")";
            else if (v.mWhen == 1) res += "To Fixed Times (" + v.mMoment + ")";
            else {
                if (v.mWhen == 2) res += "Forward In Time";
                else if (v.mWhen == 3) res += "Backward In Time";
                else res += "Forwards And Backwards In Time";
                res += " (" + span[v.mSpan] + ")";
            }
            if (v.mTimLoc > 0) {
                if (v.mTimLoc != 3) res += "; To " + v.mTimWhr;
                else res += "; To Any Location";
            }
        }
        else {
            res += "Extra-Dimensional Movementϴ";
            if (v.mNumDim < 1) res += "Single Dimenstion (" + v.mDim + ")";
            else if (v.mNumDim == 1) res += "; Related Group of Dimensions (" + v.mDim + ")";
            else res += "; Any Dimension";
            if (v.mLoc > 0) {
                res += "; Any Location";
                if (v.mLoc == 2) res += " Corresponding to Current Location";
            } else res += "; To " + v.mWhere;
        }
        return res;
    }

    void numeric(int) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
#ifndef ISHSC
        PowerDialog::ref().updateForm();
#endif
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
    FTLTravel(const QJsonObject& json): AllPowers(json) { v.mSpeed   = json["speed"].toInt(0);
                                                          v.mInstant = json["instant"].toBool(false);
                                                        }
    ~FTLTravel() override { }

    FTLTravel& operator=(const FTLTravel&) = delete;
    FTLTravel& operator=(FTLTravel&&) = delete;

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   speed   = createLineEdit(parent, layout, "Velocity?", std::mem_fn(&Power::numeric));
                                                                   instant = createCheckBox(parent, layout, "Instant Lightspeed");
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   Points pts = (v.mInstant ? 20_cp : 10_cp); // NOLINT
                                                                   int steps = (int) (log((double) v.mSpeed) / log(2.0)); // NOLINT
                                                                   pts = pts + steps * 2_cp;
                                                                   return pts;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   instant->setChecked(s.mInstant);
                                                                   speed->setText(QString("%1").arg(s.mSpeed));
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mInstant = instant->isChecked();
                                                                   v.mSpeed   = speed->text().toInt();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["speed"]   = v.mSpeed;
                                                                   obj["instant"] = v.mInstant;
                                                                   return obj;
                                                                 }

    int move() override      { return v.mSpeed; }
    QString units() override { return "LY/Y"; }

private:
    struct vars {
        int     mSpeed   = -1;
        bool    mInstant = false;
    } v;

    QLineEdit* speed = nullptr;
    QCheckBox* instant = nullptr;

    QString optOut(bool showEND) {
        if (v.mSpeed < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1 LY/Y FTL Travel").arg(v.mSpeed);
        if (v.mInstant) res += "; Instant Lightspeed";
        return res;
    }

    void numeric(int) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
#ifndef ISHSC
        PowerDialog::ref().updateForm();
#endif
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }
};

class Flight: public AllPowers {
public:
    Flight(): AllPowers("Flight")                    { }
    Flight(const Flight& s): AllPowers(s)            { }
    Flight(Flight&& s): AllPowers(s)                 { }
    Flight(const QJsonObject& json): AllPowers(json) { v.mSpeed   = json["speed"].toInt(0);
                                                       v.mHover   = json["hover"].toInt(0);
                                                       v.mGlide   = json["glide"].toBool(false);
                                                       v.mSurface = json["surface"].toBool(false);
                                                     }
    ~Flight() override { }
    virtual Flight& operator=(const Flight&) = delete;
    virtual Flight& operator=(Flight&&) = delete;

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return v.mGlide ? noEnd() : Power::end(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   speed   = createLineEdit(parent, layout, "Velocity?", std::mem_fn(&Power::numeric));
                                                                   hover   = createComboBox(parent, layout, "Cannot Hover?", { "", "2m/Phase", "Half-Move/Phase" });
                                                                   glide   = createCheckBox(parent, layout, "Gliding");
                                                                   surface = createCheckBox(parent, layout, "Only In Contact With Surface");
                                                                 }
    Fraction lim() override                                      { return (v.mGlide     ? Fraction(1)               : Fraction(0)) +
                                                                          (v.mSurface   ? Fraction(1, 4)            : Fraction(0)) +
                                                                          (v.mHover > 0 ? v.mHover * Fraction(1, 4) : Fraction(0));
                                                                 }
    Points points(bool noStore = false) override                 { if (!noStore) store();
                                                                   return v.mSpeed * 1_cp;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   glide->setChecked(s.mGlide);
                                                                   speed->setText(QString("%1").arg(s.mSpeed));
                                                                   surface->setChecked(s.mSurface);
                                                                   hover->setCurrentIndex(v.mHover);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mGlide   = glide->isChecked();
                                                                   v.mSpeed   = speed->text().toInt();
                                                                   v.mSurface = surface->isChecked();
                                                                   v.mHover   = hover->currentIndex();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["speed"]   = v.mSpeed;
                                                                   obj["glide"]   = v.mGlide;
                                                                   obj["surface"] = v.mSurface;
                                                                   obj["hover"]   = v.mHover;
                                                                   return obj;
                                                                 }

    int move() override { return v.mSpeed; }

private:
    struct vars {
        int  mSpeed   = -1;
        int  mHover   = -1;
        bool mGlide   = false;
        bool mSurface = false;
    } v;

    QLineEdit* speed = nullptr;
    QComboBox* hover = nullptr;
    QCheckBox* glide = nullptr;
    QCheckBox* surface = nullptr;

    QString optOut(bool showEND) {
        if (v.mSpeed < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1m Flight").arg(v.mSpeed);
        if (v.mHover == 1) res += "Cannot Hover (2m/Phase Min)";
        if (v.mHover == 2) res += QString("Cannot Hover (%1m/Phase Min)").arg((v.mSpeed + 1) / 2);
        if (v.mGlide) res += "; Gliding";
        if (v.mSurface) res += "; Only In Contact With A Surface";
        return res;
    }

    void numeric(int) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
#ifndef ISHSC
        PowerDialog::ref().updateForm();
#endif
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }
};

class Leaping: public AllPowers {
public:
    Leaping(): AllPowers("Leaping")                   { }
    Leaping(const Leaping& s): AllPowers(s)           { }
    Leaping(Leaping&& s): AllPowers(s)                { }
    Leaping(const QJsonObject& json): AllPowers(json) { v.mSpeed    = json["speed"].toInt(0);
                                                        v.mAccurate = json["accurate"].toBool(false);
                                                        v.mLimit = json["limit"].toInt(0);
                                                      }
    ~Leaping() override { }

    Leaping& operator=(const Leaping& s) = delete;
    Leaping& operator=(Leaping&& s) = delete;

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return (showEND ? "" : "") + optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   speed    = createLineEdit(parent, layout, "Leaping?", std::mem_fn(&Power::numeric));
                                                                   accurate = createCheckBox(parent, layout, "Accurate Leap?");
                                                                   limit   = createComboBox(parent, layout, "Limited Movement?", { "Unlimited", "Upward Only", "Forward Only" });
                                                                 }
    Fraction lim() override                                      { return (v.mLimit > 0 ? Fraction(1) : Fraction(0));
                                                                 }
    Points points(bool noStore = false) override                 { if (!noStore) store();
                                                                   return (v.mSpeed + 1) / 2 * 1_cp + v.mAccurate * 5_cp; // NOLINT
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   limit->setCurrentIndex(s.mLimit);
                                                                   speed->setText(QString("%1").arg(s.mSpeed));
                                                                   accurate->setChecked(v.mAccurate);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mLimit    = limit->currentIndex();
                                                                   v.mSpeed    = speed->text().toInt();
                                                                   v.mAccurate = accurate->isChecked();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["speed"]    = v.mSpeed;
                                                                   obj["accurate"] = v.mAccurate;
                                                                   obj["limit"]    = v.mLimit;
                                                                   return obj;
                                                                 }

    int move() override { return v.mSpeed; }

private:
    struct vars {
        int  mSpeed    = 0;
        bool mAccurate = false;
        int  mLimit    = -1;
    } v;

    QLineEdit* speed = nullptr;
    QCheckBox* accurate = nullptr;
    QComboBox* limit = nullptr;

    QString optOut(bool showEND) {
        if (v.mSpeed < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("+%1m Leap").arg(v.mSpeed);
        if (v.mAccurate) res += "; Accurate Leap";
        if (v.mLimit == 1) res += "; Upward Movement Only";
        if (v.mLimit == 2) res += "; Forward Movement Only";
        return res;
    }

    void numeric(int) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
#ifndef ISHSC
        PowerDialog::ref().updateForm();
#endif
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }
};

class Running: public AllPowers {
public:
    Running(): AllPowers("Running")                   { }
    Running(const Running& s): AllPowers(s)           { }
    Running(Running&& s): AllPowers(s)                { }
    Running(const QJsonObject& json): AllPowers(json) { v.mSpeed   = json["speed"].toInt(0);
                                                        v.mCorrect = json["hover"].toBool(false);
                                                        v.mTerrain = json["terrain"].toString();
    }
    ~Running() override { }

    Running& operator=(const Running& s) = delete;
    Running& operator=(Running&& s) = delete;

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   speed   = createLineEdit(parent, layout, "Velocity?", std::mem_fn(&Power::numeric));
                                                                   correct = createCheckBox(parent, layout, "Only On Appropriate Terrain?");
                                                                   terrain = createLineEdit(parent, layout, "Terrain?");
                                                                 }
    Fraction lim() override                                      { return (v.mCorrect ? Fraction(1, 2) : Fraction(0));
                                                                 }
    Points points(bool noStore = false) override                 { if (!noStore) store();
                                                                   return v.mSpeed * 1_cp;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   speed->setText(QString("%1").arg(s.mSpeed));
                                                                   correct->setChecked(v.mCorrect);
                                                                   terrain->setText(v.mTerrain);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mSpeed   = speed->text().toInt();
                                                                   v.mCorrect = correct->isChecked();
                                                                   v.mTerrain = terrain->text();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["speed"]   = v.mSpeed;
                                                                   obj["correct"] = v.mCorrect;
                                                                   obj["terrain"] = v.mTerrain;
                                                                   return obj;
                                                                 }

    int move() override { return v.mSpeed; }

private:
    struct vars {
        int     mSpeed   = 0;
        bool    mCorrect = false;
        QString mTerrain = "";
    } v;

    QLineEdit* speed = nullptr;
    QCheckBox* correct = nullptr;
    QLineEdit* terrain = nullptr;

    QString optOut(bool showEND) {
        if (v.mSpeed < 1 || (v.mCorrect && v.mTerrain.isEmpty())) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("+%1m Running").arg(v.mSpeed);
        if (v.mCorrect) res += "; Only On Appropriate Terrain (" + v.mTerrain + ")";
        return res;
    }

    void numeric(int) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
#ifndef ISHSC
        PowerDialog::ref().updateForm();
#endif
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }
};

class Swimming: public AllPowers {
public:
    Swimming(): AllPowers("Swimming")                  { }
    Swimming(const Swimming& s): AllPowers(s)          { }
    Swimming(Swimming&& s): AllPowers(s)               { }
    Swimming(const QJsonObject& json)
        : AllPowers(json) {
        v.mSpeed = json["speed"].toInt(0);
        v.mSurface = json["surface"].toBool(false);
    }
    ~Swimming() override { }

    Swimming& operator=(const Swimming& s) = delete;
    Swimming& operator=(Swimming&& s) = delete;

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   speed   = createLineEdit(parent, layout, "Velocity?", std::mem_fn(&Power::numeric));
                                                                   surface = createCheckBox(parent, layout, "Surface Only?");
                                                                 }
    Fraction lim() override                                      { return (v.mSurface ? Fraction(1, 2) : Fraction(0));
                                                                 }
    Points points(bool noStore = false) override                 { if (!noStore) store();
                                                                   return (v.mSpeed + 1) / 2 * 1_cp;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   speed->setText(QString("%1").arg(s.mSpeed));
                                                                   surface->setChecked(v.mSurface);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mSpeed   = speed->text().toInt();
                                                                   v.mSurface = surface->isChecked();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["speed"]   = v.mSpeed;
                                                                   obj["surface"] = v.mSurface;
                                                                   return obj;
                                                                 }

    int move() override { return v.mSpeed; }

private:
    struct vars {
        int     mSpeed   = 0;
        bool    mSurface = false;
    } v;

    QLineEdit* speed = nullptr;
    QCheckBox* surface = nullptr;

    QString optOut(bool showEND) {
        if (v.mSpeed < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("+%1m Swimming").arg(v.mSpeed);
        if (v.mSurface) res += "; Surface Only";
        return res;
    }

    void numeric(int) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
#ifndef ISHSC
        PowerDialog::ref().updateForm();
#endif
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }
};

class Swinging: public AllPowers {
public:
    Swinging(): AllPowers("Swinging")                  { }
    Swinging(const Swinging& s): AllPowers(s)          { }
    Swinging(Swinging&& s): AllPowers(s)               { }
    Swinging(const QJsonObject& json)
        : AllPowers(json) {
        v.mSpeed = json["speed"].toInt(0);
    }
    ~Swinging() override { }

    Swinging& operator=(const Swinging& s) = delete;
    Swinging& operator=(Swinging&& s) = delete;

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   speed   = createLineEdit(parent, layout, "Velocity?", std::mem_fn(&Power::numeric));
                                                                 }
    Fraction lim() override                                      { return Fraction(0);
                                                                 }
    Points points(bool noStore = false) override                 { if (!noStore) store();
                                                                   return (v.mSpeed + 1) / 2 * 1_cp;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   speed->setText(QString("%1").arg(s.mSpeed));
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mSpeed   = speed->text().toInt();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["speed"]   = v.mSpeed;
                                                                   return obj;
                                                                 }

    int move() override { return v.mSpeed; }

private:
    struct vars {
        int     mSpeed   = 0;
    } v;

    QLineEdit* speed = nullptr;

    QString optOut(bool showEND) {
        if (v.mSpeed < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1m Swinging").arg(v.mSpeed);
        return res;
    }

    void numeric(int) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
#ifndef ISHSC
        PowerDialog::ref().updateForm();
#endif
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }
};

class Teleportation: public AllPowers {
public:
    Teleportation(): AllPowers("Teleportation")             { }
    Teleportation(const Teleportation& s): AllPowers(s)     { }
    Teleportation(Teleportation&& s): AllPowers(s)          { }
    Teleportation(const QJsonObject& json): AllPowers(json) { v.mSpeed = json["speed"].toInt(0);
                                                              v.mVeloc = json["veloc"].toBool(false);
                                                              v.mSafe  = json["safe"].toBool(false);
                                                              v.mFixed = json["fixed"].toInt(0);
                                                              v.mPass = json["pass"].toBool(false);
    }
    ~Teleportation() override { }
    Teleportation& operator=(const Teleportation& s) = delete;
    Teleportation& operator=(Teleportation&& s) = delete;

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
    Fraction lim() override                                      { return ((v.mFixed > -1) ? v.mFixed * Fraction(1, 2) : Fraction(0)) + (v.mPass ? Fraction(1, 4) : Fraction(0));
                                                                 }
    Points points(bool noStore = false) override                 { if (!noStore) store();
                                                                   return v.mSpeed * 1_cp + (v.mVeloc ? 10_cp : 0_cp) + (v.mSafe ? 5_cp : 0_cp); // NOLINT
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   speed->setText(QString("%1").arg(s.mSpeed));
                                                                   veloc->setChecked(v.mVeloc);
                                                                   safe->setChecked(v.mSafe);
                                                                   fixed->setCurrentIndex(v.mFixed);
                                                                   pass->setChecked(v.mPass);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mSpeed = speed->text().toInt();
                                                                   v.mVeloc = veloc->isChecked();
                                                                   v.mSafe  = safe->isChecked();
                                                                   v.mFixed = fixed->currentIndex();
                                                                   v.mPass  = pass->isChecked();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["speed"] = v.mSpeed;
                                                                   obj["veloc"] = v.mVeloc;
                                                                   obj["safe"]  = v.mSafe;
                                                                   obj["fixed"] = v.mFixed;
                                                                   obj["pass"]  = v.mPass;
                                                                   return obj;
                                                                 }

    int move() override { return v.mSpeed; }

private:
    struct vars {
        int  mSpeed = 0;
        bool mVeloc = false;
        bool mSafe  = false;
        int  mFixed = -1;
        bool mPass  = false;
    } v;

    QLineEdit* speed = nullptr;
    QCheckBox* veloc = nullptr;
    QCheckBox* safe = nullptr;
    QComboBox* fixed = nullptr;
    QCheckBox* pass = nullptr;

    QString optOut(bool showEND) {
        if (v.mSpeed < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1m Telportation").arg(v.mSpeed);
        if (v.mVeloc) res += "; No Relative Velocity";
        if (v.mSafe)  res += "; Safe Aquatic Teleport";
        if (v.mFixed == 1) res += "; Can Only Teleport To Fixed Or Floating Locations";
        if (v.mFixed == 2) res += "; Can Only Teleport To Fixed Locations";
        if (v.mPass) res += "; Must Pass Through Intervening Space";
        return res;
    }

    void numeric(int) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
#ifndef ISHSC
        PowerDialog::ref().updateForm();
#endif
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }
};

class Tunneling: public AllPowers {
public:
    Tunneling(): AllPowers("Tunneling")                 { }
    Tunneling(const Tunneling& s): AllPowers(s)         { }
    Tunneling(Tunneling&& s): AllPowers(s)              { }
    Tunneling(const QJsonObject& json): AllPowers(json) { v.mSpeed  = json["speed"].toInt(0);
                                                          v.mPD     = json["pd"].toInt(0);
                                                          v.mFillIn = json["fillin"].toBool(false);
                                                          v.mLimit  = json["limit"].toInt(0);
                                                          v.mTo = json["to"].toString();
                                                        }
    ~Tunneling() override { }

    Tunneling& operator=(const Tunneling& s) = delete;
    Tunneling& operator=(Tunneling&& s) = delete;

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
    Fraction lim() override                                      { return (v.mLimit != -1) ? v.mLimit * Fraction(1, 2) : Fraction(0);
                                                                 }
    Points points(bool noStore = false) override                 { if (!noStore) store();
                                                                   return v.mSpeed * 1_cp + v.mPD * 2_cp + (v.mFillIn ? 10_cp : 0_cp); // NOLINT
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   speed->setText(QString("%1").arg(s.mSpeed));
                                                                   pd->setText(QString("%1").arg(s.mPD));
                                                                   fillin->setChecked(s.mFillIn);
                                                                   limit->setCurrentIndex(s.mLimit);
                                                                   to->setText(s.mTo);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mSpeed = speed->text().toInt();
                                                                   v.mPD    = pd->text().toInt();
                                                                   v.mFillIn = fillin->isChecked();
                                                                   v.mLimit = limit->currentIndex();
                                                                   v.mTo  = to->text();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["speed"]  = v.mSpeed;
                                                                   obj["pd"]     = v.mPD;
                                                                   obj["fillin"] = v.mFillIn;
                                                                   obj["limit"]  = v.mLimit;
                                                                   obj["to"]     = v.mTo;
                                                                   return obj;
                                                                 }

    int move() override { return v.mSpeed; }

private:
    struct vars {
        int     mSpeed  = 0;
        int     mPD     = 0;
        bool    mFillIn = false;
        int     mLimit  = -1;
        QString mTo     = "";
    } v;

    QLineEdit* speed = nullptr;
    QLineEdit* pd = nullptr;
    QCheckBox* fillin = nullptr;
    QComboBox* limit = nullptr;
    QCheckBox* to = nullptr;

    QString optOut(bool showEND) {
        if (v.mSpeed < 1 || v.mPD < 1 || (v.mLimit > 0 && v.mTo.isEmpty())) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1m Tunneling (%2 PD").arg(v.mSpeed).arg(v.mPD);
        if (v.mFillIn) res += "; Fill In";
        if (v.mLimit == 1) res += "; Limited Medium (" + v.mTo + ")";
        if (v.mLimit == 2) res += "; Very Limited Medium (" + v.mTo + ")";
        return res;
    }

    void numeric(int) override {
        QLineEdit* edit = dynamic_cast<QLineEdit*>(sender());
        QString txt = edit->text();
#ifndef ISHSC
        PowerDialog::ref().updateForm();
#endif
        if (txt.isEmpty() || isNumber(txt)) return;
        edit->undo();
    }
};

#endif // AllPowers_H
