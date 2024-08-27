#include "apiworker.h"

APIWorker::APIWorker(QObject *parent) : QObject(parent)
{
    apiServer = new QTcpServer(this);
}

APIWorker::~APIWorker()
{
    foreach(QTcpSocket* client, clients) {
        client->close();
        delete client;
    }

    apiServer->close();
    delete  apiServer;
}

void APIWorker::doWork()
{
    if (!apiServer->listen(QHostAddress::AnyIPv4, 9110))
    {
        qDebug() << apiServer->errorString();
        apiServer->close();
    }

    connect(apiServer, &QTcpServer::newConnection, this, &APIWorker::newClient);

}

void APIWorker::newClient()
{
    QTcpSocket *clientSocket = apiServer->nextPendingConnection();
    clients[clientSocket->peerAddress().toString()] = clientSocket;

    qDebug() << "APIWorker : new client " << clientSocket->peerAddress().toString();
}

void APIWorker::notifyClient(QByteArray data)
{
    qDebug() << "APIWorker : notify client " << data.toHex();

    foreach(QTcpSocket* client, clients) {
        client->write(data);
        qDebug() << "APIWorker : notify client " << client->peerAddress().toString()
                 << u8"防区状态(字节序列第3字节):" << QString::number(data[2])
                 << u8"防区名称（字节序列第4字节至长度-2）: "
                 << QString::fromLocal8Bit(data.mid(3, data.length() - 5));
    }
}
