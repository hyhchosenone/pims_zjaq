#include "alarmtcpserver.h"
#include "alarmworkerthread.h"

#include <QThreadPool>

AlarmTcpServer::AlarmTcpServer(QObject *parent) : QTcpServer(parent)
{}

AlarmTcpServer::~AlarmTcpServer()
{
    qDebug() << "AlarmTcpServer going...";
}

void AlarmTcpServer::doWork()
{
    this->listen(QHostAddress::AnyIPv4, 8516);

    qDebug() << "AlarmTcpServer Started";
}

void AlarmTcpServer::incomingConnection(qintptr handle)
{
    startClientWorker(handle);
}

void AlarmTcpServer::startClientWorker(qintptr socketDescriptor)
{
    //传递socket descriptor进而构造socket，不允许直接跨线程传递对象
    NetClientWorker *clientWorker = new NetClientWorker(socketDescriptor);

    QString clientIP = clientWorker->clientSocket->peerAddress().toString();

    //检测是否存在client thread
    //    if (clientThreads.contains(clientIP)) {
    //        QThread *thread = clientThreads.value(clientIP);
    //        NetClientWorker *worker = clientWorkers.value(clientIP);

    //        worker->setRuning(false);

    //        thread->quit();

    //        qDebug() << "clientThread wait...";

    //        thread->wait();

    //        qDebug() << "clientThread wait end";
    //        delete thread;

    //        clientThreads.remove(clientIP);
    //        clientWorkers.remove(clientIP);
    //    }

    //    AlarmThread *clientThread = new AlarmThread;
    QThread *clientThread = new QThread;
    clientWorker->moveToThread(clientThread);

    connect(clientThread, &QThread::finished, clientWorker, &QObject::deleteLater);
    connect(clientThread, &QThread::started, clientWorker, &NetClientWorker::doWork);

    connect(clientWorker->clientSocket,
            &QTcpSocket::disconnected,
            this,
            &AlarmTcpServer::clientDisconneced);

    connect(this,
            &AlarmTcpServer::distribDefenceInfo,
            clientWorker,
            &NetClientWorker::reciveDefenceInfo,
            Qt::DirectConnection);

    connect(this,
            &AlarmTcpServer::distribRemoteCmd,
            clientWorker,
            &NetClientWorker::reciveRemoteCmd,
            Qt::DirectConnection);

    connect(clientWorker,
            &NetClientWorker::notifyServer,
            this,
            &AlarmTcpServer::recivceClientStatus,
            Qt::QueuedConnection);

    clientThread->start();

    clientWorkers[clientIP] = clientWorker;
    clientThreads[clientIP] = clientThread;

    emit uploadInfo(clientIP, 0, 7); //7: 主机上线

    qDebug() << "Server Thread : " << QThread::currentThread() << ", new client " << clientIP;
}

void AlarmTcpServer::clientDisconneced()
{
    QTcpSocket *client = dynamic_cast<QTcpSocket *>(sender());
    qDebug() << "Server Thread : " << QThread::currentThread() << ", client disconneced"
             << client->peerAddress().toString();

    emit uploadInfo(client->peerAddress().toString(), 0, 8); //8: 主机下线

    client->deleteLater();
}

void AlarmTcpServer::notifyClient(QByteArray data) {}

void AlarmTcpServer::reciveDefenceInfo(QString address, DefenceZone defence)
{
    addrDefences[address] = defence;

    emit distribDefenceInfo(defence);

    qDebug() << QThread::currentThread() << "Server reciveDefenceInfo : " << address;
}

void AlarmTcpServer::reciveRemoteCmd(QString addr, int cmd)
{
    emit distribRemoteCmd(addr, cmd);

    qDebug() << QThread::currentThread() << "Server reciveDefenceInfo : " << addr;
}

void AlarmTcpServer::recivceClientStatus(QString ip, quint8 sensorAddr, quint8 status)
{
    qDebug() << "Recivce :" << ip << ", status is " << status;

    emit uploadInfo(ip, sensorAddr, status);
}
