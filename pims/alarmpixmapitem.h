#pragma once

#include <QGraphicsPixmapItem>

class AlarmPixmapItem : public QGraphicsPixmapItem
{
	//Q_OBJECT

public:
	AlarmPixmapItem(QGraphicsItem *parent = nullptr);
	AlarmPixmapItem(const QPixmap &pixmap, QGraphicsItem *parent = nullptr);
	~AlarmPixmapItem();

protected:
	//void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	//void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	QVariant itemChange(GraphicsItemChange change, const QVariant &value) Q_DECL_OVERRIDE;
};
