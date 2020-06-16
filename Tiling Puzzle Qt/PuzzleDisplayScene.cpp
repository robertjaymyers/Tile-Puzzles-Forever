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

#include "PuzzleDisplayScene.h"

PuzzleDisplayScene::PuzzleDisplayScene(QObject *parent)
	: QGraphicsScene(parent)
{
	prefLoad();

	puzzleCurrentWholeItem.get()->setZValue(puzzleWholeZ);
	puzzleCurrentWholeItem.get()->hide();
	this->addItem(puzzleCurrentWholeItem.get());

	splashPuzzleComplete.get()->setPixmap(QPixmap(":/TilingPuzzleQt/Resources/splashPuzzleComplete.png"));
	splashTotalVictory.get()->setPixmap(QPixmap(":/TilingPuzzleQt/Resources/splashTotalVictory.png"));

	splashPuzzleComplete.get()->setZValue(splashZ);
	splashTotalVictory.get()->setZValue(splashZ);

	splashPuzzleComplete.get()->hide();
	splashTotalVictory.get()->hide();

	this->addItem(splashPuzzleComplete.get());
	this->addItem(splashTotalVictory.get());

	dirIteratorLoadPuzzles(puzzleLoadPath);

	if (puzzlesList.size() > 0)
	{
		shufflePuzzlesList();
		setUpCurrentPuzzle();
		addCurrentPuzzleToScene();
	}
	else
		gameState = GameState::INVALID;
}

void PuzzleDisplayScene::setPuzzleListLoadNew(const QString &folderPath)
{
	tempPuzzleLoadPath = folderPath;
}

void PuzzleDisplayScene::setPuzzleType(const QString &typeAsStr)
{
	tempPuzzleType = typeAsStr;
}

void PuzzleDisplayScene::setPuzzleMultipler(const QString &multiplierAsStr)
{
	tempPuzzleMultiplier = multiplierAsStr;
}

void PuzzleDisplayScene::setPuzzleApplyChanges()
{
	puzzleCurrentWholeItem.get()->show();
	removeCurrentPuzzleFromScene();

	if (!tempPuzzleType.isEmpty())
	{
		QString extracted = extractSubstringInbetweenQt("Puzzle Type: ", "", tempPuzzleType);

		qDebug() << extracted;

		if (extracted == "Rearrangement")
			puzzleType = PuzzleType::REARRANGEMENT;
		else if (extracted == "Sliding")
			puzzleType = PuzzleType::SLIDING;
	}
	if (!tempPuzzleMultiplier.isEmpty())
	{
		puzzlePiecesMultiplier = sqrt(extractSubstringInbetweenQt("Puzzle Pieces: ", "", tempPuzzleMultiplier).toInt());
	}

	auto finishPuzzleSetup = [=]() {
		setUpCurrentPuzzle();
		puzzleCurrentWholeItem.get()->hide();
		addCurrentPuzzleToScene();
		splashPuzzleComplete.get()->hide();
		splashTotalVictory.get()->hide();
		emit puzzleChanged();
		gameState = GameState::SOLVING;
	};

	if (!tempPuzzleLoadPath.isEmpty())
	{
		puzzleLoadPath = tempPuzzleLoadPath;

		dirIteratorLoadPuzzles(puzzleLoadPath);

		if (puzzlesList.size() > 0)
		{
			shufflePuzzlesList();
			finishPuzzleSetup();
		}
		else
			gameState = GameState::INVALID;
	}
	else
	{
		finishPuzzleSetup();
	}

	tempPuzzleLoadPath.clear();
	tempPuzzleType.clear();
	tempPuzzleMultiplier.clear();
}

void PuzzleDisplayScene::resizeScaleSmooth()
{
	qDebug() << "Doing manual quality scaling...";

	float centerOnX;
	float centerOnY;

	{
		QPixmap temp = puzzleCurrentWholeImgOriginal;
		puzzleCurrentWholeImg = temp.scaled(this->width(), this->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
		
		centerOnX = (this->width() - puzzleCurrentWholeImg.width()) / 2;
		centerOnY = (this->height() - puzzleCurrentWholeImg.height()) / 2;

		puzzleCurrentWholeItem.get()->setPixmap(puzzleCurrentWholeImg);
		puzzleCurrentWholeItem.get()->setPos(QPointF(centerOnX, centerOnY));
	}

	for (auto& piece : puzzleCurrentDissected)
	{
		QPixmap temp = piece.originalImg;
		piece.img = temp.scaled(this->width() / puzzlePiecesMultiplier, this->height() / puzzlePiecesMultiplier, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		piece.item.get()->setPixmap(piece.img);
		piece.item.get()->setPos
		(
			QPointF
			(
				(piece.gridPoint.x() * piece.img.width()) + centerOnX, 
				(piece.gridPoint.y() * piece.img.height()) + centerOnY
			)
		);
	}

	for (auto& piece : puzzleCurrentSlideSpot)
	{
		QPixmap temp = piece.originalImg;
		piece.img = temp.scaled(this->width() / puzzlePiecesMultiplier, this->height() / puzzlePiecesMultiplier, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		piece.item.get()->setPixmap(piece.img);
		piece.item.get()->setPos
		(
			QPointF
			(
				(piece.gridPoint.x() * piece.img.width()) + centerOnX,
				(piece.gridPoint.y() * piece.img.height()) + centerOnY
			)
		);
	}
}

void PuzzleDisplayScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (gameState == GameState::SOLVING)
	{
		if (event->button() == Qt::LeftButton)
		{
			if (puzzleType == PuzzleType::REARRANGEMENT)
			{
				for (int i = 0; i < puzzleCurrentDissected.size(); i++)
				{
					if (puzzleCurrentDissected[i].item.get()->contains(puzzleCurrentDissected[i].item.get()->mapFromScene(event->scenePos())))
					{
						qDebug() << "clicked in area";
						qDebug() << event->scenePos();

						// Swapping state determines whether user is in the process of swapping or not.
						// Having selected an image means they are. Having no image selected or having 
						// just finished swapping the position of one image with another means they aren't.
						if (swapState == SwapState::NONE)
						{
							puzzleCurrentDissected[i].item.get()->setOpacity(0.5);
							puzzleCurrentDissected[i].selectionState = puzzlePiece::SelectionState::SELECTED;
							swapState = SwapState::CHOOSING;
							selectedI = i;
						}
						else if (swapState == SwapState::CHOOSING)
						{
							if (puzzleCurrentDissected[i].selectionState == puzzlePiece::SelectionState::SELECTED)
							{
								puzzleCurrentDissected[i].item.get()->setOpacity(1);
								puzzleCurrentDissected[i].selectionState = puzzlePiece::SelectionState::UNSELECTED;
								selectedI = -1;
							}
							else if (puzzleCurrentDissected[i].selectionState == puzzlePiece::SelectionState::UNSELECTED)
							{
								puzzleCurrentDissected[selectedI].item.get()->setOpacity(1);
								puzzleCurrentDissected[selectedI].selectionState = puzzlePiece::SelectionState::UNSELECTED;

								swapPuzzlePieceLocation(puzzleCurrentDissected[i], puzzleCurrentDissected[selectedI]);

								selectedI = -1;

								if (puzzleSolved())
								{
									qDebug("WINNER!");
									startSplashTransition();
								}
							}
							swapState = SwapState::NONE;
						}
						return;
					}
				}
			}
			else if (puzzleType == PuzzleType::SLIDING)
			{
				// We only need to check if clicked on a spot adjacent to slide spot
				// (one of a few possibilities)
				// whichever is clicked swaps position with slide spot.
				for (int i = 0; i < puzzleCurrentDissected.size(); i++)
				{
					if (puzzleCurrentDissected[i].item.get()->contains(puzzleCurrentDissected[i].item.get()->mapFromScene(event->scenePos())))
					{
						qDebug() << "clicked in area";
						qDebug() << event->scenePos();

						qDebug() << puzzleCurrentDissected[i].gridPoint;

						if (adjacentToSlideSpot(i))
						{
							// Store the pre-slide positions/points before animating, so we know where stuff should go.
							QPointF posDissected = puzzleCurrentDissected[i].item.get()->pos();
							QPoint gridPointDissected = puzzleCurrentDissected[i].gridPoint;

							QPointF posSlide = puzzleCurrentSlideSpot[0].item.get()->pos();
							QPoint gridPointSlide = puzzleCurrentSlideSpot[0].gridPoint;

							slideAnimation.get()->setTargetObject(puzzleCurrentDissected[i].item.get());
							slideAnimation.get()->setPropertyName("pos");
							slideAnimation.get()->setDuration(50);
							slideAnimation.get()->setStartValue(posDissected);
							slideAnimation.get()->setEndValue(posSlide);
							slideAnimation.get()->start();

							// With animating done, visible tile should be in slide spot position.
							// We still need to update slide spot's position and the grid point of both,
							// but NOT update the position of visible tile, since that has already moved.
							// We do a check to make sure it is in the right spot and put it there manually,
							// in case of animation failure.

							if (puzzleCurrentDissected[i].item.get()->pos() != posSlide)
								puzzleCurrentDissected[i].item.get()->setPos(posSlide);
							puzzleCurrentDissected[i].gridPoint = gridPointSlide;

							puzzleCurrentSlideSpot[0].item.get()->setPos(posDissected);
							puzzleCurrentSlideSpot[0].gridPoint = gridPointDissected;

							if (puzzleSolved())
							{
								qDebug("WINNER!");
								startSplashTransition();
							}
						}
						return;
					}
				}
			}
		}
	}
	else if (gameState == GameState::PUZZLE_COMPLETE)
	{
		if (event->button() == Qt::LeftButton)
		{
			splashPuzzleComplete.get()->hide();
		}
	}
	else if (gameState == GameState::TOTAL_VICTORY)
	{
		if (event->button() == Qt::LeftButton)
		{
			splashTotalVictory.get()->hide();
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
		if (gameState == GameState::SOLVING)
		{
			if (event->key() == Qt::Key_Tab)
			{
				startSplashTransition();
			}
		}
		else if (gameState == GameState::PUZZLE_COMPLETE)
		{
			puzzleCurrentWholeItem.get()->hide();
			addCurrentPuzzleToScene();
			splashPuzzleComplete.get()->hide();
			emit puzzleChanged();
			gameState = GameState::SOLVING;
		}
		else if (gameState == GameState::TOTAL_VICTORY)
		{
			if (event->key() == Qt::Key_R)
			{
				puzzleCurrentWholeItem.get()->hide();
				addCurrentPuzzleToScene();
				splashTotalVictory.get()->hide();
				emit puzzleChanged();
				gameState = GameState::SOLVING;
			}
		}
	}
}

void PuzzleDisplayScene::prefLoad()
{
	QFile fileRead(appExecutablePath + "/config.txt");
	if (fileRead.open(QIODevice::ReadOnly))
	{
		QTextStream contents(&fileRead);
		while (!contents.atEnd())
		{
			QString line = contents.readLine();

			// If a line has // at the beginning, we treat it as a comment and move on.
			// Doing this as a precaution in the event of a comment accidentally containing
			// a key phrase that is used to find something we want and triggering a false positive.
			if (line[0] == '/' && line[1] == '/')
				continue;

			if (line.contains("puzzlePiecesMultiplier="))
			{
				int multiplier = puzzlePiecesMultiplierMin;
				multiplier = extractSubstringInbetweenQt("=", "", line).toInt();
				if (multiplier >= puzzlePiecesMultiplierMin && multiplier <= puzzlePiecesMultiplierMax)
				{
					puzzlePiecesMultiplier = multiplier;
				}
			}
			else if (line.contains("loadPuzzlesFromFolder="))
			{
				QString path = extractSubstringInbetweenQt("=", "", line);
				if (!path.isEmpty())
				{
					puzzleLoadPath = path;
				}
			}
			else if (line.contains("puzzleTypeToPlay="))
			{
				QString str = extractSubstringInbetweenQt("=", "", line);
				if (!str.isEmpty())
				{
					if (str == "REARRANGEMENT")
						puzzleType = PuzzleType::REARRANGEMENT;
					else if (str == "SLIDING")
						puzzleType = PuzzleType::SLIDING;
				}
			}
		}
		fileRead.close();
	}
}

void PuzzleDisplayScene::dirIteratorLoadPuzzles(const QString &dirPath)
{
	puzzlesList.clear();

	qDebug() << dirPath;
	QDirIterator dirIt(dirPath, QDir::AllEntries | QDir::NoDotAndDotDot);
	while (dirIt.hasNext())
	{
		QString filePath = dirIt.next();
		qDebug() << filePath;

		if (!supportedImgTypes.contains(QFileInfo(filePath).suffix(), Qt::CaseInsensitive))
			continue;

		puzzlesList.emplace_back(filePath);

		qDebug() << "Loaded " + QString::number(puzzlesList.size()) + " puzzles.";
	}

	puzzleCurrent = 0;
}

void PuzzleDisplayScene::shufflePuzzlesList()
{
	int seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(puzzlesList.begin(), puzzlesList.end(), std::default_random_engine(seed));
}

bool PuzzleDisplayScene::shuffleMadePuzzleSolved()
{
	for (int i = 0; i < puzzleCurrentDissected.size(); i++)
	{
		if (puzzleCurrentDissected[i].originalGridPoint != puzzlePieceCoordsForShuffle[i].gridPoint)
		{
			return false;
		}
	}
	return true;
}

bool PuzzleDisplayScene::puzzleSolved()
{
	for (const auto& piece : puzzleCurrentDissected)
	{
		if (piece.gridPoint != piece.originalGridPoint)
		{
			return false;
		}
	}
	return true;
}

void PuzzleDisplayScene::startSplashTransition()
{
	if (puzzleCurrent + 1 > puzzlesList.size() - 1)
	{
		if (QFile(appExecutablePath + "/audio/puzzleComplete.wav").exists())
			QSound::play(appExecutablePath + "/audio/totalVictory.wav");
		splashTotalVictory.get()->show();
		gameState = GameState::TOTAL_VICTORY;

		puzzleCurrentWholeItem.get()->show();
		removeCurrentPuzzleFromScene();
		shufflePuzzlesList();
		puzzleCurrent = 0;
		setUpCurrentPuzzle();
	}
	else
	{
		if (QFile(appExecutablePath + "/audio/puzzleComplete.wav").exists())
			QSound::play(appExecutablePath + "/audio/puzzleComplete.wav");
		splashPuzzleComplete.get()->show();
		gameState = GameState::PUZZLE_COMPLETE;

		// We make whole version of img visible so nothing looks amiss.
		// Then behind the scenes, we start transition by setting up the next puzzle.
		// This way, by the time player hits key to go to the next puzzle,
		// it's probably already loaded/ready to be added to the scene.
		// We do it this way, since we're setting up each puzzle from filesystem on the fly,
		// as opposed to storing it in memory (which would have a lot of memory overhead for a lot of puzzles).

		// In the case of sliding puzzle type, this is not only for Performance-O-Magic Show, but also necessary
		// visual design since we want completion to show the "missing" piece.
		puzzleCurrentWholeItem.get()->show();
		removeCurrentPuzzleFromScene();
		puzzleCurrent++;
		setUpCurrentPuzzle();
	}
}

void PuzzleDisplayScene::setUpCurrentPuzzle()
{
	if (puzzleType == PuzzleType::REARRANGEMENT)
	{
		puzzleCurrentDissected.clear();

		QPixmap puzzleImg = QPixmap(puzzlesList[puzzleCurrent]);

		puzzleCurrentWholeImgOriginal = puzzleImg;
		puzzleCurrentWholeImg = puzzleImg;

		int puzzleAnchorX = 0;
		int puzzleAnchorY = 0;
		int puzzlePosX = puzzleAnchorX;
		int puzzlePosY = puzzleAnchorY;
		int widthPieceSize = puzzleImg.width() / puzzlePiecesMultiplier;
		int heightPieceSize = puzzleImg.height() / puzzlePiecesMultiplier;
		for (int col = 0; col < puzzlePiecesMultiplier; col++)
		{
			for (int row = 0; row < puzzlePiecesMultiplier; row++)
			{
				puzzleCurrentDissected.emplace_back
				(
					puzzlePiece
					{
						puzzleImg.copy(QRect(puzzlePosX, puzzlePosY, widthPieceSize, heightPieceSize)),
						puzzleImg.copy(QRect(puzzlePosX, puzzlePosY, widthPieceSize, heightPieceSize)),
						QPoint(row, col),
						QPoint(row, col),
						QPointF(puzzlePosX, puzzlePosY),
						widthPieceSize,
						heightPieceSize
					}
				);
				puzzleCurrentDissected.back().item.get()->setPixmap
				(
					puzzleCurrentDissected.back().originalImg
				);
				puzzlePosX += widthPieceSize;
			}
			puzzlePosY += heightPieceSize;
			puzzlePosX = puzzleAnchorX;
		}
	}
	else if (puzzleType == PuzzleType::SLIDING)
	{
		puzzleCurrentDissected.clear();
		puzzleCurrentSlideSpot.clear();

		QPixmap puzzleImg = QPixmap(puzzlesList[puzzleCurrent]);

		puzzleCurrentWholeImgOriginal = puzzleImg;
		puzzleCurrentWholeImg = puzzleImg;

		int puzzleAnchorX = 0;
		int puzzleAnchorY = 0;
		int puzzlePosX = puzzleAnchorX;
		int puzzlePosY = puzzleAnchorY;
		int widthPieceSize = puzzleImg.width() / puzzlePiecesMultiplier;
		int heightPieceSize = puzzleImg.height() / puzzlePiecesMultiplier;
		for (int col = 0; col < puzzlePiecesMultiplier; col++)
		{
			for (int row = 0; row < puzzlePiecesMultiplier; row++)
			{
				if (col == puzzlePiecesMultiplier - 1 && row == puzzlePiecesMultiplier - 1)
				{
					puzzleCurrentSlideSpot.emplace_back
					(
						puzzlePiece
						{
							invisibleImg.scaled(widthPieceSize, heightPieceSize),
							invisibleImg.scaled(widthPieceSize, heightPieceSize),
							QPoint(col, row),
							QPoint(col, row),
							QPointF(puzzlePosX, puzzlePosY),
							widthPieceSize,
							heightPieceSize
						}
					);

					puzzleCurrentSlideSpot.back().item.get()->setPixmap
					(
						puzzleCurrentSlideSpot.back().originalImg
					);
					puzzleCurrentSlideSpot.back().item.get()->setPos
					(
						puzzleCurrentSlideSpot.back().originalPos
					);
					puzzleCurrentSlideSpot.back().item.get()->setZValue(puzzlePieceZ);
				}
				else
				{
					puzzleCurrentDissected.emplace_back
					(
						puzzlePiece
						{
							puzzleImg.copy(QRect(puzzlePosX, puzzlePosY, widthPieceSize, heightPieceSize)),
							puzzleImg.copy(QRect(puzzlePosX, puzzlePosY, widthPieceSize, heightPieceSize)),
							QPoint(row, col),
							QPoint(row, col),
							QPointF(puzzlePosX, puzzlePosY),
							widthPieceSize,
							heightPieceSize
						}
					);

					puzzleCurrentDissected.back().item.get()->setPixmap
					(
						puzzleCurrentDissected.back().originalImg
					);
					puzzleCurrentDissected.back().item.get()->setPos
					(
						puzzleCurrentDissected.back().originalPos
					);
					puzzleCurrentDissected.back().item.get()->setZValue(puzzlePieceZ);
				}

				puzzlePosX += widthPieceSize;
			}
			puzzlePosY += heightPieceSize;
			puzzlePosX = puzzleAnchorX;
		}

		while (puzzleSolved())
		{
			// We multiply simulated moves by pieces multiplier to ensure that as puzzle size shrinks/expands
			// based on settings, simulated moves is roughly the same proportionally
			int totalSimulatedMoves = 50 * puzzlePiecesMultiplier;

			for (int i = 0; i < totalSimulatedMoves; i++)
			{
				std::random_device rd;
				std::mt19937 mt(rd());
				std::uniform_int_distribution<int> dist(1, 4);
				int dirToMove = dist(mt);

				switch (dirToMove)
				{
				case 1:
					if (puzzleCurrentSlideSpot[0].gridPoint.x() + 1 <= puzzlePiecesMultiplier - 1)
						doSimulatedSlide(SlideDir::RIGHT);
					break;
				case 2:
					if (puzzleCurrentSlideSpot[0].gridPoint.x() - 1 >= 0)
						doSimulatedSlide(SlideDir::LEFT);
					break;
				case 3:
					if (puzzleCurrentSlideSpot[0].gridPoint.y() + 1 <= puzzlePiecesMultiplier - 1)
						doSimulatedSlide(SlideDir::DOWN);
					break;
				case 4:
					if (puzzleCurrentSlideSpot[0].gridPoint.y() - 1 >= 0)
						doSimulatedSlide(SlideDir::UP);
					break;
				}
			}

			while (puzzleCurrentSlideSpot[0].gridPoint != puzzleCurrentSlideSpot[0].originalGridPoint)
			{
				if (puzzleCurrentSlideSpot[0].gridPoint.x() != puzzleCurrentSlideSpot[0].originalGridPoint.x())
				{
					doSimulatedSlide(SlideDir::RIGHT);
				}
				else if (puzzleCurrentSlideSpot[0].gridPoint.y() != puzzleCurrentSlideSpot[0].originalGridPoint.y())
				{
					doSimulatedSlide(SlideDir::DOWN);
				}
			}
		}
	}

	qDebug("Puzzle setup complete.");
}

void PuzzleDisplayScene::addCurrentPuzzleToScene()
{
	if (puzzleType == PuzzleType::REARRANGEMENT)
	{
		puzzlePieceCoordsForShuffle.clear();
		for (const auto& piece : puzzleCurrentDissected)
		{
			puzzlePieceCoordsForShuffle.emplace_back(shuffleCoord{piece.originalPos, piece.originalGridPoint});
		}

		while (shuffleMadePuzzleSolved())
		{
			qDebug("Shuffling");
			int seed = std::chrono::system_clock::now().time_since_epoch().count();
			shuffle(puzzlePieceCoordsForShuffle.begin(), puzzlePieceCoordsForShuffle.end(), std::default_random_engine(seed));
		}

		for (int i = 0; i < puzzleCurrentDissected.size(); i++)
		{
			puzzleCurrentDissected[i].item.get()->setZValue(puzzlePieceZ);
			puzzleCurrentDissected[i].gridPoint = puzzlePieceCoordsForShuffle[i].gridPoint;
			puzzleCurrentDissected[i].item.get()->setPos(puzzlePieceCoordsForShuffle[i].pos);
			this->addItem(puzzleCurrentDissected[i].item.get());
		}
		puzzleCurrentWholeItem.get()->setPixmap(puzzleCurrentWholeImg);
	}
	else if (puzzleType == PuzzleType::SLIDING)
	{
		this->addItem(puzzleCurrentSlideSpot[0].item.get());

		for (auto& piece : puzzleCurrentDissected)
		{
			this->addItem(piece.item.get());
		}

		puzzleCurrentWholeItem.get()->setPixmap(puzzleCurrentWholeImg);
	}
}

void PuzzleDisplayScene::removeCurrentPuzzleFromScene()
{
	if (puzzleType == PuzzleType::REARRANGEMENT)
	{
		for (const auto& piece : puzzleCurrentDissected)
		{
			this->removeItem(piece.item.get());
		}
	}
	else if (puzzleType == PuzzleType::SLIDING)
	{
		for (const auto& piece : puzzleCurrentDissected)
		{
			this->removeItem(piece.item.get());
		}
		this->removeItem(puzzleCurrentSlideSpot[0].item.get());
	}
}

void PuzzleDisplayScene::swapPuzzlePieceLocation(puzzlePiece &piece1, puzzlePiece &piece2)
{
	QPointF posPiece1 = piece1.item.get()->pos();
	QPoint gridPointPiece1 = piece1.gridPoint;

	qDebug() << "posPiece1: " << posPiece1;
	qDebug() << "gridPointPiece1: " << gridPointPiece1;

	QPointF posPiece2 = piece2.item.get()->pos();
	QPoint gridPointPiece2 = piece2.gridPoint;

	qDebug() << "posPiece2: " << posPiece2;
	qDebug() << "gridPointPiece2: " << gridPointPiece2;

	piece1.item.get()->setPos(posPiece2);
	piece1.gridPoint = gridPointPiece2;

	piece2.item.get()->setPos(posPiece1);
	piece2.gridPoint = gridPointPiece1;
}

bool PuzzleDisplayScene::adjacentToSlideSpot(const int i)
{
	if (
		(puzzleCurrentDissected[i].gridPoint.x() == puzzleCurrentSlideSpot[0].gridPoint.x()
			&& (puzzleCurrentDissected[i].gridPoint.y() + 1 == puzzleCurrentSlideSpot[0].gridPoint.y() ||
				puzzleCurrentDissected[i].gridPoint.y() - 1 == puzzleCurrentSlideSpot[0].gridPoint.y()))
		||
		(puzzleCurrentDissected[i].gridPoint.y() == puzzleCurrentSlideSpot[0].gridPoint.y()
			&& (puzzleCurrentDissected[i].gridPoint.x() + 1 == puzzleCurrentSlideSpot[0].gridPoint.x() ||
				puzzleCurrentDissected[i].gridPoint.x() - 1 == puzzleCurrentSlideSpot[0].gridPoint.x()))
		)
	{
		return true;
	}
	return false;
}

void PuzzleDisplayScene::doSimulatedSlide(const SlideDir &slideDir)
{
	qDebug("Simulating move...");

	switch (slideDir)
	{
	case SlideDir::RIGHT:
		for (auto& piece : puzzleCurrentDissected)
		{
			if (puzzleCurrentSlideSpot[0].gridPoint.y() == piece.gridPoint.y()
				&& puzzleCurrentSlideSpot[0].gridPoint.x() + 1 == piece.gridPoint.x())
			{
				qDebug("Moving right...");
				swapPuzzlePieceLocation(puzzleCurrentSlideSpot[0], piece);
				break;
			}
		}
		break;
	case SlideDir::LEFT:
		for (auto& piece : puzzleCurrentDissected)
		{
			if (puzzleCurrentSlideSpot[0].gridPoint.y() == piece.gridPoint.y()
				&& puzzleCurrentSlideSpot[0].gridPoint.x() - 1 == piece.gridPoint.x())
			{
				qDebug("Moving left...");
				swapPuzzlePieceLocation(puzzleCurrentSlideSpot[0], piece);
				break;
			}
		}
		break;
	case SlideDir::DOWN:
		for (auto& piece : puzzleCurrentDissected)
		{
			if (puzzleCurrentSlideSpot[0].gridPoint.x() == piece.gridPoint.x()
				&& puzzleCurrentSlideSpot[0].gridPoint.y() + 1 == piece.gridPoint.y())
			{
				qDebug("Moving down...");
				swapPuzzlePieceLocation(puzzleCurrentSlideSpot[0], piece);
				break;
			}
		}
		break;
	case SlideDir::UP:
		for (auto& piece : puzzleCurrentDissected)
		{
			if (puzzleCurrentSlideSpot[0].gridPoint.x() == piece.gridPoint.x()
				&& puzzleCurrentSlideSpot[0].gridPoint.y() - 1 == piece.gridPoint.y())
			{
				qDebug("Moving up...");
				swapPuzzlePieceLocation(puzzleCurrentSlideSpot[0], piece);
				break;
			}
		}
		break;
	}
}

QString PuzzleDisplayScene::extractSubstringInbetweenQt(const QString strBegin, const QString strEnd, const QString &strExtractFrom)
{
	QString extracted = "";
	int posFound = 0;

	if (!strBegin.isEmpty() && !strEnd.isEmpty())
	{
		while (strExtractFrom.indexOf(strBegin, posFound, Qt::CaseSensitive) != -1)
		{
			int posBegin = strExtractFrom.indexOf(strBegin, posFound, Qt::CaseSensitive) + strBegin.length();
			int posEnd = strExtractFrom.indexOf(strEnd, posBegin, Qt::CaseSensitive);
			extracted += strExtractFrom.mid(posBegin, posEnd - posBegin);
			posFound = posEnd;
		}
	}
	else if (strBegin.isEmpty() && !strEnd.isEmpty())
	{
		int posBegin = 0;
		int posEnd = strExtractFrom.indexOf(strEnd, posBegin, Qt::CaseSensitive);
		extracted += strExtractFrom.mid(posBegin, posEnd - posBegin);
		posFound = posEnd;
	}
	else if (!strBegin.isEmpty() && strEnd.isEmpty())
	{
		int posBegin = strExtractFrom.indexOf(strBegin, posFound, Qt::CaseSensitive) + strBegin.length();
		int posEnd = strExtractFrom.length();
		extracted += strExtractFrom.mid(posBegin, posEnd - posBegin);
		posFound = posEnd;
	}
	return extracted;
}