#include "stdafx.h"
#include "hvdata.h"

HVData::HVData(QObject *parent)
	: QObject(parent)
{
}

HVData::HVData(const HVData & hvdata)
{
	this->chn = hvdata.chn;
	//this->hua = hvdata.hua;
	//this->hub = hvdata.hub;
	this->huab = hvdata.huab;
	//this->hia = hvdata.hia;
	//this->hib = hvdata.hib;
	this->hiab = hvdata.hiab;
	/*
	this->lua = hvdata.lua;
	this->lub = hvdata.lub;
	this->lia = hvdata.lia;
	this->lib = hvdata.lib;
	this->tempra = hvdata.tempra;
	this->wetratio = hvdata.wetratio;
	this->iu = hvdata.iu;
	this->ii = hvdata.ii;
	this->oia = hvdata.oia;
	this->oib = hvdata.oib;
	*/
	this->alarm_num = hvdata.alarm_num;
	//this->alarms = hvdata.alarms;
}

HVData::~HVData()
{
}

QDataStream &operator<<(QDataStream &out, const HVData &hvdata)
{
	/*
	out << hvdata.chn << hvdata.hua << hvdata.hub << hvdata.huab << hvdata.hia << hvdata.hib << hvdata.hiab << hvdata.lua << hvdata.lub << hvdata.lia << hvdata.lib << hvdata.tempra << hvdata.wetratio << hvdata.iu << hvdata.ii << hvdata.oia << hvdata.oib << hvdata.alarm_num << hvdata.alarms;
	*/

	out << hvdata.chn << hvdata.huab << hvdata.hiab << hvdata.alarm_num;

	return out;
}

QDataStream &operator>>(QDataStream &in, HVData &hvdata)
{
	/*
	in >> hvdata.chn >> hvdata.hua >> hvdata.hub >> hvdata.huab >> hvdata.hia >> hvdata.hib >> hvdata.hiab >> hvdata.lua >> hvdata.lub >> hvdata.lia >> hvdata.lib >> hvdata.tempra >> hvdata.wetratio >> hvdata.iu >> hvdata.ii >> hvdata.oia >> hvdata.oib >> hvdata.alarm_num >> hvdata.alarms;
	*/

	in >> hvdata.chn >> hvdata.huab >> hvdata.hiab >> hvdata.alarm_num;

	return in;
}
