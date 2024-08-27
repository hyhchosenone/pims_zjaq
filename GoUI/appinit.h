#ifndef APPINIT_H
#define APPINIT_H

#include "GoUI_global.h"
#include <QObject>
#include <QPoint>

class GOUI_EXPORT AppInit : public QObject
{
    Q_OBJECT
public:
    explicit AppInit(QObject *parent = 0);

    void Load();

private:
    QPoint mousePoint;  //鼠标拖动时的坐标
    bool mousePressed;  //鼠标是否按下

protected:
    bool eventFilter(QObject *obj, QEvent *evt);

signals:

public slots:
};

#endif // APPINIT_H
