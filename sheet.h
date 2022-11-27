#ifndef SHEET_H
#define SHEET_H

#include <QAbstractTableModel>
#include <QEvent>
#include <QMainWindow>

#include "character.h"
#include "complication.h"
#include "option.h"
#include "sheet_ui.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Sheet; }
QT_END_NAMESPACE

// --- [Actual sheet definitions] ----------------------------------------------------------------------

class Sheet: public QMainWindow
{
    Q_OBJECT

public:
    Sheet(QWidget *parent = nullptr);
    ~Sheet();

    class _CharacteristicDef {
    public:
        _CharacteristicDef(Characteristic* c = nullptr, QLineEdit* v = nullptr, QLabel* p = nullptr, QLabel* r = nullptr)
            : _characteristic(c)
            , _val(v)
            , _pts(p)
            , _roll(r) { }

        Characteristic* characteristic() { return _characteristic; }
        QLineEdit*      value()          { return _val; }
        QLabel*         points()         { return _pts; }
        QLabel*         roll()           { return _roll; }

    private:
        Characteristic* _characteristic = nullptr;
        QLineEdit*      _val            = nullptr;
        QLabel*         _pts            = nullptr;
        QLabel*         _roll           = nullptr;
    };

private:
    Ui::Sheet *ui;
    Sheet_UI  *Ui;

    static const bool DontUpdateTotal = false;

    bool      _changed            = false;
    int       _complicationPoints = 0;
    int       _totalPoints        = 0;

    Character            _character;
    QString              _dir;
    QString              _filename;
    Option               _option;

    QMap<QObject*, _CharacteristicDef> _widget2Def;

    static const bool WordWrap = true;

    int     characteristicsCost();
    void    characteristicChanged(QLineEdit*, QString, bool update = true);
    void    characteristicEditingFinished(QLineEdit*);
    QString formatLift(int);
    bool    eventFilter(QObject* object, QEvent*);
    void    setCVs(_CharacteristicDef&, QLabel*);
    void    setCell(QTableWidget*, int, int, QString, const QFont&, bool wordWrap = false);
    void    setDamage(_CharacteristicDef&, QLabel*);
    void    setDefense(_CharacteristicDef&, int, int);
    void    setMaximum(_CharacteristicDef&, QLabel*, QLineEdit*);
    void    updateCharacteristics();
    void    updateCharacter();
    void    updateComplications();
    void    updateDisplay();
    void    updateTotals();
    QString valueToDice(int);

    static Sheet_UI _Sheet_UI;

public slots:
    void valChanged(QString txt) { characteristicChanged(dynamic_cast<QLineEdit*>(sender()), txt); }
    void valEditingFinished()    { characteristicEditingFinished(dynamic_cast<QLineEdit*>(sender())); }

    void aboutToShowComplicationsMenu();
    void alternateIdsChanged(QString);
    void campaignNameChanged(QString);
    void characterNameChanged(QString);
    void complicationsMenu(QPoint);
    void currentBODYChanged(QString);
    void currentBODYEditingFinished();
    void currentENDChanged(QString);
    void currentENDEditingFinished();
    void currentSTUNChanged(QString);
    void currentSTUNEditingFinished();
    void eyeColorChanged(QString);
    void gamemasterChanged(QString);
    void genreChanged(QString);
    void hairColorChanged(QString);
    void newchar();
    void newComplication();
    void open();
    void playerNameChanged(QString);
    void save();
    void saveAs();
    void totalExperienceEarnedChanged(QString);
    void totalExperienceEarnedEditingFinished();
};

#endif // SHEET_H
