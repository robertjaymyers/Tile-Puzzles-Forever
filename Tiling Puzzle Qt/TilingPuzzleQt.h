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

#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_TilingPuzzleQt.h"
#include "PuzzleDisplay.h"
#include <QGridLayout>

class TilingPuzzleQt : public QMainWindow
{
    Q_OBJECT

public:
    TilingPuzzleQt(QWidget *parent = Q_NULLPTR);

private:
    Ui::TilingPuzzleQtClass ui;

	std::unique_ptr<PuzzleDisplay> puzzleDisplay = std::make_unique<PuzzleDisplay>(this);
	std::unique_ptr<QGridLayout> layout = std::make_unique<QGridLayout>();
};
