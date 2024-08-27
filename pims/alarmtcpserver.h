#ifndef ALARMTCPSERVER_H
#define ALARMTCPSERVER_H

#include "alarmthread.h"
#include "netclientworker.h"

#include <QTcpServer>
#include <QThread>
#include <QtCore/QObject>

class AlarmTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit AlarmTcpServer(QObject *parent = nullptr);
    ~AlarmTcpServer();

public slots:
    void doWork();
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

protected:
    void incomingConnection(qintptr handle);

private:
    void startClientWorker(qintptr socketDescriptor);

private:
    QMap<QString, QThread *> clientThreads;         //key=防区主机地址
    QMap<QString, NetClientWorker *> clientWorkers; //key=防区主机地址
    QHash<QString, DefenceZone> addrDefences;       //key=防区主机地址
};

#endif // ALARMTCPSERVER_H
