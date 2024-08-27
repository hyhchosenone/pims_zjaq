#pragma once

#include <QObject>
#include <QSqlDatabase>

class DBHelper : public QObject
{
    Q_OBJECT

public:
    static DBHelper& getInstance();
    ~DBHelper();

    //振动报警
    QList<QVector<QString>> getVSZoneList();
    QList<QVector<QString>> getVSAlarmList();
    QList<QVector<QString>> getVSAlarmHistory();
    bool setVSAlertStatus(QString zone_no, QString alert_status);
    bool clearVSArming(QString zone_no);

    //对射报警
    QList<QVector<QString>> getMBZoneList();
    QList<QVector<QString>> getMBAlarmList();
    QList<QVector<QString>> getMBAlarmHistory();
    bool setMBAlertStatus(QString zone_no, QString alert_status);
    bool clearMBArming(QString zone_no);

private:
    DBHelper();
    DBHelper(QObject *parent);
    DBHelper(const DBHelper &copy);
    DBHelper & operator=(const DBHelper &other);
    //static DBHelper instance;
    QSqlDatabase db;
};
