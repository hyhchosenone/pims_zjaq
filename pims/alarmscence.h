#pragma once

#include <QGraphicsScene>

class AlarmScence : public QGraphicsScene
{
	Q_OBJECT

public:
	AlarmScence(QObject *parent);
	~AlarmScence();
	void setBackgroudImage(QPixmap *img);

protected:
	void drawBackground(QPainter *painter, const QRectF &rect);

private:
	QPixmap *backgroudImg = Q_NULLPTR;
};
