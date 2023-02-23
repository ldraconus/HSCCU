#ifndef STANDARDPOWERS_H
#define STANDARDPOWERS_H

#include "powers.h"
#include "sheet.h"

class Characteristics: public AllPowers {
public:
    Characteristics(): AllPowers("Characteristics")           { }
    Characteristics(const Characteristics& s): AllPowers(s)   { }
    Characteristics(Characteristics&& s): AllPowers(s)        { }
    Characteristics(const QJsonObject& json): AllPowers(json) { v._str  = json["str"].toInt(0);
                                                                v._dex  = json["dex"].toInt(0);
                                                                v._con  = json["con"].toInt(0);
                                                                v._int  = json["int"].toInt(0);
                                                                v._ego  = json["ego"].toInt(0);
                                                                v._pre  = json["pre"].toInt(0);
                                                                v._ocv  = json["ocv"].toInt(0);
                                                                v._dcv  = json["dcv"].toInt(0);
                                                                v._omcv = json["omcv"].toInt(0);
                                                                v._dmcv = json["dmcv"].toInt(0);
                                                                v._spd  = json["spd"].toInt(0);
                                                                v._pd   = json["pd"].toInt(0);
                                                                v._ed   = json["ed"].toInt(0);
                                                                v._rec  = json["rec"].toInt(0);
                                                                v._end  = json["end"].toInt(0);
                                                                v._body = json["body"].toInt(0);
                                                                v._stun = json["stun"].toInt(0);
                                                                v._put  = json["put"].toInt(0);
                                                              }
    virtual Characteristics& operator=(const Characteristics& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Characteristics& operator=(Characteristics&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   str  = createLabeledEdit(parent, layout, "STR:",  std::mem_fn(&Power::numeric));
                                                                   dex  = createLabeledEdit(parent, layout, "DEX:",  std::mem_fn(&Power::numeric));
                                                                   con  = createLabeledEdit(parent, layout, "CON:",  std::mem_fn(&Power::numeric));
                                                                   Int  = createLabeledEdit(parent, layout, "INT:",  std::mem_fn(&Power::numeric));
                                                                   ego  = createLabeledEdit(parent, layout, "EGO:",  std::mem_fn(&Power::numeric));
                                                                   pre  = createLabeledEdit(parent, layout, "PRE:",  std::mem_fn(&Power::numeric));
                                                                   ocv  = createLabeledEdit(parent, layout, "OCV:",  std::mem_fn(&Power::numeric));
                                                                   dcv  = createLabeledEdit(parent, layout, "DCV:",  std::mem_fn(&Power::numeric));
                                                                   omcv = createLabeledEdit(parent, layout, "OMCV:", std::mem_fn(&Power::numeric));
                                                                   dmcv = createLabeledEdit(parent, layout, "DMCV:", std::mem_fn(&Power::numeric));
                                                                   spd  = createLabeledEdit(parent, layout, "SPD:",  std::mem_fn(&Power::numeric));
                                                                   pd   = createLabeledEdit(parent, layout, "PD:",   std::mem_fn(&Power::numeric));
                                                                   ed   = createLabeledEdit(parent, layout, "ED:",   std::mem_fn(&Power::numeric));
                                                                   rec  = createLabeledEdit(parent, layout, "REC:",  std::mem_fn(&Power::numeric));
                                                                   End  = createLabeledEdit(parent, layout, "END:",  std::mem_fn(&Power::numeric));
                                                                   body = createLabeledEdit(parent, layout, "BODY:", std::mem_fn(&Power::numeric));
                                                                   stun = createLabeledEdit(parent, layout, "STUN:", std::mem_fn(&Power::numeric));
                                                                   put  = createComboBox(parent, layout, "Do Not add", { "Do Not Add", "Add To Primary", "Add to Secondary" });
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   Points defCost = 0_cp;
                                                                   if (Sheet::ref().character().hasTakesNoSTUN()) defCost = 3_cp * (v._pd + v._ed) + 9_cp * v._dmcv + 15_cp * v._dcv;
                                                                   else defCost = 1_cp * (v._pd + v._ed) + 3_cp * v._dmcv + 5_cp * v._dcv;
                                                                   return 1_cp * (v._str + v._con + v._int + v._ego + v._pre + v._rec + v._body) + 2_cp * v._dex + defCost +
                                                                          5_cp * v._ocv + 3_cp * v._omcv + 10_cp * v._spd + (v._end + 2) / 5 + (v._stun + 1) / 2;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   labeledEdit(str)->setText(QString("%1").arg(s._str));
                                                                   labeledEdit(dex)->setText(QString("%1").arg(s._dex));
                                                                   labeledEdit(con)->setText(QString("%1").arg(s._con));
                                                                   labeledEdit(Int)->setText(QString("%1").arg(s._int));
                                                                   labeledEdit(ego)->setText(QString("%1").arg(s._ego));
                                                                   labeledEdit(pre)->setText(QString("%1").arg(s._pre));
                                                                   labeledEdit(ocv)->setText(QString("%1").arg(s._ocv));
                                                                   labeledEdit(dcv)->setText(QString("%1").arg(s._dcv));
                                                                   labeledEdit(omcv)->setText(QString("%1").arg(s._omcv));
                                                                   labeledEdit(dmcv)->setText(QString("%1").arg(s._dmcv));
                                                                   labeledEdit(spd)->setText(QString("%1").arg(s._spd));
                                                                   labeledEdit(pd)->setText(QString("%1").arg(s._pd));
                                                                   labeledEdit(ed)->setText(QString("%1").arg(s._ed));
                                                                   labeledEdit(rec)->setText(QString("%1").arg(s._rec));
                                                                   labeledEdit(End)->setText(QString("%1").arg(s._end));
                                                                   labeledEdit(body)->setText(QString("%1").arg(s._body));
                                                                   labeledEdit(stun)->setText(QString("%1").arg(s._stun));
                                                                   put->setCurrentIndex(s._put);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._str  = labeledEdit(str)->text().toInt();
                                                                   v._dex  = labeledEdit(dex)->text().toInt();
                                                                   v._con  = labeledEdit(con)->text().toInt();
                                                                   v._int  = labeledEdit(Int)->text().toInt();
                                                                   v._ego  = labeledEdit(ego)->text().toInt();
                                                                   v._pre  = labeledEdit(pre)->text().toInt();
                                                                   v._ocv  = labeledEdit(ocv)->text().toInt();
                                                                   v._dcv  = labeledEdit(dcv)->text().toInt();
                                                                   v._omcv = labeledEdit(omcv)->text().toInt();
                                                                   v._dmcv = labeledEdit(dmcv)->text().toInt();
                                                                   v._spd  = labeledEdit(spd)->text().toInt();
                                                                   v._pd   = labeledEdit(pd)->text().toInt();
                                                                   v._ed   = labeledEdit(ed)->text().toInt();
                                                                   v._rec  = labeledEdit(rec)->text().toInt();
                                                                   v._end  = labeledEdit(End)->text().toInt();
                                                                   v._body = labeledEdit(body)->text().toInt();
                                                                   v._stun = labeledEdit(stun)->text().toInt();
                                                                   v._put  = put->currentIndex();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["str"]  = v._str;
                                                                   obj["dex"]  = v._dex;
                                                                   obj["con"]  = v._con;
                                                                   obj["int"]  = v._int;
                                                                   obj["ego"]  = v._ego;
                                                                   obj["pre"]  = v._pre;
                                                                   obj["ocv"]  = v._ocv;
                                                                   obj["dcv"]  = v._dcv;
                                                                   obj["omcv"] = v._omcv;
                                                                   obj["dmcv"] = v._dmcv;
                                                                   obj["spd"]  = v._spd;
                                                                   obj["pd"]   = v._pd;
                                                                   obj["ed"]   = v._ed;
                                                                   obj["rec"]  = v._rec;
                                                                   obj["end"]  = v._end;
                                                                   obj["body"] = v._body;
                                                                   obj["stun"] = v._stun;
                                                                   obj["put"]  = v._put;
                                                                   return obj;
                                                                 }

int place() override { return v._put; }

private:
    struct vars {
        int _str  = 0;
        int _dex  = 0;
        int _con  = 0;
        int _int  = 0;
        int _ego  = 0;
        int _pre  = 0;
        int _ocv  = 0;
        int _dcv  = 0;
        int _omcv = 0;
        int _dmcv = 0;
        int _spd  = 0;
        int _pd   = 0;
        int _ed   = 0;
        int _rec  = 0;
        int _end  = 0;
        int _body = 0;
        int _stun = 0;
        int _put  = -1;
    } v;

    int characteristic(int x) override {
        switch (x) {
        case -1: return v._put;
        case 0:  return v._str;
        case 1:  return v._dex;
        case 2:  return v._con;
        case 3:  return v._int;
        case 4:  return v._ego;
        case 5:  return v._pre;
        case 6:  return v._ocv;
        case 7:  return v._dcv;
        case 8:  return v._omcv;
        case 9:  return v._dmcv;
        case 10: return v._spd;
        case 11: return v._pd;
        case 12: return v._ed;
        case 13: return v._rec;
        case 14: return v._end;
        case 15: return v._body;
        default: return v._stun;
        }
    }

    QWidget*   str;
    QWidget*   dex;
    QWidget*   con;
    QWidget*   Int;
    QWidget*   ego;
    QWidget*   pre;
    QWidget*   ocv;
    QWidget*   dcv;
    QWidget*   omcv;
    QWidget*   dmcv;
    QWidget*   spd;
    QWidget*   pd;
    QWidget*   ed;
    QWidget*   rec;
    QWidget*   End;
    QWidget*   body;
    QWidget*   stun;
    QComboBox* put;

    QString  end() override {
        if (v._str != 0) return Power::end();
        return noEnd();
    }

    QString optOut(bool showEND) {
        if (v._str < 1 && v._dex < 1 && v._con < 1 && v._int < 1 && v._ego < 1 && v._pre < 1  && v._ocv < 1 && v._dcv < 1 && v._omcv < 1 && v._dmcv < 1 &&
            v._spd < 1 && v._pd < 1  && v._ed < 1  && v._rec < 1 && v._end < 1 && v._body < 1 && v._stun < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        QString sep = "";
        if (v._str > 0)  { res += sep + QString("+%1 STR").arg(v._str);   sep = ", "; }
        if (v._dex > 0)  { res += sep + QString("+%1 DEX").arg(v._dex);   sep = ", "; }
        if (v._con > 0)  { res += sep + QString("+%1 CON").arg(v._con);   sep = ", "; }
        if (v._int > 0)  { res += sep + QString("+%1 INT").arg(v._int);   sep = ", "; }
        if (v._ego > 0)  { res += sep + QString("+%1 EGO").arg(v._ego);   sep = ", "; }
        if (v._pre > 0)  { res += sep + QString("+%1 PRE").arg(v._pre);   sep = ", "; }
        if (v._ocv > 0)  { res += sep + QString("+%1 OCV").arg(v._ocv);   sep = ", "; }
        if (v._dcv > 0)  { res += sep + QString("+%1 DCV").arg(v._dcv);   sep = ", "; }
        if (v._omcv > 0) { res += sep + QString("+%1 OMCV").arg(v._omcv); sep = ", "; }
        if (v._dmcv > 0) { res += sep + QString("+%1 DMCV").arg(v._dmcv); sep = ", "; }
        if (v._spd > 0)  { res += sep + QString("+%1 SPD").arg(v._spd);   sep = ", "; }
        if (v._pd > 0)   { res += sep + QString("+%1 PD").arg(v._pd);     sep = ", "; }
        if (v._ed > 0)   { res += sep + QString("+%1 ED").arg(v._ed);     sep = ", "; }
        if (v._rec > 0)  { res += sep + QString("+%1 REC").arg(v._rec);   sep = ", "; }
        if (v._end > 0)  { res += sep + QString("+%1 END").arg(v._end);   sep = ", "; }
        if (v._body > 0) { res += sep + QString("+%1 BODY").arg(v._body); sep = ", "; }
        if (v._stun > 0) { res += sep + QString("+%1 STUN").arg(v._stun); sep = ", "; }
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

class Clinging: public AllPowers {
public:
    Clinging(): AllPowers("Clinging")                  { }
    Clinging(const Clinging& s): AllPowers(s)          { }
    Clinging(Clinging&& s): AllPowers(s)               { }
    Clinging(const QJsonObject& json): AllPowers(json) { v._str  = json["str"].toInt(0);
                                                              v._knck = json["knck"].toBool(false);
                                                            }
    virtual Clinging& operator=(const Clinging& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Clinging& operator=(Clinging&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   str  = createLineEdit(parent, layout, "Extra STR", std::mem_fn(&Power::numeric));
                                                                   knck = createCheckBox(parent, layout, "Cannot Resist Knockback");
                                                                 }
    Fraction lim() override                                      { return v._knck ? Fraction(1, 4) : Fraction(0); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return 10_cp + (v._str + 1) / 3;;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   str->setText(QString("%1").arg(s._str));
                                                                   knck->setChecked(s._knck);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._str  = str->text().toInt();
                                                                   v._knck = knck->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["str"]  = v._str;
                                                                   obj["knck"] = v._knck;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int  _str  = 0;
        bool _knck = false;
    } v;

    QLineEdit* str;
    QCheckBox* knck;

    QString optOut(bool showEND) {
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        int prim = Sheet::ref().character().STR().primary() + Sheet::ref().character().STR().base() + v._str;
        int secd = prim + Sheet::ref().character().STR().secondary();
        if (prim != secd) res += res += QString("%1/%2 STR Clinging").arg(prim).arg(secd);
        else res += QString("%1 STR Clinging").arg(prim);
        if (v._knck) res += "; Cannot Resist Knockback";
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

class Dispel: public AllPowers {
public:
    Dispel(): AllPowers("Dispel")                  { }
    Dispel(const Dispel& s): AllPowers(s)          { }
    Dispel(Dispel&& s): AllPowers(s)               { }
    Dispel(const QJsonObject& json): AllPowers(json) { v._dice  = json["dice"].toInt(0);
                                                          }
    virtual Dispel& operator=(const Dispel& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Dispel& operator=(Dispel&& s) {
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
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return v._dice * 3_cp;
                                                                 }
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
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1d6 Dispel").arg(v._dice);
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

class LifeSupport: public AllPowers {
public:
    LifeSupport(): AllPowers("Life Support")              { }
    LifeSupport(const LifeSupport& s): AllPowers(s)       { }
    LifeSupport(LifeSupport&& s): AllPowers(s)            { }
    LifeSupport(const QJsonObject& json): AllPowers(json) { v._extend = json["extend"].toInt(0);
                                                                 v._self   = json["self"].toBool(false);
                                                                 v._expand = json["expand"].toString();
                                                                 v._eating = json["eating"].toInt(0);
                                                                 v._sleep  = json["sleep"].toInt(0);
                                                                 v._selpv  = json["selpv"].toBool(false);
                                                                 v._sehp   = json["sehp"].toBool(false);
                                                                 v._sehr   = json["sehr"].toBool(false);
                                                                 v._seic   = json["seic"].toBool(false);
                                                                 v._seih   = json["seih"].toBool(false);
                                                                 v._long   = json["long"].toInt(0);
                                                                 v._immun  = json["immun"].toInt(0);
                                                                 v._to     = json["to"].toString();
                                                               }
    virtual LifeSupport& operator=(const LifeSupport& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual LifeSupport& operator=(LifeSupport&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return Fraction(0); }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    QString  end() override                                      { return noEnd(); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   extend = createComboBox(parent, layout, "Extended Breathing?", { "", "1 END/Turn", "1 END/Minute", "1 END/5 Minutes",
                                                                                                                                    "1 END/Hour" });
                                                                   self   = createCheckBox(parent, layout, "Self-Contained Breathing");
                                                                   expand = createLineEdit(parent, layout, "Expanded Breathing?");
                                                                   eating = createComboBox(parent, layout, "Diminished Eating?", { "", "1 Meal/Week", "1 Meal/Year", "Need Not Eat/Drink" });
                                                                   sleep  = createComboBox(parent, layout, "Diminished Sleepng?", { "", "8 Hours/Week", "1 Hours/Year", "Need Not Sleep" });
                                                                   selpv  = createCheckBox(parent, layout, "Safe Environment: Low Pressure/Vacuum");
                                                                   sehp   = createCheckBox(parent, layout, "Safe Environment: High Pressure");
                                                                   sehr   = createCheckBox(parent, layout, "Safe Environment: High Radiation");
                                                                   seic   = createCheckBox(parent, layout, "Safe Environment: Intense Cold");
                                                                   seih   = createCheckBox(parent, layout, "Safe Environment: Intense Heat");
                                                                   Long   = createComboBox(parent, layout, "Longevity?", { "", "200 Year Lifespan", "400 Year Lifespan", "800 Year Lifespan",
                                                                                                                           "1,600 Year Lifespan", "Immortal" });
                                                                   immun  = createComboBox(parent, layout, "Immunity?", { "", "Uncommon Item", "Common Item", "Small Group", "Large Group",
                                                                                                                          "All Terrestrial Poisons", "All Terrestrial Diseases",
                                                                                                                          "All Terrestrial Poisions/Diseases" });
                                                                   to     = createLineEdit(parent, layout, "Immune To?");
                                                                 }
    Fraction lim() override                                      { return Fraction(0); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return ((v._extend >= 1) ? v._extend * 1_cp : 0_cp) +
                                                                          (v._self ? 10_cp : 0_cp) +
                                                                          countCommas(v._expand) * 5_cp +
                                                                          ((v._eating >= 1) ? v._eating * 1_cp : 0_cp) +
                                                                          ((v._sleep >= 1) ? v._sleep * 1_cp : 0_cp) +
                                                                          (v._selpv ? 2_cp : 0_cp) +
                                                                          (v._sehp ? 1_cp : 0_cp) +
                                                                          (v._sehr ? 2_cp : 0_cp) +
                                                                          (v._seic ? 2_cp : 0_cp) +
                                                                          (v._seih ? 2_cp : 0_cp) +
                                                                          ((v._long >= 1) ? v._long * 1_cp : 0_cp) +
                                                                          ((v._immun >= 1) ? ((v._immun >= 5 && v._immun <= 6) ? 5_cp : ((v._immun == 7) ? 10_cp : v._immun * 1_cp)) : 0_cp);
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   extend->setCurrentIndex(s._extend);
                                                                   self->setChecked(s._self);
                                                                   expand->setText(s._expand);
                                                                   eating->setCurrentIndex(s._eating);
                                                                   sleep->setCurrentIndex(s._sleep);
                                                                   selpv->setChecked(s._selpv);
                                                                   sehp->setChecked(s._sehp);
                                                                   sehr->setChecked(s._sehr);
                                                                   seic->setChecked(s._seic);
                                                                   seih->setChecked(s._seih);
                                                                   Long->setCurrentIndex(s._long);
                                                                   immun->setCurrentIndex(s._immun);
                                                                   to->setText(s._to);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._extend = extend->currentIndex();
                                                                   v._self   = self->isChecked();
                                                                   v._expand = expand->text();
                                                                   v._eating = eating->currentIndex();
                                                                   v._sleep  = sleep->currentIndex();
                                                                   v._selpv  = selpv->isChecked();
                                                                   v._sehp   = sehp->isChecked();
                                                                   v._sehr   = sehr->isChecked();
                                                                   v._seic   = seic->isChecked();
                                                                   v._seih   = seih->isChecked();
                                                                   v._long   = Long->currentIndex();
                                                                   v._immun  = immun->currentIndex();
                                                                   v._to     = to->text();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["extend"] = v._extend;
                                                                   obj["self"]   = v._self;
                                                                   obj["expand"] = v._expand;
                                                                   obj["eating"] = v._eating;
                                                                   obj["sleep"]  = v._sleep;
                                                                   obj["selpv"]  = v._selpv;
                                                                   obj["sehp"]   = v._sehp;
                                                                   obj["sehr"]   = v._sehr;
                                                                   obj["seic"]   = v._seic;
                                                                   obj["seih"]   = v._seih;
                                                                   obj["long"]   = v._long;
                                                                   obj["immun"]  = v._immun;
                                                                   obj["to"]     = v._to;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     _extend = -1;
        bool    _self   = false;
        QString _expand = "";
        int     _eating = -1;
        int     _sleep  = -1;
        bool    _selpv  = false;
        bool    _sehp   = false;
        bool    _sehr   = false;
        bool    _seic   = false;
        bool    _seih   = false;
        int     _long   = -1;
        int     _immun  = -1;
        QString _to     = "";
    } v;

    QComboBox* extend;
    QCheckBox* self;
    QLineEdit* expand;
    QComboBox* eating;
    QComboBox* sleep;
    QCheckBox* selpv;
    QCheckBox* sehp;
    QCheckBox* sehr;
    QCheckBox* seic;
    QCheckBox* seih;
    QComboBox* Long;
    QComboBox* immun;
    QLineEdit* to;

    QString optOut(bool showEND) {
        if (v._extend < 1 && !v._self && v._expand.isEmpty() && v._eating < 1 && v._sleep  < 1 && !v._selpv && !v._sehp && !v._sehr && !v._seic && !v._seih && v._long < 1 &&
            v._immun < 1) return "<incomplete>";
        if (v._immun > 0 && v._immun < 5 && v._to.isEmpty()) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("Life Support");
        QStringList extend { "", "1 END/Turn", "1 END/Minute", "1 END/5 Minutes", "1 End/Hour" };
        if (v._extend > 0) res += "; Extended Breating (" + extend[v._extend] + ")";
        if (v._self) res += "; Self-Contained Breathing";
        if (!v._expand.isEmpty()) res += "; Expanded Breathing (" + v._expand + ")";
        QStringList eating { "", "1 Meal/Week", "1 Meal/Year", "Need Not Eat/Drink" };
        if (v._eating > 0) res += "; Diminished Eating (" + eating[v._eating] + ")";
        QStringList sleep { "", "8 Hours/Week", "8 Hours/Year", "Need Not Sleep" };
        if (v._sleep > 0) res += "; Diminished Sleep (" + sleep[v._sleep] + ")";
        QString sep = "; Safe Environment (";
        if (v._selpv) { res += sep + "Low Pressure/Vacuum"; sep = ", "; }
        if (v._sehp)  { res += sep + "High Pressure";       sep = ", "; }
        if (v._sehr)  { res += sep + "High Radiation";      sep = ", "; }
        if (v._seic)  { res += sep + "Intense Cold";        sep = ", "; }
        if (v._seih)  { res += sep + "Intense Heat";        sep = ", "; }
        if (sep == ", ") res += ")";
        QStringList Long { "", "200 Year Lifespan", "400 Year Lifespan", "800 Year Lifespan", "1,600 Year Lifespan", "Immortal" };
        if (v._long >= 1) res += "; " + Long[v._long];
        if (v._immun >= 1) {
            if (v._immun < 5) res += "; Immune to " + v._to;
            else if (v._immun == 5) res += "; Immunity Terrestrial Poisons";
            else if (v._immun == 6) res += "; Immunity Terrestrial Diseases";
            else res += "; Immunity Terrestrial Poisons/Diseases";
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
};

class Summon: public AllPowers {
public:
    Summon(): AllPowers("Summonϴ")                 { }
    Summon(const Summon& s): AllPowers(s)          { }
    Summon(Summon&& s): AllPowers(s)               { }
    Summon(const QJsonObject& json): AllPowers(json) { v._pts    = json["pts"].toInt(0);
                                                            v._dbl    = json["dbl"].toInt(0);
                                                            v._crtr   = json["crtr"].toString();
                                                            v._ami    = json["ami"].toInt(0);
                                                            v._tasks  = json["tasks"].toInt(0);
                                                            v._expand = json["tasks"].toInt(0);
                                                            v._spec   = json["spec"].toBool(false);
                                                            v._weak   = json["weak"].toInt(0);
                                                            v._antag  = json["antag"].toInt(0);
                                                            v._arrive = json["arrive"].toBool(false);
                                                            v._fewer  = json["fewer"].toInt(0);
                                                            v._strong = json["strong"].toInt(0);
                                                            v._must   = json["must"].toBool(false);
                                                          }
    virtual Summon& operator=(const Summon& s) {
        if (this != &s) {
            AllPowers::operator=(s);
            v = s.v;
        }
        return *this;
    }
    virtual Summon& operator=(Summon&& s) {
        AllPowers::operator=(s);
        v = s.v;
        return *this;
    }

    Fraction adv() override                                      { return ((v._ami > 0)     ? v._ami * Fraction(1, 4)   : Fraction(0)) +
                                                                          ((v._tasks > 0)   ? v._tasks * Fraction(1, 4) : Fraction(0)) +
                                                                          ((v._expand == 1) ? Fraction(1, 4)            : Fraction(0)) +
                                                                          ((v._expand == 2) ? Fraction(1, 2)            : Fraction(0)) +
                                                                          ((v._expand == 3) ? Fraction(1)               : Fraction(0)) +
                                                                          (v._spec          ? Fraction(1)               : Fraction(0)) +
                                                                          ((v._weak > 0)    ? v._weak * Fraction(1, 4)  : Fraction(0));
                                                                 }
    QString  description(bool showEND = false) override          { return optOut(showEND); }
    void     form(QWidget* parent, QVBoxLayout* layout) override { AllPowers::form(parent, layout);
                                                                   pts    = createLineEdit(parent, layout, "Pointa?", std::mem_fn(&Power::numeric));
                                                                   dbl    = createLineEdit(parent, layout, "Doublings Of Creatures?", std::mem_fn(&Power::numeric));
                                                                   crtr   = createLineEdit(parent, layout, "Creature?");
                                                                   ami    = createComboBox(parent, layout, "Amicable?", { "Neutral", "Friendly", "Loyal", "Devoted", "Slavishly Devoted" });
                                                                   tasks  = createLineEdit(parent, layout, "Doublings Of Tasks?", std::mem_fn(&Power::numeric));
                                                                   expand = createComboBox(parent, layout, "Expanded Class?", { "None", "Very Limited Group", "Limited Group", "Any" });
                                                                   spec   = createCheckBox(parent, layout, "Specific Beingϴ");
                                                                   weak   = createComboBox(parent, layout, "Weak Willed?", { "Normal", "Weak Willed", "Very Weak Willed" });
                                                                   antag  = createComboBox(parent, layout, "Antagonistic?", { "Normal", "Annoyed", "Hostile", "Violent" });
                                                                   arrive = createCheckBox(parent, layout, "Arrives Under Own Power");
                                                                   fewer  = createLineEdit(parent, layout, "Halvings Of Tasks?", std::mem_fn(&Power::numeric));
                                                                   strong = createComboBox(parent, layout, "Strong Willed?", { "Normal", "Strong Willed", "Very Strong Willed" });
                                                                   must   = createCheckBox(parent, layout, "Summoned Being Must Inhabit Locale");
                                                                 }
    Fraction lim() override                                      { return ((v._antag > 0)  ? v._antag * Fraction(1, 4)  : Fraction(0)) +
                                                                          (v._arrive       ? Fraction(1, 2)             : Fraction(0)) +
                                                                          ((v._fewer > 0)  ? v._fewer * Fraction(1, 4)  : Fraction(0)) +
                                                                          ((v._strong > 0) ? v._strong * Fraction(1, 4) : Fraction(0)) +
                                                                          (v._must         ? Fraction(1, 2)             : Fraction(0)); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return (v._pts + 2) / 5 * 1_cp + 5 * v._dbl;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   pts->setText(QString("%1").arg(s._pts));
                                                                   dbl->setText(QString("%1").arg(s._dbl));
                                                                   crtr->setText(s._crtr);
                                                                   ami->setCurrentIndex(v._ami);
                                                                   tasks->setText(QString("%1").arg(s._tasks));
                                                                   expand->setCurrentIndex(s._expand);
                                                                   spec->setChecked(s._spec);
                                                                   weak->setCurrentIndex(s._weak);
                                                                   antag->setCurrentIndex(v._antag);
                                                                   arrive->setChecked(v._arrive);
                                                                   fewer->setText(QString("%1").arg(s._fewer));
                                                                   strong->setCurrentIndex(v._strong);
                                                                   must->setChecked(v._must);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v._pts    = pts->text().toInt();
                                                                   v._dbl    = dbl->text().toInt();
                                                                   v._crtr   = crtr->text();
                                                                   v._ami    = ami->currentIndex();
                                                                   v._tasks  = tasks->text().toInt();
                                                                   v._expand = expand->currentIndex();
                                                                   v._spec   = spec->isChecked();
                                                                   v._weak   = weak->currentIndex();
                                                                   v._antag  = antag->currentIndex();
                                                                   v._arrive = arrive->isChecked();
                                                                   v._fewer  = fewer->text().toInt();
                                                                   v._strong = strong->currentIndex();
                                                                   v._must   = must->isChecked();
                                                                 }
    QJsonObject toJson() override                                { QJsonObject obj = AllPowers::toJson();
                                                                   obj["pts"]    = v._pts;
                                                                   obj["dbl"]    = v._dbl;
                                                                   obj["crtr"]   = v._crtr;
                                                                   obj["ami"]    = v._ami;
                                                                   obj["tasks"]  = v._tasks;
                                                                   obj["expand"] = v._expand;
                                                                   obj["spec"]   = v._spec;
                                                                   obj["weak"]   = v._weak;
                                                                   obj["antag"]  = v._antag;
                                                                   obj["arrive"] = v._arrive;
                                                                   obj["fewer"]  = v._fewer;
                                                                   obj["strong"] = v._strong;
                                                                   obj["must"]   = v._must;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     _pts    = 0;
        int     _dbl    = 0;
        QString _crtr   = "";
        int     _ami    = -1;
        int     _tasks  = 0;
        int     _expand = -1;
        bool    _spec   = false;
        int     _weak   = -1;
        int     _antag  = -1;
        bool    _arrive = false;
        int     _fewer  = -1;
        int     _strong = -1;
        bool    _must   = false;
    } v;

    QLineEdit* pts;
    QLineEdit* dbl;
    QLineEdit* crtr;
    QComboBox* ami;
    QLineEdit* tasks;
    QComboBox* expand;
    QCheckBox* spec;
    QComboBox* weak;
    QComboBox* antag;
    QCheckBox* arrive;
    QLineEdit* fewer;
    QComboBox* strong;
    QCheckBox* must;

    QString optOut(bool showEND) {
        if (v._pts < 1 || v._crtr.isEmpty()) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("Summonϴ ").arg(v._pts);
        if (v._dbl > 0) res += QString(" Up To %1 ").arg((int) pow(2, v._dbl));
        res += QString("%1 pt %2").arg(v._pts).arg(v._crtr);
        QStringList amicable { "", "Friendly", "Loyal", "Devoted", "Slavishly Devoted" };
        if (v._ami > 0) res += "; Creature(s) are " + amicable[v._ami];
        if (v._tasks > 0) res += QString("; x%1 Tasks").arg((int) pow(2, v._tasks));
        QStringList expand { "", "Very Limited Group of Creatures", "Limited Group of Creatures", "Any Creature" };
        if (v._expand) res += "; " + expand[v._expand];
        if (v._spec) res += "; Specific Beingϴ";
        QStringList weak { "", "Weak Willed", "Very Weak Willed" };
        if (v._weak > 0) res += "; " + weak[v._weak];
        QStringList antag { "", "Annoyed", "Hostile", "Violent" };
        if (v._antag > 0) res += "; " + antag[v._antag];
        if (v._arrive) res += "; Arrives Under Own Power";
        if (v._fewer > 0) res += QString("; Tasks/%1").arg((int) pow(2, v._tasks));
        QStringList strong { "", "Strong Willed", "Very Strong Willed" };
        if (v._strong > 0) res += "; " + strong[v._strong];
        if (v._must) res += "; Summoned Being Must Inhabit Locale";
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
