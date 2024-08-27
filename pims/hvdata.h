#pragma once

#include <QObject>
#include <QDataStream>

class HVData : public QObject
{
	Q_OBJECT

public:
	HVData(QObject *parent = Q_NULLPTR);
	HVData(const HVData &hvdata);
	~HVData();

	friend QDataStream &operator<<(QDataStream &out, const HVData &hvdata);
	friend QDataStream &operator>>(QDataStream &in, HVData &hvdata);

public:
	qint8  chn;    //电网编号: 0~15
	//qint16 hua;		//高压电压A相(V) -- 高压箱状态
	//qint16 hub;		//高压电压B相(V)
	qint16 huab;		//高压电压AB(V)
	//qint16 hia;		//高压电流A相(mA)
	//qint16 hib;		//高压电流B相(mA)
	qint16 hiab;		//高压电流AB(mA)
	/*
	qint16 lua;		//高压输入电压A相(V)
	qint16 lub;		//高压输入电压B相(V)
	qint16 lia;		//高压输入电流A相(A)
	qint16 lib;		//高压输入电流B相(A)
	qint8  tempra;		//温度(摄氏度)
	qint8  wetratio;	//湿度(%)
	qint16 iu;			//低压输入电压(V) --低压箱状态
	qint16 ii;			//低压输入电流(A)
	qint16 oia;		//低压分电流A(A)
	qint16 oib;		//低压分电流B(A)
	*/
	qint8  alarm_num;	//告警数量 -- 告警信息 1:高压断网,2:电源断电,3:短路,4:有人触网
	//QString alarms;		//告警内容
};
