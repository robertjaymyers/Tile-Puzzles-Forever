#include "TilingPuzzleQt.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TilingPuzzleQt w;
    w.show();
    return a.exec();
}
