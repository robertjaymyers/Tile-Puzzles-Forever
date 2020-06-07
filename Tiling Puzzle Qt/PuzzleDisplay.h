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

protected:
	void resizeEvent(QResizeEvent *event);

private:
	std::unique_ptr<PuzzleDisplayScene> scene = std::make_unique<PuzzleDisplayScene>(this);

	void resizeForPuzzleChange();
};