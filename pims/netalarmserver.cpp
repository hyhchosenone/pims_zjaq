#include "netalarmserver.h"
#include "netclientworker.h"

#include <QTcpSocket>

NetAlarmServer::NetAlarmServer(QObject *parent) : QObject(parent)
{
    alarmServer = new QTcpServer(this);
}

NetAlarmServer::~NetAlarmServer()
{
    foreach (QTcpSocket *client, clients) {
        client->close();
        delete client;
    }

    alarmServer->close();
    delete alarmServer;
}

void NetAlarmServer::doWork()
{
    if (!alarmServer->listen(QHostAddress::AnyIPv4, 8516)) {
        qDebug() << alarmServer->errorString();
        alarmServer->close();
    }

    qDebug() << "NetAlarmServer Started";

    connect(alarmServer, &QTcpServer::newConnection, this, &NetAlarmServer::newClient);
}

void NetAlarmServer::newClient()
{
    QTcpSocket *clientSocket = alarmServer->nextPendingConnection();
    qintptr socketDesc = clientSocket->socketDescriptor();

    //传递socket descriptor进而构造socket，不允许直接跨线程传递对象
    NetClientWorker *clientWorker = new NetClientWorker(socketDesc);

    QString clientIP = clientWorker->clientSocket->peerAddress().toString();

    clientSocket->setSocketDescriptor(0);

    //检测是否存在client thread
    if (clientThreads.contains(clientIP)) {
        QThread *thread = clientThreads.value(clientIP);
        NetClientWorker *worker = clientWorkers.value(clientIP);

        worker->setRuning(false);

        thread->quit();

        qDebug() << "NetAlarmServer wait...";

        thread->wait();

        qDebug() << "NetAlarmServer wait end";
        delete thread;
        //        delete worker;

        clientThreads.remove(clientIP);
        clientWorkers.remove(clientIP);
    }

    QThread *clientThread = new QThread();
    clientWorker->moveToThread(clientThread);

    connect(clientThread, &QThread::finished, clientWorker, &QObject::deleteLater);
    connect(clientWorker->clientSocket,
            &QTcpSocket::disconnected,
            this,
            &NetAlarmServer::clientDisconneced);

    connect(this,
            &NetAlarmServer::distribDefenceInfo,
            clientWorker,
            &NetClientWorker::reciveDefenceInfo);

    connect(this,
            &NetAlarmServer::distribRemoteCmd,
            clientWorker,
            &NetClientWorker::reciveRemoteCmd,
            Qt::QueuedConnection);

    connect(clientThread, &QThread::started, clientWorker, &NetClientWorker::doWork);

    connect(clientWorker,
            &NetClientWorker::notifyServer,
            this,
            &NetAlarmServer::recivceClientStatus);

    clientThread->start();

    clientWorkers[clientIP] = clientWorker;
    clientThreads[clientIP] = clientThread;

    emit uploadInfo(clientIP, 0, 7); //7: 主机上线

    qDebug() << "Server Thread : " << QThread::currentThread() << ", new client " << clientIP;
}

void NetAlarmServer::clientDisconneced()
{
    QTcpSocket *client = dynamic_cast<QTcpSocket *>(sender());
    qDebug() << "Server Thread : " << QThread::currentThread() << ", client disconneced"
             << client->peerAddress().toString();

    emit uploadInfo(client->peerAddress().toString(), 0, 8); //8: 主机下线

    client->deleteLater();
}

void NetAlarmServer::notifyClient(QByteArray data) {}

void NetAlarmServer::reciveDefenceInfo(QString address, DefenceZone defence)
{
    addrDefences[address] = defence;

    emit distribDefenceInfo(defence);

    qDebug() << "Server ReciveInfo : " << address << "device count is " << defence.vsList.count();
}

void NetAlarmServer::reciveRemoteCmd(QString addr, int cmd)
{
    emit distribRemoteCmd(addr, cmd);
}

void NetAlarmServer::recivceClientStatus(QString ip, quint8 sensorAddr, quint8 status)
{
    qDebug() << "Recivce :" << ip << ", status is " << status;

    emit uploadInfo(ip, sensorAddr, status);
}
