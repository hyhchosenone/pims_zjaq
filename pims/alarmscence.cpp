#include "alarmscence.h"
#include <QGraphicsView>

AlarmScence::AlarmScence(QObject *parent)
    : QGraphicsScene(parent)
{

}

AlarmScence::~AlarmScence()
{
}

void AlarmScence::setBackgroudImage(QPixmap * img)
{
    this->backgroudImg = img;
}

void AlarmScence::drawBackground(QPainter * painter, const QRectF & rect)
{
    QGraphicsView *view = this->views()[0];
    QRect contentRect = view->viewport()->contentsRect();
    QRectF sceneRect = view->mapToScene(contentRect).boundingRect();
    painter->drawPixmap(sceneRect, *backgroudImg, QRectF());
}
