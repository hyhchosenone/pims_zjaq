#include "alarmreciver.h"

AlarmReciver::AlarmReciver()
{
}

AlarmReciver::~AlarmReciver()
{
}

void AlarmReciver::doWork()
{
    client->connectToHost("192.168.1.123", 53180);

    connect(client, SIGNAL(readyRead()), this, SLOT(revData()));
}

void AlarmReciver::revData()
{

}
