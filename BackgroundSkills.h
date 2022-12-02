#ifndef BACKGROUNDSKILLS_H
#define BACKGROUNDSKILLS_H

#include "character.h"
#include "sheet.h"
#include "skilltalentorperk.h"

class BackgroundSkill: public SkillTalentOrPerk {
public:
    BackgroundSkill(): SkillTalentOrPerk() { }
    BackgroundSkill(QString name)
        : _name(name) { }
    BackgroundSkill(const BackgroundSkill& s)
        : SkillTalentOrPerk()
        , _name(s._name) { }
    BackgroundSkill(BackgroundSkill&& s)
        : SkillTalentOrPerk()
        , _name(s._name) { }
    BackgroundSkill(const QJsonObject& json)
        : SkillTalentOrPerk()
        , _name(json["name"].toString("")) { }

    virtual BackgroundSkill& operator=(const BackgroundSkill& s) {
        if (this != &s) {
            _name = s._name;
        }
        return *this;
    }
    virtual BackgroundSkill& operator=(BackgroundSkill&& s) {
        _name = s._name;
        return *this;
    }

    QString description(bool showRoll = false) override { return showRoll ? _name : _name; }
    void form(QWidget*, QVBoxLayout*) override          { }
    int points(bool noStore = false) override           { return noStore ? 0 : 0; }
    void restore() override                             { }
    QString roll() override                             { return ""; }
    void    store() override                            { }

    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"] = _name;
        return obj;
    }

private:
    QString _name;
};

#define CLASS(x)\
    class x: public BackgroundSkill {\
    public:\
        x(): BackgroundSkill(#x) { }\
        x(const x& s): BackgroundSkill(s) { }\
        x(x&& s): BackgroundSkill(s) { }\
        x(const QJsonObject& json): BackgroundSkill(json) { }\
    };
#define CLASS_SPACE(x,y)\
    class x: public BackgroundSkill {\
    public:\
        x(): BackgroundSkill(y) { }\
        x(const x& s): BackgroundSkill(s) { }\
        x(x&& s): BackgroundSkill(s) { }\
        x(const QJsonObject& json): BackgroundSkill(json) { }\
    };

class KS: public BackgroundSkill {
public:
    KS(): BackgroundSkill("Knowledge Skill")           { }
    KS(const KS& s): BackgroundSkill(s)    { }
    KS(KS&& s): BackgroundSkill(s)         { }
    KS(const QJsonObject& json): BackgroundSkill(json) { _introll = json["introll"].toBool(false);
                                                         _plus    = json["plus"].toInt(1);
                                                         _for     = json["for"].toString("");
                                                         _type    = json["type"].toInt(0);
                                                       }

    QString description(bool showRoll = false) override         { return (showRoll ? (_introll ? add(Sheet::ref().character().DEX().roll(), _plus)
                                                                                              : QString("%1-").arg(11 + _plus)) + " "
                                                                                   : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { introll = createCheckBox(parent, layout, "Base off INT");
                                                                  plus = createLineEdit(parent, layout, "How many pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  type = createComboBox(parent, layout, "Type of Knowledge Skill", { "Group", "People", "Areas", "City", "Things" });
                                                                  forwhat = createLineEdit(parent, layout, "Applies to what?");
                                                                }
    int     points(bool noStore = false) override               { if (!noStore) store();
                                                                  return _plus * 2 + (_introll ? 3 : 2);
    }
    void    restore() override                                  { plus->setText(QString("%1").arg(_plus));
                                                                  type->setCurrentIndex(_type);
                                                                  forwhat->setText(_for);
                                                                  introll->setChecked(_introll);
                                                                }
    QString roll() override                                     { return _introll ? add(Sheet::ref().character().DEX().roll(), _plus) : QString("%1-").arg(11 + _plus); }
    void    store() override                                    { _plus = plus->text().toInt(0);
                                                                  _for  = forwhat->text();
                                                                  _type = type->currentIndex();
                                                                  _introll = introll->isChecked();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = BackgroundSkill::toJson();
                                                                  obj["plus"] = _plus;
                                                                  obj["for"]  = _for;
                                                                  obj["type"] = _type;
                                                                  obj["introll"] = _introll;
                                                                  return obj;
                                                                }

private:
    int     _introll = -1;
    int     _plus    = 0;
    QString _for     = "";
    int     _type    = -1;

    QCheckBox* introll;
    QLineEdit* plus;
    QLineEdit* forwhat;
    QComboBox* type;

    QString optOut() {
        if (_plus < 0 || _for.isEmpty()) return "<incomplete>";
        QString res;
        switch (_type) {
        case 0:
        case 1:
        case 4: res += QString("KS: %2 (+%1)").arg(_plus).arg(_for); break;
        case 2: res += QString("AK: %2 (+%1)").arg(_plus).arg(_for); break;
        case 3: res += QString("CK: %2 (+%1)").arg(_plus).arg(_for); break;
        default: return "<incomplete>";
        }

        return res;
    }

    void numeric(QString) override {
        QString txt = plus->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        plus->undo();
    }
};

class Language: public BackgroundSkill {
public:
    Language(): BackgroundSkill("Language")           { }
    Language(const Language& s): BackgroundSkill(s)    { }
    Language(Language&& s): BackgroundSkill(s)         { }
    Language(const QJsonObject& json): BackgroundSkill(json) { _which    = json["which"].toString("");
                                                               _level    = json["level"].toInt(0);
                                                               _literate = json["literate"].toBool(false);
                                                             }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { which    = createLineEdit(parent, layout, "Which language?");
                                                                  level    = createComboBox(parent, layout, "Fluency", { });
                                                                  literate = createCheckBox(parent, layout, "Literate (If not standard)");
                                                                }
    int     points(bool noStore = false) override               { if (!noStore) store();
                                                                  return _level + 1 + (_literate ? 1 : 0);
    }
    void    restore() override                                  { which->setText(_which);
                                                                  level->setCurrentIndex(_level);
                                                                  literate->setChecked(_literate);
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { _which    = which->text();
                                                                  _level    = level->currentIndex();
                                                                  _literate = literate->isChecked();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = BackgroundSkill::toJson();
                                                                  obj["which"]    = _which;
                                                                  obj["level"]    = _level;
                                                                  obj["literate"] = _literate;
                                                                  return obj;
                                                                }

private:
    QString _which    = "";
    int     _level    = -1;
    bool    _literate = "";

    QLineEdit* which;
    QComboBox* level;
    QCheckBox* literate;

    QString optOut() {
        if (_level < 0 || _which.isEmpty()) return "<incomplete>";
        return "Language: " + _which + " (" + QStringList{ "Basic", "Fluent", "Fluent w/accent1", "Idiomatic", "Imitate Dialects" }[_level] + (_literate ? ", Literate)" : ")");
    }
};

class PS: public BackgroundSkill {
public:
    PS(): BackgroundSkill("Professional Skill")           { }
    PS(const PS& s): BackgroundSkill(s)    { }
    PS(PS&& s): BackgroundSkill(s)         { }
    PS(const QJsonObject& json): BackgroundSkill(json) { _what = json["what"].toString("");
                                                         _plus = json["plus"].toInt(0);
                                                         _stat = json["stat"].toInt(-1);
                                                       }

    QString description(bool showRoll = false) override         { return (showRoll ? "(" + QString("+%1").arg(_plus) + ") ": "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { what = createLineEdit(parent, layout, "What profession?");
                                                                  plus = createLineEdit(parent, layout, "Pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  stat = createComboBox(parent, layout, "Base on a stat?", { "STR", "DEX", "CON", "INT", "EGO", "PRE"});
                                                                }
    int     points(bool noStore = false) override               { if (!noStore) store();
                                                                  return _plus * 2 + (2 + ((_stat >= 0) ? 1 : 0));
                                                                }
    void    restore() override                                  { what->setText(_what);
                                                                  plus->setText(QString("%1").arg(_plus));
                                                                  stat->setCurrentIndex(_stat);
                                                                }
    QString roll() override                                     { return (_stat >= 0) ? add(Sheet::ref().character().characteristic(_stat).roll(), _plus)
                                                                                      : QString("%1-").arg(11 + _plus); }
    void    store() override                                    { _what = what->text();
                                                                  _plus = plus->text().toInt(0);
                                                                  _stat = stat->currentIndex();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = BackgroundSkill::toJson();
                                                                  obj["what"] = _what;
                                                                  obj["plus"] = _plus;
                                                                  obj["stat"] = _stat;
                                                                  return obj;
                                                                }

private:
    QString _what = "";
    int     _plus = 0;
    int     _stat  = -1;

    QLineEdit* what;
    QLineEdit* plus;
    QComboBox* stat;

    QString optOut() {
        if (_plus <= 0 || _what.isEmpty()) return "<incomplete>";
        return "PS: " + _what + ((_stat != -1) ? QStringList { " (STR)", " (DEX)", " (CON)", " (INT)", " (EGO)", " (PRE)" }[_stat] : "");
    }

    void numeric(QString) override {
        QString txt = plus->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        plus->undo();
    }
};

class SS: public BackgroundSkill {
public:
    SS(): BackgroundSkill("Science Skill")           { }
    SS(const SS& s): BackgroundSkill(s)    { }
    SS(SS&& s): BackgroundSkill(s)         { }
    SS(const QJsonObject& json): BackgroundSkill(json) { _what = json["what"].toString("");
                                                         _plus = json["plus"].toInt(0);
                                                         _int  = json["int"].toBool(false);
                                                       }

    QString description(bool showRoll = false) override         { return (showRoll ? "(" + QString("+%1").arg(_plus) + ") ": "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { what    = createLineEdit(parent, layout, "What profession?");
                                                                  plus    = createLineEdit(parent, layout, "Pluses?", std::mem_fn(&SkillTalentOrPerk::numeric));
                                                                  intstat = createCheckBox(parent, layout, "Based on INT");
                                                                }
    int     points(bool noStore = false) override               { if (!noStore) store();
                                                                  return _plus * 2 + (2 + (_int ? 1 : 0));
                                                                }
    void    restore() override                                  { what->setText(_what);
                                                                  plus->setText(QString("%1").arg(_plus));
                                                                  intstat->setChecked(_int);
                                                                }
    QString roll() override                                     { return _int ? add(Sheet::ref().character().INT().roll(), _plus)
                                                                              : QString("%1-").arg(11 + _plus); }
    void    store() override                                    { _what = what->text();
                                                                  _plus = plus->text().toInt(0);
                                                                  _int  = intstat->isChecked();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = BackgroundSkill::toJson();
                                                                  obj["what"] = _what;
                                                                  obj["plus"] = _plus;
                                                                  obj["int"]  = _int;
                                                                  return obj;
                                                                }

private:
    QString _what = "";
    int     _plus = 0;
    bool    _int = false;

    QLineEdit* what;
    QLineEdit* plus;
    QCheckBox* intstat;

    QString optOut() {
        if (_plus <= 0 || _what.isEmpty()) return "<incomplete>";
        return "SS: " + _what + (_int ? " (INT)" : "");
    }

    void numeric(QString) override {
        QString txt = plus->text();
        if (txt.isEmpty() || isNumber(txt)) return;
        plus->undo();
    }
};

class TF: public BackgroundSkill {
public:
    TF(): BackgroundSkill("Transport Familiarity")     { }
    TF(const TF& s): BackgroundSkill(s)                { }
    TF(TF&& s): BackgroundSkill(s)                     { }
    TF(const QJsonObject& json): BackgroundSkill(json) { _what = json["what"].toInt(0);
                                                         _with = json["with"].toString("");
                                                       }

    QString description(bool showRoll = false) override         { return (showRoll ? "" : "") + optOut(); }
    void    form(QWidget* parent, QVBoxLayout* layout) override { what = createComboBox(parent, layout, "Familar with?", { "One class of conveyances",
                                                                                                                           "Broad category of conveyances"});
                                                                  with = createLineEdit(parent, layout, "Applies to what?");
                                                                }
    int     points(bool noStore = false) override               { if (!noStore) store();
                                                                  return QList<int>{ 0, 1, 2 }[_what + 1]; }
    void    restore() override                                  { what->setCurrentIndex(_what);
                                                                  with->setText(_with);
                                                                }
    QString roll() override                                     { return ""; }
    void    store() override                                    { _what = what->currentIndex();
                                                                  _with = with->text();
                                                                }
    QJsonObject toJson() override                               { QJsonObject obj = BackgroundSkill::toJson();
                                                                  obj["what"] = _what;
                                                                  obj["with"] = _with;
                                                                  return obj;
                                                                }

private:
    int     _what = -1;
    QString _with = "";

    QComboBox* what;
    QLineEdit* with;

    QString optOut() {
        if (_with.isEmpty()) return "<incomplete>";
        QString res = "TF: ";
        switch (_what) {
        case 0: res += QString("(One Class of Conveyances; %1)").arg(_with); break;
        case 1: res += QString("(Broad Category of Conveyancess; %1)").arg(_with);   break;
        default: return "<incomplete>";
        }

        return res;
    }
};

#undef CLASS
#undef CLASS_SPACE

#endif // BACKGROUNDSKILLS_H
