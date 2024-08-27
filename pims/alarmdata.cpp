#include "alarmdata.h"

AlarmData::AlarmData(QObject *parent)
    : QObject(parent)
{

}

AlarmData::AlarmData(const AlarmData & data)
{
    this->alarmInfo = data.alarmInfo;
}

AlarmData::~AlarmData()
{
}

QDataStream &operator<<(QDataStream &out, const AlarmData &data)
{
    out << data.alarmInfo.namelenth;
    out.writeRawData(data.alarmInfo.name, data.alarmInfo.namelenth);
    out << data.alarmInfo.count;

    DefenceInfo *d;

    for(d = data.alarmInfo.defence; d < data.alarmInfo.defence + data.alarmInfo.count; d++) {
        out << d->chn << d->status << d->alarm;
    }

    return out;
}

QDataStream &operator>>(QDataStream &in, AlarmData &data)
{
    in >> data.alarmInfo.namelenth;
    in.readRawData(data.alarmInfo.name, data.alarmInfo.namelenth);
    in >> data.alarmInfo.count;

    DefenceInfo *d = new DefenceInfo[data.alarmInfo.count];

    for(int i = 0; i < data.alarmInfo.count; i++) {
        in >> d[i].chn >> d[i].status >> d[i].alarm;
    }

    data.alarmInfo.defence = d;

    //delete [] d;
    return in;
}
