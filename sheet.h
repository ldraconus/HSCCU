#ifndef SHEET_H
#define SHEET_H

#include <QAbstractButton>
#include <QAbstractTableModel>
#include <QDialogButtonBox>
#include <QEvent>
#include <QMainWindow>
#include <QPrinter>

#include "powerdialog.h"
#include "complicationsdialog.h"
#include "skilldialog.h"
#ifdef __wasm__
#include "complicationsmenudialog.h"
#include "editmenudialog.h"
#include "filemenudialog.h"
#include "imgmenudialog.h"
#include "PowerMenuDialog.h"
#include "skillmenudialog.h"
#endif
#include "character.h"
#include "option.h"
#include "shared.h"
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

    shared_ptr<class optionDialog> _optionDlg;

    static Sheet* _sheet; // NOLINT

public:
    Sheet(QWidget *parent = nullptr);
    Sheet(const Sheet&) = delete;
    Sheet(const Sheet&&) = delete;

    Sheet& operator=(const Sheet&) = delete;
    Sheet& operator=(Sheet&&) = delete;

    ~Sheet() override;

    static Sheet& ref() { return* _sheet; }

    void       changed()   { _changed = true; }
    Character& character() { return _character; }
    Option     getOption() { return _option; }

    void        addPower(shared_ptr<Power>);
    void        fixButtonBox(QDialogButtonBox*);
    QStringList getBanners();
    void        setCell(QTableWidget*, int, int, QString, const QFont&, bool wordWrap = false);
    void        setCellLabel(QTableWidget*, int, int, QString, const QFont&);
    void        setCellLabel(QTableWidget*, int, int, QString);
    void        updateDisplay();
    void        updatePower(shared_ptr<Power>);

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

    gsl::owner<Sheet_UI*> getUi() { return Ui; }

    static const bool WordWrap = true;

#ifdef __wasm__
    gsl::owner<Ui::wasm*> UI() { return ui; }
#else
    gsl::owner<Ui::Sheet*> UI() { return ui; }
#endif

    Option& option() { return _option; }

private:
#ifndef __wasm__
    gsl::owner<Ui::Sheet*> ui = nullptr;
#else
    gsl::owner<Ui::wasm*> ui = nullptr;
    gsl::owner<QWidget*> editButton = nullptr;
    gsl::owner<QWidget*> fileButton = nullptr;
    gsl::owner<QWidget*> newButton = nullptr;
    gsl::owner<QWidget*> openButton = nullptr;
    gsl::owner<QWidget*> saveButton = nullptr;
    gsl::owner<QWidget*> cutButton = nullptr;
    gsl::owner<QWidget*> copyButton = nullptr;
    gsl::owner<QWidget*> pasteButton = nullptr;
    gsl::owner<QWidget*> optionButton = nullptr;
    gsl::owner<QAction*> action_File = nullptr;
    gsl::owner<QAction*> action_New = nullptr;
    gsl::owner<QAction*> action_Open = nullptr;
    gsl::owner<QAction*> action_Save = nullptr;
    gsl::owner<QAction*> action_Edit = nullptr;
    gsl::owner<QAction*> action_Cut = nullptr;
    gsl::owner<QAction*> actionC_opy = nullptr;
    gsl::owner<QAction*> action_Paste = nullptr;
    gsl::owner<QAction*> actionOptions = nullptr;

    shared_ptr<ComplicationsMenuDialog>  _complicationsMenuDialog = nullptr;
    shared_ptr<FileMenuDialog>           _fileMenuDialog          = nullptr;
    shared_ptr<EditMenuDialog>           _editMenuDialog          = nullptr;
    shared_ptr<ImgMenuDialog>            _imgMenuDialog           = nullptr;
    shared_ptr<PowerMenuDialog>          _powerMenuDialog         = nullptr;
    shared_ptr<SkillMenuDialog>          _skillMenuDialog         = nullptr;

#endif
    gsl::owner<Sheet_UI*>  Ui = nullptr;
    gsl::owner<QPrinter*>  printer = nullptr;

    static const bool DontUpdateTotal = false;
    static const bool noD6            = false;

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
    bool      _saveChanged = false;

    std::array<int, 19> _hitLocations { }; // NOLINT

    QMap<QObject*, _CharacteristicDef> _widget2Def;

    Points             characteristicsCost();
    void               characteristicChanged(QLineEdit*, QString, bool update = true);
    void               characteristicEditingFinished(QLineEdit*);
    bool               checkClose();
    void               clearHitLocations();
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
    void               hitLocations(std::shared_ptr<Power>&);
    void               justClose();
    QString            KAwSTR(int);

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
    void               rebuildBasicManeuvers(QFont& font);
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
    void               updateBanner();
    void               updateCharacteristics();
    void               updateCharacter();
    void               updateComplications();
    void               updateHitLocations();
    void               updatePowersAndEquipment();
    void               updateSkillsTalentsAndPerks();
    void               updateSkillRolls();
    void               updateSkills(shared_ptr<SkillTalentOrPerk>);
    void               updateTotals();
    QString            valueToDice(int, bool showD6 = !noD6);

    static Sheet_UI _Sheet_UI; // NOLINT

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
    static shared_ptr<class QMessageBox> Box; // NOLINT

    static std::function<void ()> _Cancel; // NOLINT
    static std::function<void ()> _No; // NOLINT
    static std::function<void ()> _Ok; // NOLINT
    static std::function<void ()> _Yes; // NOLINT

public slots:
    void button(QAbstractButton*);
};

#endif // SHEET_H
