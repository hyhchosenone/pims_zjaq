#ifndef VIBSENSORWORKER_H
#define VIBSENSORWORKER_H

#include <QObject>
#include <QSerialPort>

class VibSensorWorker : public QObject
{
    Q_OBJECT
public:
    explicit VibSensorWorker(QObject *parent = nullptr);
    ~VibSensorWorker();

public slots:
    void doWork();
    void setRuning(bool flag);
    void setWorkState(bool state);
    void setAddrList(QList<quint8> list);
    void listenSensor(int des);

signals:
    void uploadData(QByteArray data);

private slots:
    void readMessage();

private:
    QString com;
    QSerialPort *serialPort = nullptr;
    QList<quint8> addrList;
    bool state = false;
    bool running = true;

};

#endif // VIBSENSORWORKER_H
