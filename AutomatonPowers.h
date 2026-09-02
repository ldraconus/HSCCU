#ifndef AUTOMATONPOWERS_H
#define AUTOMATONPOWERS_H

#include "powers.h"

class CannotBeStunned: public AllPowers {
public:
    CannotBeStunned(): AllPowers("Cannot Be Stunnedϴ")  { }
    CannotBeStunned(QJsonObject& json): AllPowers(json) { }

    Fraction adv() override                                      { return Fraction(0); }
    QString  abbreviation(bool showEND = false) override         { return optOut(showEND, true); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override                 { if (!noStore) store();
                                                                   return 15_cp; } // NOLINT
    void     restore() override                                  { AllPowers::restore();
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   return obj;
                                                                 }

private:
    QString optOut(bool showEND, bool abbr = false) {
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString(abbr ? "Can't" : "Cannot") + " Be Stunnedϴ";
        return res;
    }
};

class DoesNotBleed: public AllPowers {
public:
    DoesNotBleed(): AllPowers("Does Not Bleedϴ")     { }
    DoesNotBleed(QJsonObject& json): AllPowers(json) { }

    Fraction adv() override                                      { return Fraction(0); }
    QString  abbreviation(bool showEND = false) override         { return optOut(showEND, true); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override                 { if (!noStore) store();
                                                                   return 15_cp; } // NOLINT
    void     restore() override                                  { AllPowers::restore();
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   return obj;
                                                                 }

private:
    QString optOut(bool showEND, bool abbr = false) {
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString(abbr ? "Doesn't" : "Does Not") + " Bleedϴ";
        return res;
    }
};

class NoHitLocations: public AllPowers {
public:
    NoHitLocations(): AllPowers("No Hit Locationsϴ")   { }
    NoHitLocations(QJsonObject& json): AllPowers(json) { }

    Fraction adv() override                                      { return Fraction(0); }
    QString  abbreviation(bool showEND = false) override         { return optOut(showEND, true); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override                 { if (!noStore) store();
                                                                   return 10_cp; } // NOLINT
    void     restore() override                                  { AllPowers::restore();
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   return obj;
                                                                 }

private:
    QString optOut(bool showEND, bool abbr = false) {
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "No Hit " + QString(abbr ? "Locs" : "Locationsϴ");
        return res;
    }
};

class TakesNoSTUN: public AllPowers {
public:
    TakesNoSTUN(): AllPowers("Takes No STUNϴ")      { }
    TakesNoSTUN(QJsonObject& json): AllPowers(json) { v.mDamage = json["damage"].toBool(false);
                                                    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   damage = createCheckBox(parent, layout, "Loses powers on BODY damage");
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override                 { if (!noStore) store();
                                                                   return (v.mDamage ? 45_cp : 60_cp); } // NOLINT
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   damage->setChecked(s.mDamage);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mDamage = damage->isChecked();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["damage"] = v.mDamage;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        bool mDamage = false;
    } v;

    QCheckBox* damage = nullptr;

    QString optOut(bool showEND) {
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "Takes No STUNϴ";
        if (v.mDamage) res += "; Loses powers on BODY damage";
        return res;
    }
};

#endif // AUTOMATONPOWERS_H
