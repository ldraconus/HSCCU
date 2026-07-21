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

    shared_ptr<class optionDialog> mOptionDlg;

    static Sheet* sSheet; // NOLINT

public:
    Sheet(QWidget *parent = nullptr);
    Sheet(const Sheet&) = delete;
    Sheet(const Sheet&&) = delete;

    Sheet& operator=(const Sheet&) = delete;
    Sheet& operator=(Sheet&&) = delete;

    ~Sheet() override;

    static Sheet& ref() { return* sSheet; }

    void       changed()   { mChanged = true; }
    Character& character() { return mCharacter; }
    Option     getOption() { return mOption; }

    void        addPower(shared_ptr<Power>);
    void        fixButtonBox(QDialogButtonBox*);
    QStringList getBanners();
    void        setCell(QTableWidget*, int, int, QString, const QFont&, bool wordWrap = false);
    void        setCellLabel(QTableWidget*, int, int, QString, const QFont&);
    void        setCellLabel(QTableWidget*, int, int, QString);
    void        updateDisplay();
    void        updatePower(shared_ptr<Power>);

    void closeEvent(QCloseEvent*) override;
    bool event(QEvent*) override;
    void mousePressEvent(QMouseEvent*) override;

    void doNothing() { }

    class cCharacteristicDef {
    public:
        cCharacteristicDef(Characteristic* c = nullptr, QLineEdit* v = nullptr, QLabel* p = nullptr, QLabel* r = nullptr)
            : mCharacteristic(c)
            , mValue(v)
            , mPoints(p)
            , mRoll(r) { }

        Characteristic* characteristic() { return mCharacteristic; }
        QLineEdit*      value()          { return mValue; }
        QLabel*         points()         { return mPoints; }
        QLabel*         roll()           { return mRoll; }

    private:
        Characteristic* mCharacteristic = nullptr;
        QLineEdit*      mValue          = nullptr;
        QLabel*         mPoints         = nullptr;
        QLabel*         mRoll           = nullptr;
    };

    Sheet_UI* getUi() { return Ui; }

    static const bool WordWrap = true;

#ifdef __wasm__
    Ui::wasm* UI() { return ui; }
#else
    Ui::Sheet* UI() { return ui; }
#endif

    Option& option() { return mOption; }

private:
#ifndef __wasm__
    Ui::Sheet* ui = nullptr;
#else
    Ui::wasm* ui = nullptr;
    QWidget* complicationsButton = nullptr;
    QWidget* editButton = nullptr;
    QWidget* fileButton = nullptr;
    QWidget* imageButton = nullptr;
    QWidget* powersAndEquipmentButton = nullptr;
    QWidget* skillsTalentsAndPerksButton = nullptr;

public:
    QAction* action_File = nullptr;
    QAction* action_New = nullptr;
    QAction* action_Open = nullptr;
    QAction* action_Save = nullptr;
    QAction* action_Edit = nullptr;
    QAction* action_Cut = nullptr;
    QAction* actionC_opy = nullptr;
    QAction* action_Paste = nullptr;
    QAction* actionOptions = nullptr;
    QAction* action_Image = nullptr;
    QAction* action_ImgNew = nullptr;
    QAction* action_ImgClear = nullptr;
    QAction* action_Complications = nullptr;
    QAction* action_CompNew = nullptr;
    QAction* action_CompDel = nullptr;
    QAction* action_CompCut = nullptr;
    QAction* action_CompCopy = nullptr;
    QAction* action_CompPaste = nullptr;
    QAction* action_CompMoveUp = nullptr;
    QAction* action_CompMoveDown = nullptr;
    QAction* action_Powers = nullptr;
    QAction* action_PowNew = nullptr;
    QAction* action_PowDel = nullptr;
    QAction* action_PowCut = nullptr;
    QAction* action_PowCopy = nullptr;
    QAction* action_PowPaste = nullptr;
    QAction* action_PowMoveUp = nullptr;
    QAction* action_PowMoveDown = nullptr;
    QAction* action_STP = nullptr;
    QAction* action_StpNew = nullptr;
    QAction* action_StpDel = nullptr;
    QAction* action_StpCut = nullptr;
    QAction* action_StpCopy = nullptr;
    QAction* action_StpPaste = nullptr;
    QAction* action_StpMoveUp = nullptr;
    QAction* action_StpMoveDown = nullptr;

private:
    shared_ptr<ComplicationsMenuDialog>  mCompMenuDialog  = nullptr;
    shared_ptr<FileMenuDialog>           mFileMenuDialog  = nullptr;
    shared_ptr<EditMenuDialog>           mEditMenuDialog  = nullptr;
    shared_ptr<ImgMenuDialog>            mImgMenuDialog   = nullptr;
    shared_ptr<PowerMenuDialog>          mPowerMenuDialog = nullptr;
    shared_ptr<SkillMenuDialog>          mSkillMenuDialog = nullptr;

#endif
    Sheet_UI*    Ui = nullptr;
    QPrinter*    printer = nullptr;
    bool         mExpired = true;
    bool         mRunning = false;
    QPointF      mTouchStart;
    QTimer       mLongPressTimer;
    QMouseEvent* mSyntheticEvent = nullptr;

    static const bool DontUpdateTotal = false;
    static const bool noD6            = false;

    bool   mChanged                    = false;
    Points mComplicationPoints         = 0_cp;
    Points mPowersOrEquipmentPoints    = 0_cp;
    Points mSkillsTalentsOrPerksPoints = 0_cp;
    Points mCharactersticPoints        = 0_cp;
    Points mTotalPoints                = 0_cp;

    shared_ptr<ComplicationsDialog> mCompDlg  = nullptr;
    shared_ptr<PowerDialog>         mPowerDlg = nullptr;
    shared_ptr<SkillDialog>         mSkillDlg = nullptr;

    Character mCharacter;
    QString   mDir;
    QString   mFilename;
    QFont     mFont;
    Option    mOption;
    bool      mSaveChanged = false;

    std::array<int, 19> mHitLocations { }; // NOLINT

    QMap<QObject*, cCharacteristicDef> mWidget2Def;

    Points             characteristicsCost();
    void               characteristicChanged(QLineEdit*, QString, bool update = true);
    void               characteristicEditingFinished(QLineEdit*);
    bool               checkClose();
    void               clearHitLocations();
    void               closeDialogs(QMouseEvent*);
#ifdef __EMSCRIPTEN__
    QWidget*           createToolBarItem(QToolBar*, QAction*, const QString, const QString, QAction*);
    void               createMenuItem(QAction*& action, const QString& name, const char* slot);
    QWidget*           createToolBarItem(QToolBar*, const QString, const QString, QAction*&, const char*);
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
#ifndef __EMSCRIPTEN__
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
    void               setCVs(cCharacteristicDef&, QLabel*);
    void               setDamage(cCharacteristicDef&, QLabel*);
    QString            setDefense(int, int);
    void               setDefense(cCharacteristicDef&, int, int, QLineEdit*);
    void               setDefense(int, int, int, int);
    void               setMaximum(cCharacteristicDef&, QLabel*, QLineEdit*);
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

    static Sheet_UI sSheet_UI; // NOLINT

public slots:
    void complicationDoubleClicked(QTableWidgetItem*)          { editComplication(); }
    void exitClicked()                                         { close(); }
    void powersandequipmentDoubleClicked(QTableWidgetItem*)    { editPowerOrEquipment(); }
    void skillstalentsandperksDoubleClicked(QTableWidgetItem*) { editSkillstalentsandperks(); }
    void heightChanged(QString txt)                            { mCharacter.height(txt); }
    void weightChanged(QString txt)                            { mCharacter.weight(txt); }
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
    void imageMenu();
    void powerMenu();
    void stpMenu();
    void complicationsMenu();
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

    static std::function<void ()> mCancel; // NOLINT
    static std::function<void ()> mNo; // NOLINT
    static std::function<void ()> mOk; // NOLINT
    static std::function<void ()> mYes; // NOLINT

public slots:
    void button(QAbstractButton*);
};

#endif // SHEET_H
