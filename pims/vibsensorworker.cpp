#include "vibsensorworker.h"
#include "protocol.h"
#include "util.h"
#include <QDebug>
#include <QSettings>
#include <app.h>

VibSensorWorker::VibSensorWorker(QObject *parent) : QObject(parent)
{
    //配置文件
    QString fileName = QString("%1/config/%2_Config.ini").arg(App::AppPath).arg(App::AppName);
    QSettings config(fileName, QSettings::IniFormat);
    config.beginGroup("SerialPort");
    QString com = config.value("com").toString();
    config.endGroup();

    if(com.isEmpty()) {
        this->com = "COM1";
    }else {
        this->com = com;
    }
}

VibSensorWorker::~VibSensorWorker()
{
    serialPort->close();
    delete serialPort;
}

void VibSensorWorker::doWork()
{
    qDebug() << "VibSensorWorker doWork";
    this->serialPort = new QSerialPort(this->com);

    if(!serialPort->open(QIODevice::ReadWrite)) {
        qDebug() << "VibSensorWorker SerialPort Open Fail!";
    }

    this->state = true;

    while (running) {
        int len = addrList.size();
//        qDebug() << "VibSensorWorker addr list :" << addrList.size();

        for (int index = 0 ;index < len; index++) {
            if(!this->state) {
                qDebug() << "break is" << state;
                break;
            }

            quint8 addr = addrList[index];
            listenSensor(addr);

//            qDebug() << QString::number(addr);
        }
    }

    qDebug() << "out woring" << state;
}

void VibSensorWorker::setRuning(bool flag)
{
    this->running = flag;
}

void VibSensorWorker::setWorkState(bool state)
{
    this->state = state;
    qDebug() << "woring state" << state;
}

void VibSensorWorker::setAddrList(QList<quint8> list)
{
    this->addrList = list;
    qDebug() << "setAddrList" << list.size();
}

void VibSensorWorker::listenSensor(int des)
{
    //1.检查格式
//    qDebug() << "listen sensor addr " << des;

    //2.构造指令
    QByteArray ba;
    QByteArray data = Util::buildData(des, USR_READ_DEV_DATA, ba);

//    qDebug() << "listen sensor addr hex" << data.toHex();

    this->serialPort->write(data);

    QByteArray buff;

    while (serialPort->waitForReadyRead(DEV_WAIT_TIME)) {
        buff.append(serialPort->readAll());

        if(buff.length() == 10) {
            break;
        }
    }

    if(buff.length() == 10) {
        emit uploadData(buff);
    }else {
        emit uploadData(data);
    }

//    qDebug() << "listenSensor update data:" << buff;
}

void VibSensorWorker::readMessage()
{

}
