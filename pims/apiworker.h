#ifndef APIWORKER_H
#define APIWORKER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class APIWorker : public QObject
{
    Q_OBJECT
public:
    explicit APIWorker(QObject *parent = nullptr);
    ~APIWorker();

public slots:
    void doWork();
    void newClient();
    void notifyClient(QByteArray data);

signals:

private:
    QTcpServer *apiServer = Q_NULLPTR;
    QMap<QString, QTcpSocket*> clients;
};

#endif // APIWORKER_H
