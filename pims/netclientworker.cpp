#include "netclientworker.h"
#include "protocol_v3.h"
#include "util.h"
#include <QDateTime>
#include <QHostAddress>
#include <QThread>

NetClientWorker::NetClientWorker(qintptr socketDescriptor, QObject *parent)
    : QObject(parent)
{
    this->clientSocket = new QTcpSocket(this);
    clientSocket->setSocketDescriptor(socketDescriptor);

    setWorkState(1);
}

NetClientWorker::~NetClientWorker()
{
    logFile->flush();
    logFile->close();
}

void NetClientWorker::doWork()
{
    qDebug() << "Client Thread : " << QThread::currentThread() << ", new client "
             << this->clientSocket->peerAddress().toString();

    while (running) {
        if (defence.armingState == 1) {
            int len = defence.vsList.size();

            for (int index = 0; index < len; index++) {
                socketLock.lock();

                VibSensor vs = defence.vsList[index];

                listenSensor(vs.addr.toUInt());

                socketLock.unlock();

                QThread::msleep(20);
            }
        } else {
            QThread::msleep(1000); //防止撤防状态下cpu占用过高
        }
    }

    emit notifyServer(this->clientSocket->peerAddress().toString(), 0,
                      8); //1 : alarmed

    qDebug() << "NetClientWorker Stoped";
}

void NetClientWorker::reciveDefenceInfo(DefenceZone defence)
{
    if (this->clientSocket->peerAddress().toString() == defence.address) {
        this->defence = defence;

        if (logFile == nullptr) {
            logFile = new QFile("log/" + QString::fromLocal8Bit(defence.name.toLocal8Bit().data())
                                + "_" + this->clientSocket->peerAddress().toString() + "_log.txt");
            logFile->open(QIODevice::WriteOnly | QIODevice::Append);
        }

        qDebug() << QThread::currentThread() << "Client ReciveInfo : " << defence.address;

        sensorStatus.clear();

        for (int index = 0; index < defence.vsList.size(); index++) {
            VibSensor vs = defence.vsList[index];
            vs.xGyro = 0;
            vs.yGyro = 0;
            vs.zGyro = 0;

            sensorStatus.insert(vs.addr.toUShort(), vs);

            qDebug() << vs.addr << " : " << vs.sens;
        }
    }
}

void NetClientWorker::reciveRemoteCmd(QString addr, int cmd)
{
    qDebug() << QThread::currentThread() << "Client reciveRemoteCmd : " << addr;

    if (addr == defence.address) {
        switch (cmd) {
        case 17: //清除现场报警

            socketLock.lock();

            setRelay(false);

            QThread::msleep(100);

            setRelay(false);

            socketLock.unlock();

            break;
        }
    }
}

void NetClientWorker::setRuning(bool flag)
{
    this->running = flag;
}

//0:初始化，1:布防，2：撤防，3：报警
void NetClientWorker::setWorkState(int state)
{
    this->state = state;
}

void NetClientWorker::listenSensor(int des)
{
    QTextStream logStream(logFile);

    if (!sensorStatus.contains(des)) {
        qDebug() << "not contains addr " << des;
        return;
    }

    VibSensor vs = sensorStatus.value(des);

    //0. 检查传感器状态
    if (vs.state == 2 || vs.armingState == 0) {
        //        qDebug() << "armingState is 0, addr " << des;
        return;
    }

    //1.检查格式
    //    qDebug() << "listen sensor addr " << des;

    //2.传输指令
    QByteArray ba;
    ba.append(USR_READ_DEV_ACC_V);

    QByteArray data = Util::buildDataV3(des, ba);

    //    qDebug() << "Req :" << data.toHex();

    this->clientSocket->write(data);

    QByteArray buff;

    while (clientSocket->waitForReadyRead(DEV_WAIT_TIME)) {
        buff.append(clientSocket->readAll());

        if (buff.length() == 12) {
            break;
        }
    }

    //    qDebug() << "Res :" << buff.toHex();

    //3. 解析数据
    if (buff.length() == 0) {
        qDebug() << "Res length is 0";
        //        vs.state = 2;
    } else if (buff.length() == 12) {
        bool ok;
        short gx = buff.mid(4, 2).toHex().toUShort(&ok, 16);
        short gy = buff.mid(6, 2).toHex().toUShort(&ok, 16);
        short gz = buff.mid(8, 2).toHex().toUShort(&ok, 16);

        //        logStream << des << " Gyro Curr : " << gx << ", " << gy << ", " << gz << endl;

        //计算x,y,z加速度
        float ax = gx / 16383.5; //32767.0 * 2;
        float ay = gy / 16383.5; //32767.0 * 2;
        float az = gz / 16383.5; //32767.0 * 2;

        //选择灵敏度
        float sens = vs.sens > 0.0 && vs.sens < 1.0 ? vs.sens : defence.sens;

        if ((az > sens && az < 0.9) || (az > 1.1 && az < 1.9)) {
            vs.alarmCount += 1;

            logStream << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + " : " << des
                      << " : " << ax << ", " << ay << ", " << az << endl;

            //选择报警频率阈值
            int alarmFreq = vs.freq > 0 ? vs.freq : defence.alarmFreq;

            if (vs.alarmCount >= alarmFreq) { //连续报警，才触发报警
                vs.alarmCount = 0;   //立即清零

                emit notifyServer(this->clientSocket->peerAddress().toString(),
                                  des,
                                  1); //1 : alarmed

                //                logStream << des << " Alarming, sens value is " << vs.sens << endl;

                //控制现场继电器
                setRelay(true);
                QThread::msleep(20);
                setRelay(true);

                QThread::sleep(defence.alarmTime);

                setRelay(false);
                QThread::msleep(20);
                setRelay(false);
            }
        } else {
            vs.alarmCount = 0; //只要不是连续报警，就清零计数
        }
    } else {
        qDebug() << "Res length is " << buff.length();
    }

    sensorStatus[des] = vs;
}

void NetClientWorker::setSensValue(int value)
{
    //    this->sensValue = 65535 / value;
}

void NetClientWorker::setRelay(bool flag)
{
    QByteArray ba;
    ba.resize(8);
    ba[0] = 0xf1;
    ba[1] = 0x05;
    ba[2] = 0x00;
    ba[3] = 0x00;

    if (flag) { //打开报警，闭合继电器
        ba[4] = 0xff;
        ba[5] = 0x00;
        ba[6] = 0x98;
        ba[7] = 0xca;
    } else { //关闭报警，打开继电器
        ba[4] = 0x00;
        ba[5] = 0x00;
        ba[6] = 0xd9;
        ba[7] = 0x3a;
    }

    qDebug() << "Relay Req :" << ba.toHex();

    this->clientSocket->write(ba);

    QByteArray buff;

    while (clientSocket->waitForReadyRead(DEV_WAIT_TIME)) {
        buff.append(clientSocket->readAll());

        if (buff.length() == 8) {
            break;
        }
    }

    qDebug() << "Relay Res :" << buff.toHex();
}
