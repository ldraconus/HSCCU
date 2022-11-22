#include "sheet.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Sheet w;
    w.show();
    return a.exec();
}
