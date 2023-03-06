#ifdef __wasm__
#include "wasm.h"
#else
#include "sheet.h"
#endif

#include <QApplication>

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
