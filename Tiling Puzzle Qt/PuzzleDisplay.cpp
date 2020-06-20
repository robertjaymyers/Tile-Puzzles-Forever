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

#include "PuzzleDisplay.h"

PuzzleDisplay::PuzzleDisplay(QWidget *parent)
	: QGraphicsView(parent)
{
	setRenderHint(QPainter::SmoothPixmapTransform);
	setScene(scene.get());

	/*
	FROM QT DOCUMENTATION:
	It's common to call fitInView() from inside a reimplementation of resizeEvent(), 
	to ensure that the whole scene, or parts of the scene, scales automatically to fit the 
	new size of the viewport as the view is resized. Note though, that calling fitInView() 
	from inside resizeEvent() can lead to unwanted resize recursion, if the new transformation 
	toggles the automatic state of the scrollbars. You can toggle the scrollbar policies to always 
	on or always off to prevent this (see horizontalScrollBarPolicy() and verticalScrollBarPolicy()).
	*/

	// For this reason, we to set scroll bars to always off. In our case, we don't want scrolling
	// to be part of the program, since puzzles should be viewable within the screen in full.
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	connect(scene.get(), &PuzzleDisplayScene::puzzleChanged, this, &PuzzleDisplay::resizeForPuzzleChange);
}

void PuzzleDisplay::resizeEvent(QResizeEvent *event)
{
	scene.get()->setSceneRect(this->rect());
	scene.get()->resizeScaleSmooth();
}

void PuzzleDisplay::resizeForPuzzleChange()
{
	scene.get()->setSceneRect(this->rect());
	scene.get()->resizeScaleSmooth();
}