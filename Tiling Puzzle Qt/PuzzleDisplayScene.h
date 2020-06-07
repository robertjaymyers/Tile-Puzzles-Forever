#pragma once
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QDirIterator>
#include <QFileInfo>
#include <QSplashScreen>
#include <QCoreApplication>
#include <QSound>
#include <QDebug>
#include <random>
#include <chrono>
#include <algorithm>

class PuzzleDisplayScene : public QGraphicsScene
{
	Q_OBJECT

public:
	PuzzleDisplayScene(QObject *parent = nullptr);

protected:
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void keyReleaseEvent(QKeyEvent *event);

private:
	const QString appExecutablePath = QCoreApplication::applicationDirPath();

	QString puzzleLoadPath = appExecutablePath + "/puzzles";

	QStringList supportedImgTypes = { "BMP", "GIF", "PNG", "JPG" };

	const int puzzleZ = 1;
	const int splashZ = 2;

	std::unique_ptr<QGraphicsPixmapItem> splashPuzzleComplete = std::make_unique<QGraphicsPixmapItem>(nullptr);
	std::unique_ptr<QGraphicsPixmapItem> splashTotalVictory = std::make_unique<QGraphicsPixmapItem>(nullptr);

	enum class GameState { INVALID, SOLVING, PUZZLE_COMPLETE, TOTAL_VICTORY };
	GameState gameState = GameState::SOLVING;

	enum class SwapState { NONE, CHOOSING };
	SwapState swapState = SwapState::NONE;
	int selectedI = -1;

	// Determines how many pieces puzzles get broken up into.
	// Ex: 4 would be used as 4*4 = 16 pieces, 5 would be 5*5 = 25 pieces, etc.
	const int puzzlePiecesMultiplierMin = 2;
	const int puzzlePiecesMultiplierMax = 20;
	int puzzlePiecesMultiplier = 4;

	struct puzzlePiece
	{
		const QPoint gridPoint;
		const QPointF originalPos;
		const int width;
		const int height;
		enum class SelectionState { UNSELECTED, SELECTED };
		SelectionState selectionState = SelectionState::UNSELECTED;
		std::unique_ptr<QGraphicsPixmapItem> item = std::make_unique<QGraphicsPixmapItem>(nullptr);
	};

	std::vector<QPointF> puzzlePieceCoordsForShuffle;

	int puzzleCurrent = 0;
	std::vector<std::vector<puzzlePiece>> puzzlesList;

	void prefLoad();
	void dirIteratorLoadPuzzles(const QString &dirPath);
	void shufflePuzzlesList();
	bool puzzleSolved();
	void addCurrentPuzzleToScene();
	void removeCurrentPuzzleFromScene();
	QString extractSubstringInbetweenQt(const QString strBegin, const QString strEnd, const QString &strExtractFrom);

signals:
	void puzzleChanged();
};