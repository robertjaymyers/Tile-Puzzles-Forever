#include "TilingPuzzleQt.h"

TilingPuzzleQt::TilingPuzzleQt(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	ui.centralWidget->setLayout(layout.get());
	layout.get()->setMargin(0);
	layout.get()->addWidget(puzzleDisplay.get(), 0, 0);
	puzzleDisplay.get()->show();

	setWindowState(Qt::WindowMaximized);
}
