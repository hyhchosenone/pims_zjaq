#pragma once

#include <QObject>
#include <QTcpSocket>
#include "alarmdata.h"

class TcpClient : public QObject
{
    Q_OBJECT
public:
    TcpClient(QObject *parent = Q_NULLPTR);
    ~TcpClient();
    void setIpAndPort(QString ip, int port);

public slots:
    void doWork();
    void processCmd(QByteArray data);

signals:
    void uploadData(QByteArray data);

private slots:
    void readMessage();
    void connectServer();

private:
    QString srvIP;
    quint16 srvPort;
    QTcpSocket *socket = nullptr;
};
