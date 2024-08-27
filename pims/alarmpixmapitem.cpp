#include "alarmpixmapitem.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

AlarmPixmapItem::AlarmPixmapItem(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
{

}

AlarmPixmapItem::AlarmPixmapItem(const QPixmap &pixmap, QGraphicsItem *parent)
    : QGraphicsPixmapItem(pixmap, parent)
{
    //this->setShapeMode(BoundingRectShape);
    setCacheMode(DeviceCoordinateCache);
}

AlarmPixmapItem::~AlarmPixmapItem()
{
}

QVariant AlarmPixmapItem::itemChange(GraphicsItemChange change, const QVariant & value)
{

    if (change == ItemPositionChange && scene()) {
        // value is the new position.
        QPointF newPos = value.toPointF();
        QRectF rect = scene()->sceneRect();

        if (!rect.contains(newPos)) {
            // Keep the item inside the scene rect.
            newPos.setX(qMin(rect.right(), qMax(newPos.x(), rect.left())));
            newPos.setY(qMin(rect.bottom(), qMax(newPos.y(), rect.top())));
            return newPos;
        }
    }

    return QGraphicsItem::itemChange(change, value);
}

void AlarmPixmapItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();

    QRectF itemRect = this->boundingRect();
    QPointF newPos = pos() + (event->scenePos() - event->lastScenePos());
    QPointF checkPos = newPos + QPointF(itemRect.width(), itemRect.height()); //加上自身尺寸
    QRectF rect = scene()->sceneRect();

    if (rect.contains(newPos) && rect.contains(checkPos))
    {
        setPos(newPos);
    }


}
