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
#include "Satan.h"
#include "PuzzlePixmapItem.h"
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QDirIterator>
#include <QFileInfo>
#include <QSplashScreen>
#include <QCoreApplication>
#include <QSound>
#include <QPropertyAnimation>
#include <QDebug>
#include <random>
#include <chrono>
#include <algorithm>

class PuzzleDisplayScene : public QGraphicsScene
{
	Q_OBJECT

public:
	PuzzleDisplayScene(QObject *parent = nullptr);
	void setPuzzleListLoadNew(const QString &folderPath);
	void setPuzzleType(const QString &typeAsStr);
	void setPuzzleMultipler(const QString &multiplierAsStr);
	void setPuzzleApplyChanges();
	void resizeScaleSmooth();

protected:
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void keyReleaseEvent(QKeyEvent *event);

private:
	const QString appExecutablePath = QCoreApplication::applicationDirPath();

	QStringList supportedImgTypes = { "BMP", "GIF", "PNG", "JPG" };

	const int puzzlePieceZ = 1;
	const int puzzleWholeZ = 2;
	const int splashZ = 3;

	std::unique_ptr<QGraphicsPixmapItem> splashPuzzleComplete = std::make_unique<QGraphicsPixmapItem>(nullptr);
	std::unique_ptr<QGraphicsPixmapItem> splashTotalVictory = std::make_unique<QGraphicsPixmapItem>(nullptr);

	enum class GameState { INVALID, SOLVING, PUZZLE_COMPLETE, TOTAL_VICTORY };
	GameState gameState = GameState::SOLVING;

	// We store UI config changes made in these variables.
	// If changes are confirmed, these variables are used to apply any changes.
	QString tempPuzzleLoadPath;
	QString tempPuzzleType;
	QString tempPuzzleMultiplier;

	QString puzzleLoadPath = appExecutablePath + "/puzzles";

	enum class PuzzleType { REARRANGEMENT, SLIDING, ROTATION };
	PuzzleType puzzleType = PuzzleType::REARRANGEMENT;

	enum class PuzzleSolvedType { POSITION, ROTATION };

	// Determines how many pieces puzzles get broken up into.
	// Ex: 4 would be used as 4*4 = 16 pieces, 5 would be 5*5 = 25 pieces, etc.
	const int puzzlePiecesMultiplierMin = 2;
	const int puzzlePiecesMultiplierMax = 20;
	int puzzlePiecesMultiplier = 4;

	struct puzzlePiece
	{
		const QPixmap originalImg;
		QPixmap img;
		QPoint gridPoint; // "Sliding" Puzzle
		const QPoint originalGridPoint; // "Sliding" Puzzle
		const QPointF originalPos;
		const double width;
		const double height;
		enum class SelectionState { UNSELECTED, SELECTED }; // "Rearrangement" Puzzle
		SelectionState selectionState = SelectionState::UNSELECTED; // "Rearrangement" Puzzle
		std::unique_ptr<PuzzlePixmapItem> item = std::make_unique<PuzzlePixmapItem>(nullptr);
	};

	int puzzleCurrent;
	QPixmap puzzleCurrentWholeImgOriginal;
	QPixmap puzzleCurrentWholeImg;
	std::unique_ptr<QGraphicsPixmapItem> puzzleCurrentWholeItem = std::make_unique<QGraphicsPixmapItem>(nullptr);
	std::vector<puzzlePiece> puzzleCurrentDissected;
	std::vector<QString> puzzlesList;

	enum class MoveDir { UP, DOWN, LEFT, RIGHT };
	QPoint simulatedMoveGridPoint = QPoint(0, 0);

	// "Rearrangement" Puzzle: 
	// These parts are only used by the aforementioned type of puzzle.
	enum class SwapState { NONE, CHOOSING };
	SwapState swapState = SwapState::NONE;
	int selectedI = -1;
	struct shuffleCoord { QPointF pos; QPoint gridPoint; };
	std::vector<shuffleCoord> puzzlePieceCoordsForShuffle;

	// "Sliding" Puzzle:
	// These parts are only used by the aforementioned type of puzzle.
	std::vector<puzzlePiece> puzzleCurrentSlideSpot;
	const QPixmap invisibleImg = QPixmap(":/TilingPuzzleQt/Resources/invisible.png");
	std::unique_ptr<QPropertyAnimation> slideAnimation = std::make_unique<QPropertyAnimation>();

	// "Rotation" Puzzle:
	// These parts are only used by the aforementioned type of puzzle.
	const int totalPossibleAdjacentRotate = 4;
	std::vector<std::unique_ptr<QPropertyAnimation>> rotateAnimationList;

	void prefLoad();
	void dirIteratorLoadPuzzles(const QString &dirPath);
	void shufflePuzzlesList();
	bool shuffleMadePuzzleSolved();
	bool puzzleSolved(const PuzzleSolvedType &puzzleSolvedType);
	void startSplashTransition();
	void setUpCurrentPuzzle();
	void addCurrentPuzzleToScene();
	void removeCurrentPuzzleFromScene();
	void swapPuzzlePieceLocation(puzzlePiece &piece1, puzzlePiece &piece2);
	bool adjacentToSlideSpot(const int i); // "Sliding" Puzzle
	void doSimulatedMove(const MoveDir &moveDir); // "Sliding" Puzzle
	std::vector<int> getAdjacentToPiece(const int pieceIndex); // "Rotation" Puzzle
	qreal getNextClockwiseRotationSimulated(const int pieceIndex); // "Rotation" Puzzle
	qreal getNextClockwiseRotationAnimated(const int pieceIndex); // "Rotation" Puzzle
	QString extractSubstringInbetweenQt(const QString strBegin, const QString strEnd, const QString &strExtractFrom);

signals:
	void puzzleChanged();
};