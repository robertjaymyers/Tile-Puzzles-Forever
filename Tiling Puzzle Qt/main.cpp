#include "TilingPuzzleQt.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	a.setWindowIcon(QIcon(":/TilingPuzzleQt/Icon/ProgramIcon.ico"));
    TilingPuzzleQt w;
    w.show();
    return a.exec();
}
