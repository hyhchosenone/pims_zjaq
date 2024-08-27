#pragma once

#include "alarmscence.h"
#include "defence.h"
//#include "rtsplayer.h"
#include "ui_alarmwindows.h"
#include "vibsensor.h"
#include "vibsensorworker.h"
#include <QGraphicsScene>
#include <QPrinter>
#include <QSqlDatabase>
#include <QStandardItemModel>
#include <QTextToSpeech>
#include <QThread>
#include <QWidget>

class AlarmWindows : public QWidget
{
    Q_OBJECT

public:
    AlarmWindows(QWidget *parent = Q_NULLPTR);
    ~AlarmWindows();

    typedef QList<quint8> AddrList;
    //    QList<quint8> getAddrList();

public slots:
    void updateData(QByteArray data);
    //    void reciveData(QByteArray data);
    void updateStatus(QString address, quint8 sensorAddr, qint8 status);

signals:
    void requestCmd(QByteArray data);
    void notify(QByteArray data);
    void startWork();
    void stopWork(bool state);
    void updateWorkAddrList(QList<quint8> addrList);
    void updateNetAddrList(QHash<QString, QList<quint8>> newAddrList);
    void distribDefenceInfo(QString address, DefenceZone defence);
    void distribRemoteCmd(QString addr, int cmd);

private slots:
    void startWorkerThread();
    void analyseData();
    void showMapMenu(const QPoint &pos);
    void showTreeMenu(const QPoint &pos);
    void on_deviceTree_itemChanged(QTreeWidgetItem* item, int column); //修改分组
    void addDefence(); //添加防区
    void modifyDefence(); //修改防区
    void deleteDefence(); //删除防区
    void updateAddrList(); //更新传感器地址列表
    void addAlarmItem();
    void showAlarmItem(); //显示报警点位
    void deleteAlarmItem();
    void modifyAlarmItem();
    void saveMap(); //保存地图
    void delayLoad(); //延迟加载
    void on_searchAlarmButton_clicked(); //查询报警记录
    void on_exportButton_clicked(); //导出报警记录
    void on_printButton_clicked(); //打印报警记录
    void paintDocument(QPagedPaintDevice *device);
    void printPrivew(QPrinter* printer);
    void arming(); //布防
    void disArming(); //撤防
    void clearArming(); //消警

private:
    void initMenu();
    void initDb();
    void initData();
    void initList();
    void addDefenceItem(DefenceZone defence);
    bool ping(const QString ip);

    QList<VibSensor> loadDeviceList(int defenceId);

    Ui::AlarmWindows ui;

    QTimer *listenTimer;
    QThread *workerThread; //工作线程
    QThread *alarmServerThread;
    VibSensorWorker *vsWorker;
    QHash<int, VibSensor> sensorStates;

    QHash<int, QString> armTable;           //布防状态定义
    QHash<int, Qt::GlobalColor> armColor;   //布防颜色定义
    QHash<int, QString> alarmTable;         //报警状态定义
    QHash<int, Qt::GlobalColor> alarmColor; //报警颜色定义
    QHash<int, QString> onlineTable;        //在线状态定义
    QHash<int, Qt::GlobalColor> onlineColor; //在线颜色定义
    QHash<int, int> alarmStatus; //报警状态

//    QHash<int, QString> groups; //分组列表
    QHash<int, QTreeWidgetItem *> groupItems;          //顶层分组节点列表
    QHash<int, QTreeWidgetItem *> defenceItems;        //防区节点列表
    QHash<QString, DefenceZone> addrDefences;          //防区节点列表,key=address
    QHash<QString, QTreeWidgetItem*> defenceNameItems; //防区节点列表,key=防区名称
    QHash<QString, QTreeWidgetItem *> defenceIpItems;  //防区节点列表，key=ip
    QList<quint8> addrList;
    QHash<QString, QList<quint8>> netAddrList; //网络防区主机地址
    QHash<QString, int> netDeviceSens;         //网络防区主机灵敏度

    QMenu *addGroupMenu;
    QMenu *modifyGroupMenu;

    QMenu *armingMenu; //报警菜单
    QMenu *alarmMenu = Q_NULLPTR; //地图菜单
    QAction* addAlarmAction = Q_NULLPTR;
    QAction* showAlarmAction =  Q_NULLPTR;
    QAction* saveAction = Q_NULLPTR;

    QMenu *itemMenu; //报警点菜单
    QAction* deleteAlarmAction = Q_NULLPTR;
    QAction *modifyAlarmAction = Q_NULLPTR;
    QGraphicsItem *currentItem = Q_NULLPTR;

    AlarmScence *alarmScene = Q_NULLPTR;

    QSqlDatabase db;
    QStandardItemModel *alarm_model;
    int alarmIndex = 0;

    QTextToSpeech *tts;
//    RTSPlayer *player;
};
