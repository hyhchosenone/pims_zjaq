#ifndef DEFENCEZONE_H
#define DEFENCEZONE_H

#include "vibsensor.h"
#include <QList>
#include <QString>

class DefenceZone
{
public:
    explicit DefenceZone();

    int id;             //防区
    QString name;       //防区名称
    quint8 type;        //防区类型
    float sens;         //灵敏度，0.1~1.0
    QString address;    //防区主机地址，ip或者总线地址
    quint8 state;       //运行状态, 0:离线, 1:在线, 2:故障, 3:调试
    quint8 armingState; //警戒状态, 0:布防, 1:撤防
    quint8 alarmState;  //报警状态, 0:正常, 1:报警
    quint8 alarmTime;   //报警时长，单位秒，0~255
    quint8 alarmFreq;   //报警频率阈值，1~5
    int mapX;           //地图X轴坐标
    int mapY;           //地图Y轴坐标
    QString rtspAddress; //防区摄像机rtsp地址
    int groupId;        //分组ID

    QList<VibSensor> vsList; //传感器列表
    //    QList<quint8> addrList; //防区内传感器地址列表
};

Q_DECLARE_METATYPE(DefenceZone)

#endif // DEFENCEZONE_H
