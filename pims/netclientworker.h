#ifndef NETCLIENTWORKER_H
#define NETCLIENTWORKER_H

#include <QtCore/QObject>

#include "defence.h"
#include "vibsensor.h"
#include <QTcpSocket>

#include <QFile>
#include <QMutex>

class NetClientWorker : public QObject
{
    Q_OBJECT
public:
    explicit NetClientWorker(qintptr socketDescriptor, QObject *parent = nullptr);
    ~NetClientWorker();

    QTcpSocket *clientSocket = nullptr;

public slots:
    void doWork();
    void reciveDefenceInfo(DefenceZone defence);
    void reciveRemoteCmd(QString addr, int cmd);
    void setRuning(bool flag);
    void setWorkState(int state);
    void listenSensor(int des);
    void setSensValue(int value);
    void setRelay(bool flag);

signals:
    void uploadData(QByteArray data);
    void notifyServer(QString ip, quint8 sensorAddr, quint8 status);

private:
    DefenceZone defence;
    QHash<quint8, VibSensor> sensorStatus;
    int state = 0; //0:初始化，1:布防，2：撤防，3：报警
    bool running = true;
    QMutex socketLock;
    QFile *logFile = nullptr;
};

#endif // NETCLIENTWORKER_H
