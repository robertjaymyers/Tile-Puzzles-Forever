/*
This file is part of Tile Puzzles Forever.
	Tile Puzzles Forever is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	Tile Puzzles Forever is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with Tile Puzzles Forever.  If not, see <https://www.gnu.org/licenses/>.
*/

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
