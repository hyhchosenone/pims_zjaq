#ifndef NETALARMSERVER_H
#define NETALARMSERVER_H

#include "defence.h"
#include "netclientworker.h"

#include <QtCore/QObject>

#include <QTcpServer>
#include <QThread>

class NetAlarmServer : public QObject
{
    Q_OBJECT
public:
    explicit NetAlarmServer(QObject *parent = nullptr);
    ~NetAlarmServer();

public slots:
    void doWork();
    void newClient();
    void clientDisconneced();
    void notifyClient(QByteArray data);
    void reciveDefenceInfo(QString address, DefenceZone defence);
    void reciveRemoteCmd(QString addr, int cmd);
    void recivceClientStatus(QString ip, quint8 sensorAddr, quint8 status);

signals:
    /****
     * address : 防区主机地址，ip或总线地址
     * status  : 状态，1=防区报警, 7=主机上线, 8=主机下线
     * 
    */
    void uploadInfo(QString address, quint8 sensorAddr, qint8 status);
    void distribDefenceInfo(DefenceZone defence);
    void distribRemoteCmd(QString addr, int cmd);

private:
    QTcpServer *alarmServer = Q_NULLPTR;
    QMap<QString, QTcpSocket *> clients;      //key=防区主机地址
    QMap<QString, QThread *> clientThreads;   //key=防区主机地址
    QMap<QString, NetClientWorker *> clientWorkers; //key=防区主机地址
    QHash<QString, DefenceZone> addrDefences; //key=防区主机地址
};

#endif // NETALARMSERVER_H
