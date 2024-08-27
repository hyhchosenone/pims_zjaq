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
	qint8  chn;    //�������: 0~15
	//qint16 hua;		//��ѹ��ѹA��(V) -- ��ѹ��״̬
	//qint16 hub;		//��ѹ��ѹB��(V)
	qint16 huab;		//��ѹ��ѹAB(V)
	//qint16 hia;		//��ѹ����A��(mA)
	//qint16 hib;		//��ѹ����B��(mA)
	qint16 hiab;		//��ѹ����AB(mA)
	/*
	qint16 lua;		//��ѹ�����ѹA��(V)
	qint16 lub;		//��ѹ�����ѹB��(V)
	qint16 lia;		//��ѹ�������A��(A)
	qint16 lib;		//��ѹ�������B��(A)
	qint8  tempra;		//�¶�(���϶�)
	qint8  wetratio;	//ʪ��(%)
	qint16 iu;			//��ѹ�����ѹ(V) --��ѹ��״̬
	qint16 ii;			//��ѹ�������(A)
	qint16 oia;		//��ѹ�ֵ���A(A)
	qint16 oib;		//��ѹ�ֵ���B(A)
	*/
	qint8  alarm_num;	//�澯���� -- �澯��Ϣ 1:��ѹ����,2:��Դ�ϵ�,3:��·,4:���˴���
	//QString alarms;		//�澯����
};
