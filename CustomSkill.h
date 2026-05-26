#pragma once

#include "character.h"
#ifndef ISHSC
#include "sheet.h"
#endif
#include "skilltalentorperk.h"

class CustomSkill: public SkillTalentOrPerk {
public:
    CustomSkill(): SkillTalentOrPerk()
        , v({ "Custom" }) { }
    CustomSkill(const CustomSkill& s)
        : SkillTalentOrPerk()
        , v(s.v) { }
    CustomSkill(CustomSkill&& s)
        : SkillTalentOrPerk()
        , v(s.v) { }
    CustomSkill(const QJsonObject& json)
        : SkillTalentOrPerk()
        , v { json["name"].toString(""),
              json["descr"].toString(""),
              json["stat"].toInt(0),
              json["plus"].toInt(0) } { }
    ~CustomSkill() override { }

    CustomSkill& operator=(const CustomSkill& s) {
        if (this != &s) v = s.v;
        return *this;
    }
    CustomSkill& operator=(CustomSkill&& s) {
        v = s.v;
        return *this;
    }

    bool isSkill() override { return true; }

    QString description(bool showRoll = false) override {
        static QStringList stats { "", "STR", "DEX", "CON", "INT", "EGO", "PRE" };
        return v.mDescr + " (" + (showRoll ? roll() : "+" + QString("%1").arg(v.mPlus)) + (v.mStat > 0 ? stats[v.mStat] : "") + ")";
    }
    bool form(QWidget* parent, QVBoxLayout* layout) override {
        descr = createLineEdit(parent, layout, "Skill name?");
        stat = createComboBox(parent, layout, "Based on?", { "", "STR", "DEX", "CON", "INT", "EGO", "PRE" });
        plus = createLineEdit(parent, layout, "Pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
        return true;
    }
    QString name() override {
        return v.mName;
    }
    Points points(bool noStore = false) override {
        if (!noStore) store();
        if (v.mStat > 0) return 3_cp + v.mPlus * 2_cp;
        else return 2_cp + v.mPlus * 1_cp;
    }
    void restore() override {
        vars s = v;
        descr->setText(s.mDescr);
        plus->setText(QString("%1").arg(s.mPlus));
        stat->setCurrentIndex(s.mStat);
        v = s;
    }
#ifndef ISHSC
    QString roll() override {
        if (v.mStat > 0) return add(Sheet::ref().character().characteristic(v.mStat - 1).roll(), v.mPlus);
        return QString("%1-").arg(BaseRoll, v.mPlus);
    }
#else
    QString roll() override {
        return QString("+%1").arg(v._plus);
    }
#endif
    void store() override {
        v.mPlus = plus->text().toInt(0);
        v.mDescr = descr->text();
        v.mStat = stat->currentIndex();
    }

    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]  = v.mName;
        obj["descr"] = v.mDescr;
        obj["plus"]  = v.mPlus;
        obj["stat"]  = v.mStat;
        return obj;
    }

private:
    struct vars {
        QString mName = "";
        QString mDescr = "";
        int     mStat = -1;
        int     mPlus = 0;
    } v;

    QLineEdit* plus = nullptr;
    QLineEdit* descr = nullptr;
    QComboBox* stat = nullptr;

    void numeric(QString) override {
        QString txt = plus->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        plus->undo();
    }
};

#undef CLASS
#undef CLASS_SPACE
