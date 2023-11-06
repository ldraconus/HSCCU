#ifndef AUTOMATONPOWERS_H
#define AUTOMATONPOWERS_H

#include "powers.h"

class CannotBeStunned: public AllPowers {
public:
    CannotBeStunned(): AllPowers("Cannot Be Stunnedϴ")        { }
    CannotBeStunned(const CannotBeStunned& s): AllPowers(s)   { }
    CannotBeStunned(CannotBeStunned&& s): AllPowers(s)        { }
    CannotBeStunned(const QJsonObject& json): AllPowers(json) { }
    ~CannotBeStunned() override { }

    CannotBeStunned& operator=(const CannotBeStunned& s) {
        if (this != &s) AllPowers::operator=(s);
        return *this;
    }
    CannotBeStunned& operator=(CannotBeStunned&& s) {
        AllPowers::operator=(s);
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
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
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   return obj;
                                                                 }

private:
    QString optOut(bool showEND) {
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "Cannot Be Stunnedϴ";
        return res;
    }
};

class DoesNotBleed: public AllPowers {
public:
    DoesNotBleed(): AllPowers("Does Not Bleedϴ")           { }
    DoesNotBleed(const DoesNotBleed& s): AllPowers(s)      { }
    DoesNotBleed(DoesNotBleed&& s): AllPowers(s)           { }
    DoesNotBleed(const QJsonObject& json): AllPowers(json) { }
    ~DoesNotBleed() override { }

    DoesNotBleed& operator=(const DoesNotBleed& s) {
        if (this != &s) AllPowers::operator=(s);
        return *this;
    }
    DoesNotBleed& operator=(DoesNotBleed&& s) {
        AllPowers::operator=(s);
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
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
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   return obj;
                                                                 }

private:
    QString optOut(bool showEND) {
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "Does Not Bleedϴ";
        return res;
    }
};

class NoHitLocations: public AllPowers {
public:
    NoHitLocations(): AllPowers("No Hit Locationsϴ")         { }
    NoHitLocations(const NoHitLocations& s): AllPowers(s)    { }
    NoHitLocations(NoHitLocations&& s): AllPowers(s)         { }
    NoHitLocations(const QJsonObject& json): AllPowers(json) { }
    ~NoHitLocations() override { }

    NoHitLocations& operator=(const NoHitLocations& s) {
        if (this != &s) AllPowers::operator=(s);
        return *this;
    }
    NoHitLocations& operator=(NoHitLocations&& s) {
        AllPowers::operator=(s);
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
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
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   return obj;
                                                                 }

private:
    QString optOut(bool showEND) {
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "No Hit Locationsϴ";
        return res;
    }
};

class TakesNoSTUN: public AllPowers {
public:
    TakesNoSTUN(): AllPowers("Takes No STUNϴ")            { }
    TakesNoSTUN(const TakesNoSTUN& s): AllPowers(s)       { }
    TakesNoSTUN(TakesNoSTUN&& s): AllPowers(s)            { }
    TakesNoSTUN(const QJsonObject& json): AllPowers(json) { v._damage = json["damage"].toBool(false);
                                                          }
    ~TakesNoSTUN() override { }

    TakesNoSTUN& operator=(const TakesNoSTUN& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    TakesNoSTUN& operator=(TakesNoSTUN&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   damage = createCheckBox(parent, layout, "Loses powers on BODY damage");
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override                 { if (!noStore) store();
                                                                   return (v._damage ? 45_cp : 60_cp); } // NOLINT
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   damage->setChecked(s._damage);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._damage = damage->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["damage"] = v._damage;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        bool _damage = false;
    } v;

    QCheckBox* damage = nullptr;

    QString optOut(bool showEND) {
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "Takes No STUNϴ";
        if (v._damage) res += "; Loses powers on BODY damage";
        return res;
    }
};

#endif // AUTOMATONPOWERS_H
