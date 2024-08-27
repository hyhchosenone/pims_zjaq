#ifndef PIMS_H
#define PIMS_H

#include "ui_pims.h"
#include <QtWidgets/QDialog>
#include <QThread>
#include "alarmwindows.h"
#include "datawindows.h"
#include "frmconfig.h"


class PIMS : public QDialog
{
    Q_OBJECT

public:
    PIMS(QWidget *parent = 0);
    ~PIMS();

    AlarmWindows *alarmWindws = nullptr;

private slots:
    void button_clicked();
    void startAPIServerThread();
    void startNetAlarmServerThread();
    void startTcpClient();
    void delayStart();

private:
    void InitStyle();   //初始化无边框窗体
    void InitForm();    //初始化窗体数据
    void InitDb(); //初始化数据库

    Ui::VMSClass ui;
    QSqlDatabase db;
    DataWindows *dataWindows = Q_NULLPTR;
    frmConfig *configWindows = nullptr;
    QThread *dataThread = nullptr;
    QThread *apiThread = nullptr;
    QThread *alarmServerThread = nullptr;
};

#endif // VMS_H
