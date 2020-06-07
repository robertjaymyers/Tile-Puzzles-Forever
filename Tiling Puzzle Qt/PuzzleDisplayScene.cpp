#include "PuzzleDisplayScene.h"

PuzzleDisplayScene::PuzzleDisplayScene(QObject *parent)
	: QGraphicsScene(parent)
{
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

	dirIteratorLoadPuzzles("C:/Users/Robert/source/repos/Tiling Puzzle Qt/x64/Debug/puzzles");
	shufflePuzzlesList();
	addCurrentPuzzleToScene();
	/*for (const auto& piece : puzzlesList[puzzleCurrent])
	{
		puzzlePieceCoordsForShuffle.emplace_back(QPointF(piece.originalPos));
	}
	int seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(puzzlePieceCoordsForShuffle.begin(), puzzlePieceCoordsForShuffle.end(), std::default_random_engine(seed));*/

	//for (auto& piece : puzzlePiecesAll)
	//{
	//	piece.item.get()->setPos(piece.pos);
	//	//scene.get()->addItem(piece.item.get());
	//}

	/*for (int i = 0; i < puzzlesList[puzzleCurrent].size(); i++)
	{
		puzzlesList[puzzleCurrent][i].item.get()->setPos(puzzlePieceCoordsForShuffle[i]);
		this->addItem(puzzlesList[puzzleCurrent][i].item.get());
	}*/

	/*for (int i = 0; i < (16 / 4); i++)
	{
		for (int j = 0; j < (16 / 4); j++)
		{

		}
	}*/

	//puzzle.get()->setPixmap(QPixmap("C:/Users/Robert/source/repos/Tiling Puzzle Qt/x64/Debug/puzzles/lara17.png"));
	//scene.get()->setSceneRect(this->rect());
	//fitInView(scene.get()->sceneRect(), Qt::KeepAspectRatio);

	qDebug() << this->sceneRect();
}

void PuzzleDisplayScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (gameState == GameState::SOLVING)
	{
		if (event->button() == Qt::LeftButton)
		{
			for (int i = 0; i < puzzlesList[puzzleCurrent].size(); i++)
			{
				if (puzzlesList[puzzleCurrent][i].item.get()->contains(puzzlesList[puzzleCurrent][i].item.get()->mapFromScene(event->scenePos())))
				{
					qDebug() << "clicked in area";
					qDebug() << event->scenePos();

					// Swapping state determines whether user is in the process of swapping or not.
					// Having selected an image means they are. Having no image selected or having 
					// just finished swapping the position of one image with another means they aren't.
					if (swapState == SwapState::NONE)
					{
						puzzlesList[puzzleCurrent][i].item.get()->setOpacity(0.5);
						puzzlesList[puzzleCurrent][i].selectionState = puzzlePiece::SelectionState::SELECTED;
						swapState = SwapState::CHOOSING;
						selectedI = i;
					}
					else if (swapState == SwapState::CHOOSING)
					{
						if (puzzlesList[puzzleCurrent][i].selectionState == puzzlePiece::SelectionState::SELECTED)
						{
							puzzlesList[puzzleCurrent][i].item.get()->setOpacity(1);
							puzzlesList[puzzleCurrent][i].selectionState = puzzlePiece::SelectionState::UNSELECTED;
							selectedI = -1;
						}
						else if (puzzlesList[puzzleCurrent][i].selectionState == puzzlePiece::SelectionState::UNSELECTED)
						{
							puzzlesList[puzzleCurrent][selectedI].item.get()->setOpacity(1);
							puzzlesList[puzzleCurrent][selectedI].selectionState = puzzlePiece::SelectionState::UNSELECTED;

							QPointF posI = puzzlesList[puzzleCurrent][i].item.get()->pos();
							QPointF posSelectedI = puzzlesList[puzzleCurrent][selectedI].item.get()->pos();

							puzzlesList[puzzleCurrent][i].item.get()->setPos(posSelectedI);
							puzzlesList[puzzleCurrent][selectedI].item.get()->setPos(posI);

							selectedI = -1;
							// check for win state
							if (puzzleSolved())
							{
								qDebug("WINNER!");

								// if all puzzles in list have been completed,
								// go to victory state for restart / reshuffle message

								// otherwise, go to normal transition state
								// (splash screen, wait for key input to continue - press any key maybe?)
								if (puzzleCurrent + 1 > puzzlesList.size() - 1)
									gameState = GameState::TOTAL_VICTORY;
								else
									gameState = GameState::TRANSITIONING;
							}
						}
						swapState = SwapState::NONE;
					}
					return;
				}
			}
		}
	}
}

void PuzzleDisplayScene::keyReleaseEvent(QKeyEvent *event)
{
	if (event->isAutoRepeat())
	{
		event->ignore();
	}
	else
	{
		if (gameState == GameState::TRANSITIONING)
		{
			removeCurrentPuzzleFromScene();
			puzzleCurrent++;
			addCurrentPuzzleToScene();
			gameState = GameState::SOLVING;
		}
		else if (gameState == GameState::TOTAL_VICTORY)
		{
			if (event->key() == Qt::Key_R)
			{
				removeCurrentPuzzleFromScene();
				shufflePuzzlesList();
				puzzleCurrent = 0;
				addCurrentPuzzleToScene();
				gameState = GameState::SOLVING;
			}
		}
	}
}

void PuzzleDisplayScene::dirIteratorLoadPuzzles(const QString &dirPath)
{
	qDebug() << dirPath;
	QDirIterator dirIt(dirPath, QDir::AllEntries | QDir::NoDotAndDotDot);
	while (dirIt.hasNext())
	{
		QString filePath = dirIt.next();
		qDebug() << filePath;

		if (!supportedImgTypes.contains(QFileInfo(filePath).suffix(), Qt::CaseInsensitive))
			continue;

		QPixmap puzzleImg = QPixmap(filePath);

		puzzlesList.resize(puzzlesList.size() + 1);
		/*puzzlesList.emplace_back
		(
			std::vector<puzzlePiece>
			{

			}
		);*/

		int puzzleAnchorX = 0;
		int puzzleAnchorY = 0;
		int puzzlePosX = puzzleAnchorX;
		int puzzlePosY = puzzleAnchorY;
		int widthPieceSize = puzzleImg.width() / 4;
		int heightPieceSize = puzzleImg.height() / 4;
		for (int col = 0; col < 4; col++)
		{
			for (int row = 0; row < 4; row++)
			{
				puzzlesList.back().emplace_back
				(
					puzzlePiece
					{
						QPoint(row, col),
						QPointF(puzzlePosX, puzzlePosY),
						widthPieceSize,
						heightPieceSize
					}
				);
				puzzlesList.back().back().item.get()->setPixmap
				(
					puzzleImg.copy(QRect(puzzlePosX, puzzlePosY, widthPieceSize, heightPieceSize)) // x, y, w, h
				);
				puzzlePosX += widthPieceSize;
			}
			puzzlePosY += heightPieceSize;
			puzzlePosX = puzzleAnchorX;
		}

		qDebug() << "Loaded " + QString::number(puzzlesList.size()) + " puzzles.";
	}
}

void PuzzleDisplayScene::shufflePuzzlesList()
{
	int seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(puzzlesList.begin(), puzzlesList.end(), std::default_random_engine(seed));
}

bool PuzzleDisplayScene::puzzleSolved()
{
	for (const auto& piece : puzzlesList[puzzleCurrent])
	{
		if (piece.item.get()->pos() != piece.originalPos)
		{
			return false;
		}
	}
	return true;
}

void PuzzleDisplayScene::addCurrentPuzzleToScene()
{
	puzzlePieceCoordsForShuffle.clear();
	for (const auto& piece : puzzlesList[puzzleCurrent])
	{
		puzzlePieceCoordsForShuffle.emplace_back(QPointF(piece.originalPos));
	}
	int seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(puzzlePieceCoordsForShuffle.begin(), puzzlePieceCoordsForShuffle.end(), std::default_random_engine(seed));

	for (int i = 0; i < puzzlesList[puzzleCurrent].size(); i++)
	{
		puzzlesList[puzzleCurrent][i].item.get()->setPos(puzzlePieceCoordsForShuffle[i]);
		this->addItem(puzzlesList[puzzleCurrent][i].item.get());
	}
}

void PuzzleDisplayScene::removeCurrentPuzzleFromScene()
{
	// We remove rather than using QGraphicsScene clear() function,
	// to avoid deleting the items (they need to be reusable).
	for (const auto& piece : puzzlesList[puzzleCurrent])
	{
		this->removeItem(piece.item.get());
	}
}