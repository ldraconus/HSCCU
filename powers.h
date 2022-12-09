#ifndef POWERS_H
#define POWERS_H

#include "powerdialog.h"
#include "fraction.h"

#include <functional>

#include <QCheckBox>
#include <QComboBox>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QObject>
#include <QTableWidget>
#include <QVBoxLayout>

class Power: public QWidget {
    Q_OBJECT

protected:
    QCheckBox*    createCheckBox(QWidget*, QVBoxLayout*, QString, std::_Mem_fn<void (Power::*)(bool)>);
    QCheckBox*    createCheckBox(QWidget*, QVBoxLayout*, QString);
    QComboBox*    createComboBox(QWidget*, QVBoxLayout*, QString, QList<QString>, std::_Mem_fn<void (Power::*)(int)>);
    QComboBox*    createComboBox(QWidget*, QVBoxLayout*, QString, QList<QString>);
    QLabel*       createLabel(QWidget*, QVBoxLayout*, QString);
    QLineEdit*    createLineEdit(QWidget*, QVBoxLayout*, QString, std::_Mem_fn<void (Power::*)(QString)> callback);
    QLineEdit*    createLineEdit(QWidget*, QVBoxLayout*, QString);

    QTableWidget* createAdvantages(QWidget* parent, QVBoxLayout* layout) {
        return PowerDialog::ref().createAdvantages(parent, layout);
    }
    QTableWidget* createLimitations(QWidget* parent, QVBoxLayout* layout) {
        return PowerDialog::ref().createLimitations(parent, layout);
    }

    QMap<QCheckBox*, std::_Mem_fn<void (Power::*)(bool)>>     _callbacksCB;
    QMap<QComboBox*, std::_Mem_fn<void (Power::*)(int)>>      _callbacksCBox;
    QMap<QLineEdit*, std::_Mem_fn<void (Power::*)(QString)>>  _callbacksEdit;
    void empty(bool)      { }

    QString add(QString n, int p) {
        QList<QString> vals = n.split("/");
        QString res;
        bool first = true;
        for (const auto& val: vals) {
            if (first) first = false;
            else res += "/";
            QList<QString> num = val.split("-");
            res += QString("%1-").arg(num[0].toInt(0) + p);
        }
        return res;
    }

    QWidget* _sender;

public:
    Ui::PowerDialog* ui;

    class adjustmentBase {
    public:
        adjustmentBase() { }
        adjustmentBase(const adjustmentBase&) { }
        adjustmentBase(adjustmentBase*) { }

        virtual Power* create()                        = 0;
        virtual Power* create(const QJsonObject& json) = 0;
    };

    template <typename T>
    class adjustmentPower: public adjustmentBase {
    public:
        adjustmentPower(): adjustmentBase()                          { }
        adjustmentPower(const adjustmentPower& b): adjustmentBase(b) { }
        adjustmentPower(adjustmentPower* b): adjustmentBase(b)       { }

        Power* create() override                        { return new T; }
        Power* create(const QJsonObject& json) override { return new T(json); }
    };

    Power();
    virtual ~Power() { }

    static const bool NoStore = true;
    static const bool ShowEND = true;

    virtual QString     description(bool roll = false) = 0;
    virtual void        form(QWidget*, QVBoxLayout*)   = 0;
    virtual QString     name()                         = 0;
    virtual int         points(bool noStore = false)   = 0;
    virtual QString     nickname()                     = 0;
    virtual void        restore()                      = 0;
    virtual void        store()                        = 0;
    virtual QJsonObject toJson()                       = 0;

    virtual Fraction adv()            { return Fraction(0); }
    virtual void     checked(bool)    { }
    virtual QString  end()            { return ""; }
    virtual Fraction lim()            { return Fraction(0); }
    virtual void     numeric(QString) { }

    QWidget* sender() { return _sender; }

    void callback(QCheckBox*);
    void callback(QLineEdit*);
    void createForm(QWidget*, QVBoxLayout*);

    static QList<QString>      Available();
    static void                ClearForm(QVBoxLayout*);
    static QList<QString>      AdjustmentPowers();
    static Power*              ByName(QString);
    static Power*              FromJson(QString, const QJsonObject&);

    bool isNumber(QString);

private:
    static QMap<QString, adjustmentBase*> _adjustmentPower;

public slots:
};

#endif // POWERS_H
