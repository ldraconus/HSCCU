#include "sheet_ui.h"
#include "sheet.h"

#ifdef Q_OS_ANDROID
#include "ui_android.h"
#endif

#include <QGraphicsScene>

void Sheet_UI::graphicsViewSetup(QWidget **widget, QWidget **hidden) {
#ifdef Q_OS_ANDROID
    QLabel* label = nullptr;
    QLabel* optLabel = nullptr;
    QWidget* base = nullptr;
    QGraphicsScene* scene = new QGraphicsScene(Sheet::ref().UI()->graphicsView);
    Sheet::ref().UI()->graphicsView->setScene(scene);
    base = new QWidget();
    auto* lyt = new QVBoxLayout();
    base->setLayout(lyt);
    *widget = label = new QLabel(base);
    *hidden = optLabel = new QLabel(base);
    scene->addWidget(base);
    Sheet::ref().setWidgets(label, optLabel);
#endif
}
