#ifndef SHEET_H
#define SHEET_H

#include <QAbstractTableModel>
#include <QEvent>
#include <QMainWindow>
#include <QPrinter>

#include "shared.h"
#include "character.h"
#include "option.h"
#include "sheet_ui.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Sheet; }
QT_END_NAMESPACE

// ϴ▲Ꚛϟ 03F4,25B2,A69A,03DF

// --- [Actual sheet definitions] ----------------------------------------------------------------------

class Sheet: public QMainWindow {
    Q_OBJECT

public:
    Sheet(QWidget *parent = nullptr);
    ~Sheet();

    static Sheet* _sheet;
    static Sheet& ref() { return* _sheet; }

    Character& character() { return _character; }

    void setCell(QTableWidget*, int, int, QString, const QFont&, bool wordWrap = false);
    void setCellLabel(QTableWidget*, int, int, QString, const QFont&);
    void setCellLabel(QTableWidget*, int, int, QString);
    void updatePower(shared_ptr<Power>);

    void closeEvent(QCloseEvent* event) override;

    class _CharacteristicDef {
    public:
        _CharacteristicDef(Characteristic* c = nullptr, QLineEdit* v = nullptr, QLabel* p = nullptr, QLabel* r = nullptr)
            : _characteristic(c)
            , _value(v)
            , _points(p)
            , _roll(r) { }

        Characteristic* characteristic() { return _characteristic; }
        QLineEdit*      value()          { return _value; }
        QLabel*         points()         { return _points; }
        QLabel*         roll()           { return _roll; }

    private:
        Characteristic* _characteristic = nullptr;
        QLineEdit*      _value          = nullptr;
        QLabel*         _points         = nullptr;
        QLabel*         _roll           = nullptr;
    };

    static const bool WordWrap = true;

private:
    Ui::Sheet* ui;
    Sheet_UI*  Ui;
    QPrinter*  printer;

    static const bool DontUpdateTotal = false;

    bool     _changed                    = false;
    Points<> _complicationPoints         = 0_cp;
    Points<> _powersOrEquipmentPoints    = 0_cp;
    Points<> _skillsTalentsOrPerksPoints = 0_cp;
    Points<> _totalPoints                = 0_cp;

    Character _character;
    QString   _dir;
    QString   _filename;
    QFont     _font;
    Option    _option;

    QMap<QObject*, _CharacteristicDef> _widget2Def;

    void               addPower(shared_ptr<Power>&);
    int                characteristicsCost();
    void               characteristicChanged(QLineEdit*, QString, bool update = true);
    void               characteristicEditingFinished(QLineEdit*);
    bool               checkClose();
    void               delPower(int);
    int                displayPowerAndEquipment(int& row, shared_ptr<Power> pe);
    bool               eventFilter(QObject* object, QEvent*) override;
    QString            formatLift(int);
    shared_ptr<Power>& getPower(int, QList<shared_ptr<Power>>&);
    void               putPower(int, shared_ptr<Power>);
    void               print(QPainter&, QPoint&, QWidget*);
    void               rebuildCharacteristics();
    QString            rebuildCombatSkillLevel(shared_ptr<SkillTalentOrPerk>);
    void               rebuildCombatSkillLevels();
    void               rebuildDefenses();
    void               rebuildMartialArt(shared_ptr<SkillTalentOrPerk>, QFont&);
    void               rebuildMartialArts();
    void               rebuildMovement();
    void               rebuildSenses();
    void               setCVs(_CharacteristicDef&, QLabel*);
    void               setDamage(_CharacteristicDef&, QLabel*);
    void               setDefense(_CharacteristicDef&, int, int);
    void               setMaximum(_CharacteristicDef&, QLabel*, QLineEdit*);
    void               setResistantDefense(int, int, int, int);
    void               updateCharacteristics();
    void               updateCharacter();
    void               updateComplications();
    void               updateDisplay();
    void               updatePowersAndEquipment();
    void               updateSkillsTalentsAndPerks();
    void               updateSkillRolls();
    void               updateSkills(shared_ptr<SkillTalentOrPerk>);
    void               updateTotals();
    QString            valueToDice(int);

    static Sheet_UI _Sheet_UI;

public slots:
    void complicationDoubleClicked(QTableWidgetItem*)          { editComplication(); }
    void exitClicked()                                         { close(); }
    void powersandequipmentDoubleClicked(QTableWidgetItem*)    { editPowerOrEquipment(); }
    void skillstalentsandperksDoubleClicked(QTableWidgetItem*) { editSkillstalentsandperks(); }
    void valChanged(QString txt)                               { characteristicChanged(dynamic_cast<QLineEdit*>(sender()), txt); }
    void valEditingFinished()                                  { characteristicEditingFinished(dynamic_cast<QLineEdit*>(sender())); }

    void aboutToHideFileMenu();
    void aboutToShowComplicationsMenu();
    void aboutToShowFileMenu();
    void aboutToShowPowersAndEquipmentMenu();
    void aboutToShowSkillsPerksAndTalentsMenu();
    void alternateIdsChanged(QString);
    void campaignNameChanged(QString);
    void characterNameChanged(QString);
    void clearImage();
    void complicationsMenu(QPoint);
    void copyComplication();
    void copyPowerOrEquipment();
    void copySkillTalentOrPerk();
    void currentBODYChanged(QString);
    void currentBODYEditingFinished();
    void currentENDChanged(QString);
    void currentENDEditingFinished();
    void currentSTUNChanged(QString);
    void currentSTUNEditingFinished();
    void cutComplication();
    void cutPowerOrEquipment();
    void cutSkillTalentOrPerk();
    void deleteComplication();
    void deletePowerOrEquipment();
    void deleteSkillstalentsandperks();
    void editComplication();
    void editPowerOrEquipment();
    void editSkillstalentsandperks();
    void eyeColorChanged(QString);
    void focusChanged(QWidget*, QWidget*);
    void gamemasterChanged(QString);
    void genreChanged(QString);
    void hairColorChanged(QString);
    void imageMenu(QPoint);
    void moveComplicationDown();
    void moveComplicationUp();
    void movePowerOrEquipmentDown();
    void movePowerOrEquipmentUp();
    void moveSkillTalentOrPerkDown();
    void moveSkillTalentOrPerkUp();
    void newchar();
    void newComplication();
    void newImage();
    void newPowerOrEquipment();
    void newSkillTalentOrPerk();
    void open();
    void pageSetup();
    void pasteComplication();
    void pastePowerOrEquipment();
    void pasteSkillTalentOrPerk();
    void playerNameChanged(QString);
    void powersandequipmentMenu(QPoint);
    void print();
    void save();
    void saveAs();
    void skillstalentsandperksMenu(QPoint);
    void totalExperienceEarnedChanged(QString);
    void totalExperienceEarnedEditingFinished();
};

#endif // SHEET_H
