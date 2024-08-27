#pragma once

#include <QObject>
#include <QTcpSocket>

class AlarmReciver : public QObject
{
    Q_OBJECT

public:
    AlarmReciver();
    ~AlarmReciver();

public slots:
    void doWork();
    void revData(); //接收来自服务端的数据

signals:
    void alarming(const QString &result);

private:
    QTcpSocket *client;
};
