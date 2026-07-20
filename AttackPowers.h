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
    Blast(const QJsonObject& json): AllPowers(json) { v.mDice = json["dice"].toInt(0);
                                                      v.mPdEd = json["pded"].toInt(0);
                                                      v.mStun = json["stun"].toBool(false);
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
                                                                   return Points(v.mDice * 5); }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dice->setText(QString("%1").arg(s.mDice));
                                                                   pded->setCurrentIndex(s.mPdEd);
                                                                   stun->setChecked(s.mStun);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mDice = dice->text().toInt();
                                                                   v.mPdEd = pded->currentIndex();
                                                                   v.mStun = stun->isChecked();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dice"] = v.mDice;
                                                                   obj["pded"] = v.mPdEd;
                                                                   obj["stun"] = v.mStun;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int  mDice = 0;
        int  mPdEd = -1;
        bool mStun = false;
    } v;

    QLineEdit* dice;
    QComboBox* pded;
    QCheckBox* stun;

    QString optOut(bool showEND) {
        if (v.mDice < 1 || v.mPdEd < 0) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1").arg(v.mDice) + "d6 Blast vs " + ((v.mPdEd == 0) ? "PD" : "ED");
        if (v.mStun) res += "; STUN Only";
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
    bool mCreate = true;
    bool mData   = true;
    int  mIndex;

public:
    ChangeEnvironment(): AllPowers("Change Environment")        { }
    ChangeEnvironment(const ChangeEnvironment& s): AllPowers(s) { }
    ChangeEnvironment(ChangeEnvironment&& s): AllPowers(s)      { }
    ChangeEnvironment(const QJsonObject& json): AllPowers(json) { QJsonArray arr = json["effects"].toArray();
                                                                  mCreate = false;
                                                                  mData   = true;
                                                                  for (int i = 0; i < arr.count(); ++i) {
                                                                      QJsonObject obj = arr[i].toObject();
                                                                      effects eff(obj["which"].toInt(0),
                                                                                  obj["idx"].toInt(0),
                                                                                  obj["level"].toInt(0),
                                                                                  { },
                                                                                  obj["val"].toString(""));
                                                                      v.mEffects.push_back(eff);
                                                                  }
                                                                  v.mLasting = json["lasting"].toInt(-1);
                                                                  v.mVarying = json["varying"].toBool(false);
                                                                  v.mEnvs    = json["envs"].toInt(-1);
                                                                  v.mWhat    = json["what"].toString("");
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

    Fraction adv() override                                      { return (v.mEnvs + 1) * Fraction(1, 4); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   effect  = createComboBox(parent, layout, "Effects?", { "",
                                                                                                                          "Movement",
                                                                                                                          "PER Roll for one Sense",
                                                                                                                          "PER Roll for one Sense Group",
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
                                                                   mCreate = true;
                                                                   mData   = false;
                                                                   mIndex  = 0;
                                                                   mLayout = layout;
                                                                   mParent = parent;
                                                                   for (auto& effect: std::as_const(v.mEffects)) { indexed(effect.which, NoUpdate); ++mIndex; }
                                                                   mData = true;
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   QJsonArray arr;
                                                                   for (auto& effect: std::as_const(v.mEffects)) {
                                                                       QJsonObject eff;
                                                                       eff["which"] = effect.which;
                                                                       eff["idx"]   = effect.idx;
                                                                       eff["level"] = effect.level;
                                                                       eff["val"]   = effect.val;
                                                                       arr.append(eff);
                                                                   }
                                                                   obj["effects"] = arr;
                                                                   obj["lasting"] = v.mLasting;
                                                                   obj["varying"] = v.mVarying;
                                                                   obj["envs"]    = v.mEnvs;
                                                                   obj["what"]    = v.mWhat;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        QList<effects> mEffects;
        int            mLasting = -1;
        bool           mVarying = false;
        int            mEnvs    = -1;
        QString        mWhat    = "";
    } v;
    QVBoxLayout* mLayout;
    QWidget*     mParent;

    QComboBox*   effect;
    QComboBox*   lasting;
    QCheckBox*   varying;
    QComboBox*   envs;
    QLineEdit*   what;

    void restore() override {
        vars s = v;
        AllPowers::restore();
        for (auto& effect: std::as_const(s.mEffects)) {
            int count = effect.widgets.count();
            for (int i = 0; i < count; ++i) {
                QLineEdit* edit = dynamic_cast<QLineEdit*>(effect.widgets[i]);
                if (edit) edit->setText(effect.val);
                QComboBox* combo = dynamic_cast<QComboBox*>(effect.widgets[i]);
                if (combo) combo->setCurrentIndex(effect.idx);
            }
        }
        lasting->setCurrentIndex(s.mLasting);
        varying->setChecked(s.mVarying);
        envs->setCurrentIndex(s.mEnvs);
        what->setText(s.mWhat);
        v = s;
        mCreate = true;
        mData   = true;
    }

    void store() override {
        AllPowers::store();
        for (auto& effect: v.mEffects) {
            int count = effect.widgets.count();
            for (int i = 0; i < count - 1; ++i) {
                QLineEdit* edit = dynamic_cast<QLineEdit*>(effect.widgets[i]);
                if (edit) effect.val = edit->text();
                QComboBox* combo = dynamic_cast<QComboBox*>(effect.widgets[i]);
                if (combo) effect.idx = combo->currentIndex();
            }
        }
        v.mLasting = lasting->currentIndex();
        v.mVarying = varying->isChecked();
        v.mEnvs    = envs->currentIndex();
        v.mWhat    = what->text();
    }

    void deleteEffect(QWidget* w) {
        for (auto effect = v.mEffects.cbegin(); effect != v.mEffects.cend(); ++effect) {
            for (const auto& wgt: std::as_const(effect->widgets)) if (w == wgt) {
                for (const auto& wgt: effect->widgets) wgt->deleteLater();
                v.mEffects.erase(effect);
                return;
            }
        }
    }

    effects& findEffect(QWidget* w) {
        for (auto& effect: v.mEffects) {
            for (int i = 0; i < effect.widgets.count(); ++i) {
                auto& wgt = effect.widgets[i];
                if (w == wgt) return effect;
            }
        }
        static effects none { -1, -1, -1, { }, "" };
        return none;
    }

    Points points(bool noStore = false) override {
        if (!noStore) store();
        Points pts = (v.mLasting + 1) * 2_cp;
        if (v.mVarying) pts += 10_cp;
        for (int i = 0; i < v.mEffects.count(); ++i) {
            auto& effect = v.mEffects[i];
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
        int idx = mLayout->indexOf(lasting);
        QList<QWidget*> widgets { };
        switch (pick) {
        case 0:
            break;
        case 1:  // Movement
            if (mCreate) {
                widgets.emplaceBack(createComboBox(mParent, mLayout, "Movement Type?", { "",
                                                                                         "Running",
                                                                                         "Swimming",
                                                                                         "Jumping",
                                                                                         "Flight",
                                                                                         "Teleportation",
                                                                                         "Tunneling"
                                                                                       }, idx));
                widgets.emplaceBack(createLineEdit(mParent, mLayout, "Movement Penalty?", std::mem_fn(&Power::numeric), idx + 1));
                if (mData) v.mEffects.emplaceBack(0, -1, 0, widgets);
                else v.mEffects[mIndex].widgets = widgets;
            } else if (mData) v.mEffects.emplaceBack(0, -1, 0, widgets);
            break;
        case 2:  // PER Roll for one Sense
            if (mCreate) {
                widgets.emplaceBack(createComboBox(mParent, mLayout, "Sense?", { "",
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
                widgets.emplaceBack(createLineEdit(mParent, mLayout, "PER Roll Penalty?", std::mem_fn(&Power::numeric), idx + 1));
                if (mData) v.mEffects.emplaceBack(1, -1, 0, widgets);
                else v.mEffects[mIndex].widgets = widgets;
            } else if (mData) v.mEffects.emplaceBack(1, -1, 0, widgets);
            break;
        case 3:  // PER Rolll for one Sense Group
            if (mCreate) {
                widgets.emplaceBack(createComboBox(mParent, mLayout, "Sense Group?", { "",
                                                                                       "Hearing",
                                                                                       "Mental",
                                                                                       "Radio",
                                                                                       "Sight",
                                                                                       "Smell/Taste",
                                                                                       "Touch"
                                                                                     }, idx));
                widgets.emplaceBack(createLineEdit(mParent, mLayout, "PER Roll Penalty?", std::mem_fn(&Power::numeric), idx + 1));
                if (mData) v.mEffects.emplaceBack(2, -1, 0, widgets);
                else v.mEffects[mIndex].widgets = widgets;
            } else if (mData) v.mEffects.emplaceBack(2, -1, 0, widgets);
            break;
        case 4:  // Characteristic Roll
            if (mCreate) {
                widgets.emplaceBack(createComboBox(mParent, mLayout, "Charactreristic?", { "",
                                                                                           "STR",
                                                                                           "DEX",
                                                                                           "CON",
                                                                                           "INT",
                                                                                           "EGO",
                                                                                           "PRE"
                                                                                          }, idx));
                widgets.emplaceBack(createLineEdit(mParent, mLayout, "Characteristic Roll Penalty?", std::mem_fn(&Power::numeric), idx + 1));
                if (mData) v.mEffects.emplaceBack(3, -1, 0, widgets);
                else v.mEffects[mIndex].widgets = widgets;
            } else if (mData) v.mEffects.emplaceBack(3, -1, 0, widgets);
            break;
        case 5:  // Skill Roll
            if (mCreate) {
                widgets.emplaceBack(createLineEdit(mParent, mLayout, "Skills?", idx));
                widgets.emplaceBack(createLineEdit(mParent, mLayout, "Skill Roll Penalty?", std::mem_fn(&Power::numeric), idx + 1));
                if (mData) v.mEffects.emplaceBack(4, -1, 0, widgets);
                else v.mEffects[mIndex].widgets = widgets;
            } else if (mData) v.mEffects.emplaceBack(4, -1, 0, widgets);
            break;
        case 6:  // Increase Temperature
            if (mCreate) {
                widgets.emplaceBack(createLineEdit(mParent, mLayout, "Temperature Levels Increase?", std::mem_fn(&Power::numeric), idx));
                if (mData) v.mEffects.emplaceBack(5, -1, 0, widgets);
                else v.mEffects[mIndex].widgets = widgets;
            } else if (mData) v.mEffects.emplaceBack(5, -1, 0, widgets);
            break;
        case 7:  // Decrease Temperature
            if (mCreate) {
                widgets.emplaceBack(createLineEdit(mParent, mLayout, "Temperature Levels Decrease?", std::mem_fn(&Power::numeric), idx));
                if (mData) v.mEffects.emplaceBack(6, -1, 0, widgets);
                else v.mEffects[mIndex].widgets = widgets;
            } else if (mData) v.mEffects.emplaceBack(6, -1, 0, widgets);
            break;
        case 8:  // Range Modifier
            if (mCreate) {
                widgets.emplaceBack(createLineEdit(mParent, mLayout, "Range Modifier Penalty?", std::mem_fn(&Power::numeric), idx));
                if (mData) v.mEffects.emplaceBack(7, -1, 0, widgets);
                else v.mEffects[mIndex].widgets = widgets;
            } else if (mData) v.mEffects.emplaceBack(7, -1, 0, widgets);
            break;
        case 9:  // Negative Combat Modifier▲
            if (mCreate) {
                widgets.emplaceBack(createLineEdit(mParent, mLayout, "Combat Modifier▲?", idx));
                widgets.emplaceBack(createLineEdit(mParent, mLayout, "Penalty?", std::mem_fn(&Power::numeric), idx + 1));
                if (mData) v.mEffects.emplaceBack(8, -1, 0, widgets);
                else v.mEffects[mIndex].widgets = widgets;
            } else if (mData) v.mEffects.emplaceBack(8, -1, 0, widgets);
            break;
        case 10:  // Characteristic Roll and all assoc. Skill Rolls
            if (mCreate) {
                widgets.emplaceBack(createComboBox(mParent, mLayout, "Charactreristic?", { "",
                                                                                           "STR",
                                                                                           "DEX",
                                                                                           "CON",
                                                                                           "INT",
                                                                                           "EGO",
                                                                                           "PRE"
                                                                                          }, idx));
                widgets.emplaceBack(createLineEdit(mParent, mLayout, "Characteristicc & Skill Roll Penalty?", std::mem_fn(&Power::numeric), idx + 1));
                if (mData) v.mEffects.emplaceBack(9, -1, 0, widgets);
                else v.mEffects[mIndex].widgets = widgets;
            } else if (mData) v.mEffects.emplaceBack(9, -1, 0, widgets);
            break;
        case 11: // Damage
            if (mCreate) {
                widgets.emplaceBack(createLineEdit(mParent, mLayout, "Damage Points?", std::mem_fn(&Power::numeric), idx));
                if (mData) v.mEffects.emplaceBack(10, -1, 0, widgets);
                else v.mEffects[mIndex].widgets = widgets;
            } else if (mData) v.mEffects.emplaceBack(10, -1, 0, widgets);
            break;
        case 12: // Telekinesis
            if (mCreate) {
                widgets.emplaceBack(createLineEdit(mParent, mLayout, "Telekinetic STR?", std::mem_fn(&Power::numeric), idx));
                if (mData) v.mEffects.emplaceBack(11, -1, 0, widgets);
                else v.mEffects[mIndex].widgets = widgets;
            } else if (mData) v.mEffects.emplaceBack(11, -1, 0, widgets);
            break;
        case 13: // Increase Wind Level
            if (mCreate) {
                widgets.emplaceBack(createLineEdit(mParent, mLayout, "Wind Increase?", std::mem_fn(&Power::numeric), idx));
                if (mData) v.mEffects.emplaceBack(12, -1, 0, widgets);
                else v.mEffects[mIndex].widgets = widgets;
            } else if (mData) v.mEffects.emplaceBack(12, -1, 0, widgets);
            break;
        case 14: // Decrease Wind Level
            if (mCreate) {
                widgets.emplaceBack(createLineEdit(mParent, mLayout, "Wind Decrease?", std::mem_fn(&Power::numeric), idx));
                if (mData) v.mEffects.emplaceBack(13, -1, 0, widgets);
                else v.mEffects[mIndex].widgets = widgets;
            } else if (mData) v.mEffects.emplaceBack(13, -1, 0, widgets);
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
        if (v.mEffects.isEmpty() || (v.mEnvs > 0 && v.mWhat.isEmpty())) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "Change Environment: ";
        QString sep = "";
        for (int i = 0; i < v.mEffects.count(); ++ i) {
            auto& effect = v.mEffects[i];
            res += sep + describe(effect);
            sep = ", ";
        }
        sep = " (";
        QStringList lasting { "1 Turn", "1 Minute", "5 Minutes", "20 Minutes",
                              "1 Hour", "6 Hours", "1 Day", "1 Week",
                              "1 Month", "1 Season", "1 Year", "5 Years" };
        if (v.mLasting != -1) { res += sep + "Long Lasting: " + lasting[v.mLasting]; sep = "; "; }
        if (v.mVarying) { res += sep + "Varying Combat Effects "; sep = "; "; }
        if (v.mEnvs > 0) { res += sep + " Varying Environments (" + v.mWhat + ")"; sep += "; "; }
        if (sep == "; ") res += ")";
        return res;
    }

    void numeric(int) override {
        QLineEdit* line = dynamic_cast<QLineEdit*>(sender());
        QString txt = line->text();
        if (!txt.isEmpty() && txt == "0") {
            line->undo();
            effects& effect = findEffect(line);
            if (effect.widgets.count() != 0) {
                deleteEffect(line);
                mLayout->update();
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
    Entangle(const QJsonObject& json): AllPowers(json) { v.mDice   = json["dice"].toInt(0);
                                                            v.mBody   = json["body"].toInt(0);
                                                            v.mDef    = json["def"].toInt(0);
                                                            v.mDmg    = json["dmg"].toInt(0);
                                                            v.mWhat   = json["what"].toString();
                                                            v.mSenses = json["senses"].toString();
                                                            v.mGroups = json["groups"].toString();
                                                            v.mFoci   = json["foci"].toBool(false);
                                                            v.mOne    = json["one"].toBool(false);
                                                            v.mNoDef  = json["nodef"].toBool(false);
                                                            v.mSet    = json["set"].toInt(0);
                                                            v.mSusc   = json["susc"].toInt(0);
                                                            v.mSuscTo = json["suscTo"].toString();
                                                            v.mVuln   = json["vuln"].toInt(0);
                                                            v.mVulnTo = json["vulnTo"].toString();
                                                          }

    Fraction adv() override                                      { return ((v.mDmg == 0) ? Fraction(1, 4) : Fraction(0)) +
                                                                          ((v.mDmg == 1) ? Fraction(1, 4) : Fraction(0)) +
                                                                          ((v.mDmg == 2) ? Fraction(1, 2) : Fraction(0)) +
                                                                          ((v.mDmg == 3) ? Fraction(1)    : Fraction(0)); }
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
    Fraction lim() override                                      { return (v.mFoci        ? Fraction(1)                 : Fraction(0)) +
                                                                          (v.mOne         ? Fraction(1, 2)              : Fraction(0)) +
                                                                          (v.mNoDef       ? Fraction(1, Fraction(1, 2)) : Fraction(0)) +
                                                                          ((v.mSet >= 1)  ? Fraction(1)                 : Fraction(0)) +
                                                                          ((v.mSusc == 1) ? Fraction(1, 4)              : Fraction(0)) +
                                                                          ((v.mSusc == 2) ? Fraction(1, 2)              : Fraction(0)) +
                                                                          ((v.mSusc == 3) ? Fraction(1)                 : Fraction(0)) +
                                                                          ((v.mVuln == 1) ? Fraction(1, 4)              : Fraction(0)) +
                                                                          ((v.mVuln == 2) ? Fraction(1, 2)              : Fraction(0)) +
                                                                          ((v.mVuln == 3) ? Fraction(1)                 : Fraction(0)); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v.mDice * 10_cp +
                                                                          v.mBody * 5_cp +
                                                                          ((v.mDef + 1) / 2) * 5_cp +
                                                                          countCommas(v.mSenses) * 5_cp +
                                                                          countCommas(v.mGroups) * 10_cp; }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dice->setText(QString("%1").arg(s.mDice));
                                                                   body->setText(QString("%1").arg(s.mBody));
                                                                   def->setText(QString("%1").arg(s.mDef));
                                                                   dmg->setCurrentIndex(s.mDmg);
                                                                   what->setText(s.mWhat);
                                                                   senses->setText(s.mSenses);
                                                                   groups->setText(s.mGroups);
                                                                   one->setChecked(s.mOne);
                                                                   nodef->setChecked(s.mNoDef);
                                                                   set->setCurrentIndex(s.mSet);
                                                                   susc->setCurrentIndex(s.mSusc);
                                                                   suscTo->setText(s.mSuscTo);
                                                                   vuln->setCurrentIndex(s.mVuln);
                                                                   vulnTo->setText(s.mVulnTo);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mDice   = dice->text().toInt();
                                                                   v.mBody   = body->text().toInt();
                                                                   v.mDef    = def->text().toInt();
                                                                   v.mDmg    = dmg->currentIndex();
                                                                   v.mWhat   = what->text();
                                                                   v.mSenses = senses->text();
                                                                   v.mGroups = groups->text();
                                                                   v.mFoci   = foci->isChecked();
                                                                   v.mOne    = one->isChecked();
                                                                   v.mNoDef  = nodef->isChecked();
                                                                   v.mSet    = set->currentIndex();
                                                                   v.mSusc   = susc->currentIndex();
                                                                   v.mSuscTo = suscTo->text();
                                                                   v.mVuln   = vuln->currentIndex();
                                                                   v.mVulnTo = vulnTo->text();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dice"]   = v.mDice;
                                                                   obj["pded"]   = v.mBody;
                                                                   obj["def"]    = v.mDef;
                                                                   obj["dmg"]    = v.mDmg;
                                                                   obj["what"]   = v.mWhat;
                                                                   obj["senses"] = v.mSenses;
                                                                   obj["groups"] = v.mGroups;
                                                                   obj["foci"]   = v.mFoci;
                                                                   obj["one"]    = v.mOne;
                                                                   obj["nodef"]  = v.mNoDef;
                                                                   obj["set"]    = v.mSet;
                                                                   obj["susc"]   = v.mSusc;
                                                                   obj["suscTo"] = v.mSuscTo;
                                                                   obj["vuln"]   = v.mVuln;
                                                                   obj["vulnTo"] = v.mVulnTo;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     mDice   = 0;
        int     mBody   = 0;
        int     mDef    = 0;
        int     mDmg    = -1;
        QString mWhat   = "";
        QString mSenses = "";
        QString mGroups = "";
        bool    mFoci   = false;
        bool    mOne    = false;
        bool    mNoDef  = false;
        int     mSet    = -1;
        int     mSusc   = -1;
        QString mSuscTo = "";
        int     mVuln   = -1;
        QString mVulnTo = "";
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
        if (v.mDice < 1 ||
            (v.mDmg == 2 && v.mWhat.isEmpty()) ||
            (v.mSusc >= 1 && v.mSuscTo.isEmpty()) ||
            (v.mVuln >= 1 && v.mVulnTo.isEmpty())) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("+%1").arg(v.mDice) + "d6 Entangle";
        if (v.mBody > 0) res += " +" + QString("+%1").arg(v.mBody) + "d6 (Body Only)";
        if (v.mDef > 0) res += " +" + QString("+%1").arg(v.mDef) + " DEF";
        QStringList dmg { "",
                          "Both Take Damage",
                          "Takes No Damage From Certain Attacks",
                          "Transparent To All Attacks",
                          "Only Affected By Target" };
        if (v.mDmg > 0) {
            res += "; " + dmg[v.mDmg];
            if (v.mDmg == 2) res += " (" + v.mWhat + ")";
        }
        if (!v.mSenses.isEmpty()) {
            res += "; Blocks " + v.mSenses;
            if (!v.mGroups.isEmpty()) res += "and " + v.mGroups;
        } else if (!v.mGroups.isEmpty()) res += "; Blocks " + v.mGroups;
        if (v.mFoci) res += "; Allows use of Foci";
        if (v.mOne) res += "; 1 BODY";
        if (v.mNoDef) res += "; No Defense";
        if (v.mSet > 0) {
            if (v.mSet == 1) res += "; Only Hands";
            else res += "; Only Feet";
        }
        QStringList common { "", " (Uncommon)", " (Common)", " (Very Common)" };
        if (v.mSusc > 0) res += "; Susceptible to " + v.mSuscTo + common[v.mSusc];
        if (v.mVuln > 0) res += "; Vulnerable to " + v.mVulnTo + common[v.mVuln];
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
    Flash(const QJsonObject& json): AllPowers(json) { v.mDice      = json["dice"].toInt(0);
                                                      v.mTgt       = json["tgt"].toString();
                                                      v.mNonTgt    = json["nonTgt"].toString();
                                                      v.mDesolid   = json["stun"].toBool(false);
                                                      v.mIndTgt    = json["indTgt"].toString();
                                                      v.mIndNonTgt = json["indNonTgt"].toString();
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
    Fraction lim() override                                      { return v.mDesolid ? Fraction(1, 4) : Fraction(0); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v.mDice * (!v.mTgt.isEmpty() ? 5_cp : 3_cp) +
                                                                      ((countCommas(v.mTgt) > 1) ? (countCommas(v.mTgt) - 1) * 10_cp : 0_cp) +
                                                                      ((countCommas(v.mNonTgt) > 0) ? (countCommas(v.mNonTgt) - (v.mTgt.isEmpty() ? 1 : 0)) * 5_cp : 0_cp) +
                                                                      countCommas(v.mIndTgt) * 5_cp + countCommas(v.mIndNonTgt) * 3_cp;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dice->setText(QString("%1").arg(s.mDice));
                                                                   tgt->setText(s.mTgt);
                                                                   nonTgt->setText(s.mNonTgt);
                                                                   desolid->setChecked(s.mDesolid);
                                                                   indTgt->setText(s.mIndTgt);
                                                                   indNonTgt->setText(s.mIndNonTgt);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mDice      = dice->text().toInt();
                                                                   v.mTgt       = tgt->text();
                                                                   v.mNonTgt    = nonTgt->text();
                                                                   v.mDesolid   = desolid->isChecked();
                                                                   v.mIndTgt    = indTgt->text();
                                                                   v.mIndNonTgt = indNonTgt->text();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dice"]      = v.mDice;
                                                                   obj["tgt"]       = v.mTgt;
                                                                   obj["nonTgt"]    = v.mNonTgt;
                                                                   obj["desolid"]   = v.mDesolid;
                                                                   obj["indTgt"]    = v.mIndTgt;
                                                                   obj["indNonTgt"] = v.mIndNonTgt;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     mDice      = 0;
        QString mTgt       = "";
        QString mIndTgt    = "";
        QString mNonTgt    = "";
        QString mIndNonTgt = "";
        bool    mDesolid = false;
    } v;

    QLineEdit* dice;
    QLineEdit* tgt;
    QLineEdit* indTgt;
    QLineEdit* nonTgt;
    QLineEdit* indNonTgt;
    QCheckBox* desolid;

    QString optOut(bool showEND) {
        if (v.mDice < 1 || (v.mTgt.isEmpty() && v.mNonTgt.isEmpty())) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("+%1").arg(v.mDice) + "d6 Flash vs ";
        QString sep = "";
        if (!v.mTgt.isEmpty()) { res += sep + v.mTgt; sep = ", "; }
        if (!v.mNonTgt.isEmpty()) { res += sep + v.mNonTgt; sep = ", "; }
        if (!v.mIndTgt.isEmpty()) { res += sep + v.mIndTgt; sep = ", "; }
        if (!v.mIndNonTgt.isEmpty()) { res += sep + v.mIndNonTgt; sep = ", "; }
        if (v.mDesolid) res += "; Does Not Effect Desolid";
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
    HandToHandAttack(const QJsonObject& json): AllPowers(json) { v.mDice = json["dice"].toInt(0);
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
                                                                   return Points(v.mDice * 5); }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dice->setText(QString("%1").arg(s.mDice));
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mDice = dice->text().toInt();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dice"] = v.mDice;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int  mDice = 0;
    } v;

    QLineEdit* dice;

    QString optOut(bool showEND) {
        if (v.mDice < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("+%1").arg(v.mDice) + "d6 HA; Hand-To-Hand Attack";
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
    KillingAttack(const QJsonObject& json): AllPowers(json) { v.mDice  = json["dice"].toInt(0);
                                                              v.mRange = json["range"].toBool(false);
                                                              v.mExtra = json["extra"].toInt(0);
                                                              v.mPdEd  = json["pded"].toInt(0);
                                                              v.mIncr  = json["incr"].toInt(0);
                                                              v.mDecr  = json["decr"].toInt(0);
                                                              v.mStr   = json["str"].toBool(false);
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

    Fraction adv() override                                      { return v.mIncr * Fraction(1, 4); }
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
    Fraction lim() override                                      { return (v.mDecr + 1) * Fraction(1, 4) + (v.mStr ? Fraction(1, 2) : Fraction(0)); }
    Points   points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v.mDice * 15 + ((v.mExtra > -1) ? v.mExtra * 5_cp : 0_cp); }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   range->setChecked(s.mRange);
                                                                   dice->setText(QString("%1").arg(s.mDice));
                                                                   extra->setCurrentIndex(s.mExtra);
                                                                   pded->setCurrentIndex(s.mPdEd);
                                                                   incr->setText(QString("%1").arg(s.mIncr));
                                                                   decr->setCurrentIndex(v.mDecr);
                                                                   str->setChecked(v.mStr);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mRange = range->isChecked();
                                                                   v.mDice  = dice->text().toInt();
                                                                   v.mExtra = extra->currentIndex();
                                                                   v.mPdEd  = pded->currentIndex();
                                                                   v.mIncr  = incr->text().toInt();
                                                                   v.mDecr  = decr->currentIndex();
                                                                   v.mStr   = str->isChecked();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["range"] = v.mRange;
                                                                   obj["dice"]  = v.mDice;
                                                                   obj["extra"] = v.mExtra;
                                                                   obj["pded"]  = v.mPdEd;
                                                                   obj["incr"]  = v.mIncr;
                                                                   obj["decr"]  = v.mDecr;
                                                                   obj["str"]   = v.mStr;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        bool mRange = false;
        int  mDice  = 0;
        int  mExtra = -1;
        int  mPdEd  = -1;
        int  mIncr  = 0;
        int  mDecr  = 0;
        bool mStr   = false;
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
        int dice = v.mDice + STR / 15;
        int rem = STR % 15;
        int extra = rem / 10 + 1;
        if (extra == 1) {
            if (rem >= 5) extra = 1;
            else extra = 0;
        }
        switch (v.mExtra) {
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
        if ((v.mDice < 1 && v.mExtra < 1) || v.mPdEd == -1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("+%1%2d6%3 %4").arg(v.mDice).arg((v.mExtra == 2) ? Fraction(1, 2).toString() : "", (v.mExtra == 1) ? "+1" : "", v.mRange ? "R" : "H") + "KA vs " +
               ((v.mPdEd == 0) ? "PD" : "ED") + ((v.mRange || v.mStr) ? "" : ", " + KAwSTR() + " w/STR");
        if (v.mIncr > 0) res += "; " + QString("+%1 Increased STUN Multiplier").arg(v.mIncr);
        if (v.mDecr > 0) res += "; "+ QString("-%1 Decreased STUN Multipier").arg(v.mDecr + 1);
        if (v.mStr) res += "; No STR Bonus";
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
    Reflection(const QJsonObject& json): AllPowers(json) { v.mPoints   = json["points"].toInt(0);
                                                              v.mAny      = json["any"].toBool(false);
                                                              v.mFeedback = json["feedback"].toBool(false);
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

    Fraction adv() override                                      { return v.mAny ? Fraction(1, 2) : Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   point    = createLineEdit(parent, layout, "Active Points Reflected?", std::mem_fn(&Power::numeric));
                                                                   any      = createCheckBox(parent, layout, "Any Target");
                                                                   feedback = createCheckBox(parent, layout, "Feedback");
                                                                 }
    Fraction lim() override                                      { return v.mFeedback ? Fraction(1) : Fraction(0); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return (v.mPoints + 2_cp) / 3_cp * 2_cp; }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   point->setText(QString("%1").arg(s.mPoints));
                                                                   any->setChecked(s.mAny);
                                                                   feedback->setChecked(s.mFeedback);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mPoints   = point->text().toInt();
                                                                   v.mAny      = any->isChecked();
                                                                   v.mFeedback = feedback->isChecked();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["points"]   = v.mPoints;
                                                                   obj["any"]      = v.mAny;
                                                                   obj["feedback"] = v.mFeedback;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int  mPoints   = 0;
        bool mAny      = false;
        bool mFeedback = false;
    } v;

    QLineEdit* point;
    QCheckBox* any;
    QCheckBox* feedback;

    QString optOut(bool showEND) {
        if (v.mPoints < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1").arg(v.mPoints) + " CP Reflection";
        if (v.mAny) res += "; Any Target";
        if (v.mFeedback) res += "; Feedback";
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
    Telekinesis(const QJsonObject& json): AllPowers(json) { v.mStr   = json["str"].toInt(0);
                                                            v.mFine  = json["fine"].toBool(false);
                                                            v.mWhole = json["whole"].toBool(false);
                                                            v.mLimit = json["limit"].toInt(0);
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
    Fraction lim() override                                      { return (v.mWhole ? Fraction(1, 4) : Fraction(0)) +
                                                                          ((v.mLimit > 0) ? v.mLimit * Fraction(1, 2) : Fraction(0)); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return 3_cp * ((v.mStr + 1) / 2) + (v.mFine ? 10_cp : 0_cp); }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   STR->setText(QString("%1").arg(s.mStr));
                                                                   fine->setChecked(s.mFine);
                                                                   whole->setChecked(s.mWhole);
                                                                   limit->setCurrentIndex(s.mLimit);
                                                                   what->setText(s._what);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mStr   = STR->text().toInt();
                                                                   v.mFine  = fine->isChecked();
                                                                   v.mWhole = whole->isChecked();
                                                                   v.mLimit = limit->currentIndex();
                                                                   v._what  = what->text();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["str"]   = v.mStr;
                                                                   obj["fine"]  = v.mFine;
                                                                   obj["whole"] = v.mWhole;
                                                                   obj["limit"] = v.mLimit;
                                                                   obj["what"]  = v._what;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     mStr   = 0;
        bool    mFine  = false;
        bool    mWhole = false;
        int     mLimit = -1;
        QString _what = "";
    } v;

    QLineEdit* STR;
    QCheckBox* fine;
    QCheckBox* whole;
    QComboBox* limit;
    QLineEdit* what;

    QString optOut(bool showEND) {
        if (v.mStr < 1 || (v.mLimit >= 1 && v._what.isEmpty())) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1").arg(v.mStr) + " STR Telekinesis";
        if (v.mLimit > 0) res += " with " + v._what;
        if (v.mFine) res += "; Fine Manipulation";
        if (v.mWhole) res += "; Affects Whole Object";
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
    Transform(const QJsonObject& json): AllPowers(json) { v.mDice    = json["dice"].toInt(0);
                                                          v.mInto    = json["into"].toString();
                                                          v.mDegree  = json["degree"].toInt(0);
                                                          v.mResult  = json["result"].toInt(0);
                                                          v.mGroup   = json["group"].toString();
                                                          v.mPartial = json["partial"].toBool(false);
                                                          v.mHeal    = json["heal"].toBool(false);
                                                          v.mMethod  = json["method"].toString();
                                                          v.mAll     = json["all"].toBool(false);
                                                          v.mTarget  = json["target"].toInt(0);
                                                          v.mWhat    = json["what"].toString();
                                                          v.mRapid   = json["rapid"].toInt(0);
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

    Fraction adv() override                                      { return ((v.mResult == 0) ? Fraction(1, 4) : Fraction(0)) +
                                                                          ((v.mResult == 1) ? Fraction(1)    : Fraction(0)) +
                                                                          (v.mPartial       ? Fraction(1, 2) : Fraction(0)) +
                                                                          (v.mHeal          ? Fraction(1, 4) : Fraction(0));
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
    Fraction lim() override                                      { return (v.mAll           ? Fraction(1, 2) : Fraction(0)) +
                                                                          ((v.mTarget == 1) ? Fraction(1, 4) : Fraction(0)) +
                                                                          ((v.mTarget == 2) ? Fraction(1, 2) : Fraction(0)) +
                                                                          ((v.mTarget == 3) ? Fraction(1)    : Fraction(0)) +
                                                                          (v.mRapid) * Fraction(1, 4);
                                                                 }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   QList<Points> cost { 0_cp, 3_cp, 5_cp, 10_cp, 15_cp };
                                                                   return v.mDice * cost[v.mDegree + 1]; }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   dice->setText(QString("%1").arg(s.mDice));
                                                                   into->setText(v.mInto);
                                                                   degree->setCurrentIndex(s.mDegree);
                                                                   partial->setChecked(s.mPartial);
                                                                   result->setCurrentIndex(v.mResult);
                                                                   group->setText(v.mGroup);
                                                                   heal->setChecked(v.mHeal);
                                                                   method->setText(v.mMethod);
                                                                   all->setChecked(v.mAll);
                                                                   what->setText(v.mWhat);
                                                                   target->setCurrentIndex(v.mTarget);
                                                                   rapid->setCurrentIndex(v.mRapid);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mDice    = dice->text().toInt();
                                                                   v.mInto    = into->text();
                                                                   v.mDegree  = degree->currentIndex();
                                                                   v.mPartial = partial->isChecked();
                                                                   v.mResult  = result->currentIndex();
                                                                   v.mGroup   = group->text();
                                                                   v.mHeal    = heal->isChecked();
                                                                   v.mMethod  = method->text();
                                                                   v.mAll     = all->isChecked();
                                                                   v.mTarget  = target->currentIndex();
                                                                   v.mWhat    = what->text();
                                                                   v.mRapid   = rapid->currentIndex();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["dice"]    = v.mDice;
                                                                   obj["into"]    = v.mInto;
                                                                   obj["degree"]  = v.mDegree;
                                                                   obj["result"]  = v.mResult;
                                                                   obj["group"]   = v.mGroup;
                                                                   obj["partial"] = v.mPartial;
                                                                   obj["heal"]    = v.mHeal;
                                                                   obj["method"]  = v.mMethod;
                                                                   obj["all"]     = v.mAll;
                                                                   obj["target"]  = v.mTarget;
                                                                   obj["what"]    = v.mWhat;
                                                                   obj["rapid"]   = v.mRapid;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     mDice    = 0;
        QString mInto    = "";
        int     mDegree  = -1;
        int     mResult  = -1;
        QString mGroup   = "";
        bool    mPartial = false;
        bool    mHeal    = false;
        QString mMethod  = "";
        bool    mAll     = false;
        int     mTarget  = -1;
        QString mWhat    = "";
        int     mRapid   = -1;
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
        if (v.mDice < 1 ||
            v.mInto.isEmpty() ||
            v.mDegree == -1 ||
            (v.mResult == 1 && v.mGroup.isEmpty()) ||
            (v.mHeal && v.mMethod.isEmpty()) ||
            (v.mTarget >= 1 && v.mWhat.isEmpty())) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        QStringList degree { "Cosmetic", "Minor", "Major", "Severe" };
        res += QString("%1").arg(v.mDice) + "d6 " + degree[v.mDegree] + " Transformϴ";
        if (v.mResult >= 1) {
            if (v.mResult == 2) res += "; To Anything";
            else res += "; To " + v.mInto + ", from " + v.mGroup;
        } else res += "; To " + v.mInto;
        if (v.mPartial) res += "; Partial Transform";
        if (v.mHeal) res += "; Alternative Healing (" + v.mMethod + ")";
        if (v.mAll) res += "; All or nothing";
        if (v.mTarget == 1) res += "; Slightly Limited Target (" + v.mWhat + ")";
        if (v.mTarget == 2) res += "; Limited Target (" + v.mWhat + ")";
        if (v.mTarget == 3) res += "; Very Limited Target (" + v.mWhat + ")";
        QStringList rapid { "", "1 Week", "1 Day", "6 Hours", "1 Hour",
                            "20 Minutes", "5 Minutes", "1 Minute",
                            "1 Turn" };
        if (v.mRapid >= 1) res += "; Rapid Healing (" + rapid[v.mRapid] + ")";
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
