#ifndef ALARMINFO_H
#define ALARMINFO_H

typedef struct Defence {
    unsigned char chn;    //编号: 1~255
    unsigned char status; // 0:离线, 1:布防, 2:撤防
    unsigned char alarm;  // 0:正常, 1:报警
} DefenceInfo;

struct AlarmInfo {
    unsigned short namelenth; //名称长度
    char name[16]; //名称
    unsigned short count; //防区数量
    DefenceInfo *defence; //防区
};

#endif // ALARMINFO_H
