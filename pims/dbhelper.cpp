#include "dbhelper.h"
#include <QSqlQuery>
#include <QVariant>
#include <QVector>

DBHelper::DBHelper()
{
    this->db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("7.44.232.194");
    //db.setHostName("127.0.0.1");
    //db.setHostName("192.171.23.55");
    db.setDatabaseName("pims");
    db.setUserName("postgres");
    db.setPassword("p0st45576");
    bool ok = db.open();
}

DBHelper::DBHelper(QObject *parent)
    : QObject(parent)
{
    DBHelper();
}

DBHelper::~DBHelper()
{
    this->db.close();
}

QList<QVector<QString>> DBHelper::getVSZoneList()
{
    QList<QVector<QString>> list;

    if (!this->db.isOpen())
    {
        return list;
    }

    QSqlQuery sql("SELECT zone_no, zone_name, alert_status FROM vs_zone ORDER BY id", this->db);


    while (sql.next())
    {
        QVector<QString> zone;
        zone.append(sql.value(0).toString());
        zone.append(sql.value(1).toString());
        zone.append(sql.value(2).toString());

        list.append(zone);
    }

    return list;
}

QList<QVector<QString>> DBHelper::getVSAlarmList()
{
    QList<QVector<QString>> list;

    if (!this->db.isOpen())
    {
        return list;
    }

    QSqlQuery sql("SELECT zone_no, alarm_status, alarm_time FROM vs_alarm WHERE alarm_status = '报警' ORDER BY id DESC LIMIT 30", this->db);


    while (sql.next())
    {
        QVector<QString> zone;
        zone.append(sql.value(0).toString());
        zone.append(sql.value(1).toString());
        zone.append(sql.value(2).toString());

        list.append(zone);
    }

    return list;
}

QList<QVector<QString>> DBHelper::getVSAlarmHistory()
{
    QList<QVector<QString>> list;

    if (!this->db.isOpen())
    {
        return list;
    }

    QSqlQuery sql("SELECT zone_no, alarm_status, alarm_time FROM vs_alarm ORDER BY id DESC LIMIT 100", this->db);


    while (sql.next())
    {
        QVector<QString> zone;
        zone.append(sql.value(0).toString());
        zone.append(sql.value(1).toString());
        zone.append(sql.value(2).toString());

        list.append(zone);
    }

    return list;
}

bool DBHelper::setVSAlertStatus(QString zone_no, QString alert_status)
{
    QSqlQuery sql(this->db);
    sql.prepare("UPDATE vs_zone SET alert_status = :alert_status WHERE zone_no = :zone_no");
    sql.bindValue(":zone_no", zone_no);
    sql.bindValue(":alert_status", alert_status);
    bool result = sql.exec();

    return result;
}

bool DBHelper::clearVSArming(QString zone_no)
{
    QSqlQuery sql(this->db);
    sql.prepare("UPDATE vs_alarm SET alarm_status = :alarm_status WHERE zone_no = :zone_no");
    sql.bindValue(":zone_no", zone_no);
    sql.bindValue(":alarm_status", "消警");
    bool result = sql.exec();

    return result;
}

QList<QVector<QString>> DBHelper::getMBZoneList()
{
    QList<QVector<QString>> list;

    if (!this->db.isOpen())
    {
        return list;
    }

    QSqlQuery sql("SELECT zone_no, zone_name, alert_status FROM mb_zone ORDER BY id", this->db);


    while (sql.next())
    {
        QVector<QString> zone;
        zone.append(sql.value(0).toString());
        zone.append(sql.value(1).toString());
        zone.append(sql.value(2).toString());

        list.append(zone);
    }

    return list;
}

QList<QVector<QString>> DBHelper::getMBAlarmList()
{
    QList<QVector<QString>> list;

    if (!this->db.isOpen())
    {
        return list;
    }

    QSqlQuery sql("SELECT zone_no, alarm_status, alarm_time FROM mb_alarm WHERE alarm_status = '报警' ORDER BY id DESC LIMIT 30", this->db);


    while (sql.next())
    {
        QVector<QString> zone;
        zone.append(sql.value(0).toString());
        zone.append(sql.value(1).toString());
        zone.append(sql.value(2).toString());

        list.append(zone);
    }

    return list;
}

QList<QVector<QString>> DBHelper::getMBAlarmHistory()
{
    QList<QVector<QString>> list;

    if (!this->db.isOpen())
    {
        return list;
    }

    QSqlQuery sql("SELECT zone_no, alarm_status, alarm_time FROM mb_alarm ORDER BY id DESC LIMIT 100", this->db);


    while (sql.next())
    {
        QVector<QString> zone;
        zone.append(sql.value(0).toString());
        zone.append(sql.value(1).toString());
        zone.append(sql.value(2).toString());

        list.append(zone);
    }

    return list;
}

bool DBHelper::setMBAlertStatus(QString zone_no, QString alert_status)
{
    QSqlQuery sql(this->db);
    sql.prepare("UPDATE mb_zone SET alert_status = :alert_status WHERE zone_no = :zone_no");
    sql.bindValue(":zone_no", zone_no);
    sql.bindValue(":alert_status", alert_status);
    bool result = sql.exec();

    return result;
}

bool DBHelper::clearMBArming(QString zone_no)
{
    QSqlQuery sql(this->db);
    sql.prepare("UPDATE mb_alarm SET alarm_status = :alarm_status WHERE zone_no = :zone_no");
    sql.bindValue(":zone_no", zone_no);
    sql.bindValue(":alarm_status", "消警");
    bool result = sql.exec();

    return result;
}

DBHelper& DBHelper::getInstance()
{
    static DBHelper instrance;
    return instrance;
}
