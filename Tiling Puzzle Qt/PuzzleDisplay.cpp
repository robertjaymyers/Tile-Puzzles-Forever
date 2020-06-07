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

	/*setViewportMargins(-2, -2, -2, -2);
	setFrameStyle(QFrame::NoFrame);*/

	//scene.get()->addItem(puzzle.get());

	//int puzzleAnchorX = 0;
	//int puzzleAnchorY = 0;
	//int puzzlePosX = puzzleAnchorX;
	//int puzzlePosY = puzzleAnchorY;
	//int widthPieceSize = testImg.width() / 4;
	//int heightPieceSize = testImg.height() / 4;
	//for (int col = 0; col < 4; col++)
	//{
	//	for (int row = 0; row < 4; row++)
	//	{
	//		puzzlePiecesAll.emplace_back
	//		(
	//			puzzlePiece
	//			{
	//				QPoint(row, col),
	//				QPointF(puzzlePosX, puzzlePosY),
	//				widthPieceSize,
	//				heightPieceSize
	//			}
	//		);
	//		puzzlePiecesAll.back().item.get()->setPixmap
	//		(
	//			testImg.copy(QRect(puzzlePosX, puzzlePosY, widthPieceSize, heightPieceSize)) // x, y, w, h
	//		);
	//		puzzlePieceCoordsForShuffle.emplace_back(QPointF(puzzlePosX, puzzlePosY));
	//		puzzlePosX += widthPieceSize;
	//	}
	//	puzzlePosY += heightPieceSize;
	//	puzzlePosX = puzzleAnchorX;
	//}

	//int seed = std::chrono::system_clock::now().time_since_epoch().count();
	//shuffle(puzzlePieceCoordsForShuffle.begin(), puzzlePieceCoordsForShuffle.end(), std::default_random_engine(seed));

	////for (auto& piece : puzzlePiecesAll)
	////{
	////	piece.item.get()->setPos(piece.pos);
	////	//scene.get()->addItem(piece.item.get());
	////}

	//for (int i = 0; i < puzzlePiecesAll.size(); i++)
	//{
	//	puzzlePiecesAll[i].item.get()->setPos(puzzlePieceCoordsForShuffle[i]);
	//	scene.get()->addItem(puzzlePiecesAll[i].item.get());
	//}

	///*for (int i = 0; i < (16 / 4); i++)
	//{
	//	for (int j = 0; j < (16 / 4); j++)
	//	{

	//	}
	//}*/

	////puzzle.get()->setPixmap(QPixmap("C:/Users/Robert/source/repos/Tiling Puzzle Qt/x64/Debug/puzzles/lara17.png"));
	////scene.get()->setSceneRect(this->rect());
	////fitInView(scene.get()->sceneRect(), Qt::KeepAspectRatio);

	//qDebug() << scene.get()->sceneRect();
	//qDebug() << scene.get()->height();
}

void PuzzleDisplay::resizeEvent(QResizeEvent *event)
{
	fitInView(scene.get()->itemsBoundingRect(), Qt::KeepAspectRatio);
}

//void PuzzleDisplay::mousePressEvent(QMouseEvent *event)
//{
//	if (event->button() == Qt::LeftButton)
//	{
//		for (int i = 0; i < puzzlePiecesAll.size(); i++)
//		{
//			// check if mouse is within bound of image in scene
//			// if it is and no image is selected, change the image to selected state
//			// if it is and an image is selected, swap the clicked image with the selected one
//			// if it is and you clicked the selected image, unselect it
//
//			// selected image can use a generic overlay text resized to the proper size to cover the image
//			// or maybe just change the alpha of the selected image if that's a thing?
//			// looks like I may be able to use setOpacity
//			// that would certainly make it a lot simpler
//
//			if (puzzlePiecesAll[i].item.get()->contains(puzzlePiecesAll[i].item.get()->mapFromScene(event->scenePos())))
//			{
//				qDebug() << "clicked in area";
//				qDebug() << event->localPos();
//
//				if (puzzlePiecesAll[i].selectionState == puzzlePiece::SelectionState::UNSELECTED)
//				{
//					puzzlePiecesAll[i].item.get()->setOpacity(0.5);
//					puzzlePiecesAll[i].selectionState = puzzlePiece::SelectionState::SELECTED;
//				}
//				else if (puzzlePiecesAll[i].selectionState == puzzlePiece::SelectionState::SELECTED)
//				{
//					puzzlePiecesAll[i].item.get()->setOpacity(1);
//					puzzlePiecesAll[i].selectionState = puzzlePiece::SelectionState::UNSELECTED;
//				}
//			}
//
//			/*if (event->x() >= puzzlePiecesAll[i].item.get()->x() &&
//				event->x() <= puzzlePiecesAll[i].item.get()->x() + puzzlePiecesAll[i].width &&
//				event->y() >= puzzlePiecesAll[i].item.get()->y() &&
//				event->y() <= puzzlePiecesAll[i].item.get()->y() + puzzlePiecesAll[i].height)
//			{
//				qDebug() << "clicked in area";
//			}*/
//		}
//	}
//}

//bool mouseWithinRectBound(const SDL_MouseButtonEvent &btn, const SDL_Rect &rect)
//{
//	if (btn.x >= rect.x &&
//		btn.x <= rect.x + rect.w &&
//		btn.y >= rect.y &&
//		btn.y <= rect.y + rect.h)
//	{
//		return true;
//	}
//	return false;
//}