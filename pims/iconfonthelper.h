#pragma once

#include <QObject>
#include <QFont>
#include <QMutex>
#include <QLabel>
#include <QAbstractButton>

class IconfontHelper : public QObject
{
    Q_OBJECT

private:
    explicit IconfontHelper(QObject *parent = nullptr);
    QFont iconfont;
    static IconfontHelper *instance;

public:
    static IconfontHelper *Instance()
    {
        static QMutex mutex;
        if (!instance) {
            QMutexLocker locker(&mutex);
            if (!instance) {
                instance = new IconfontHelper;
            }
        }
        return instance;
    }

    ~IconfontHelper();

    void setIcon(QLabel *lab, QChar c, int size = 14);
    void setIcon(QAbstractButton *btn, QChar c, int size = 14);
    void setIcon(QAbstractButton *btn, QChar icon, QString text, int size = 14);
};
