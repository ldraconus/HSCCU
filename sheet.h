#ifndef SHEET_H
#define SHEET_H

#include <QMainWindow>

#include "character.h"
#include "option.h"
#include "sheet_ui.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Sheet; }
QT_END_NAMESPACE

class Sheet : public QMainWindow
{
    Q_OBJECT

public:
    Sheet(QWidget *parent = nullptr);
    ~Sheet();

private:
    Ui::Sheet *ui;
    Sheet_UI  *Ui;

    bool      _changed = false;
    Character _character;
    Option    _option;

public slots:
    void alternateIdsChanged(QString);
    void characterNameChanged(QString);
    void playerNameChanged(QString);
    void totalExperienceEarnedChanged(QString txt);
};

#endif // SHEET_H
