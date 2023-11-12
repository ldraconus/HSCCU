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
        return v._descr + " (" + (showRoll ? roll() : "+" + QString("%1").arg(v._plus)) + (v._stat > 0 ? stats[v._stat] : "") + ")";
    }
    bool form(QWidget* parent, QVBoxLayout* layout) override {
        descr = createLineEdit(parent, layout, "Skill name?");
        stat = createComboBox(parent, layout, "Based on?", { "", "STR", "DEX", "CON", "INT", "EGO", "PRE" });
        plus = createLineEdit(parent, layout, "Pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
        return true;
    }
    QString name() override {
        return v._name;
    }
    Points points(bool noStore = false) override {
        if (!noStore) store();
        if (v._stat > 0) return 3_cp + v._plus * 2_cp;
        else return 2_cp + v._plus * 1_cp;
    }
    void restore() override {
        vars s = v;
        descr->setText(s._descr);
        plus->setText(QString("%1").arg(s._plus));
        stat->setCurrentIndex(s._stat);
        v = s;
    }
#ifndef ISHSC
    QString roll() override {
        if (v._stat > 0) return add(Sheet::ref().character().characteristic(v._stat - 1).roll(), v._plus);
        return QString("%1-").arg(BaseRoll, v._plus);
    }
#else
    QString roll() override {
        return QString("+%1").arg(v._plus);
    }
#endif
    void store() override {
        v._plus = plus->text().toInt(0);
        v._descr = descr->text();
        v._stat = stat->currentIndex();
    }

    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]  = v._name;
        obj["descr"] = v._descr;
        obj["plus"]  = v._plus;
        obj["stat"]  = v._stat;
        return obj;
    }

private:
    struct vars {
        QString _name = "";
        QString _descr = "";
        int     _stat = -1;
        int     _plus = 0;
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
