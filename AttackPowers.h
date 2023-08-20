#ifndef ATTACKPOWERS_H
#define ATTACKPOWERS_H

#include "powers.h"
#ifndef ISHSC
#include "sheet.h"
#endif

class Blast: public AllPowers {
public:
    Blast(): AllPowers("Blast")                     { }
    Blast(const Blast& s): AllPowers(s)             { }
    Blast(Blast&& s): AllPowers(s)                  { }
    Blast(const QJsonObject& json): AllPowers(json) { v._dice = json["dice"].toInt(0);
                                                      v._pded = json["pded"].toInt(0);
                                                      v._stun = json["stun"].toBool(false);
                                                    }
    virtual Blast& operator=(const Blast& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Blast& operator=(Blast&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   dice = createLineEdit(parent, layout, "Dice of Damage?", std::mem_fn(&Power::numeric));
                                                                   pded = createComboBox(parent, layout, "Versus?", { "PD", "ED" });
                                                                   stun = createCheckBox(parent, layout, "STUN Only");
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return Points(v._dice * 5); }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dice->setText(QString("%1").arg(s._dice));
                                                                   pded->setCurrentIndex(s._pded);
                                                                   stun->setChecked(s._stun);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._dice = dice->text().toInt();
                                                                   v._pded = pded->currentIndex();
                                                                   v._stun = stun->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dice"] = v._dice;
                                                                   obj["pded"] = v._pded;
                                                                   obj["stun"] = v._stun;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int  _dice = 0;
        int  _pded = -1;
        bool _stun = false;
    } v;

    QLineEdit* dice;
    QComboBox* pded;
    QCheckBox* stun;

    QString optOut(bool showEND) {
        if (v._dice < 1 || v._pded < 0) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1").arg(v._dice) + "d6 Blast vs " + ((v._pded == 0) ? "PD" : "ED");
        if (v._stun) res += "; STUN Only";
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

class ChangeEnvironment: public AllPowers {
private:
    struct effects {
        int             which;
        int             idx;
        int             level;
        QList<QWidget*> widgets;
        QString         val;

        effects(int w, int i, int l, QList<QWidget*> wl = { }, QString v = "")
            : which(w)
            , idx(i)
            , level(l)
            , widgets(wl)
            , val(v) { }
    };
    bool _create = true;
    bool _data   = true;
    int  _index;

public:
    ChangeEnvironment(): AllPowers("Change Environment")        { }
    ChangeEnvironment(const ChangeEnvironment& s): AllPowers(s) { }
    ChangeEnvironment(ChangeEnvironment&& s): AllPowers(s)      { }
    ChangeEnvironment(const QJsonObject& json): AllPowers(json) { QJsonArray arr = json["effects"].toArray();
                                                                  _create = false;
                                                                  _data   = true;
                                                                  for (int i = 0; i < arr.count(); ++i) {
                                                                      QJsonObject obj = arr[i].toObject();
                                                                      effects eff(obj["which"].toInt(0),
                                                                                  obj["idx"].toInt(0),
                                                                                  obj["level"].toInt(0),
                                                                                  { },
                                                                                  obj["val"].toString(""));
                                                                      v._effects.push_back(eff);
                                                                  }
                                                                  v._lasting = json["lasting"].toInt(-1);
                                                                  v._varying = json["varying"].toBool(false);
                                                                  v._envs    = json["envs"].toInt(-1);
                                                                  v._what    = json["what"].toString("");
                                                                }

    virtual ChangeEnvironment& operator=(const ChangeEnvironment& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual ChangeEnvironment& operator=(ChangeEnvironment&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return (v._envs + 1) * Fraction(1, 4); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   effect  = createComboBox(parent, layout, "Effects?", { "",
                                                                                                                          "Movement",
                                                                                                                          "PER Rolll for one Sense",
                                                                                                                          "PER Rolll for one Sense Group",
                                                                                                                          "Characteristic Roll",
                                                                                                                          "Skill Roll",
                                                                                                                          "Increase Temperature",
                                                                                                                          "Decrease Temperature",
                                                                                                                          "Range Modifier",
                                                                                                                          "Negative Combat Modifier▲",
                                                                                                                          "Characteristic Roll and all assoc. Skill Rolls",
                                                                                                                          "Damage",
                                                                                                                          "Telekinesis",
                                                                                                                          "Increate Wind Level",
                                                                                                                          "Decrease Wind Level"
                                                                                                                        },
                                                                                             std::mem_fn(&Power::activate));
                                                                   lasting = createComboBox(parent, layout, "Lasting?", { "", "1 Turn", "1 Minute", "5 Minutes", "20 Minutes",
                                                                                                                          "1 Hour", "6 Hours", "1 Day", "1 Week",
                                                                                                                          "1 Month", "1 Season", "1 Year", "5 Years" });
                                                                   varying = createCheckBox(parent, layout, "Varying Combat Effects");
                                                                   envs    = createComboBox(parent, layout, "Varying Environments?", { "",
                                                                                                                                       "Very Limited Group",
                                                                                                                                       "Limited Group",
                                                                                                                                       "Broad Group"
                                                                                                                                     });
                                                                   what    = createLineEdit(parent, layout, "What Environmets?");
                                                                   _create = true;
                                                                   _data   = false;
                                                                   _index  = 0;
                                                                   _layout = layout;
                                                                   _parent = parent;
                                                                   for (auto& effect: v._effects) { indexed(effect.which, NoUpdate); ++_index; }
                                                                   _data = true;
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   QJsonArray arr;
                                                                   for (auto& effect: v._effects) {
                                                                       QJsonObject eff;
                                                                       eff["which"] = effect.which;
                                                                       eff["idx"]   = effect.idx;
                                                                       eff["level"] = effect.level;
                                                                       eff["val"]   = effect.val;
                                                                       arr.append(eff);
                                                                   }
                                                                   obj["effects"] = arr;
                                                                   obj["lasting"] = v._lasting;
                                                                   obj["varying"] = v._varying;
                                                                   obj["envs"]    = v._envs;
                                                                   obj["what"]    = v._what;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        QList<effects> _effects;
        int            _lasting = -1;
        bool           _varying = false;
        int            _envs    = -1;
        QString        _what    = "";
    } v;
    QVBoxLayout* _layout;
    QWidget*     _parent;

    QComboBox*   effect;
    QComboBox*   lasting;
    QCheckBox*   varying;
    QComboBox*   envs;
    QLineEdit*   what;

    void restore() override {
        vars s = v;
        AllPowers::restore();
        for (auto& effect: s._effects) {
            int count = effect.widgets.count();
            for (int i = 0; i < count; ++i) {
                QLineEdit* edit = dynamic_cast<QLineEdit*>(effect.widgets[i]);
                if (edit) edit->setText(effect.val);
                QComboBox* combo = dynamic_cast<QComboBox*>(effect.widgets[i]);
                if (combo) combo->setCurrentIndex(effect.idx);
            }
        }
        lasting->setCurrentIndex(s._lasting);
        varying->setChecked(s._varying);
        envs->setCurrentIndex(s._envs);
        what->setText(s._what);
        v = s;
        _create = true;
        _data   = true;
    }

    void store() override {
        AllPowers::store();
        for (auto& effect: v._effects) {
            int count = effect.widgets.count();
            for (int i = 0; i < count - 1; ++i) {
                QLineEdit* edit = dynamic_cast<QLineEdit*>(effect.widgets[i]);
                if (edit) effect.val = edit->text();
                QComboBox* combo = dynamic_cast<QComboBox*>(effect.widgets[i]);
                if (combo) effect.idx = combo->currentIndex();
            }
        }
        v._lasting = lasting->currentIndex();
        v._varying = varying->isChecked();
        v._envs    = envs->currentIndex();
        v._what    = what->text();
    }

    void deleteEffect(QWidget* w) {
        for (auto effect = v._effects.cbegin(); effect != v._effects.cend(); ++effect) {
            for (const auto& wgt: effect->widgets) if (w == wgt) {
                for (const auto& wgt: effect->widgets) wgt->deleteLater();
                v._effects.erase(effect);
                return;
            }
        }
    }

    effects& findEffect(QWidget* w) {
        for (auto& effect: v._effects) {
            for (const auto& wgt: effect.widgets) if (w == wgt) return effect;
        }
        static effects none { -1, -1, -1, { }, "" };
        return none;
    }

    Points points(bool noStore = false) override {
        if (!noStore) store();
        Points pts = (v._lasting + 1) * 2_cp;
        if (v._varying) pts += 10_cp;
        for (const auto& effect: v._effects) {
            switch (effect.which) {
            case 0:                                   break;
            case 1:  pts = pts + effect.level;        break;
            case 2:  pts = pts + effect.level * 2_cp; break;
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:  pts = pts + effect.level * 3_cp; break;
            case 10: pts = pts + effect.level * 4_cp; break;
            case 11:
            case 12:
            case 13:
            case 14: pts = pts + effect.level * 5_cp; break;
            }
        }
        return pts;
    }

    void activate(int pick, int) override {
        indexed(pick, true);
    }

#ifdef ISHSC
    void indexed(int pick, bool) {
#else
    void indexed(int pick, bool update = DoUpdate) {
#endif
        int idx = _layout->indexOf(lasting);
        QList<QWidget*> widgets { };
        switch (pick) {
        case 0:
            break;
        case 1:  // Movement
            if (_create) {
                widgets.emplaceBack(createComboBox(_parent, _layout, "Movement Type?", { "",
                                                                                         "Running",
                                                                                         "Swimming",
                                                                                         "Jumping",
                                                                                         "Flight",
                                                                                         "Teleportation",
                                                                                         "Tunneling"
                                                                                       }, idx));
                widgets.emplaceBack(createLineEdit(_parent, _layout, "Movement Penalty?", std::mem_fn(&Power::numeric), idx + 1));
                if (_data) v._effects.emplaceBack(0, -1, 0, widgets);
                else v._effects[_index].widgets = widgets;
            } else if (_data) v._effects.emplaceBack(0, -1, 0, widgets);
            break;
        case 2:  // PER Roll for one Sense
            if (_create) {
                widgets.emplaceBack(createComboBox(_parent, _layout, "Sense?", { "",
                                                                                 "Normal Hearing",
                                                                                 "Active Sonar",
                                                                                 "Ultrasonic Perception",
                                                                                 "Mental Awareness",
                                                                                 "Mind Scan",
                                                                                 "Radio Perception",
                                                                                 "Radar",
                                                                                 "Normal Sight",
                                                                                 "Nightvision",
                                                                                 "Infrared Pereception",
                                                                                 "Ultraviolet Perception",
                                                                                 "Spatial Awreness",
                                                                                 "Clairsentience",
                                                                                 "Normal Smell",
                                                                                 "Normal Taste",
                                                                                 "Normal Touch"
                                                                               }, idx));
                widgets.emplaceBack(createLineEdit(_parent, _layout, "PER Roll Penalty?", std::mem_fn(&Power::numeric), idx + 1));
                if (_data) v._effects.emplaceBack(1, -1, 0, widgets);
                else v._effects[_index].widgets = widgets;
            } else if (_data) v._effects.emplaceBack(1, -1, 0, widgets);
            break;
        case 3:  // PER Rolll for one Sense Group
            if (_create) {
                widgets.emplaceBack(createComboBox(_parent, _layout, "Sense Group?", { "",
                                                                                       "Hearing",
                                                                                       "Mental",
                                                                                       "Radio",
                                                                                       "Sight",
                                                                                       "Smell/Taste",
                                                                                       "Touch"
                                                                                     }, idx));
                widgets.emplaceBack(createLineEdit(_parent, _layout, "PER Roll Penalty?", std::mem_fn(&Power::numeric), idx + 1));
                if (_data) v._effects.emplaceBack(2, -1, 0, widgets);
                else v._effects[_index].widgets = widgets;
            } else if (_data) v._effects.emplaceBack(2, -1, 0, widgets);
            break;
        case 4:  // Characteristic Roll
            if (_create) {
                widgets.emplaceBack(createComboBox(_parent, _layout, "Charactreristic?", { "",
                                                                                           "STR",
                                                                                           "DEX",
                                                                                           "CON",
                                                                                           "INT",
                                                                                           "EGO",
                                                                                           "PRE"
                                                                                          }, idx));
                widgets.emplaceBack(createLineEdit(_parent, _layout, "Characteristic Roll Penalty?", std::mem_fn(&Power::numeric), idx + 1));
                if (_data) v._effects.emplaceBack(3, -1, 0, widgets);
                else v._effects[_index].widgets = widgets;
            } else if (_data) v._effects.emplaceBack(3, -1, 0, widgets);
            break;
        case 5:  // Skill Roll
            if (_create) {
                widgets.emplaceBack(createLineEdit(_parent, _layout, "Skills?", idx));
                widgets.emplaceBack(createLineEdit(_parent, _layout, "Skill Roll Penalty?", std::mem_fn(&Power::numeric), idx + 1));
                if (_data) v._effects.emplaceBack(4, -1, 0, widgets);
                else v._effects[_index].widgets = widgets;
            } else if (_data) v._effects.emplaceBack(4, -1, 0, widgets);
            break;
        case 6:  // Increase Temperature
            if (_create) {
                widgets.emplaceBack(createLineEdit(_parent, _layout, "Temperature Levels Increase?", std::mem_fn(&Power::numeric), idx));
                if (_data) v._effects.emplaceBack(5, -1, 0, widgets);
                else v._effects[_index].widgets = widgets;
            } else if (_data) v._effects.emplaceBack(5, -1, 0, widgets);
            break;
        case 7:  // Decrease Temperature
            if (_create) {
                widgets.emplaceBack(createLineEdit(_parent, _layout, "Temperature Levels Decrease?", std::mem_fn(&Power::numeric), idx));
                if (_data) v._effects.emplaceBack(6, -1, 0, widgets);
                else v._effects[_index].widgets = widgets;
            } else if (_data) v._effects.emplaceBack(6, -1, 0, widgets);
            break;
        case 8:  // Range Modifier
            if (_create) {
                widgets.emplaceBack(createLineEdit(_parent, _layout, "Range Modifier Penalty?", std::mem_fn(&Power::numeric), idx));
                if (_data) v._effects.emplaceBack(7, -1, 0, widgets);
                else v._effects[_index].widgets = widgets;
            } else if (_data) v._effects.emplaceBack(7, -1, 0, widgets);
            break;
        case 9:  // Negative Combat Modifier▲
            if (_create) {
                widgets.emplaceBack(createLineEdit(_parent, _layout, "Combat Modifier▲?", idx));
                widgets.emplaceBack(createLineEdit(_parent, _layout, "Penalty?", std::mem_fn(&Power::numeric), idx + 1));
                if (_data) v._effects.emplaceBack(8, -1, 0, widgets);
                else v._effects[_index].widgets = widgets;
            } else if (_data) v._effects.emplaceBack(8, -1, 0, widgets);
            break;
        case 10:  // Characteristic Roll and all assoc. Skill Rolls
            if (_create) {
                widgets.emplaceBack(createComboBox(_parent, _layout, "Charactreristic?", { "",
                                                                                           "STR",
                                                                                           "DEX",
                                                                                           "CON",
                                                                                           "INT",
                                                                                           "EGO",
                                                                                           "PRE"
                                                                                          }, idx));
                widgets.emplaceBack(createLineEdit(_parent, _layout, "Characteristicc & Skill Roll Penalty?", std::mem_fn(&Power::numeric), idx + 1));
                if (_data) v._effects.emplaceBack(9, -1, 0, widgets);
                else v._effects[_index].widgets = widgets;
            } else if (_data) v._effects.emplaceBack(9, -1, 0, widgets);
            break;
        case 11: // Damage
            if (_create) {
                widgets.emplaceBack(createLineEdit(_parent, _layout, "Damage Points?", std::mem_fn(&Power::numeric), idx));
                if (_data) v._effects.emplaceBack(10, -1, 0, widgets);
                else v._effects[_index].widgets = widgets;
            } else if (_data) v._effects.emplaceBack(10, -1, 0, widgets);
            break;
        case 12: // Telekinesis
            if (_create) {
                widgets.emplaceBack(createLineEdit(_parent, _layout, "Telekinetic STR?", std::mem_fn(&Power::numeric), idx));
                if (_data) v._effects.emplaceBack(11, -1, 0, widgets);
                else v._effects[_index].widgets = widgets;
            } else if (_data) v._effects.emplaceBack(11, -1, 0, widgets);
            break;
        case 13: // Increase Wind Level
            if (_create) {
                widgets.emplaceBack(createLineEdit(_parent, _layout, "Wind Increase?", std::mem_fn(&Power::numeric), idx));
                if (_data) v._effects.emplaceBack(12, -1, 0, widgets);
                else v._effects[_index].widgets = widgets;
            } else if (_data) v._effects.emplaceBack(12, -1, 0, widgets);
            break;
        case 14: // Decrease Wind Level
            if (_create) {
                widgets.emplaceBack(createLineEdit(_parent, _layout, "Wind Decrease?", std::mem_fn(&Power::numeric), idx));
                if (_data) v._effects.emplaceBack(13, -1, 0, widgets);
                else v._effects[_index].widgets = widgets;
            } else if (_data) v._effects.emplaceBack(13, -1, 0, widgets);
            break;
        }

#ifndef ISHSC
        if (update) PowerDialog::ref().updateForm();
#endif
    }

    QString describe(const effects& effect) {
        if (effect.idx < 0) return "";
        QStringList movement { "None",
                               "Running",
                               "Swimming",
                               "Jumping",
                               "Flight",
                               "Teleportation",
                               "Tunneling" };
        QStringList sense { "None",
                            "Normal Hearing",
                            "Active Sonar",
                            "Ultrasonic Perception",
                            "Mental Awareness",
                            "Mind Scan",
                            "Radio Perception",
                            "Radar",
                            "Normal Sight",
                            "Nightvision",
                            "Infrared Pereception",
                            "Ultraviolet Perception",
                            "Normal Smell",
                            "Normal Taste",
                            "Normal Touch" };
        QStringList senseGroup { "None",
                                 "Hearing",
                                 "Mental",
                                 "Radio",
                                 "Sight",
                                 "Smell/Taste",
                                 "Touch" };
        QStringList stat { "None",
                           "STR",
                           "DEX",
                           "CON",
                           "INT",
                           "EGO",
                           "PRE" };
        switch (effect.which) {
        case 0:  return QString("-%1m ").arg(effect.level) + movement[effect.idx];
        case 1:  return QString("-%1 PER Roll w/").arg(effect.level) + sense[effect.idx];
        case 2:  return QString("-%1 PER Roll w/").arg(effect.level) + senseGroup[effect.idx];
        case 3:  return QString("-%1 Roll w/").arg(effect.level) + stat[effect.idx];
        case 4:  return QString("-%1 w/").arg(effect.level) + effect.val;
        case 5:  return QString("+%1 Temperature Level%2").arg(effect.level).arg((effect.level > 1) ? "s" : "");
        case 6:  return QString("-%1 Temperature Level%2").arg(effect.level).arg((effect.level > 1) ? "s" : "");
        case 7:  return QString("+%1 to Range MOD").arg(effect.level);
        case 8:  return QString("-%1 w/").arg(effect.level) + effect.val + "▲";
        case 9:  return QString("-%1 Roll w/").arg(effect.level) + stat[effect.idx + 1] + " and Skills";
        case 10: return QString("%1 Damage").arg(effect.level);
        case 11: return QString("%1 Telekinetic STR").arg(effect.level);
        case 12: return QString("+%1 Wind Level%2").arg(effect.level).arg((effect.level > 1) ? "s" : "");
        case 13: return QString("-%1 Wind Level%2").arg(effect.level).arg((effect.level > 1) ? "s" : "");
        }

        return "";
    }

    QString optOut(bool showEND) {
        if (v._effects.isEmpty() || (v._envs > 0 && v._what.isEmpty())) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "Change Environment: ";
        QString sep = "";
        for (const auto& effect: v._effects) { res += sep + describe(effect); sep = ", "; }
        sep = " (";
        QStringList lasting { "1 Turn", "1 Minute", "5 Minutes", "20 Minutes",
                              "1 Hour", "6 Hours", "1 Day", "1 Week",
                              "1 Month", "1 Season", "1 Year", "5 Years" };
        if (v._lasting != -1) { res += sep + "Long Lasting: " + lasting[v._lasting]; sep = "; "; }
        if (v._varying) { res += sep + "Varying Combat Effects "; sep = "; "; }
        if (v._envs > 0) { res += sep + " Varying Environments (" + v._what + ")"; sep += "; "; }
        if (sep == "; ") res += ")";
        return res;
    }

    void numeric(int) override {
        QLineEdit* line = static_cast<QLineEdit*>(sender());
        QString txt = line->text();
        if (!txt.isEmpty() && txt == "0") {
            line->undo();
            effects& effect = findEffect(line);
            if (effect.widgets.count() != 0) {
                deleteEffect(line);
                _layout->update();
                return;
            }
        }
        if (txt.isEmpty() || isNumber(txt)) {
            effects& effect = findEffect(line);
            effect.level = txt.toInt();
            return;
        }
        line->undo();
    }
};

class Entangle: public AllPowers {
public:
    Entangle(): AllPowers("Entangle")                  { }
    Entangle(const Entangle& s): AllPowers(s)          { }
    Entangle(Entangle&& s): AllPowers(s)               { }
    Entangle(const QJsonObject& json): AllPowers(json) { v._dice   = json["dice"].toInt(0);
                                                            v._body   = json["body"].toInt(0);
                                                            v._def    = json["def"].toInt(0);
                                                            v._dmg    = json["dmg"].toInt(0);
                                                            v._what   = json["what"].toString();
                                                            v._senses = json["senses"].toString();
                                                            v._groups = json["groups"].toString();
                                                            v._foci   = json["foci"].toBool(false);
                                                            v._one    = json["one"].toBool(false);
                                                            v._nodef  = json["nodef"].toBool(false);
                                                            v._set    = json["set"].toInt(0);
                                                            v._susc   = json["susc"].toInt(0);
                                                            v._suscTo = json["suscTo"].toString();
                                                            v._vuln   = json["vuln"].toInt(0);
                                                            v._vulnTo = json["vulnTo"].toString();
                                                          }

    Fraction adv() override                                      { return ((v._dmg == 0) ? Fraction(1, 4) : Fraction(0)) +
                                                                          ((v._dmg == 1) ? Fraction(1, 4) : Fraction(0)) +
                                                                          ((v._dmg == 2) ? Fraction(1, 2) : Fraction(0)) +
                                                                          ((v._dmg == 3) ? Fraction(1)    : Fraction(0)); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   dice   = createLineEdit(parent, layout, "Dice of Entangle?", std::mem_fn(&Power::numeric));
                                                                   body   = createLineEdit(parent, layout, "Additional BODY dice?", std::mem_fn(&Power::numeric));
                                                                   def    = createLineEdit(parent, layout, "Additional Defense?", std::mem_fn(&Power::numeric));
                                                                   dmg    = createComboBox(parent, layout, "Damage Allocation?", { "Entagle Takes Damage And Acts As rDEF For Target",
                                                                                                                                   "Entangle And Target Both Take Damage",
                                                                                                                                   "Entangle Takes No Damage From Certain Attacks",
                                                                                                                                   "Entangle is Transparent To All Attacks",
                                                                                                                                   "Entangle Only Affected By Target"
                                                                                                                                 });
                                                                   what   = createLineEdit(parent, layout, "Effected by What?");
                                                                   senses = createLineEdit(parent, layout, "Entangle Stops Sense(s)?");
                                                                   groups = createLineEdit(parent, layout, "Entangle Stops Sense Group(s)?");
                                                                   foci   = createCheckBox(parent, layout, "Character Can Use Foci");
                                                                   one    = createCheckBox(parent, layout, "Entangle Has 1 BODY");
                                                                   nodef  = createCheckBox(parent, layout, "No Defense");
                                                                   set    = createComboBox(parent, layout, "Set Effect?", { "Whole Body",
                                                                                                                            "Hands Only",
                                                                                                                            "Feet Only"
                                                                                                                          });
                                                                   susc   = createComboBox(parent, layout, "Susceptible?", { "Not Susceptible",
                                                                                                                             "Uncommon",
                                                                                                                             "Common",
                                                                                                                             "Very Common"});
                                                                   suscTo = createLineEdit(parent, layout, "Susceptible To?");
                                                                   vuln   = createComboBox(parent, layout, "Vulnerable?", { "Not Vulnerable",
                                                                                                                            "Uncommon",
                                                                                                                            "Common",
                                                                                                                            "Very Common"});
                                                                   vulnTo = createLineEdit(parent, layout, "Vulnerable To?");
                                                                 }
    Fraction lim() override                                      { return (v._foci        ? Fraction(1)                 : Fraction(0)) +
                                                                          (v._one         ? Fraction(1, 2)              : Fraction(0)) +
                                                                          (v._nodef       ? Fraction(1, Fraction(1, 2)) : Fraction(0)) +
                                                                          ((v._set >= 1)  ? Fraction(1)                 : Fraction(0)) +
                                                                          ((v._susc == 1) ? Fraction(1, 4)              : Fraction(0)) +
                                                                          ((v._susc == 2) ? Fraction(1, 2)              : Fraction(0)) +
                                                                          ((v._susc == 3) ? Fraction(1)                 : Fraction(0)) +
                                                                          ((v._vuln == 1) ? Fraction(1, 4)              : Fraction(0)) +
                                                                          ((v._vuln == 2) ? Fraction(1, 2)              : Fraction(0)) +
                                                                          ((v._vuln == 3) ? Fraction(1)                 : Fraction(0)); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v._dice * 10_cp +
                                                                          v._body * 5_cp +
                                                                          ((v._def + 1) / 2) * 5_cp +
                                                                          countCommas(v._senses) * 5_cp +
                                                                          countCommas(v._groups) * 10_cp; }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dice->setText(QString("%1").arg(s._dice));
                                                                   body->setText(QString("%1").arg(s._body));
                                                                   def->setText(QString("%1").arg(s._def));
                                                                   dmg->setCurrentIndex(s._dmg);
                                                                   what->setText(s._what);
                                                                   senses->setText(s._senses);
                                                                   groups->setText(s._groups);
                                                                   one->setChecked(s._one);
                                                                   nodef->setChecked(s._nodef);
                                                                   set->setCurrentIndex(s._set);
                                                                   susc->setCurrentIndex(s._susc);
                                                                   suscTo->setText(s._suscTo);
                                                                   vuln->setCurrentIndex(s._vuln);
                                                                   vulnTo->setText(s._vulnTo);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._dice   = dice->text().toInt();
                                                                   v._body   = body->text().toInt();
                                                                   v._def    = def->text().toInt();
                                                                   v._dmg    = dmg->currentIndex();
                                                                   v._what   = what->text();
                                                                   v._senses = senses->text();
                                                                   v._groups = groups->text();
                                                                   v._foci   = foci->isChecked();
                                                                   v._one    = one->isChecked();
                                                                   v._nodef  = nodef->isChecked();
                                                                   v._set    = set->currentIndex();
                                                                   v._susc   = susc->currentIndex();
                                                                   v._suscTo = suscTo->text();
                                                                   v._vuln   = vuln->currentIndex();
                                                                   v._vulnTo = vulnTo->text();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dice"]   = v._dice;
                                                                   obj["pded"]   = v._body;
                                                                   obj["def"]    = v._def;
                                                                   obj["dmg"]    = v._dmg;
                                                                   obj["what"]   = v._what;
                                                                   obj["senses"] = v._senses;
                                                                   obj["groups"] = v._groups;
                                                                   obj["foci"]   = v._foci;
                                                                   obj["one"]    = v._one;
                                                                   obj["nodef"]  = v._nodef;
                                                                   obj["set"]    = v._set;
                                                                   obj["susc"]   = v._susc;
                                                                   obj["suscTo"] = v._suscTo;
                                                                   obj["vuln"]   = v._vuln;
                                                                   obj["vulnTo"] = v._vulnTo;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     _dice   = 0;
        int     _body   = 0;
        int     _def    = 0;
        int     _dmg    = -1;
        QString _what   = "";
        QString _senses = "";
        QString _groups = "";
        bool    _foci   = false;
        bool    _one    = false;
        bool    _nodef  = false;
        int     _set    = -1;
        int     _susc   = -1;
        QString _suscTo = "";
        int     _vuln   = -1;
        QString _vulnTo = "";
    } v;

    QLineEdit* dice;
    QLineEdit* body;
    QLineEdit* def;
    QComboBox* dmg;
    QLineEdit* what;
    QLineEdit* senses;
    QLineEdit* groups;
    QCheckBox* foci;
    QCheckBox* one;
    QCheckBox* nodef;
    QComboBox* set;
    QComboBox* susc;
    QLineEdit* suscTo;
    QComboBox* vuln;
    QLineEdit* vulnTo;

    QString optOut(bool showEND) {
        if (v._dice < 1 ||
            (v._dmg == 2 && v._what.isEmpty()) ||
            (v._susc >= 1 && v._suscTo.isEmpty()) ||
            (v._vuln >= 1 && v._vulnTo.isEmpty())) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("+%1").arg(v._dice) + "d6 Entangle";
        if (v._body > 0) res += " +" + QString("+%1").arg(v._body) + "d6 (Body Only)";
        if (v._def > 0) res += " +" + QString("+%1").arg(v._def) + " DEF";
        QStringList dmg { "",
                          "Both Take Damage",
                          "Takes No Damage From Certain Attacks",
                          "Transparent To All Attacks",
                          "Only Affected By Target" };
        if (v._dmg > 0) {
            res += "; " + dmg[v._dmg];
            if (v._dmg == 2) res += " (" + v._what + ")";
        }
        if (!v._senses.isEmpty()) {
            res += "; Blocks " + v._senses;
            if (!v._groups.isEmpty()) res += "and " + v._groups;
        } else if (!v._groups.isEmpty()) res += "; Blocks " + v._groups;
        if (v._foci) res += "; Allows use of Foci";
        if (v._one) res += "; 1 BODY";
        if (v._nodef) res += "; No Defense";
        if (v._set > 0) {
            if (v._set == 1) res += "; Only Hands";
            else res += "; Only Feet";
        }
        QStringList common { "", " (Uncommon)", " (Common)", " (Very Common)" };
        if (v._susc > 0) res += "; Susceptible to " + v._suscTo + common[v._susc];
        if (v._vuln > 0) res += "; Vulnerable to " + v._vulnTo + common[v._vuln];
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

class Flash: public AllPowers {
public:
    Flash(): AllPowers("Flash")                     { }
    Flash(const Flash& s): AllPowers(s)             { }
    Flash(Flash&& s): AllPowers(s)                  { }
    Flash(const QJsonObject& json): AllPowers(json) { v._dice      = json["dice"].toInt(0);
                                                      v._tgt       = json["tgt"].toString();
                                                      v._nonTgt    = json["nonTgt"].toString();
                                                      v._desolid   = json["stun"].toBool(false);
                                                      v._indTgt    = json["indTgt"].toString();
                                                      v._indNonTgt = json["indNonTgt"].toString();
                                                    }
    virtual Flash& operator=(const Flash& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Flash& operator=(Flash&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   dice      = createLineEdit(parent, layout, "Dice of Damage?", std::mem_fn(&Power::numeric));
                                                                   tgt       = createLineEdit(parent, layout, "Targeting Sense Groups?");
                                                                   indTgt    = createLineEdit(parent, layout, "Targeting Senses?");
                                                                   nonTgt    = createLineEdit(parent, layout, "Non-Targeting Sense Groups?");
                                                                   indNonTgt = createLineEdit(parent, layout, "Non-Targeting Senses?");
                                                                   desolid   = createCheckBox(parent, layout, "Does not Effect Desolid");
                                                                 }
    Fraction lim() override                                      { return v._desolid ? Fraction(1, 4) : Fraction(0); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v._dice * (!v._tgt.isEmpty() ? 5_cp : 3_cp) +
                                                                      ((countCommas(v._tgt) > 1) ? (countCommas(v._tgt) - 1) * 10_cp : 0_cp) +
                                                                      ((countCommas(v._nonTgt) > 0) ? (countCommas(v._nonTgt) - (v._tgt.isEmpty() ? 1 : 0)) * 5_cp : 0_cp) +
                                                                      countCommas(v._indTgt) * 5_cp + countCommas(v._indNonTgt) * 3_cp;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dice->setText(QString("%1").arg(s._dice));
                                                                   tgt->setText(s._tgt);
                                                                   nonTgt->setText(s._nonTgt);
                                                                   desolid->setChecked(s._desolid);
                                                                   indTgt->setText(s._indTgt);
                                                                   indNonTgt->setText(s._indNonTgt);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._dice      = dice->text().toInt();
                                                                   v._tgt       = tgt->text();
                                                                   v._nonTgt    = nonTgt->text();
                                                                   v._desolid   = desolid->isChecked();
                                                                   v._indTgt    = indTgt->text();
                                                                   v._indNonTgt = indNonTgt->text();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dice"]      = v._dice;
                                                                   obj["tgt"]       = v._tgt;
                                                                   obj["nonTgt"]    = v._nonTgt;
                                                                   obj["desolid"]   = v._desolid;
                                                                   obj["indTgt"]    = v._indTgt;
                                                                   obj["indNonTgt"] = v._indNonTgt;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     _dice      = 0;
        QString _tgt       = "";
        QString _indTgt    = "";
        QString _nonTgt    = "";
        QString _indNonTgt = "";
        bool    _desolid = false;
    } v;

    QLineEdit* dice;
    QLineEdit* tgt;
    QLineEdit* indTgt;
    QLineEdit* nonTgt;
    QLineEdit* indNonTgt;
    QCheckBox* desolid;

    QString optOut(bool showEND) {
        if (v._dice < 1 || (v._tgt.isEmpty() && v._nonTgt.isEmpty())) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("+%1").arg(v._dice) + "d6 Flash vs ";
        QString sep = "";
        if (!v._tgt.isEmpty()) { res += sep + v._tgt; sep = ", "; }
        if (!v._nonTgt.isEmpty()) { res += sep + v._nonTgt; sep = ", "; }
        if (!v._indTgt.isEmpty()) { res += sep + v._indTgt; sep = ", "; }
        if (!v._indNonTgt.isEmpty()) { res += sep + v._indNonTgt; sep = ", "; }
        if (v._desolid) res += "; Does Not Effect Desolid";
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

class HandToHandAttack: public AllPowers {
public:
    HandToHandAttack(): AllPowers("Hand-To-Hand Attack")       { }
    HandToHandAttack(const HandToHandAttack& s): AllPowers(s)  { }
    HandToHandAttack(HandToHandAttack&& s): AllPowers(s)       { }
    HandToHandAttack(const QJsonObject& json): AllPowers(json) { v._dice = json["dice"].toInt(0);
                                                                  }
    virtual HandToHandAttack& operator=(const HandToHandAttack& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual HandToHandAttack& operator=(HandToHandAttack&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   dice = createLineEdit(parent, layout, "Dice of Damage?", std::mem_fn(&Power::numeric));
                                                                 }
    Fraction lim() override                                      { return Fraction(1, 4); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return Points(v._dice * 5); }
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
        int  _dice = 0;
    } v;

    QLineEdit* dice;

    QString optOut(bool showEND) {
        if (v._dice < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("+%1").arg(v._dice) + "d6 HA; Hand-To-Hand Attack";
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

class KillingAttack: public AllPowers {
public:
    KillingAttack(): AllPowers("Killing Attack")            { }
    KillingAttack(const KillingAttack& s): AllPowers(s)     { }
    KillingAttack(KillingAttack&& s): AllPowers(s)          { }
    KillingAttack(const QJsonObject& json): AllPowers(json) { v._dice  = json["dice"].toInt(0);
                                                              v._range = json["range"].toBool(false);
                                                              v._extra = json["extra"].toInt(0);
                                                              v._pded  = json["pded"].toInt(0);
                                                              v._incr  = json["incr"].toInt(0);
                                                              v._decr  = json["decr"].toInt(0);
                                                              v._str   = json["str"].toBool(false);
                                                            }
    virtual KillingAttack& operator=(const KillingAttack& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual KillingAttack& operator=(KillingAttack&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return v._incr * Fraction(1, 4); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   range = createCheckBox(parent, layout, "Ranged");
                                                                   dice  = createLineEdit(parent, layout, "Dice of Damage?", std::mem_fn(&Power::numeric));
                                                                   extra = createComboBox(parent, layout, "Partial Dice?", { "", "+1", Fraction(1, 2).toString() + "d6" });
                                                                   pded  = createComboBox(parent, layout, "Versus?", { "PD", "ED" });
                                                                   incr  = createLineEdit(parent, layout, "STUN Multiplier Increases?", std::mem_fn(&Power::numeric));
                                                                   decr  = createComboBox(parent, layout, "STUN Multiplier Decreases?", { "None", "1x", "2x" });
                                                                   str   = createCheckBox(parent, layout, "No STR Bonus");
                                                                 }
    Fraction lim() override                                      { return (v._decr + 1) * Fraction(1, 4) + (v._str ? Fraction(1, 2) : Fraction(0)); }
    Points   points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v._dice * 15 + ((v._extra > -1) ? v._extra * 5_cp : 0_cp); }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   range->setChecked(s._range);
                                                                   dice->setText(QString("%1").arg(s._dice));
                                                                   extra->setCurrentIndex(s._extra);
                                                                   pded->setCurrentIndex(s._pded);
                                                                   incr->setText(QString("%1").arg(s._incr));
                                                                   decr->setCurrentIndex(v._decr);
                                                                   str->setChecked(v._str);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._range = range->isChecked();
                                                                   v._dice  = dice->text().toInt();
                                                                   v._extra = extra->currentIndex();
                                                                   v._pded  = pded->currentIndex();
                                                                   v._incr  = incr->text().toInt();
                                                                   v._decr  = decr->currentIndex();
                                                                   v._str   = str->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["range"] = v._range;
                                                                   obj["dice"]  = v._dice;
                                                                   obj["extra"] = v._extra;
                                                                   obj["pded"]  = v._pded;
                                                                   obj["incr"]  = v._incr;
                                                                   obj["decr"]  = v._decr;
                                                                   obj["str"]   = v._str;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        bool _range = false;
        int  _dice  = 0;
        int  _extra = -1;
        int  _pded  = -1;
        int  _incr  = 0;
        int  _decr  = 0;
        bool _str   = false;
    } v;

    QCheckBox* range;
    QLineEdit* dice;
    QComboBox* extra;
    QComboBox* pded;
    QLineEdit* incr;
    QComboBox* decr;
    QCheckBox* str;

    QString KAwSTR() {
#ifndef ISHSC
        int STR = Sheet::ref().character().STR().base() +
                  Sheet::ref().character().STR().primary() +
                  Sheet::ref().character().STR().secondary();
#else
        int STR = 0;
#endif
        int dice = v._dice + STR / 15;
        int rem = STR % 15;
        int extra = rem / 10 + 1;
        if (extra == 1) {
            if (rem >= 5) extra = 1;
            else extra = 0;
        }
        switch (v._extra) {
        case 0: break;
        case 1:
            switch (extra) {
            case 0:                    break;
            case 1: extra = 2;         break;
            case 2: dice++; extra = 0; break;
            }
            break;
        case 2:
            switch (extra) {
            case 0:                    break;
            case 1: dice++; extra = 0; break;
            case 2: dice++; extra = 1; break;
            }
            break;
        }
        return QString("%1%2d6%3").arg(dice).arg((extra == 2) ? Fraction(1, 2).toString() : "",(extra == 1) ? "+1" : "");
    }

    QString optOut(bool showEND) {
        if ((v._dice < 1 && v._extra < 1) || v._pded == -1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("+%1%2d6%3 %4").arg(v._dice).arg((v._extra == 2) ? Fraction(1, 2).toString() : "", (v._extra == 1) ? "+1" : "", v._range ? "R" : "H") + "KA vs " +
               ((v._pded == 0) ? "PD" : "ED") + ((v._range || v._str) ? "" : ", " + KAwSTR() + " w/STR");
        if (v._incr > 0) res += "; " + QString("+%1 Increased STUN Multiplier").arg(v._incr);
        if (v._decr > 0) res += "; "+ QString("-%1 Decreased STUN Multipier").arg(v._decr + 1);
        if (v._str) res += "; No STR Bonus";
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

class Reflection: public AllPowers {
public:
    Reflection(): AllPowers("Reflection")                { }
    Reflection(const Reflection& s): AllPowers(s)        { }
    Reflection(Reflection&& s): AllPowers(s)             { }
    Reflection(const QJsonObject& json): AllPowers(json) { v._points   = json["points"].toInt(0);
                                                              v._any      = json["any"].toBool(false);
                                                              v._feedback = json["feedback"].toBool(false);
                                                            }
    virtual Reflection& operator=(const Reflection& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Reflection& operator=(Reflection&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return v._any ? Fraction(1, 2) : Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   point    = createLineEdit(parent, layout, "Active Points Reflected?", std::mem_fn(&Power::numeric));
                                                                   any      = createCheckBox(parent, layout, "Any Target");
                                                                   feedback = createCheckBox(parent, layout, "Feedback");
                                                                 }
    Fraction lim() override                                      { return v._feedback ? Fraction(1) : Fraction(0); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return (v._points + 2_cp) / 3_cp * 2_cp; }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   point->setText(QString("%1").arg(s._points));
                                                                   any->setChecked(s._any);
                                                                   feedback->setChecked(s._feedback);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._points   = point->text().toInt();
                                                                   v._any      = any->isChecked();
                                                                   v._feedback = feedback->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["points"]   = v._points;
                                                                   obj["any"]      = v._any;
                                                                   obj["feedback"] = v._feedback;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int  _points   = 0;
        bool _any      = false;
        bool _feedback = false;
    } v;

    QLineEdit* point;
    QCheckBox* any;
    QCheckBox* feedback;

    QString optOut(bool showEND) {
        if (v._points < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1").arg(v._points) + " CP Reflection";
        if (v._any) res += "; Any Target";
        if (v._feedback) res += "; Feedback";
        return res;
    }

    void numeric(int) override {
        QString txt = point->text();
#ifndef ISHSC
        PowerDialog::ref().updateForm();
#endif
        if (txt.isEmpty() || isNumber(txt)) return;
        point->undo();
    }
};

class Telekinesis: public AllPowers {
public:
    Telekinesis(): AllPowers("Telekinesis")               { }
    Telekinesis(const Telekinesis& s): AllPowers(s)       { }
    Telekinesis(Telekinesis&& s): AllPowers(s)            { }
    Telekinesis(const QJsonObject& json): AllPowers(json) { v._str   = json["str"].toInt(0);
                                                            v._fine  = json["fine"].toBool(false);
                                                            v._whole = json["whole"].toBool(false);
                                                            v._limit = json["limit"].toInt(0);
                                                            v._what  = json["what"].toString();
                                                          }
    virtual Telekinesis& operator=(const Telekinesis& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Telekinesis& operator=(Telekinesis&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   STR   = createLineEdit(parent, layout, "Strength?", std::mem_fn(&Power::numeric));
                                                                   fine  = createCheckBox(parent, layout, "Fine Manipulation");
                                                                   whole = createCheckBox(parent, layout, "Affects Whole Object");
                                                                   limit = createComboBox(parent, layout, "Only Works On?", { "Everything", "Limited Group", "Very Limited Group" });
                                                                   what  = createLineEdit(parent, layout, "What?");
                                                                 }
    Fraction lim() override                                      { return (v._whole ? Fraction(1, 4) : Fraction(0)) +
                                                                          ((v._limit > 0) ? v._limit * Fraction(1, 2) : Fraction(0)); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return 3_cp * ((v._str + 1) / 2) + (v._fine ? 10_cp : 0_cp); }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   STR->setText(QString("%1").arg(s._str));
                                                                   fine->setChecked(s._fine);
                                                                   whole->setChecked(s._whole);
                                                                   limit->setCurrentIndex(s._limit);
                                                                   what->setText(s._what);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._str   = STR->text().toInt();
                                                                   v._fine  = fine->isChecked();
                                                                   v._whole = whole->isChecked();
                                                                   v._limit = limit->currentIndex();
                                                                   v._what  = what->text();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["str"]   = v._str;
                                                                   obj["fine"]  = v._fine;
                                                                   obj["whole"] = v._whole;
                                                                   obj["limit"] = v._limit;
                                                                   obj["what"]  = v._what;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     _str   = 0;
        bool    _fine  = false;
        bool    _whole = false;
        int     _limit = -1;
        QString _what = "";
    } v;

    QLineEdit* STR;
    QCheckBox* fine;
    QCheckBox* whole;
    QComboBox* limit;
    QLineEdit* what;

    QString optOut(bool showEND) {
        if (v._str < 1 || (v._limit >= 1 && v._what.isEmpty())) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1").arg(v._str) + " STR Telekinesis";
        if (v._limit > 0) res += " with " + v._what;
        if (v._fine) res += "; Fine Manipulation";
        if (v._whole) res += "; Affects Whole Object";
        return res;
    }

    void numeric(int) override {
        QString txt = STR->text();
#ifndef ISHSC
        PowerDialog::ref().updateForm();
#endif
        if (txt.isEmpty() || isNumber(txt)) return;
        STR->undo();
    }
};

class Transform: public AllPowers {
public:
    Transform(): AllPowers("Transformϴ")                { }
    Transform(const Transform& s): AllPowers(s)         { }
    Transform(Transform&& s): AllPowers(s)              { }
    Transform(const QJsonObject& json): AllPowers(json) { v._dice    = json["dice"].toInt(0);
                                                          v._into    = json["into"].toString();
                                                          v._degree  = json["degree"].toInt(0);
                                                          v._result  = json["result"].toInt(0);
                                                          v._group   = json["group"].toString();
                                                          v._partial = json["partial"].toBool(false);
                                                          v._heal    = json["heal"].toBool(false);
                                                          v._method  = json["method"].toString();
                                                          v._all     = json["all"].toBool(false);
                                                          v._target  = json["target"].toInt(0);
                                                          v._what    = json["what"].toString();
                                                          v._rapid   = json["rapid"].toInt(0);
                                                        }
    virtual Transform& operator=(const Transform& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Transform& operator=(Transform&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return ((v._result == 0) ? Fraction(1, 4) : Fraction(0)) +
                                                                          ((v._result == 1) ? Fraction(1)    : Fraction(0)) +
                                                                          (v._partial       ? Fraction(1, 2) : Fraction(0)) +
                                                                          (v._heal          ? Fraction(1, 4) : Fraction(0));
                                                                 }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   dice    = createLineEdit(parent, layout, "Dice of Transform?", std::mem_fn(&Power::numeric));
                                                                   into    = createLineEdit(parent, layout, "Into?");
                                                                   degree  = createComboBox(parent, layout, "Degree?", { "Cosmetic", "Minor", "Major", "Severe" });
                                                                   result  = createComboBox(parent, layout, "Improved Results?", { "None", "Limited Group", "Any" });
                                                                   group   = createLineEdit(parent, layout, "Group?");
                                                                   partial = createCheckBox(parent, layout, "Partial Transform");
                                                                   heal    = createCheckBox(parent, layout, "Alternate Healing Method");
                                                                   method  = createLineEdit(parent, layout, "Healing Method?");
                                                                   all     = createCheckBox(parent, layout, "All or Nothing");
                                                                   target  = createComboBox(parent, layout, "Limited Targtet?", { "Not Limited",
                                                                                                                                  "Slightly Limited",
                                                                                                                                  "Limited",
                                                                                                                                  "Very Limited" });
                                                                   what    = createLineEdit(parent, layout, "What targets?");
                                                                   rapid   = createComboBox(parent, layout, "Rapid Healing?", { "", "1 Month", "1 Week", "1 Day", "6 Hours", "1 Hour",
                                                                                                                                "20 Minutes", "5 Minutes", "1 Minute",
                                                                                                                                "1 Turn" });
                                                                 }
    Fraction lim() override                                      { return (v._all           ? Fraction(1, 2) : Fraction(0)) +
                                                                          ((v._target == 1) ? Fraction(1, 4) : Fraction(0)) +
                                                                          ((v._target == 2) ? Fraction(1, 2) : Fraction(0)) +
                                                                          ((v._target == 3) ? Fraction(1)    : Fraction(0)) +
                                                                          (v._rapid) * Fraction(1, 4);
                                                                 }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   QList<Points> cost { 0_cp, 3_cp, 5_cp, 10_cp, 15_cp };
                                                                   return v._dice * cost[v._degree + 1]; }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dice->setText(QString("%1").arg(s._dice));
                                                                   into->setText(v._into);
                                                                   degree->setCurrentIndex(s._degree);
                                                                   partial->setChecked(s._partial);
                                                                   result->setCurrentIndex(v._result);
                                                                   group->setText(v._group);
                                                                   heal->setChecked(v._heal);
                                                                   method->setText(v._method);
                                                                   all->setChecked(v._all);
                                                                   what->setText(v._what);
                                                                   target->setCurrentIndex(v._target);
                                                                   rapid->setCurrentIndex(v._rapid);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._dice    = dice->text().toInt();
                                                                   v._into    = into->text();
                                                                   v._degree  = degree->currentIndex();
                                                                   v._partial = partial->isChecked();
                                                                   v._result  = result->currentIndex();
                                                                   v._group   = group->text();
                                                                   v._heal    = heal->isChecked();
                                                                   v._method  = method->text();
                                                                   v._all     = all->isChecked();
                                                                   v._target  = target->currentIndex();
                                                                   v._what    = what->text();
                                                                   v._rapid   = rapid->currentIndex();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dice"]    = v._dice;
                                                                   obj["into"]    = v._into;
                                                                   obj["degree"]  = v._degree;
                                                                   obj["result"]  = v._result;
                                                                   obj["group"]   = v._group;
                                                                   obj["partial"] = v._partial;
                                                                   obj["heal"]    = v._heal;
                                                                   obj["method"]  = v._method;
                                                                   obj["all"]     = v._all;
                                                                   obj["target"]  = v._target;
                                                                   obj["what"]    = v._what;
                                                                   obj["rapid"]   = v._rapid;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     _dice    = 0;
        QString _into    = "";
        int     _degree  = -1;
        int     _result  = -1;
        QString _group   = "";
        bool    _partial = false;
        bool    _heal    = false;
        QString _method  = "";
        bool    _all     = false;
        int     _target  = -1;
        QString _what    = "";
        int     _rapid   = -1;
    } v;

    QLineEdit* dice;
    QLineEdit* into;
    QComboBox* degree;
    QComboBox* result;
    QLineEdit* group;
    QCheckBox* partial;
    QCheckBox* heal;
    QLineEdit* method;
    QCheckBox* all;
    QComboBox* target;
    QLineEdit* what;
    QComboBox* rapid;

    QString optOut(bool showEND) {
        if (v._dice < 1 ||
            v._into.isEmpty() ||
            v._degree == -1 ||
            (v._result == 1 && v._group.isEmpty()) ||
            (v._heal && v._method.isEmpty()) ||
            (v._target >= 1 && v._what.isEmpty())) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        QStringList degree { "Cosmetic", "Minor", "Major", "Severe" };
        res += QString("%1").arg(v._dice) + "d6 " + degree[v._degree] + " Transformϴ";
        if (v._result >= 1) {
            if (v._result == 2) res += "; To Anything";
            else res += "; To " + v._into + ", from " + v._group;
        } else res += "; To " + v._into;
        if (v._partial) res += "; Partial Transform";
        if (v._heal) res += "; Alternative Healing (" + v._method + ")";
        if (v._all) res += "; All or nothing";
        if (v._target == 1) res += "; Slightly Limited Target (" + v._what + ")";
        if (v._target == 2) res += "; Limited Target (" + v._what + ")";
        if (v._target == 3) res += "; Very Limited Target (" + v._what + ")";
        QStringList rapid { "", "1 Week", "1 Day", "6 Hours", "1 Hour",
                            "20 Minutes", "5 Minutes", "1 Minute",
                            "1 Turn" };
        if (v._rapid >= 1) res += "; Rapid Healing (" + rapid[v._rapid] + ")";
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

#endif // AllPowers_H
