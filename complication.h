#ifndef COMPLICATION_H
#define COMPLICATION_H

#include <functional>

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
    QCheckBox* createCheckBox(QWidget*, QVBoxLayout*, QString, std::_Mem_fn<void (Complication::*)(bool)>);
    QCheckBox* createCheckBox(QWidget*, QVBoxLayout*, QString);
    QComboBox* createComboBox(QWidget*, QVBoxLayout*, QString, QList<QString>, std::_Mem_fn<void (Complication::*)(int)>);
    QComboBox* createComboBox(QWidget*, QVBoxLayout*, QString, QList<QString>);
    QLabel*    createLabel(QWidget*, QVBoxLayout*, QString);
    QLineEdit* createLineEdit(QWidget*, QVBoxLayout*, QString, std::_Mem_fn<void (Complication::*)(QString)> callback);
    QLineEdit* createLineEdit(QWidget*, QVBoxLayout*, QString);

    QMap<QCheckBox*, std::_Mem_fn<void (Complication::*)(bool)>>     _callbacksCB;
    QMap<QComboBox*, std::_Mem_fn<void (Complication::*)(int)>>      _callbacksCBox;
    QMap<QLineEdit*, std::_Mem_fn<void (Complication::*)(QString)>>  _callbacksEdit;
    void empty(bool)      { }

public:
    Complication() { }
    virtual ~Complication() { }

    std::_Mem_fn<void (Complication::*)(bool)> none      = std::mem_fn(&Complication::empty);

    static const bool NoStore = true;

    virtual QString     description()                = 0;
    virtual void        form(QWidget*, QVBoxLayout*) = 0;
    virtual int         points(bool noStore = false) = 0;
    virtual void        restore()                    = 0;
    virtual void        store()                      = 0;
    virtual QJsonObject toJson()                     = 0;

    virtual void checked(bool) { }
    virtual void numeric(QString) { }

    void callback(QCheckBox*);
    void callback(QLineEdit*);
    void createForm(QWidget*, QVBoxLayout*);

    static QList<QString> Available();
    static Complication*  ByIndex(int);
    static Complication*  FromJson(QString, const QJsonObject&);

    bool isNumber(QString);

private:
    QLabel* _points;
    QLabel* _description;
};

#endif // COMPLICATION_H
