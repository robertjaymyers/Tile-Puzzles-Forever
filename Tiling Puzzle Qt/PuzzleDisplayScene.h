#pragma once
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QDirIterator>
#include <QFileInfo>
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
	QStringList supportedImgTypes = { "BMP", "GIF", "PNG", "JPG" };

	enum class GameState { SOLVING, TRANSITIONING, TOTAL_VICTORY };
	GameState gameState = GameState::SOLVING;

	enum class SwapState { NONE, CHOOSING };
	SwapState swapState = SwapState::NONE;
	int selectedI = -1;
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

	void dirIteratorLoadPuzzles(const QString &dirPath);
	void shufflePuzzlesList();
	bool puzzleSolved();
	void addCurrentPuzzleToScene();
	void removeCurrentPuzzleFromScene();
};