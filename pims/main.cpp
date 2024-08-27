#include "alarmtcpserver.h"
#include "netalarmserver.h"
#include "pims.h"
#include <appinit.h>
#include <QProcess>
#include <QTimer>
#include <QtWidgets/QApplication>

QTime startTime;

void timeWork()
{
    QTime currTime = QTime::currentTime();
    bool runningFlag = startTime.msecsTo(currTime) > 12 * 60 * 60 * 1000; //时*分*秒*毫秒

    qDebug() << "starting..." << startTime.msecsTo(currTime);

    if (runningFlag || (currTime.hour() == 6 && currTime.minute() < 5)) { //6:05以内
        qApp->exit(902903);
    }
}

int main(int argc, char *argv[])
{
    //全局缩放
//    qputenv("QT_SCALE_FACTOR", QByteArray("1"));
    qputenv("QT_SCALE_FACTOR", QByteArray());

    QApplication a(argc, argv);

    AppInit *init = new AppInit;
    init->Load();

    PIMS w;

    qDebug() << "Main Thread : " << QThread::currentThread();

    //2.启动NetAlarmServer线程
    //    AlarmThread *alarmServerThread = new AlarmThread();
//    QThread *alarmServerThread = new QThread();

//    AlarmTcpServer *alarmSrv = new AlarmTcpServer();
//    alarmSrv->moveToThread(alarmServerThread);

//    QObject::connect(alarmServerThread, &AlarmThread::finished, alarmSrv, &QObject::deleteLater);
//    QObject::connect(alarmServerThread, &AlarmThread::started, alarmSrv, &AlarmTcpServer::doWork);

//    QObject::connect(w.alarmWindws,
//                     &AlarmWindows::distribDefenceInfo,
//                     alarmSrv,
//                     &AlarmTcpServer::reciveDefenceInfo,
//                     Qt::QueuedConnection);

//    QObject::connect(w.alarmWindws,
//                     &AlarmWindows::distribRemoteCmd,
//                     alarmSrv,
//                     &AlarmTcpServer::reciveRemoteCmd,
//                     Qt::QueuedConnection);

//    QObject::connect(alarmSrv,
//                     &AlarmTcpServer::uploadInfo,
//                     w.alarmWindws,
//                     &AlarmWindows::updateStatus,
//                     Qt::QueuedConnection);

//    alarmServerThread->start();

    //3.启动界面
    w.show();

    //4.reboot timer
    startTime = QTime::currentTime();

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, timeWork);
//    timer.start(5 * 60 * 1000); //分*秒*毫秒 每12小时重启计时

    int exitCode = a.exec();

    if (exitCode == 902903 || exitCode != 0) {
        QProcess::startDetached(qApp->applicationFilePath());
        return 0;
    }

    return exitCode;
}
