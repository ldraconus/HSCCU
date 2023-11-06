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
    g_app->setStyleSheet("QMenu::item { background-color: white;"
                         "              selection-background-color: lightgray;"
                         "              color: black; }"
                         "QMenu::Item:disabled { color: lightgray; }"
                         "QMenu::separator { background: white;"
                         "                   border-color: black; }"
                         "QCheckBox { color: black }");
    g_sheet = new Sheet();
    g_sheet->show();
    return 0;
}
#else
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyleSheet("QMenu::item { background-color: white;"
                    "              selection-background-color: lightgray;"
                    "              color: black; }"
                    "QMenu::Item:disabled { color: lightgray; }"
                    "QMenu::separator { background: white;"
                    "                   border-color: black; }"
                    "QCheckBox { color: black }");
    Sheet w;
    w.show();
    return a.exec();
}
#endif
