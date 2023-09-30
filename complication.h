#ifndef COMPLICATION_H
#define COMPLICATION_H

#include "shared.h"

#include <functional>
#include <gsl/gsl>

#include <QCheckBox>
#include <QComboBox>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QVBoxLayout>

class Complication
{
protected:
    gsl::owner<QCheckBox*> createCheckBox(QWidget*, QVBoxLayout*, QString, std::_Mem_fn<void (Complication::*)(bool)>);
    gsl::owner<QCheckBox*> createCheckBox(QWidget*, QVBoxLayout*, QString);
    gsl::owner<QComboBox*> createComboBox(QWidget*, QVBoxLayout*, QString, QList<QString>, std::_Mem_fn<void (Complication::*)(int)>);
    gsl::owner<QComboBox*> createComboBox(QWidget*, QVBoxLayout*, QString, QList<QString>);
    gsl::owner<QLabel*>    createLabel(QWidget*, QVBoxLayout*, QString);
    gsl::owner<QLineEdit*> createLineEdit(QWidget*, QVBoxLayout*, QString, std::_Mem_fn<void (Complication::*)(QString)> callback);
    gsl::owner<QLineEdit*> createLineEdit(QWidget*, QVBoxLayout*, QString);

private:
    QMap<QCheckBox*, std::_Mem_fn<void (Complication::*)(bool)>>     _callbacksCB;
    QMap<QComboBox*, std::_Mem_fn<void (Complication::*)(int)>>      _callbacksCBox;
    QMap<QLineEdit*, std::_Mem_fn<void (Complication::*)(QString)>>  _callbacksEdit;
    void empty(bool)      { }

public:
    Complication() { }
    Complication(const Complication&) { }
    Complication(const Complication&&) { }
    virtual ~Complication() { }

    Complication& operator=(const Complication&) { return *this; }
    Complication& operator=(Complication&&)      { return *this; }

    static const bool NoStore = true;

    virtual QString     description()                = 0;
    virtual void        form(QWidget*, QVBoxLayout*) = 0;
    virtual Points      points(bool noStore = false) = 0;
    virtual void        restore()                    = 0;
    virtual void        store()                      = 0;
    virtual QJsonObject toJson()                     = 0;

    virtual void checked(bool) { }
    virtual void numeric(QString) { }

    void callback(QCheckBox*);
    void callback(QLineEdit*);
    void createForm(QWidget*, QVBoxLayout*);

    static QList<QString> Available();
    static shared_ptr<Complication> ByIndex(int);
    static shared_ptr<Complication> FromJson(QString, const QJsonObject&);

    bool isNumber(QString);
};

class BlankComp: public Complication {
public:
    BlankComp(): Complication() { }
    BlankComp(const BlankComp&)
        : Complication() { }
    BlankComp(BlankComp&&)
        : Complication() { }
    BlankComp(const QJsonObject&)
        : Complication() { }
    ~BlankComp() override { }

    BlankComp& operator=(const BlankComp&) { return *this; }
    BlankComp& operator=(BlankComp&&)      { return *this; }

    QString description() override               { return "-"; }
    void form(QWidget*, QVBoxLayout*) override   { throw "No Form"; }
    Points points(bool) override                 { return 0_cp; }
    void restore() override                      { }
    void store() override                        { }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["name"]   = "Blank Line";
        return obj;
    }
};


#endif // COMPLICATION_H
