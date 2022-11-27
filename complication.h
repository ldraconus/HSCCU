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
    QCheckBox* createCheckBox(QWidget* parent, QVBoxLayout*, QString, std::_Mem_fn<void (Complication::*)(bool)>);
    QComboBox* createComboBox(QWidget* parent, QVBoxLayout*, QString, QList<QString>);
    QLabel*    createLabel(QWidget* parent, QVBoxLayout*, QString);
    QLineEdit* createLineEdit(QWidget*, QVBoxLayout*, QString);

    QMap<QCheckBox*, std::_Mem_fn<void (Complication::*)(bool)>> _callbacks;
    void empty(bool) { }

public:
    Complication() { }
    virtual ~Complication() { }

    std::_Mem_fn<void (Complication::*)(bool)> none = std::mem_fn(&Complication::empty);

    virtual QString     description()                = 0;
    virtual void        form(QWidget*, QVBoxLayout*) = 0;
    virtual int         points()                     = 0;
    virtual void        restore()                    = 0;
    virtual void        store()                      = 0;
    virtual QJsonObject toJson()                     = 0;

    virtual void checked(bool) { }

    void callback(QCheckBox*);
    void createForm(QWidget*, QVBoxLayout*);

    static QList<QString> Available();
    static Complication*  ByIndex(int);
    static Complication*  FromJson(QString, const QJsonObject&);

private:
    QLabel* _points;
    QLabel* _description;
};

#endif // COMPLICATION_H
