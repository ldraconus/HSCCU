#ifndef SHEET_H
#define SHEET_H

#include <QAbstractButton>
#include <QAbstractTableModel>
#include <QEvent>
#include <QMainWindow>
#include <QPrinter>

#include "shared.h"

#include "powerdialog.h"
#include "complicationsdialog.h"
#include "skilldialog.h"
#ifdef __wasm__
#include "complicationsmenudialog.h"
#include "editmenudialog.h"
#include "filemenudialog.h"
#include "imgmenudialog.h"
#include "skillmenudialog.h"
#endif
#include "character.h"
#include "option.h"
#include "sheet_ui.h"

QT_BEGIN_NAMESPACE
#ifndef __wasm__
namespace Ui { class Sheet; }
#else
namespace Ui { class wasm; }
#endif
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

    void       changed()   { _changed = true; }
    Character& character() { return _character; }
    Option     getOption() { return _option; }

    void addPower(shared_ptr<Power>);
    void setCell(QTableWidget*, int, int, QString, const QFont&, bool wordWrap = false);
    void setCellLabel(QTableWidget*, int, int, QString, const QFont&);
    void setCellLabel(QTableWidget*, int, int, QString);
    void updateDisplay();
    void updatePower(shared_ptr<Power>);

    void closeEvent(QCloseEvent*) override;
    void mousePressEvent(QMouseEvent*) override;

    void doNothing() { }

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

    Sheet_UI* getUi() { return Ui; }

    static const bool WordWrap = true;

#ifdef __wasm__
    Ui::wasm* UI() { return ui; }
#else
    Ui::Sheet* UI() { return ui; }
#endif

    shared_ptr<class optionDialog> _optionDlg;

    Option& option() { return _option; }

#ifndef __wasm__
private:
    Ui::Sheet* ui;
#else
    Ui::wasm* ui;
    QWidget* editButton;
    QWidget* fileButton;
    QWidget* newButton;
    QWidget* openButton;
    QWidget* saveButton;
    QWidget* cutButton;
    QWidget* copyButton;
    QWidget* pasteButton;
    QWidget* optionButton;
    QAction* action_File;
    QAction* action_New;
    QAction* action_Open;
    QAction* action_Save;
    QAction* action_Edit;
    QAction* action_Cut;
    QAction* actionC_opy;
    QAction* action_Paste;
    QAction* actionOptions;
#ifdef __wasm__
    shared_ptr<ComplicationsMenuDialog>  _complicationsMenuDialog = nullptr;
    shared_ptr<FileMenuDialog>           _fileMenuDialog          = nullptr;
    shared_ptr<EditMenuDialog>           _editMenuDialog          = nullptr;
    shared_ptr<ImgMenuDialog>            _imgMenuDialog           = nullptr;
    shared_ptr<SkillMenuDialog>          _skillMenuDialog         = nullptr;
#endif

private:
#endif
    Sheet_UI*  Ui;
    QPrinter*  printer;

    static const bool DontUpdateTotal = false;

    bool   _changed                    = false;
    Points _complicationPoints         = 0_cp;
    Points _powersOrEquipmentPoints    = 0_cp;
    Points _skillsTalentsOrPerksPoints = 0_cp;
    Points _charactersticPoints        = 0_cp;
    Points _totalPoints                = 0_cp;

    shared_ptr<ComplicationsDialog> _compDlg  = nullptr;
    shared_ptr<PowerDialog>         _powerDlg = nullptr;
    shared_ptr<SkillDialog>         _skillDlg = nullptr;

    Character _character;
    QString   _dir;
    QString   _filename;
    QFont     _font;
    Option    _option;
    bool      _saveChanged;

    QMap<QObject*, _CharacteristicDef> _widget2Def;

    Points             characteristicsCost();
    void               characteristicChanged(QLineEdit*, QString, bool update = true);
    void               characteristicEditingFinished(QLineEdit*);
    bool               checkClose();
    void               closeDialogs(QMouseEvent*);
#ifdef __wasm__
    QWidget*           createToolBarItem(QToolBar*, QAction*, const QString, const QString, QAction*);
    QWidget*           createToolBarItem(QToolBar*, const QString, const QString, QAction*);
    QWidget*           createToolBarItem(QToolBar*, QAction*, const QString);
    QWidget*           createToolBarItem(QToolBar*, const QString);
#endif
    void               delPower(int);
    void               deletePagefull(QPlainTextEdit*, double, QPainter*);
    void               deletePagefull(QTableWidget*);
    void               deletePagefull();
    int                displayPowerAndEquipment(int&, shared_ptr<Power>);
    void               doOpen();
    void               erase();
    bool               eventFilter(QObject*, QEvent*) override;
#ifndef __wasm__
    void               fileOpen();
#else
    void               fileOpen(const QByteArray&, QString);
#endif
    QString            formatLift(int);
    QString            getCharacter();
    int                getPageCount(QPlainTextEdit*, double, QPainter*);
    int                getPageCount(QTableWidget*);
    int                getPageCount();
    int                getPageLines(QPlainTextEdit*, double, QPainter*);
    shared_ptr<Power>& getPower(int, QList<shared_ptr<Power>>&);
    void               justClose();
    void               loadImage(QPixmap&, QString);
#ifdef __wasm__
    void               loadImage(const QByteArray&, QString);
#endif
    void               loadImage(QString);
    void               paste();
    void               putPower(int, shared_ptr<Power>);
    void               preparePrint(QPlainTextEdit*);
    void               preparePrint(QTableWidget*);
    void               print(QPainter&, QPoint&, QWidget*);
    void               rebuildCharFromPowers(QList<shared_ptr<Power>>&);
    void               rebuildCharacteristics();
    QString            rebuildCombatSkillLevel(shared_ptr<SkillTalentOrPerk>);
    void               rebuildCSLPower(QList<shared_ptr<Power>>&, bool&, QString&);
    void               rebuildCombatSkillLevels();
    void               rebuildDefFromPowers(QList<shared_ptr<Power>>&);
    void               rebuildDefenses();
    void               rebuildMartialArt(shared_ptr<SkillTalentOrPerk>, QFont&);
    void               rebuildMartialArts();
    void               rebuildMoveFromPowers(QList<shared_ptr<Power>>&, QMap<QString, int>&, QMap<QString, QString>&, QMap<QString, int>&);
    void               rebuildMovement();
    void               rebuildPowers(bool);
    void               rebuildSenseFromPowers(QList<shared_ptr<Power>>&, QString&);
    void               rebuildSenses();
#ifdef __wasm__
    void               removeMenuButtons();
#endif
    void               saveThenErase();
    void               saveThenExit();
    void               saveThenOpen();
    void               saveThenPaste();
    int                searchImprovedNoncombatMovement(QString);
    void               setupIcons();
    void               setCVs(_CharacteristicDef&, QLabel*);
    void               setDamage(_CharacteristicDef&, QLabel*);
    void               setDefense(_CharacteristicDef&, int, int, QLineEdit*);
    void               setDefense(int, int, int, int);
    void               setMaximum(_CharacteristicDef&, QLabel*, QLineEdit*);
#ifndef __wasm
    void               doLoadImage();
    void               skipLoadImage();
#endif
    void               updateCharacteristics();
    void               updateCharacter();
    void               updateComplications();
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
    void heightChanged(QString txt)                            { _character.height(txt); }
    void weightChanged(QString txt)                            { _character.weight(txt); }
    void valChanged(QString txt)                               { characteristicChanged(dynamic_cast<QLineEdit*>(sender()), txt); }
    void valEditingFinished()                                  { characteristicEditingFinished(dynamic_cast<QLineEdit*>(sender())); }

#ifndef __wasm__
    void aboutToHideEditMenu();
    void aboutToHideFileMenu();
#endif
    void aboutToShowComplicationsMenu();
#ifndef __wasm__
    void aboutToShowEditMenu();
    void aboutToShowFileMenu();
#endif
    void aboutToShowPowersAndEquipmentMenu();
    void aboutToShowSkillsPerksAndTalentsMenu();
    void acceptComplication();
    void acceptNewSkill();
    void alternateIdsChanged(QString);
    void campaignNameChanged(QString);
    void characterNameChanged(QString);
    void clearImage();
    void complicationsMenu(QPoint);
    void copyCharacter();
    void copyComplication();
    void copyPowerOrEquipment();
    void copySkillTalentOrPerk();
    void currentBODYChanged(QString);
    void currentBODYEditingFinished();
    void currentENDChanged(QString);
    void currentENDEditingFinished();
    void currentSTUNChanged(QString);
    void currentSTUNEditingFinished();
    void cutCharacter();
    void cutComplication();
    void cutPowerOrEquipment();
    void cutSkillTalentOrPerk();
    void deleteComplication();
    void deletePowerOrEquipment();
    void deleteSkillstalentsandperks();
    void doneEditComplication();
    void doneEditSkill();
    void editComplication();
    void editPowerOrEquipment();
    void editSkillstalentsandperks();
    void eyeColorChanged(QString);
#ifdef __wasm__
    void editMenu();
    void fileMenu();
#endif
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
    void noteChanged();
    void open();
    void options();
#ifdef __wasm__
    void outsideImageArea();
#endif
    void pasteCharacter();
    void pasteComplication();
    void pastePowerOrEquipment();
    void pasteSkillTalentOrPerk();
    void playerNameChanged(QString);
    void powersandequipmentMenu(QPoint);
    void print();
    void printCharacter(QPrinter*);
    void save();
    void saveAs();
    void skillstalentsandperksMenu(QPoint);
    void totalExperienceEarnedChanged(QString);
    void totalExperienceEarnedEditingFinished();
};

class Msg: public QObject {
    Q_OBJECT

public:
    static shared_ptr<class QMessageBox> Box;

    static std::function<void ()> _Cancel;
    static std::function<void ()> _No;
    static std::function<void ()> _Ok;
    static std::function<void ()> _Yes;

public slots:
    void button(QAbstractButton*);
};

#endif // SHEET_H
