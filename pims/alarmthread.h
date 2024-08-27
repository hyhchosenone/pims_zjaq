#ifndef ALARMTHREAD_H
#define ALARMTHREAD_H

#include <QThread>
#include <QtCore/QObject>

class AlarmThread : public QThread
{
    Q_OBJECT
public:
    explicit AlarmThread(QObject *parent = nullptr);

protected:
    void run();
};

#endif // ALARMTHREAD_H
