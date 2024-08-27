#include "alarmworkerthread.h"
#include "protocol_v3.h"
#include "util.h"
#include <QHostAddress>

AlarmWorkerThread::AlarmWorkerThread(qintptr socketDescriptor, QObject *parent)
    : QObject(parent)
    , QRunnable()
{
    this->clientSocket = new QTcpSocket(this);
    clientSocket->setSocketDescriptor(socketDescriptor);

    setWorkState(1);
}

void AlarmWorkerThread::run()
{
    qDebug() << "Client Thread : " << QThread::currentThread() << ", new client "
             << this->clientSocket->peerAddress().toString();

    //    exec();

    while (running) {
        if (defence.armingState == 1) {
            int len = defence.vsList.size();

            for (int index = 0; index < len; index++) {
                VibSensor vs = defence.vsList[index];

                listenSensor(vs.addr.toUInt());

                QThread::msleep(100);
            }
        } else {
            QThread::msleep(1000); //防止撤防状态下cpu占用过高
        }
    }

    qDebug() << "NetClientWorker Stoped";
}

void AlarmWorkerThread::reciveDefenceInfo(DefenceZone defence)
{
    if (this->clientSocket->peerAddress().toString() == defence.address) {
        this->defence = defence;

        qDebug() << QThread::currentThreadId() << "Client ReciveInfo : " << defence.address
                 << "alarm time is " << defence.alarmTime;

        sensorStatus.clear();

        for (int index = 0; index < defence.vsList.size(); index++) {
            VibSensor vs = defence.vsList[index];
            vs.xGyro = 0;
            vs.yGyro = 0;
            vs.zGyro = 0;
            //            vs.baseValue = (vs.sens == 0 ? 65535 / defence.sens : 65535 / vs.sens) / 5;

            sensorStatus.insert(vs.addr.toUShort(), vs);
        }
    }
}

void AlarmWorkerThread::reciveRemoteCmd(QString addr, int cmd)
{
    if (addr == defence.address) {
        switch (cmd) {
        case 17: //清除现场报警

            setRelay(false);

            QThread::msleep(100);

            setRelay(false);

            break;
        }
    }
}

void AlarmWorkerThread::setRuning(bool flag)
{
    this->running = flag;
}

//0:初始化，1:布防，2：撤防，3：报警
void AlarmWorkerThread::setWorkState(int state)
{
    this->state = state;
}

void AlarmWorkerThread::listenSensor(int des)
{
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

    //    //1.检查格式
    //    qDebug() << "listen sensor addr " << des;

    //    //2.传输指令
    //    QByteArray ba;
    //    ba.append(USR_READ_DEV_ANG_V);

    //    QByteArray data = Util::buildDataV3(des, ba);

    //    //    qDebug() << "Req :" << data.toHex();

    //    this->clientSocket->write(data);

    //    QByteArray buff;

    //    while (clientSocket->waitForReadyRead(DEV_WAIT_TIME)) {
    //        buff.append(clientSocket->readAll());

    //        if (buff.length() == 12) {
    //            break;
    //        }
    //    }

    //    //    qDebug() << "Res :" << buff.toHex();

    //    //3. 解析数据
    //    if (buff.length() == 0) {
    //        qDebug() << "Res length is 0";
    //        //        vs.state = 2;
    //    } else if (buff.length() == 12) {
    //        bool ok;
    //        short gx = buff.mid(4, 2).toHex().toUShort(&ok, 16);
    //        short gy = buff.mid(6, 2).toHex().toUShort(&ok, 16);
    //        short gz = buff.mid(8, 2).toHex().toUShort(&ok, 16);

    //        //        qDebug() << des << " Gyro Curr : " << gx << ", " << gy << ", " << gz;

    //        int gAvg = qAbs(gx + gy + gz) / 3;
    //        quint16 gMaxAbs = qMax(qAbs(gx - vs.xGyro), qMax(qAbs(gy - vs.yGyro), qAbs(gz - vs.zGyro)));

    //        //        qDebug() << des << " Gyro Diff : " << gx - vs.xGyro << ", " << gy - vs.yGyro << ", "
    //        //                 << gz - vs.zGyro << " gAvg :" << gAvg;

    //        int alarmValue = qAbs(gMaxAbs - vs.avgGyros);

    //        if (qAbs(alarmValue) > vs.baseValue) {
    //            vs.alarmCount += 1;
    //            qDebug() << des << ": alarm count + 1";
    //            qDebug() << "gyro xyz : " << ok << gx << ", " << gy << ", " << gz;
    //            qDebug() << "avg pre : " << vs.avgGyros << " , gMaxAbs : " << gMaxAbs
    //                     << ", alarm is : " << qAbs(alarmValue) << ", baseValue is " << vs.baseValue;

    //            if (vs.alarmCount >= defence.alarmFreq) { //连续报警，才触发报警
    //                vs.alarmCount = 0;                    //立即清零

    //                emit notifyServer(this->clientSocket->peerAddress().toString(),
    //                                  des,
    //                                  1); //1 : alarmed

    //                qDebug() << des << " Alarming, sens value is " << vs.sens;

    //                //控制现场继电器
    //                setRelay(true);
    //                QThread::msleep(100);
    //                setRelay(true);

    //                QThread::sleep(defence.alarmTime);

    //                setRelay(false);
    //                QThread::msleep(100);
    //                setRelay(false);
    //            }
    //        } else {
    //            vs.alarmCount = 0; //只要不是连续报警，就清零计数
    //                               //qDebug() << "alarm count set 0";
    //        }

    //        vs.xGyro = gx;
    //        vs.yGyro = gy;
    //        vs.zGyro = gz;
    //        vs.avgGyros = (gAvg + vs.avgGyros) / 2;
    //    } else {
    //        qDebug() << "Res length is " << buff.length();
    //    }

    sensorStatus[des] = vs;
}

void AlarmWorkerThread::setSensValue(int value)
{
    //    this->sensValue = 65535 / value;
}

void AlarmWorkerThread::setRelay(bool flag)
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
