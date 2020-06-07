#pragma once

#include "PuzzleDisplayScene.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QPushButton>
#include <QDebug>
#include <random>
#include <chrono>
#include <algorithm>

class PuzzleDisplay : public QGraphicsView
{
	Q_OBJECT

public:
	PuzzleDisplay(QWidget *parent = nullptr);

protected:
	void resizeEvent(QResizeEvent *event);
	//void mousePressEvent(QMouseEvent *event);

private:
	std::unique_ptr<PuzzleDisplayScene> scene = std::make_unique<PuzzleDisplayScene>();
	//std::unique_ptr<QGraphicsPixmapItem> puzzle = std::make_unique<QGraphicsPixmapItem>(nullptr);
	//std::unique_ptr<QPushButton> btn = std::make_unique<QPushButton>();

	QPixmap testImg = QPixmap("C:/Users/Robert/source/repos/Tiling Puzzle Qt/x64/Debug/puzzles/lara17.png");

	struct puzzlePiece
	{
		const QPoint gridPoint;
		const QPointF pos;
		const int width;
		const int height;
		enum class SelectionState { UNSELECTED, SELECTED };
		SelectionState selectionState = SelectionState::UNSELECTED;
		//std::unique_ptr<QPushButton> item = std::make_unique<QPushButton>();
		std::unique_ptr<QGraphicsPixmapItem> item = std::make_unique<QGraphicsPixmapItem>(nullptr);
	};
	std::vector<puzzlePiece> puzzlePiecesAll;

	std::vector<QPointF> puzzlePieceCoordsForShuffle;
	//std::vector<std::unique_ptr<QGraphicsPixmapItem>> testVec;
};