#pragma once

#include <QObject>
#include <QDataStream>
#include "alarminfo.h"

Q_DECLARE_METATYPE(Defence)
Q_DECLARE_METATYPE(AlarmInfo)

class AlarmData : public QObject
{
    Q_OBJECT

public:
    AlarmData(QObject *parent = 0);
    AlarmData(const AlarmData &data);
    ~AlarmData();

    friend QDataStream &operator<<(QDataStream &out, const AlarmData &data);
    friend QDataStream &operator>>(QDataStream &in, AlarmData &data);

public:
    AlarmInfo alarmInfo;
};
