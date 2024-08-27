#pragma once

#include "rtsplayer.h"
#include "ui_defencedialog.h"
#include "vibsensor.h"
#include <defence.h>
#include <QDialog>

class DefenceDialog : public QDialog
{
    Q_OBJECT

public:
    DefenceDialog(QWidget *parent = Q_NULLPTR);
    ~DefenceDialog();

    int getDeviceId();
    void setDeviceId(int id);
    QString getHostAddress();
    void setHostAddress(QString hostAddr);
    QString getAddress();
    void setAddress(QString address);
    QString getName();
    void setName(QString name);
    int getType();
    void setType(int type);
    void setModifyMode(bool modifyFlag);
    float getSens();
    void setSens(float sens);
    int getAlarmTime();
    void setAlarmTime(int time);
    int getAlarmFreq();
    void setAlarmFreq(int freq);
    QList<VibSensor> getDeviceList();
    void setDeviceList(QList<VibSensor> vsList);
    void setDefence(DefenceZone defence);
    QString getRtspAddress();
    void setRtspAddress(QString rtsp);

private slots:
    void on_saveButton_clicked();
    void on_cancelButton_clicked();
    void on_testRtsp_clicked();

signals:
    void sendData(QString id, QString name);

private:
    Ui::defencedialog ui;
    DefenceZone defence;
    RTSPlayer *player = nullptr;
    int deviceId;
};
