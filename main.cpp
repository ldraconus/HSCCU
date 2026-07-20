#ifdef __wasm__
#include "wasm.h"
#else
#include "sheet.h"
#endif

#include <QApplication>

#ifdef __wasm__
QApplication* g_app = nullptr;
Sheet* g_sheet = nullptr;

int main(int argc, char* argv[]) {
    g_app = new QApplication(argc, argv);
    g_app->setStyleSheet("QMenu::item { background-color: #ccc;"
                         "              selection-background-color: #aaa;"
                         "              color: black; }"
                         "QMenu::Item:disabled { color: #888; }"
                         "QMenu::separator { background: #ccc;"
                         "                   border-color: black; }"
                         "QCheckBox { color: black }"
                         "QToolTip { color: #ffffff;"
                         "           background-color: #333333;"
                         "           border: 1px solid #555555;"
                         "           font-size: 12px; }");
    g_sheet = new Sheet();
    g_sheet->show();
    return 0;
}
#else
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setStyleSheet("QMenu { color: #ffffff;"
                    "        background-color: #2b2b2b;"
                    "        border: 1px solid #555555;"
                    "        padding: 0px 0px; }"
                    "QMenu::item { background-color: #ccc;"
                    "              selection-background-color: #aaa;"
                    "              color: black; }"
                    "QMenu::Item:disabled { color: #888; }"
                    "QMenu::separator { background: #ccc;"
                    "                   border-color: black; }"
                    "QCheckBox { color: black }"
                    "QToolTip { color: #ffffff !important;"
                    "           background-color: #333333 !important;"
                    "           border: 1px solid #555555 !important;"
                    "           padding: 3px;"
                    "           font-size: 12px; }");
    QPalette toolTipPalette = QApplication::palette();
    toolTipPalette.setColor(QPalette::ToolTipBase, QColor(51, 51, 51));   // Dark grey background
    toolTipPalette.setColor(QPalette::ToolTipText, QColor(255, 255, 255)); // White text
    QApplication::setPalette(toolTipPalette);
    Sheet w;
    w.show();
    return a.exec();
}
#endif
