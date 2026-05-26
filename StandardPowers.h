#ifndef STANDARDPOWERS_H
#define STANDARDPOWERS_H

#include "powers.h"
#ifndef ISHSC
#include "powerdialog.h"
#include "sheet.h"
#endif

class Characteristics: public AllPowers {
public:
    Characteristics(): AllPowers("Characteristics")           { }
    Characteristics(const Characteristics& s): AllPowers(s)   { }
    Characteristics(Characteristics&& s): AllPowers(s)        { }
    Characteristics(const QJsonObject& json): AllPowers(json) { v.mStr  = json["str"].toInt(0);
                                                                v.mDex  = json["dex"].toInt(0);
                                                                v.mCon  = json["con"].toInt(0);
                                                                v.mInt  = json["int"].toInt(0);
                                                                v.mEgo  = json["ego"].toInt(0);
                                                                v.mPre  = json["pre"].toInt(0);
                                                                v.mOcv  = json["ocv"].toInt(0);
                                                                v.mDcv  = json["dcv"].toInt(0);
                                                                v.mOmcv = json["omcv"].toInt(0);
                                                                v.mDmcv = json["dmcv"].toInt(0);
                                                                v.mSpd  = json["spd"].toInt(0);
                                                                v.mPd   = json["pd"].toInt(0);
                                                                v.mEd   = json["ed"].toInt(0);
                                                                v.mRec  = json["rec"].toInt(0);
                                                                v.mEnd  = json["end"].toInt(0);
                                                                v.mBody = json["body"].toInt(0);
                                                                v.mStun = json["stun"].toInt(0);
                                                                v.mPut  = json["put"].toInt(0);
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
#ifndef ISHSC
                                                                   if (Sheet::ref().character().hasTakesNoSTUN()) defCost = 3_cp * (v.mPd + v.mEd) + 9_cp * v.mDmcv + 15_cp * v.mDcv;
                                                                   else
#endif
                                                                      defCost = 1_cp * (v.mPd + v.mEd) + 3_cp * v.mDmcv + 5_cp * v.mDcv;
                                                                   return 1_cp * (v.mStr + v.mCon + v.mInt + v.mEgo + v.mPre + v.mRec + v.mBody) + 2_cp * v.mDex + defCost +
                                                                          5_cp * v.mOcv + 3_cp * v.mOmcv + 10_cp * v.mSpd + (v.mEnd + 2) / 5 + (v.mStun + 1) / 2;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   labeledEdit(str)->setText(QString("%1").arg(s.mStr));
                                                                   labeledEdit(dex)->setText(QString("%1").arg(s.mDex));
                                                                   labeledEdit(con)->setText(QString("%1").arg(s.mCon));
                                                                   labeledEdit(Int)->setText(QString("%1").arg(s.mInt));
                                                                   labeledEdit(ego)->setText(QString("%1").arg(s.mEgo));
                                                                   labeledEdit(pre)->setText(QString("%1").arg(s.mPre));
                                                                   labeledEdit(ocv)->setText(QString("%1").arg(s.mOcv));
                                                                   labeledEdit(dcv)->setText(QString("%1").arg(s.mDcv));
                                                                   labeledEdit(omcv)->setText(QString("%1").arg(s.mOmcv));
                                                                   labeledEdit(dmcv)->setText(QString("%1").arg(s.mDmcv));
                                                                   labeledEdit(spd)->setText(QString("%1").arg(s.mSpd));
                                                                   labeledEdit(pd)->setText(QString("%1").arg(s.mPd));
                                                                   labeledEdit(ed)->setText(QString("%1").arg(s.mEd));
                                                                   labeledEdit(rec)->setText(QString("%1").arg(s.mRec));
                                                                   labeledEdit(End)->setText(QString("%1").arg(s.mEnd));
                                                                   labeledEdit(body)->setText(QString("%1").arg(s.mBody));
                                                                   labeledEdit(stun)->setText(QString("%1").arg(s.mStun));
                                                                   put->setCurrentIndex(s.mPut);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mStr  = labeledEdit(str)->text().toInt();
                                                                   v.mDex  = labeledEdit(dex)->text().toInt();
                                                                   v.mCon  = labeledEdit(con)->text().toInt();
                                                                   v.mInt  = labeledEdit(Int)->text().toInt();
                                                                   v.mEgo  = labeledEdit(ego)->text().toInt();
                                                                   v.mPre  = labeledEdit(pre)->text().toInt();
                                                                   v.mOcv  = labeledEdit(ocv)->text().toInt();
                                                                   v.mDcv  = labeledEdit(dcv)->text().toInt();
                                                                   v.mOmcv = labeledEdit(omcv)->text().toInt();
                                                                   v.mDmcv = labeledEdit(dmcv)->text().toInt();
                                                                   v.mSpd  = labeledEdit(spd)->text().toInt();
                                                                   v.mPd   = labeledEdit(pd)->text().toInt();
                                                                   v.mEd   = labeledEdit(ed)->text().toInt();
                                                                   v.mRec  = labeledEdit(rec)->text().toInt();
                                                                   v.mEnd  = labeledEdit(End)->text().toInt();
                                                                   v.mBody = labeledEdit(body)->text().toInt();
                                                                   v.mStun = labeledEdit(stun)->text().toInt();
                                                                   v.mPut  = put->currentIndex();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["str"]  = v.mStr;
                                                                   obj["dex"]  = v.mDex;
                                                                   obj["con"]  = v.mCon;
                                                                   obj["int"]  = v.mInt;
                                                                   obj["ego"]  = v.mEgo;
                                                                   obj["pre"]  = v.mPre;
                                                                   obj["ocv"]  = v.mOcv;
                                                                   obj["dcv"]  = v.mDcv;
                                                                   obj["omcv"] = v.mOmcv;
                                                                   obj["dmcv"] = v.mDmcv;
                                                                   obj["spd"]  = v.mSpd;
                                                                   obj["pd"]   = v.mPd;
                                                                   obj["ed"]   = v.mEd;
                                                                   obj["rec"]  = v.mRec;
                                                                   obj["end"]  = v.mEnd;
                                                                   obj["body"] = v.mBody;
                                                                   obj["stun"] = v.mStun;
                                                                   obj["put"]  = v.mPut;
                                                                   return obj;
                                                                 }

int place() override { return v.mPut; }

private:
    struct vars {
        int mStr  = 0;
        int mDex  = 0;
        int mCon  = 0;
        int mInt  = 0;
        int mEgo  = 0;
        int mPre  = 0;
        int mOcv  = 0;
        int mDcv  = 0;
        int mOmcv = 0;
        int mDmcv = 0;
        int mSpd  = 0;
        int mPd   = 0;
        int mEd   = 0;
        int mRec  = 0;
        int mEnd  = 0;
        int mBody = 0;
        int mStun = 0;
        int mPut  = -1;
    } v;

    int characteristic(int x) override {
        switch (x) {
        case -1: return v.mPut;
        case 0:  return v.mStr;
        case 1:  return v.mDex;
        case 2:  return v.mCon;
        case 3:  return v.mInt;
        case 4:  return v.mEgo;
        case 5:  return v.mPre;
        case 6:  return v.mOcv;
        case 7:  return v.mDcv;
        case 8:  return v.mOmcv;
        case 9:  return v.mDmcv;
        case 10: return v.mSpd;
        case 11: return v.mPd;
        case 12: return v.mEd;
        case 13: return v.mRec;
        case 14: return v.mEnd;
        case 15: return v.mBody;
        default: return v.mStun;
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
        if (v.mStr != 0) return Power::end();
        return noEnd();
    }

    QString optOut(bool showEND) {
        if (v.mStr < 1 && v.mDex < 1 && v.mCon < 1 && v.mInt < 1 && v.mEgo < 1 && v.mPre < 1  && v.mOcv < 1 && v.mDcv < 1 && v.mOmcv < 1 && v.mDmcv < 1 &&
            v.mSpd < 1 && v.mPd < 1  && v.mEd < 1  && v.mRec < 1 && v.mEnd < 1 && v.mBody < 1 && v.mStun < 1) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        QString sep = "";
        if (v.mStr > 0)  { res += sep + QString("+%1 STR").arg(v.mStr);   sep = ", "; }
        if (v.mDex > 0)  { res += sep + QString("+%1 DEX").arg(v.mDex);   sep = ", "; }
        if (v.mCon > 0)  { res += sep + QString("+%1 CON").arg(v.mCon);   sep = ", "; }
        if (v.mInt > 0)  { res += sep + QString("+%1 INT").arg(v.mInt);   sep = ", "; }
        if (v.mEgo > 0)  { res += sep + QString("+%1 EGO").arg(v.mEgo);   sep = ", "; }
        if (v.mPre > 0)  { res += sep + QString("+%1 PRE").arg(v.mPre);   sep = ", "; }
        if (v.mOcv > 0)  { res += sep + QString("+%1 OCV").arg(v.mOcv);   sep = ", "; }
        if (v.mDcv > 0)  { res += sep + QString("+%1 DCV").arg(v.mDcv);   sep = ", "; }
        if (v.mOmcv > 0) { res += sep + QString("+%1 OMCV").arg(v.mOmcv); sep = ", "; }
        if (v.mDmcv > 0) { res += sep + QString("+%1 DMCV").arg(v.mDmcv); sep = ", "; }
        if (v.mSpd > 0)  { res += sep + QString("+%1 SPD").arg(v.mSpd);   sep = ", "; }
        if (v.mPd > 0)   { res += sep + QString("+%1 PD").arg(v.mPd);     sep = ", "; }
        if (v.mEd > 0)   { res += sep + QString("+%1 ED").arg(v.mEd);     sep = ", "; }
        if (v.mRec > 0)  { res += sep + QString("+%1 REC").arg(v.mRec);   sep = ", "; }
        if (v.mEnd > 0)  { res += sep + QString("+%1 END").arg(v.mEnd);   sep = ", "; }
        if (v.mBody > 0) { res += sep + QString("+%1 BODY").arg(v.mBody); sep = ", "; }
        if (v.mStun > 0) { res += sep + QString("+%1 STUN").arg(v.mStun); sep = ", "; }
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

class Clinging: public AllPowers {
public:
    Clinging(): AllPowers("Clinging")                  { }
    Clinging(const Clinging& s): AllPowers(s)          { }
    Clinging(Clinging&& s): AllPowers(s)               { }
    Clinging(const QJsonObject& json): AllPowers(json) { v.mStr  = json["str"].toInt(0);
                                                              v.mKnck = json["knck"].toBool(false);
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
    Fraction lim() override                                      { return v.mKnck ? Fraction(1, 4) : Fraction(0); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return 10_cp + (v.mStr + 1) / 3;;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   str->setText(QString("%1").arg(s.mStr));
                                                                   knck->setChecked(s.mKnck);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mStr  = str->text().toInt();
                                                                   v.mKnck = knck->isChecked();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["str"]  = v.mStr;
                                                                   obj["knck"] = v.mKnck;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int  mStr  = 0;
        bool mKnck = false;
    } v;

    QLineEdit* str;
    QCheckBox* knck;

    QString optOut(bool showEND) {
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1 STR Clinging").arg(v.mStr);
        if (v.mKnck) res += "; Cannot Resist Knockback";
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

class Dispel: public AllPowers {
public:
    Dispel(): AllPowers("Dispel")                  { }
    Dispel(const Dispel& s): AllPowers(s)          { }
    Dispel(Dispel&& s): AllPowers(s)               { }
    Dispel(const QJsonObject& json): AllPowers(json) { v.mDice  = json["dice"].toInt(0);
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
                                                                   return v.mDice * 3_cp;
                                                                 }
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
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("%1d6 Dispel").arg(v.mDice);
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

class LifeSupport: public AllPowers {
public:
    LifeSupport(): AllPowers("Life Support")              { }
    LifeSupport(const LifeSupport& s): AllPowers(s)       { }
    LifeSupport(LifeSupport&& s): AllPowers(s)            { }
    LifeSupport(const QJsonObject& json): AllPowers(json) { v.mExtend = json["extend"].toInt(0);
                                                                 v.mSelf   = json["self"].toBool(false);
                                                                 v.mExpand = json["expand"].toString();
                                                                 v.mEating = json["eating"].toInt(0);
                                                                 v.mSleep  = json["sleep"].toInt(0);
                                                                 v.mSelpv  = json["selpv"].toBool(false);
                                                                 v.mSehp   = json["sehp"].toBool(false);
                                                                 v.mSehr   = json["sehr"].toBool(false);
                                                                 v.mSeic   = json["seic"].toBool(false);
                                                                 v.mSeih   = json["seih"].toBool(false);
                                                                 v.mLong   = json["long"].toInt(0);
                                                                 v.mImmun  = json["immun"].toInt(0);
                                                                 v.mTo     = json["to"].toString();
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
                                                                   return ((v.mExtend >= 1) ? v.mExtend * 1_cp : 0_cp) +
                                                                          (v.mSelf ? 10_cp : 0_cp) +
                                                                          countCommas(v.mExpand) * 5_cp +
                                                                          ((v.mEating >= 1) ? v.mEating * 1_cp : 0_cp) +
                                                                          ((v.mSleep >= 1) ? v.mSleep * 1_cp : 0_cp) +
                                                                          (v.mSelpv ? 2_cp : 0_cp) +
                                                                          (v.mSehp ? 1_cp : 0_cp) +
                                                                          (v.mSehr ? 2_cp : 0_cp) +
                                                                          (v.mSeic ? 2_cp : 0_cp) +
                                                                          (v.mSeih ? 2_cp : 0_cp) +
                                                                          ((v.mLong >= 1) ? v.mLong * 1_cp : 0_cp) +
                                                                          ((v.mImmun >= 1) ? ((v.mImmun >= 5 && v.mImmun <= 6) ? 5_cp : ((v.mImmun == 7) ? 10_cp : v.mImmun * 1_cp)) : 0_cp);
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   extend->setCurrentIndex(s.mExtend);
                                                                   self->setChecked(s.mSelf);
                                                                   expand->setText(s.mExpand);
                                                                   eating->setCurrentIndex(s.mEating);
                                                                   sleep->setCurrentIndex(s.mSleep);
                                                                   selpv->setChecked(s.mSelpv);
                                                                   sehp->setChecked(s.mSehp);
                                                                   sehr->setChecked(s.mSehr);
                                                                   seic->setChecked(s.mSeic);
                                                                   seih->setChecked(s.mSeih);
                                                                   Long->setCurrentIndex(s.mLong);
                                                                   immun->setCurrentIndex(s.mImmun);
                                                                   to->setText(s.mTo);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mExtend = extend->currentIndex();
                                                                   v.mSelf   = self->isChecked();
                                                                   v.mExpand = expand->text();
                                                                   v.mEating = eating->currentIndex();
                                                                   v.mSleep  = sleep->currentIndex();
                                                                   v.mSelpv  = selpv->isChecked();
                                                                   v.mSehp   = sehp->isChecked();
                                                                   v.mSehr   = sehr->isChecked();
                                                                   v.mSeic   = seic->isChecked();
                                                                   v.mSeih   = seih->isChecked();
                                                                   v.mLong   = Long->currentIndex();
                                                                   v.mImmun  = immun->currentIndex();
                                                                   v.mTo     = to->text();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["extend"] = v.mExtend;
                                                                   obj["self"]   = v.mSelf;
                                                                   obj["expand"] = v.mExpand;
                                                                   obj["eating"] = v.mEating;
                                                                   obj["sleep"]  = v.mSleep;
                                                                   obj["selpv"]  = v.mSelpv;
                                                                   obj["sehp"]   = v.mSehp;
                                                                   obj["sehr"]   = v.mSehr;
                                                                   obj["seic"]   = v.mSeic;
                                                                   obj["seih"]   = v.mSeih;
                                                                   obj["long"]   = v.mLong;
                                                                   obj["immun"]  = v.mImmun;
                                                                   obj["to"]     = v.mTo;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     mExtend = -1;
        bool    mSelf   = false;
        QString mExpand = "";
        int     mEating = -1;
        int     mSleep  = -1;
        bool    mSelpv  = false;
        bool    mSehp   = false;
        bool    mSehr   = false;
        bool    mSeic   = false;
        bool    mSeih   = false;
        int     mLong   = -1;
        int     mImmun  = -1;
        QString mTo     = "";
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
        if (v.mExtend < 1 && !v.mSelf && v.mExpand.isEmpty() && v.mEating < 1 && v.mSleep  < 1 && !v.mSelpv && !v.mSehp && !v.mSehr && !v.mSeic && !v.mSeih && v.mLong < 1 &&
            v.mImmun < 1) return "<incomplete>";
        if (v.mImmun > 0 && v.mImmun < 5 && v.mTo.isEmpty()) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += QString("Life Support");
        QStringList extend { "", "1 END/Turn", "1 END/Minute", "1 END/5 Minutes", "1 End/Hour" };
        if (v.mExtend > 0) res += "; Extended Breating (" + extend[v.mExtend] + ")";
        if (v.mSelf) res += "; Self-Contained Breathing";
        if (!v.mExpand.isEmpty()) res += "; Expanded Breathing (" + v.mExpand + ")";
        QStringList eating { "", "1 Meal/Week", "1 Meal/Year", "Need Not Eat/Drink" };
        if (v.mEating > 0) res += "; Diminished Eating (" + eating[v.mEating] + ")";
        QStringList sleep { "", "8 Hours/Week", "8 Hours/Year", "Need Not Sleep" };
        if (v.mSleep > 0) res += "; Diminished Sleep (" + sleep[v.mSleep] + ")";
        QString sep = "; Safe Environment (";
        if (v.mSelpv) { res += sep + "Low Pressure/Vacuum"; sep = ", "; }
        if (v.mSehp)  { res += sep + "High Pressure";       sep = ", "; }
        if (v.mSehr)  { res += sep + "High Radiation";      sep = ", "; }
        if (v.mSeic)  { res += sep + "Intense Cold";        sep = ", "; }
        if (v.mSeih)  { res += sep + "Intense Heat";        sep = ", "; }
        if (sep == ", ") res += ")";
        QStringList Long { "", "200 Year Lifespan", "400 Year Lifespan", "800 Year Lifespan", "1,600 Year Lifespan", "Immortal" };
        if (v.mLong >= 1) res += "; " + Long[v.mLong];
        if (v.mImmun >= 1) {
            if (v.mImmun < 5) res += "; Immune to " + v.mTo;
            else if (v.mImmun == 5) res += "; Immunity Terrestrial Poisons";
            else if (v.mImmun == 6) res += "; Immunity Terrestrial Diseases";
            else res += "; Immunity Terrestrial Poisons/Diseases";
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
};

class Summon: public AllPowers {
public:
    Summon(): AllPowers("Summonϴ")                 { }
    Summon(const Summon& s): AllPowers(s)          { }
    Summon(Summon&& s): AllPowers(s)               { }
    Summon(const QJsonObject& json): AllPowers(json) { v.mPts    = json["pts"].toInt(0);
                                                            v.mDbl    = json["dbl"].toInt(0);
                                                            v.mCrtr   = json["crtr"].toString();
                                                            v.mAmi    = json["ami"].toInt(0);
                                                            v.mTasks  = json["tasks"].toInt(0);
                                                            v.mExpand = json["tasks"].toInt(0);
                                                            v.mSpec   = json["spec"].toBool(false);
                                                            v.mWeak   = json["weak"].toInt(0);
                                                            v.mAntag  = json["antag"].toInt(0);
                                                            v.mArrive = json["arrive"].toBool(false);
                                                            v.mFewer  = json["fewer"].toInt(0);
                                                            v.mStrong = json["strong"].toInt(0);
                                                            v.mMust   = json["must"].toBool(false);
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

    Fraction adv() override                                      { return ((v.mAmi > 0)     ? v.mAmi * Fraction(1, 4)   : Fraction(0)) +
                                                                          ((v.mTasks > 0)   ? v.mTasks * Fraction(1, 4) : Fraction(0)) +
                                                                          ((v.mExpand == 1) ? Fraction(1, 4)            : Fraction(0)) +
                                                                          ((v.mExpand == 2) ? Fraction(1, 2)            : Fraction(0)) +
                                                                          ((v.mExpand == 3) ? Fraction(1)               : Fraction(0)) +
                                                                          (v.mSpec          ? Fraction(1)               : Fraction(0)) +
                                                                          ((v.mWeak > 0)    ? v.mWeak * Fraction(1, 4)  : Fraction(0));
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
    Fraction lim() override                                      { return ((v.mAntag > 0)  ? v.mAntag * Fraction(1, 4)  : Fraction(0)) +
                                                                          (v.mArrive       ? Fraction(1, 2)             : Fraction(0)) +
                                                                          ((v.mFewer > 0)  ? v.mFewer * Fraction(1, 4)  : Fraction(0)) +
                                                                          ((v.mStrong > 0) ? v.mStrong * Fraction(1, 4) : Fraction(0)) +
                                                                          (v.mMust         ? Fraction(1, 2)             : Fraction(0)); }
    Points points(bool noStore = false) override               { if (!noStore) store();
                                                                   return (v.mPts + 2) / 5 * 1_cp + 5 * v.mDbl;
                                                                 }
    void     restore() override                                  { vars s = v;
                                                                   AllPowers::restore();
                                                                   pts->setText(QString("%1").arg(s.mPts));
                                                                   dbl->setText(QString("%1").arg(s.mDbl));
                                                                   crtr->setText(s.mCrtr);
                                                                   ami->setCurrentIndex(v.mAmi);
                                                                   tasks->setText(QString("%1").arg(s.mTasks));
                                                                   expand->setCurrentIndex(s.mExpand);
                                                                   spec->setChecked(s.mSpec);
                                                                   weak->setCurrentIndex(s.mWeak);
                                                                   antag->setCurrentIndex(v.mAntag);
                                                                   arrive->setChecked(v.mArrive);
                                                                   fewer->setText(QString("%1").arg(s.mFewer));
                                                                   strong->setCurrentIndex(v.mStrong);
                                                                   must->setChecked(v.mMust);
                                                                   v = s;
                                                                 }
    void     store() override                                    { AllPowers::store();
                                                                   v.mPts    = pts->text().toInt();
                                                                   v.mDbl    = dbl->text().toInt();
                                                                   v.mCrtr   = crtr->text();
                                                                   v.mAmi    = ami->currentIndex();
                                                                   v.mTasks  = tasks->text().toInt();
                                                                   v.mExpand = expand->currentIndex();
                                                                   v.mSpec   = spec->isChecked();
                                                                   v.mWeak   = weak->currentIndex();
                                                                   v.mAntag  = antag->currentIndex();
                                                                   v.mArrive = arrive->isChecked();
                                                                   v.mFewer  = fewer->text().toInt();
                                                                   v.mStrong = strong->currentIndex();
                                                                   v.mMust   = must->isChecked();
                                                                 }
    QJsonObject toJson() const override                          { QJsonObject obj = AllPowers::toJson();
                                                                   obj["pts"]    = v.mPts;
                                                                   obj["dbl"]    = v.mDbl;
                                                                   obj["crtr"]   = v.mCrtr;
                                                                   obj["ami"]    = v.mAmi;
                                                                   obj["tasks"]  = v.mTasks;
                                                                   obj["expand"] = v.mExpand;
                                                                   obj["spec"]   = v.mSpec;
                                                                   obj["weak"]   = v.mWeak;
                                                                   obj["antag"]  = v.mAntag;
                                                                   obj["arrive"] = v.mArrive;
                                                                   obj["fewer"]  = v.mFewer;
                                                                   obj["strong"] = v.mStrong;
                                                                   obj["must"]   = v.mMust;
                                                                   return obj;
                                                                 }

private:
    struct vars {
        int     mPts    = 0;
        int     mDbl    = 0;
        QString mCrtr   = "";
        int     mAmi    = -1;
        int     mTasks  = 0;
        int     mExpand = -1;
        bool    mSpec   = false;
        int     mWeak   = -1;
        int     mAntag  = -1;
        bool    mArrive = false;
        int     mFewer  = -1;
        int     mStrong = -1;
        bool    mMust   = false;
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
        if (v.mPts < 1 || v.mCrtr.isEmpty()) return "<incomplete>";
        QString res;
        if (showEND && !nickname().isEmpty()) res = nickname() + " " + end() + " ";
        res += "Summonϴ ";
        if (v.mDbl > 0) res += QString(" Up To %1 ").arg((int) pow(2, v.mDbl));
        res += QString("%1 pt %2").arg(v.mPts).arg(v.mCrtr);
        QStringList amicable { "", "Friendly", "Loyal", "Devoted", "Slavishly Devoted" };
        if (v.mAmi > 0) res += "; Creature(s) are " + amicable[v.mAmi];
        if (v.mTasks > 0) res += QString("; x%1 Tasks").arg((int) pow(2, v.mTasks));
        QStringList expand { "", "Very Limited Group of Creatures", "Limited Group of Creatures", "Any Creature" };
        if (v.mExpand > 0) res += "; " + expand[v.mExpand + 1];
        if (v.mSpec) res += "; Specific Beingϴ";
        QStringList weak { "", "Weak Willed", "Very Weak Willed" };
        if (v.mWeak > 0) res += "; " + weak[v.mWeak];
        QStringList antag { "", "Annoyed", "Hostile", "Violent" };
        if (v.mAntag > 0) res += "; " + antag[v.mAntag];
        if (v.mArrive) res += "; Arrives Under Own Power";
        if (v.mFewer > 0) res += QString("; Tasks/%1").arg((int) pow(2, v.mTasks));
        QStringList strong { "", "Strong Willed", "Very Strong Willed" };
        if (v.mStrong > 0) res += "; " + strong[v.mStrong];
        if (v.mMust) res += "; Summoned Being Must Inhabit Locale";
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
