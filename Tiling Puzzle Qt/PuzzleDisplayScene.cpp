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
		addCurrentPuzzleToScene();
	}
	else
		gameState = GameState::INVALID;
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
			removeCurrentPuzzleFromScene();
			puzzleCurrent++;
			addCurrentPuzzleToScene();
			splashPuzzleComplete.get()->hide();
			emit puzzleChanged();
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
		}
		fileRead.close();
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

void PuzzleDisplayScene::startSplashTransition()
{
	if (puzzleCurrent + 1 > puzzlesList.size() - 1)
	{
		if (QFile(appExecutablePath + "/audio/puzzleComplete.wav").exists())
			QSound::play(appExecutablePath + "/audio/totalVictory.wav");
		splashTotalVictory.get()->show();
		gameState = GameState::TOTAL_VICTORY;
	}
	else
	{
		if (QFile(appExecutablePath + "/audio/puzzleComplete.wav").exists())
			QSound::play(appExecutablePath + "/audio/puzzleComplete.wav");
		splashPuzzleComplete.get()->show();
		gameState = GameState::PUZZLE_COMPLETE;
	}
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
		puzzlesList[puzzleCurrent][i].item.get()->setZValue(puzzleZ);
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