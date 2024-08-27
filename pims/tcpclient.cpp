#include "tcpclient.h"
#include <QDataStream>
#include <QHostAddress>
#include <QNetworkProxy>
#include <QThread>

TcpClient::TcpClient(QObject *parent)
    : QObject(parent)
{
    socket = new QTcpSocket(this);
    socket->setProxy(QNetworkProxy::NoProxy);
}

TcpClient::~TcpClient()
{
    delete socket;
}

void TcpClient::setIpAndPort(QString ip, int port)
{
    this->srvIP = ip;
    this->srvPort = port;
}

void TcpClient::doWork()
{
    connectServer();

    connect(socket, &QTcpSocket::readyRead, this, &TcpClient::readMessage,Qt::QueuedConnection);
    connect(socket, &QTcpSocket::disconnected, this, &TcpClient::connectServer,Qt::QueuedConnection);

    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            [=](QAbstractSocket::SocketError socketError){
        if(socketError != QAbstractSocket::SocketTimeoutError) {
            qDebug() << "socketError Error" << socketError << socket->errorString();
        }
    });
}

void TcpClient::processCmd(QByteArray data)
{
    socket->write(data);
    qDebug() << "Process CMD" << data.toHex();
}

void TcpClient::readMessage()
{
    QByteArray buff = socket->readAll();

    while(socket->waitForReadyRead(10)) {
        buff.append(socket->readAll());
    }

    if(socket->error() == QTcpSocket::SocketAccessError) {
        qDebug() << "Read Error" << socket->errorString();

    }else if(socket->error() == QTcpSocket::SocketTimeoutError) {
        //qDebug() << "Read Timeout" << socket->errorString();
    }

    emit uploadData(buff);
}

void TcpClient::connectServer()
{
    socket->connectToHost(QHostAddress(srvIP), srvPort);

    while (!socket->waitForConnected(3000)) {
        qDebug() << "Connected Again";
        socket->connectToHost(QHostAddress(srvIP), srvPort);
    }

    qDebug() << "Connected" << socket->peerAddress();
}
