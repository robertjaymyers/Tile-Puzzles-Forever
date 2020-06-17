#pragma once

#include <QGraphicsPixmapItem>

class PuzzlePixmapItem : public QObject, public QGraphicsPixmapItem
{
	Q_OBJECT
	Q_PROPERTY(QPointF pos READ pos WRITE setPos)
	Q_PROPERTY(qreal rotation READ rotation WRITE setRotation)

public:
	PuzzlePixmapItem(QGraphicsItem *parent = nullptr);
};