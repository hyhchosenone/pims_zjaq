#ifndef ALARMWORKERTHREAD_H
#define ALARMWORKERTHREAD_H

#include "defence.h"

#include <QRunnable>
#include <QTcpSocket>
#include <QThread>
#include <QtCore/QObject>

class AlarmWorkerThread : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit AlarmWorkerThread(qintptr socketDescriptor, QObject *parent = nullptr);

    QTcpSocket *clientSocket = nullptr;

public slots:
    void reciveDefenceInfo(DefenceZone defence);
    void reciveRemoteCmd(QString addr, int cmd);
    void setRuning(bool flag);
    void setWorkState(int state);
    void listenSensor(int des);
    void setSensValue(int value);
    void setRelay(bool flag);

protected:
    void run();
signals:
    void uploadData(QByteArray data);
    void notifyServer(QString ip, quint8 sensorAddr, quint8 status);

private:
    DefenceZone defence;
    QHash<quint8, VibSensor> sensorStatus;
    int state = 0; //0:初始化，1:布防，2：撤防，3：报警
    bool running = true;
};

#endif // ALARMWORKERTHREAD_H
