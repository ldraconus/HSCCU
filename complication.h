#ifndef COMPLICATION_H
#define COMPLICATION_H

#include "shared.h"

#include <functional>

#include <QCheckBox>
#include <QComboBox>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QVBoxLayout>

class Complication {
protected:
    using BoolCallback   = std::function<void (Complication*, bool)>;
    using IntCallback    = std::function<void (Complication*, int)>;
    using StringCallback = std::function<void (Complication*, QString)>;

    QCheckBox* createCheckBox(QWidget*, QVBoxLayout*, QString, BoolCallback);
    QCheckBox* createCheckBox(QWidget*, QVBoxLayout*, QString);
    QComboBox* createComboBox(QWidget*, QVBoxLayout*, QString, QList<QString>, IntCallback);
    QComboBox* createComboBox(QWidget*, QVBoxLayout*, QString, QList<QString>);
    QLabel*    createLabel(QWidget*, QVBoxLayout*, QString);
    QLineEdit* createLineEdit(QWidget*, QVBoxLayout*, QString, StringCallback);
    QLineEdit* createLineEdit(QWidget*, QVBoxLayout*, QString);

private:
    QMap<QCheckBox*, BoolCallback>   mCallbacksCB;
    QMap<QComboBox*, IntCallback>    mCallbacksCBox;
    QMap<QLineEdit*, StringCallback> mCallbacksEdit;

    void empty(bool) { }

public:
    Complication() { id({ }); }
    Complication(const Complication& c)
        : mGuid(c.mGuid) { }
    Complication(const Complication&&c)
        : mGuid(c.mGuid) { }
    virtual ~Complication() { }

    Complication& operator=(const Complication& c) { mGuid = c.mGuid; return *this; }
    Complication& operator=(Complication&& c)      { mGuid = c.mGuid; return *this; }

    static const bool NoStore = true;

    virtual QString     abbreviation() { return description(); }
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

protected:
    QString mGuid;

    void id(const QJsonObject& json) { mGuid = json["id"].toString(); if (mGuid.isEmpty()) mGuid = QUuid().toString(QUuid::WithoutBraces); }
};

class BlankComp: public Complication {
public:
    BlankComp(): Complication() { }
    BlankComp(const BlankComp& c)
        : Complication(c) { }
    BlankComp(BlankComp&& c)
        : Complication(std::move(c)) { }
    BlankComp(const QJsonObject& obj)
        : Complication() { id(obj); }
    ~BlankComp() override { }

    BlankComp& operator=(const BlankComp& c) { Complication::operator=(c); return *this; }
    BlankComp& operator=(BlankComp&& c)      { Complication::operator=(std::move(c)); return *this; }

    QString description() override               { return "-"; }
    void form(QWidget*, QVBoxLayout*) override   { throw "No Form"; }
    Points points(bool) override                 { return 0_cp; }
    void restore() override                      { }
    void store() override                        { }
    QJsonObject toJson() override {
        QJsonObject obj;
        obj["id"]     = mGuid;
        obj["name"]   = "Blank Line";
        return obj;
    }
};


#endif // COMPLICATION_H
