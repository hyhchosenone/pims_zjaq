#include "alarmthread.h"
#include <QDebug>

AlarmThread::AlarmThread(QObject *parent) : QThread(parent)
{
    
}

void AlarmThread::run()
{
    qDebug() << "AlarmThread run : " << QThread::currentThread();
    exec();
}
