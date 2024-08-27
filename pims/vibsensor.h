#ifndef VIBSENSOR_H
#define VIBSENSOR_H

#include<QObject>

class VibSensor
{
public:
    explicit VibSensor();

    int id;      //编号
    QString addr;     //传感器地址
    quint32 baudRate; //波特率
    float sens;       //灵敏度，0.1~1.0，越小灵敏度越高
    quint8 freq;      //频率阈值
    qint16 xGyro;
    qint16 yGyro;
    qint16 zGyro;

    quint8 state;       //0:离线, 1:在线, 2:故障
    quint8 armingState; //警戒状态, 0:布防, 1:撤防
    quint8 alarmState;  //报警状态, 0:正常, 1:报警
    quint8 alarmCount;  //连续报警次数
    quint8 boot;        //0:初始化，1:已启动
    int mapX;        //地图X轴坐标
    int mapY;        //地图Y轴坐标
};

Q_DECLARE_METATYPE(VibSensor)

#endif // VIBSENSOR_H
