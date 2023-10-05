#ifndef MENTAL_POWERS_H
#define MENTAL_POWERS_H

#include "powers.h"
#include "powerdialog.h"

class MentalBlast: public AllPowers {
public:
    MentalBlast(): AllPowers("Mental Blast")              { }
    MentalBlast(const MentalBlast& s): AllPowers(s)       { }
    MentalBlast(MentalBlast&& s): AllPowers(s)            { }
    MentalBlast(const QJsonObject& json): AllPowers(json) { v._dice = json["dice"].toInt(0);
                                                          }
    ~MentalBlast() override { }
    MentalBlast& operator=(const MentalBlast& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    MentalBlast& operator=(MentalBlast&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   dice = createLineEdit(parent, layout, "Dice?", std::mem_fn(&Power::numeric));
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override                 { if (!noStore) store();
                                                                   return v._dice * 10_cp; } // NOLINT
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dice->setText(QString("%1").arg(s._dice));
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._dice = dice->text().toInt();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dice"] = v._dice;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int _dice = 0;
    } v;

    QLineEdit* dice = nullptr;

    QString optOut(bool showEND) {
        if (v._dice < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1").arg(v._dice) + "d6 Mental Blast";
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

class MentalIllusions: public AllPowers {
public:
    MentalIllusions(): AllPowers("Mental Illusions")          { }
    MentalIllusions(const MentalIllusions& s): AllPowers(s)   { }
    MentalIllusions(MentalIllusions&& s): AllPowers(s)        { }
    MentalIllusions(const QJsonObject& json): AllPowers(json) { v._dice   = json["dice"].toInt(0);
                                                                  v._harm   = json["harm"].toBool(false);
                                                                  v._depend = json["depend"].toBool(false);
                                                                  v._sense  = json["sense"].toInt(0);
                                                                  v._self   = json["self"].toBool(false);
                                                                }
    ~MentalIllusions() override { }
    MentalIllusions& operator=(const MentalIllusions& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    MentalIllusions& operator=(MentalIllusions&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   dice   = createLineEdit(parent, layout, "Dice?", std::mem_fn(&Power::numeric));
                                                                   harm   = createCheckBox(parent, layout, "Cannot Cause Harm");
                                                                   depend = createCheckBox(parent, layout, "Depends On Character's Knowledge");
                                                                   sense  = createComboBox(parent, layout, "Senses Not Affected?", { "No Senses",
                                                                                                                                     "Non-Targeting Senses",
                                                                                                                                     "Targeting Senses" });
                                                                   self   = createCheckBox(parent, layout, "Self Only");
                                                                 }
    Fraction lim() override                                      { return (v._harm   ? Fraction(1, 4) : Fraction(0)) +
                                                                          (v._depend ? Fraction(1, 2) : Fraction(0)) +
                                                                          (v._self   ? Fraction(1)    : Fraction(0)) +
                                                                          v._sense * Fraction(1, 4); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v._dice * 5_cp; } // NOLINT
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dice->setText(QString("%1").arg(s._dice));
                                                                   harm->setChecked(s._harm);
                                                                   depend->setChecked(s._depend);
                                                                   sense->setCurrentIndex(s._sense);
                                                                   self->setChecked(s._self);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._dice   = dice->text().toInt();
                                                                   v._harm   = harm->isChecked();
                                                                   v._depend = depend->isChecked();
                                                                   v._sense  = sense->currentIndex();
                                                                   v._self   = self->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dice"]   = v._dice;
                                                                   obj["harm"]   = v._harm;
                                                                   obj["depend"] = v._depend;
                                                                   obj["sense"]  = v._sense;
                                                                   obj["self"]   = v._self;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int  _dice   = 0;
        bool _harm   = false;
        bool _depend = false;
        int  _sense  = -1;
        bool _self = false;
    } v;

    QLineEdit* dice = nullptr;
    QCheckBox* harm = nullptr;
    QCheckBox* depend = nullptr;
    QComboBox* sense = nullptr;
    QCheckBox* self = nullptr;

    QString optOut(bool showEND) {
        if (v._dice < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1").arg(v._dice) + "d6 Mental Illiusions";
        if (v._harm) res += "; Cannot Cause Harm";
        if (v._depend) res += "; Depends On Character's Knowledge";
        if (v._sense >= 1) {
            if (v._sense == 1) res += "; Doesn't Affect Non-Targeting Senses";
            else res += "; Doesnt Affect Targeting Senses";
        }
        if (v._self) res += "; Self Only";
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

class MindControl: public AllPowers {
public:
    MindControl(): AllPowers("Mind Control")              { }
    MindControl(const MindControl& s): AllPowers(s)       { }
    MindControl(MindControl&& s): AllPowers(s)            { }
    MindControl(const QJsonObject& json): AllPowers(json) { v._dice = json["dice"].toInt(0);
                                                               v._tele = json["tele"].toBool(false);
                                                               v._lit  = json["lit"].toBool(false);
                                                               v._set  = json["set"].toInt(0);
                                                               v._cmd  = json["cmd"].toString();
                                                             }
    ~MindControl() override { }
    MindControl& operator=(const MindControl& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    MindControl& operator=(MindControl&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return v._tele ? Fraction(1, 4) : Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   dice = createLineEdit(parent, layout, "Dice?", std::mem_fn(&Power::numeric));
                                                                   tele = createCheckBox(parent, layout, "Telepathic");
                                                                   lit  = createCheckBox(parent, layout, "Literal Interpretation");
                                                                   set  = createComboBox(parent, layout, "Set Effect?", { "Unrestricted",
                                                                                                                          "Restricted",
                                                                                                                          "Very Restricted",
                                                                                                                          "Completely Restricted" });
                                                                   cmd  = createLineEdit(parent, layout, "Command?");
                                                                 }
    Fraction lim() override                                      { return (v._lit ? Fraction(1, 4) : Fraction(0)) +
                                                                          ((v._set >= 1) ? Fraction(1, 4) : Fraction(0)) +
                                                                          v._set * Fraction(1, 4); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v._dice * 5_cp; } // NOLINT
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dice->setText(QString("%1").arg(s._dice));
                                                                   tele->setChecked(s._tele);
                                                                   lit->setChecked(s._lit);
                                                                   set->setCurrentIndex(s._set);
                                                                   cmd->setText(s._cmd);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._dice = dice->text().toInt();
                                                                   v._tele = tele->isChecked();
                                                                   v._lit  = lit->isChecked();
                                                                   v._set  = set->currentIndex();
                                                                   v._cmd  = cmd->text();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dice"] = v._dice;
                                                                   obj["tele"] = v._tele;
                                                                   obj["lit"]  = v._lit;
                                                                   obj["set"]  = v._set;
                                                                   obj["cmd"]  = v._cmd;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     _dice = 0;
        bool    _tele = false;
        bool    _lit  = false;
        int     _set  = -1;
        QString _cmd  = "";
    } v;

    QLineEdit* dice = nullptr;
    QCheckBox* tele = nullptr;
    QCheckBox* lit = nullptr;
    QComboBox* set = nullptr;
    QLineEdit* cmd = nullptr;

    QString optOut(bool showEND) {
        if (v._dice < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1").arg(v._dice) + "d6 Mind Control";
        if (v._tele) res += "; Telepathic";
        if (v._lit) res += "; Literal Interpretation";
        if (v._set != 0) res += "Set Effect (" + v._cmd + ")";
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

class MindLink: public AllPowers {
public:
    MindLink(): AllPowers("Mind Link")                 { }
    MindLink(const MindLink& s): AllPowers(s)          { }
    MindLink(MindLink&& s): AllPowers(s)               { }
    MindLink(const QJsonObject& json): AllPowers(json) { v._minds = json["minds"].toInt(0);
                                                         v._inv   = json["inv"].toInt(0);
                                                         v._who   = json["who"].toString();
                                                         v._range = json["range"].toInt(0);
                                                         v._los   = json["los"].toBool(false);
                                                         v._bond  = json["bond"].toString();
                                                         v._float = json["float"].toBool(false);
                                                         v._feed  = json["feed"].toInt(0);
                                                         v._only  = json["only"].toBool(false);
                                                       }
    ~MindLink() override { }
    MindLink& operator=(const MindLink& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    MindLink& operator=(MindLink&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   minds    = createLineEdit(parent, layout, "Minds?", std::mem_fn(&Power::numeric));
                                                                   inv      = createComboBox(parent, layout, "Minds Involved?", { "A Single Mind",
                                                                                                                                  "A Group of Minds",
                                                                                                                                  "Any Mind" });
                                                                   who      = createLineEdit(parent, layout, "Who?");
                                                                   range    = createComboBox(parent, layout, "Range?", { "Planetary",
                                                                                                                         "Unlimited",
                                                                                                                         "Unlimited, pan-dimensional" });
                                                                   los      = createCheckBox(parent, layout, "No LOS Needed");
                                                                   bond     = createLineEdit(parent, layout, "Psychic Bond(s)");
                                                                   floating = createCheckBox(parent, layout, "Floating Psychic Bond");
                                                                   feed     = createComboBox(parent, layout, "Feedback?", { "None", "STUN", "STUN and BODY" });
                                                                   only     = createCheckBox(parent, layout, "Only With Others Who Have Mind Link");
                                                                 }
    Fraction lim() override                                      { return Fraction(v._feed < 0 ? 0 : v._feed) + (v._only ? Fraction(1) : Fraction(0)); }
    Points points(bool noStore = false) override                 { if (!noStore) store();
                                                                   Points pts = 5_cp * (v._inv < 0 ? 0 : v._inv); // NOLINT
                                                                   int steps = 0;
                                                                   if (v._minds > 0) steps = (int) (log((double) v._minds) / log(2.0)); // NOLINT
                                                                   pts = pts + steps * 5_cp; // NOLINT
                                                                   pts = pts + (v._los ? 10_cp : 0_cp); // NOLINT
                                                                   pts = pts + (v._range < 0 ? 0 : v._range) * 5; // NOLINT
                                                                   pts = pts + countCommas(v._bond) * 5_cp; // NOLINT
                                                                   pts = pts + (v._float ? 10_cp : 0_cp); // NOLINT
                                                                   return pts;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   minds->setText(QString("%1").arg(s._minds));
                                                                   los->setChecked(s._los);
                                                                   bond->setText(s._bond);
                                                                   inv->setCurrentIndex(s._inv);
                                                                   who->setText(s._who);
                                                                   range->setCurrentIndex(s._range);
                                                                   floating->setChecked(s._float);
                                                                   feed->setCurrentIndex(s._feed);
                                                                   only->setChecked(s._only);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._minds = minds->text().toInt();
                                                                   v._los   = los->isChecked();
                                                                   v._bond  = bond->text();
                                                                   v._inv   = inv->currentIndex();
                                                                   v._who   = who->text();
                                                                   v._range = range->currentIndex();
                                                                   v._float = floating->isChecked();
                                                                   v._feed  = feed->currentIndex();
                                                                   v._only  = only->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["minds"] = v._minds;
                                                                   obj["inv"]   = v._inv;
                                                                   obj["who"]   = v._who;
                                                                   obj["los"]   = v._los;
                                                                   obj["bond"]  = v._bond;
                                                                   obj["range"] = v._range;
                                                                   obj["float"] = v._float;
                                                                   obj["feed"]  = v._feed;
                                                                   obj["only"]  = v._only;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     _minds = 0;
        int     _inv   = -1;
        QString _who   = "";
        int     _range = -1;
        bool    _los   = false;
        QString _bond  = "";
        bool    _float = false;
        int     _feed  = -1;
        bool    _only  = false;
    } v;

    QLineEdit* minds = nullptr;
    QComboBox* inv = nullptr;
    QLineEdit* who = nullptr;
    QComboBox* range = nullptr;
    QCheckBox* los = nullptr;
    QLineEdit* bond = nullptr;
    QCheckBox* floating = nullptr;
    QComboBox* feed = nullptr;
    QCheckBox* only = nullptr;

    QString optOut(bool showEND) {
        if (v._minds < 1 || v._inv < 0 || (v._inv != 2 && v._who.isEmpty()) || v._range < 0) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "Mind Link with ";
        switch (v._inv) {
        case 0: res += "A Single Mind(" + v._who + ")";     break;
        case 1: res += "A Group of Minds (" + v._who + ")"; break;
        case 2: res += "Any Mind";                          break;
        }
        switch (v._range) {
        case 0: res += "; Plantary Range";                   break;
        case 1: res += "; Unlimited Range";                  break;
        case 2: res += "; Unlimited Range, Pan-Dimensional"; break;
        }
        if (v._los) res += "; No LOS Needed";
        if (!v._bond.isEmpty()) res += ": Psychic Bond with " + v._bond;
        if (v._float) res += ": Floating Psychic Bond";
        switch (v._feed) {
        case 1: res += "; Link Channels STUN Damage";
        case 2: res += ": Link Channels STUN and BODY Damage";
        }
        if (v._only) res += "; Only With Others Who Have Mind Link";
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

class MindScan: public AllPowers {
public:
    MindScan(): AllPowers("Mind Scan")                 { }
    MindScan(const MindScan& s): AllPowers(s)          { }
    MindScan(MindScan&& s): AllPowers(s)               { }
    MindScan(const QJsonObject& json): AllPowers(json) { v._dice  = json["dice"].toInt(0);
                                                            v._link  = json["link"].toBool(false);
                                                            v._lock  = json["lit"].toBool(false);
                                                            v._bonus = json["set"].toInt(0);
                                                            v._cant  = json["cant"].toInt(0);
                                                            v._part  = json["part"].toBool(false);
                                                          }
    ~MindScan() override { }
    MindScan& operator=(const MindScan& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    MindScan& operator=(MindScan&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return (v._link ? Fraction(1)    : Fraction(0)) +
                                                                          (v._lock ? Fraction(1, 2) : Fraction(0)); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   dice  = createLineEdit(parent, layout, "Dice?", std::mem_fn(&Power::numeric));
                                                                   link  = createCheckBox(parent, layout, "One Way Link");
                                                                   lock  = createCheckBox(parent, layout, "Partial Lock-On");
                                                                   bonus = createLineEdit(parent, layout, "Bonus OMCV?", std::mem_fn(&Power::numeric));
                                                                   cant  = createComboBox(parent, layout, "Cannot Attack Through Link", { "Attacks Possible",
                                                                                                                                          "No Attack",
                                                                                                                                          "No Attack or Communication",
                                                                                                                                          "Only Target Can Attack" });
                                                                   part  = createCheckBox(parent, layout, "Partial Effect");
                                                                 }
    Fraction lim() override                                      { return ((v._cant > 0) ? v._cant * Fraction(1, 2) : Fraction(0)) + (v._part ? Fraction(1, 2) : Fraction(0)); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v._dice * 5_cp + v._bonus * 2_cp; } // NOLINT
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dice->setText(QString("%1").arg(s._dice));
                                                                   link->setChecked(s._link);
                                                                   lock->setChecked(s._lock);
                                                                   bonus->setText(QString("%1").arg(s._bonus));
                                                                   cant->setCurrentIndex(s._cant);
                                                                   part->setChecked(s._part);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._dice  = dice->text().toInt();
                                                                   v._link  = link->isChecked();
                                                                   v._lock  = lock->isChecked();
                                                                   v._bonus = bonus->text().toInt();
                                                                   v._cant  = cant->currentIndex();
                                                                   v._part  = part->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dice"]  = v._dice;
                                                                   obj["tele"]  = v._link;
                                                                   obj["lock"]  = v._lock;
                                                                   obj["bonus"] = v._bonus;
                                                                   obj["cant"]  = v._cant;
                                                                   obj["part"]  = v._part;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int  _dice  = 0;
        bool _link  = false;
        bool _lock  = false;
        int  _bonus = 0;
        int  _cant  = -1;
        bool _part  = false;
    } v;

    QLineEdit* dice = nullptr;
    QCheckBox* link = nullptr;
    QCheckBox* lock = nullptr;
    QLineEdit* bonus = nullptr;
    QComboBox* cant = nullptr;
    QCheckBox* part = nullptr;

    QString optOut(bool showEND) {
        if (v._dice < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1").arg(v._dice) + "d6 Mind Scan";
        if (v._bonus > 0) res += QString(", +%1 OMCV").arg(v._bonus);
        if (v._link) res += "; One Way Link";
        if (v._lock) res += "; Partial Lock-On";
        switch (v._cant) {
        case 1: res += "; No Attacks Through Link";                 break;
        case 2: res += "; No Attack or Communication Through Link"; break;
        case 3: res += "; Only Traget May Attack Through Link";     break;
        }
        if (v._part) res += "; Partial Effect";
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

class Telepathy: public AllPowers {
public:
    Telepathy(): AllPowers("Telepathy")                 { }
    Telepathy(const Telepathy& s): AllPowers(s)         { }
    Telepathy(Telepathy&& s): AllPowers(s)              { }
    Telepathy(const QJsonObject& json): AllPowers(json) { v._dice  = json["dice"].toInt(0);
                                                             v._bcast = json["bcast"].toBool(false);
                                                             v._excl  = json["excl"].toBool(false);
                                                             v._emp   = json["emp"].toInt(0);
                                                             v._emo   = json["emo"].toString();
                                                             v._feed  = json["feed"].toInt(0);
                                                             v._lang  = json["lang"].toBool(false);
                                                             v._recv  = json["recv"].toBool(false);
                                                           }
    ~Telepathy() override { }
    Telepathy& operator=(const Telepathy& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    Telepathy& operator=(Telepathy&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   dice  = createLineEdit(parent, layout, "Dice?", std::mem_fn(&Power::numeric));
                                                                   bcast = createCheckBox(parent, layout, "Broadcast Only");
                                                                   excl  = createCheckBox(parent, layout, "Communication Only");
                                                                   emp   = createComboBox(parent, layout, "Empathy?", { "Read Thoughts",
                                                                                                                        "Only Read Emptions",
                                                                                                                        "Only Read An Emotion" });
                                                                   emo   = createLineEdit(parent, layout, "Command?");
                                                                   feed  = createComboBox(parent, layout, "Feedback?", { "None",
                                                                                                                         "STUN Only",
                                                                                                                         "STUN and BODY" });
                                                                   lang  = createCheckBox(parent, layout, "Language Barrier");
                                                                   recv  = createCheckBox(parent, layout, "Receive Only");
                                                                 }
    Fraction lim() override                                      { return (v._bcast ? Fraction(1, 2) : Fraction(0)) +
                                                                          (v._excl ? Fraction(1, 4) : Fraction(0)) +
                                                                          (v._emp + 1) * Fraction(1, 2) +
                                                                          (v._feed + 1) * Fraction(1) +
                                                                          (v._lang ? Fraction(1, 2) : Fraction(0)) +
                                                                          (v._recv ? Fraction(1, 2) : Fraction(0)); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v._dice * 5_cp; } // NOLINT
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dice->setText(QString("%1").arg(s._dice));
                                                                   bcast->setChecked(s._bcast);
                                                                   excl->setChecked(s._excl);
                                                                   emp->setCurrentIndex(s._emp);
                                                                   emo->setText(s._emo);
                                                                   feed->setCurrentIndex(s._feed);
                                                                   lang->setChecked(s._lang);
                                                                   recv->setChecked(s._recv);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._dice  = dice->text().toInt();
                                                                   v._bcast = bcast->isChecked();
                                                                   v._excl  = excl->isChecked();
                                                                   v._emp   = emp->currentIndex();
                                                                   v._emo   = emo->text();
                                                                   v._feed  = feed->currentIndex();
                                                                   v._lang  = lang->isChecked();
                                                                   v._recv  = recv->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dice"]  = v._dice;
                                                                   obj["bcast"] = v._bcast;
                                                                   obj["excl"]  = v._excl;
                                                                   obj["emp"]   = v._emp;
                                                                   obj["emo"]   = v._emo;
                                                                   obj["feed"]  = v._feed;
                                                                   obj["lang"]  = v._lang;
                                                                   obj["recv"]  = v._recv;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     _dice  = 0;
        bool    _bcast = false;
        bool    _excl  = false;
        int     _emp   = -1;
        QString _emo   = "";
        int     _feed  = -1;
        bool    _lang  = false;
        bool    _recv  = false;
    } v;

    QLineEdit* dice = nullptr;
    QCheckBox* bcast = nullptr;
    QCheckBox* excl = nullptr;
    QComboBox* emp = nullptr;
    QLineEdit* emo = nullptr;
    QComboBox* feed = nullptr;
    QCheckBox* lang = nullptr;
    QCheckBox* recv = nullptr;

    QString optOut(bool showEND) {
        if (v._dice < 1 || (v._emp == 1 && v._emo.isEmpty())) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1").arg(v._dice) + "d6 Telepathy";
        if (v._bcast) res += "; Broadcast Only";
        if (v._excl) res += "; Communication Only";
        if (v._emp == 1) res += "; Empathy";
        else if (v._emp == 2) res += "Empathy (" + v._emo + " Only)";
        if (v._feed == 1) res += "; Feedback (STUN)";
        else if (v._feed == 2) res += "; Feedback (STUN & BODY)";
        if (v._lang) res += "; Language Barrier";
        if (v._recv) res += "; Receive Only";
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
