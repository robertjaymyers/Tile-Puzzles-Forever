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

#include "PuzzleDisplayScene.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QDebug>

class PuzzleDisplay : public QGraphicsView
{
	Q_OBJECT

public:
	PuzzleDisplay(QWidget *parent = nullptr);
	std::unique_ptr<PuzzleDisplayScene> scene = std::make_unique<PuzzleDisplayScene>(this);

protected:
	void resizeEvent(QResizeEvent *event);

private:
	void resizeForPuzzleChange();
};