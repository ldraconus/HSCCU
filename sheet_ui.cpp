#include "sheet_ui.h"
#include "sheet.h"

#if !defined(__wasm__)
#include "ui_sheet.h"
#endif

#include <QFont>
#include <QGraphicsScene>

void Sheet_UI::graphicsViewSetup(QWidget **widget, QWidget **hidden) {
#if !defined(__wasm__)
    QLabel* label = nullptr;
    QLabel* optLabel = nullptr;
    QGraphicsScene* scene = new QGraphicsScene(Sheet::ref().UI()->graphicsView);
    Sheet::ref().UI()->graphicsView->setScene(scene);
    mWidget = new QWidget();
    auto* lyt = new QVBoxLayout();
    lyt->setContentsMargins(0, 0, 0, 0);
    lyt->setSpacing(0);
    mWidget->setLayout(lyt);

    *widget = label = new QLabel(mWidget);
    label->setObjectName("label");
    label->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
    label->setPixmap(QPixmap(QString::fromUtf8(":/gfx/Sheet.png")));
    lyt->addWidget(label);

    *hidden = optLabel = new QLabel(mWidget);
    optLabel->setObjectName("optLabel");
    optLabel->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
    optLabel->setPixmap(QPixmap(QString::fromUtf8(":/gfx/Page3.png")));
    lyt->addWidget(optLabel);

    scene->addWidget(mWidget);
    Sheet::ref().setWidgets(label, optLabel);

    auto* viewport = Sheet::ref().UI()->graphicsView->viewport();

    viewport->setAttribute(Qt::WA_AcceptTouchEvents);
    viewport->grabGesture(Qt::PinchGesture);
    viewport->installEventFilter(&Sheet::ref());
#endif
}
