#pragma once

#include <QGraphicsPixmapItem>

class PuzzlePixmapItem : public QObject, public QGraphicsPixmapItem
{
	Q_OBJECT
	Q_PROPERTY(QPointF pos READ pos WRITE setPos)

public:
	PuzzlePixmapItem(QGraphicsItem *parent = nullptr);
};