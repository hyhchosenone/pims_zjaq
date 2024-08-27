#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QString>

const quint8 USR_READ_DEV_DATA = 0x03; //读传感器数据
const quint8 USR_READ_DEV_TH = 0x04; //读报警限值
const quint8 USR_READ_DEV_ADDR = 0x05; //读设备地址
const quint8 USR_READ_DEV_RATE = 0x06; //读设备波特率
const quint8 USR_READ_DEV_ANG_V = 0x08; //读设备角速度
const quint8 USR_READ_DEV_ACC_V = 0x09; //读设备加速度

const quint8 USR_WRITE_DEV_TH = 0x34; //写报警限值
const quint8 USR_WRITE_DEV_ADDR = 0x35; //写设备地址
const quint8 USR_WRITE_DEV_RATE = 0x36; //写设备波特率

enum BaudRate {
    Baud1200 = 1200,
    Baud2400 = 2400,
    Baud4800 = 4800,
    Baud9600 = 9600,
    Baud19200 = 19200,
    Baud38400 = 38400,
    Baud57600 = 57600,
    Baud115200 = 115200,
    UnknownBaud = -1,
};

const quint16 DEV_WAIT_TIME = 500; //超时时间,ms
#endif // PROTOCOL_H
