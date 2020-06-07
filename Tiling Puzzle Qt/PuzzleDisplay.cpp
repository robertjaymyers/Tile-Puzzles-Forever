#include "PuzzleDisplay.h"

PuzzleDisplay::PuzzleDisplay(QWidget *parent)
	: QGraphicsView(parent)
{
	setStyleSheet("background: #FFFFFF; border: none;");

	scene.get()->setParent(this->parent());
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
}

void PuzzleDisplay::resizeEvent(QResizeEvent *event)
{
	fitInView(scene.get()->itemsBoundingRect(), Qt::KeepAspectRatio);
}