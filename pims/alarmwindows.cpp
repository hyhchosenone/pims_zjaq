#include "alarmwindows.h"
#include "alarmpixmapitem.h"
#include "app.h"
#include "defencedialog.h"
#include "gohelper.h"
#include "gopdf.h"
#include "myhelper.h"
#include "netalarmserver.h"
#include <locale>
#include <QDebug>
#include <QFileDialog>
#include <QMenu>
#include <QNetworkConfigurationManager>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QProcess>
#include <QSettings>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTimer>
#include <qmessagebox.h>

AlarmWindows::AlarmWindows(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    qRegisterMetaType<QList<quint8>>("QList<quint8>");

    //1.配置文件
    QString fileName = QString("%1/config/%2_Config.ini").arg(App::AppPath).arg(App::AppName);
    QSettings config(fileName, QSettings::IniFormat);

    config.beginGroup("Map");
    QString mapFile = config.value("mapFile").toString();
    config.endGroup();

    if(mapFile.length() == 0)
    {
        mapFile = "://Resources/map/demo_1.jpg";
    }

    //2.状态定义
    armTable.insert(0, u8"撤防");
    armTable.insert(1, u8"布防");
    armColor.insert(0, Qt::yellow);
    armColor.insert(1, Qt::green);

    alarmTable.insert(0, u8"正常");
    alarmTable.insert(1, u8"报警");
    alarmColor.insert(0, Qt::green);
    alarmColor.insert(1, Qt::red);

    onlineTable.insert(0, u8"离线");
    onlineTable.insert(1, u8"在线");
    onlineColor.insert(0, Qt::yellow);
    onlineColor.insert(1, Qt::green);

    //3.地图设置
    ui.alarmView->setAlignment(Qt::AlignLeft | Qt::AlignTop); //设置对齐左上角
    ui.alarmView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui.alarmView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    alarmScene = new AlarmScence(this);
    alarmScene->setBackgroudImage(new QPixmap(mapFile));
    ui.alarmView->setScene(alarmScene);
    ui.alarmView->show();

    initDb();
    initData();
    initMenu();
    initList();

    //语音合成引擎
    tts = new QTextToSpeech(this);
    tts->setLocale(QLocale::Chinese);//设置语言环境
    //tts->setRate(1.0);//设置语速-1.0到1.0
    //tts->setPitch(1.0);//设置音高-1.0到1.0
    //tts->setVolume(1.0);//设置音量0.0-1.0

    //3.联动语音报警
    if (tts->state() == QTextToSpeech::Ready) {
        QString alarmStr = "请注意报警!";
        tts->say(alarmStr);
        tts->say(alarmStr);
        tts->say("alarmString");
    }

    //视频窗口
//    player = new RTSPlayer(this);
    //            player->setWindowFlags(player->windowFlags() | Qt::Dialog);

    QTimer::singleShot(1000, this, SLOT(delayLoad()));

    listenTimer = new QTimer(this);
    listenTimer->setInterval(3000);
    connect(listenTimer, &QTimer::timeout, this, &AlarmWindows::analyseData);
//    listenTimer->start();
}

AlarmWindows::~AlarmWindows()
{
    //    vsWorker->setWorkState(false);
    //    vsWorker->setRuning(false);
    //    workerThread->quit();
    //    workerThread->wait();
    //    delete  workerThread;

    listenTimer->stop();
    delete listenTimer;

    QSqlDatabase::removeDatabase("pims_alarmWin");

    delete armingMenu; //列表菜单
    delete alarmMenu;
    delete itemMenu; //报警点菜单

    delete alarmScene;
    delete tts;

//    if (player != nullptr) {
//        delete player;
//    }
}
/*
QList<quint8> AlarmWindows::getAddrList()
{
    return this->addrList;
}
*/
void AlarmWindows::updateData(QByteArray data)
{
    quint8 addr = data[0];

    if(sensorStates.contains(addr)) {
        VibSensor vs = sensorStates[addr];

        if(data.length() != 10) {
            vs.state = 2; //0:离线, 1:在线, 2:故障
        }else {
            vs.alarmState = data[3];
        }

        sensorStates[addr] = vs;
    }

    qDebug() << addr <<  " updateData : " << data.toHex();
}

/*
void AlarmWindows::reciveData(QByteArray data)
{
    qDebug() << "reciveData : " << data.toHex();
    QByteArray nameData = data.mid(3, data.size() - 5);
    QString dfName = QString::fromLocal8Bit(nameData);
    quint8 dfState = data[2];

    qDebug() << nameData.toHex() << "defence name " << dfName;

    QTreeWidgetItem *item = defenceNameItems.value(dfName);

    if ((item)->parent() != nullptr) { //防区节点
        QVariant v = (item)->data(0, Qt::UserRole);
        DefenceZone defence;

        if(v.canConvert<DefenceZone>()) {
            defence = v.value<DefenceZone>();
        }

        //判断报警
        if(dfState == 1) {
            defence.alarmState = 1;

            //0.通知APIServer
            QByteArray alarmData;
            alarmData.append(0xff);
            alarmData.append(0xff);
            alarmData.append(0x01);//报警
            alarmData.append(defence.name.toLocal8Bit());
            alarmData.append(0xee);
            alarmData.append(0xee);
            emit notify(alarmData);

            //1.添加报警记录
            QSqlQuery saveAlarm(db);
            saveAlarm.prepare("INSERT INTO alarm_info ( sub_system, defence_name, alarm_status, alarm_datetime) VALUES( :sub_system, :defence_name, :alarm_status, :alarm_datetime)");

            alarm_model->setItem(alarmIndex, 0, new QStandardItem(u8"震动报警")); //类型
            alarm_model->item(alarmIndex, 0)->setTextAlignment(Qt::AlignCenter);
            saveAlarm.bindValue(":sub_system", u8"震动报警");

            alarm_model->setItem(alarmIndex, 1, new QStandardItem(defence.name)); //防区
            alarm_model->item(alarmIndex, 1)->setTextAlignment(Qt::AlignCenter);
            saveAlarm.bindValue(":defence_name", defence.name);

            alarm_model->setItem(alarmIndex, 2, new QStandardItem(u8"报警")); //报警状态
            alarm_model->item(alarmIndex, 2)->setTextAlignment(Qt::AlignCenter);
            alarm_model->item(alarmIndex, 2)->setForeground(QBrush(QColor(255, 0, 0)));
            saveAlarm.bindValue(":alarm_status", u8"报警");

            QString alarmDateTimeStr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            alarm_model->setItem(alarmIndex, 3, new QStandardItem(alarmDateTimeStr)); //报警时间
            saveAlarm.bindValue(":alarm_datetime", alarmDateTimeStr);

            saveAlarm.exec();
            alarmIndex++;

            //2.联动地图报警
            for (QGraphicsItem* item : alarmScene->items())
            {
                if (item->data(0).toInt() == defence.id)
                {
                    item->setVisible(true);
                }
            }

            //3.联动语音报警
            if (tts->state() == QTextToSpeech::Ready)
            {
                QString alarmStr = "请注意" + defence.name + "报警!";
                tts->say(alarmStr);
                tts->say(alarmStr);
//                tts->say(alarmStr);
            }
        }else {
            defence.alarmState = 0;

            //2.联动地图报警
            for (QGraphicsItem* item : alarmScene->items())
            {
                if (item->data(0).toInt() == defence.id)
                {
                    item->setVisible(false);
                }
            }
        }

        //2.显示报警
        item->setText(1, armTable.value(1)); //布防状态
        item->setData(1, Qt::UserRole, 1);
        item->setBackground(1, armColor.value(1));

        item->setText(2, alarmTable.value(defence.alarmState));//报警状态
        item->setBackground(2, alarmColor.value(defence.alarmState));
    }
}
*/

void AlarmWindows::updateStatus(QString address, quint8 sensorAddr, qint8 status)
{
    QTreeWidgetItem *item = defenceIpItems.value(address);

    QVariant v = (item)->data(0, Qt::UserRole);
    DefenceZone defence;

    if (v.canConvert<DefenceZone>()) {
        defence = v.value<DefenceZone>();
    }

    if (defence.armingState == 0) {
        return;
    }

    defence.alarmState = 1;

    switch (status) {
    case 1: //报警信息
    {
        //0.通知APIServer
        QByteArray alarmData;
        alarmData.append(0xff);
        alarmData.append(0xff);
        alarmData.append(0x01); //报警
        alarmData.append(defence.name.toLocal8Bit());
        alarmData.append(0xee);
        alarmData.append(0xee);
        emit notify(alarmData);

        //1.添加报警记录
        QSqlQuery saveAlarm(db);
        saveAlarm.prepare(
            "INSERT INTO alarm_info ( sub_system, defence_name, alarm_status, alarm_datetime) "
            "VALUES( :sub_system, :defence_name, :alarm_status, :alarm_datetime)");

        alarm_model->setItem(alarmIndex, 0, new QStandardItem(u8"震动报警")); //类型
        alarm_model->item(alarmIndex, 0)->setTextAlignment(Qt::AlignCenter);
        saveAlarm.bindValue(":sub_system", u8"震动报警");

        alarm_model->setItem(alarmIndex, 1, new QStandardItem(defence.name)); //防区
        alarm_model->item(alarmIndex, 1)->setTextAlignment(Qt::AlignCenter);
        saveAlarm.bindValue(":defence_name", defence.name);

        //        alarm_model->setItem(alarmIndex, 2,
        //                             new QStandardItem(u8"防区报警")); //报警状态
        alarm_model->setItem(alarmIndex,
                             2,
                             new QStandardItem(u8"防区报警，探头 : "
                                               + QString::number(sensorAddr))); //报警状态
        alarm_model->item(alarmIndex, 2)->setTextAlignment(Qt::AlignCenter);
        alarm_model->item(alarmIndex, 2)->setForeground(QBrush(QColor(255, 0, 0)));
        //        saveAlarm.bindValue(":alarm_status", u8"防区报警");
        saveAlarm.bindValue(":alarm_status", u8"防区报警，探头 : " + QString::number(sensorAddr));

        QString alarmDateTimeStr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        alarm_model->setItem(alarmIndex, 3, new QStandardItem(alarmDateTimeStr)); //报警时间
        saveAlarm.bindValue(":alarm_datetime", alarmDateTimeStr);

        saveAlarm.exec();
        alarmIndex++;

        //2.联动地图报警
        for (QGraphicsItem *item : alarmScene->items()) {
            if (item->data(0).toInt() == defence.id) {
                item->setVisible(true);
            }
        }

        //3.联动语音报警
        if (tts->state() == QTextToSpeech::Ready) {
            QString alarmStr = "请注意" + defence.name + "报警!";
            tts->say(alarmStr);
            tts->say(alarmStr);
            //                tts->say(alarmStr);
        }

        //4.弹出视频
//        if (player != nullptr) {
//            player->play(defence.rtspAddress);
//            player->show();
//        }

        //5.显示报警
        item->setText(1, armTable.value(1)); //布防状态
        item->setData(1, Qt::UserRole, 1);
        item->setBackground(1, armColor.value(1));

        item->setText(2, alarmTable.value(defence.alarmState)); //报警状态
        item->setBackground(2, alarmColor.value(defence.alarmState));

        break;
    }

    case 7: //连接上线

        item->setText(3, onlineTable.value(1)); //更新为在线状态
        item->setBackground(3, onlineColor.value(1));

        emit distribDefenceInfo(address, addrDefences[address]);
        emit distribDefenceInfo(address, addrDefences[address]);
        break;

    case 8: //主机下线

        item->setText(3, onlineTable.value(0)); //更新为离线状态
        item->setBackground(3, onlineColor.value(0));

        //2.联动地图报警
        for (QGraphicsItem *item : alarmScene->items()) {
            if (item->data(0).toInt() == defence.id) {
                item->setVisible(true);
            }
        }

        //3.联动语音报警
        if (tts->state() == QTextToSpeech::Ready) {
            QString alarmStr = "请注意" + defence.name + "离线!";
            tts->say(alarmStr);
            tts->say(alarmStr);
            //                tts->say(alarmStr);
        }

        break;
    }
}

void AlarmWindows::startWorkerThread()
{
    //    alarmServerThread = new QThread();
    //    NetAlarmServer *netAlarmSrv = new NetAlarmServer();
    //    netAlarmSrv->moveToThread(alarmServerThread);
    //    netAlarmSrv->setAddrList(this->netAddrList);
    //    netAlarmSrv->setDeviceSens(this->netDeviceSens);

    //    connect(alarmServerThread, &QThread::finished, netAlarmSrv, &QObject::deleteLater);
    //    connect(alarmServerThread, &QThread::started, netAlarmSrv, &NetAlarmServer::doWork);
    //    connect(this, &AlarmWindows::updateNetAddrList, netAlarmSrv, &NetAlarmServer::setAddrList);
    //    connect(netAlarmSrv, &NetAlarmServer::uploadInfo, this, &AlarmWindows::updateStatus);

    //    alarmServerThread->start();

    //    workerThread = new QThread();
    //    vsWorker = new VibSensorWorker();
    //    vsWorker->setAddrList(this->addrList);
    //    vsWorker->moveToThread(workerThread);

    //    connect(workerThread, &QThread::finished, vsWorker, &QObject::deleteLater);
    //    connect(workerThread, &QThread::started, vsWorker, &VibSensorWorker::doWork);
    //    connect(this, &AlarmWindows::startWork, vsWorker, &VibSensorWorker::doWork);
    //    connect(this, &AlarmWindows::stopWork, vsWorker, &VibSensorWorker::setWorkState);
    //    connect(this, &AlarmWindows::updateWorkAddrList, vsWorker, &VibSensorWorker::setAddrList);
    //    connect(vsWorker, &VibSensorWorker::uploadData, this, &AlarmWindows::updateData);

    //    workerThread->start();
}

/*
 * Brief 检测当前网络是否能ping通某段IP地址
 * Param ip (QString) IP地址(类型：QString)
 * Return bool 是否能ping通传入的IP地址
 */

bool AlarmWindows::ping(const QString ip)
{
    // #Linux指令 "ping -s 1 -c 1 IP"
    //QString cmdstr = QString("ping -s 1 -c 1 %1")
    //		.arg(ip);

    // #Windows指令 "ping IP -n 1 -w 超时(ms)"
    QString cmdstr = QString("ping %1 -n 1 -w %2").arg(ip).arg(1000);

    QProcess cmd;
    cmd.start(cmdstr);
    cmd.waitForReadyRead(1000);
    cmd.waitForFinished(1000);

    QString response = cmd.readAll();
    if (response.indexOf("TTL") == -1) {
        return false;
    } else {
        return true;
    }
}

void AlarmWindows::analyseData()
{
    QNetworkConfigurationManager mgr;

    //0.检测防区主机
    for (DefenceZone defence : addrDefences.values()) {
        if (!ping(defence.address)) {
            //1.添加报警记录
            alarm_model->setItem(alarmIndex, 0, new QStandardItem(u8"网络报警")); //类型
            alarm_model->item(alarmIndex, 0)->setTextAlignment(Qt::AlignCenter);

            alarm_model->setItem(alarmIndex, 1, new QStandardItem(u8"网络连接")); //防区
            alarm_model->item(alarmIndex, 1)->setTextAlignment(Qt::AlignCenter);

            alarm_model->setItem(alarmIndex,
                                 2,
                                 new QStandardItem(defence.name + u8"网络断开")); //报警状态

            alarm_model->item(alarmIndex, 2)->setTextAlignment(Qt::AlignCenter);
            alarm_model->item(alarmIndex, 2)->setForeground(QBrush(QColor(255, 0, 0)));

            QString alarmDateTimeStr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            alarm_model->setItem(alarmIndex, 3, new QStandardItem(alarmDateTimeStr)); //报警时间

            alarmIndex++;

            //2.联动地图
            for (QGraphicsItem *item : alarmScene->items()) {
                if (item->data(0).toInt() == defence.id) {
                    item->setVisible(true);
                }
            }

            //3.联动语音报警
            if (tts->state() == QTextToSpeech::Ready) {
                QString alarmStr = "请注意防区网络断开!";
                tts->say(alarmStr);
                tts->say(alarmStr);
                tts->say(alarmStr);
            }
        }
    }

    if (!mgr.isOnline()) {
        //1.添加报警记录
        alarm_model->setItem(alarmIndex, 0, new QStandardItem(u8"网络报警")); //类型
        alarm_model->item(alarmIndex, 0)->setTextAlignment(Qt::AlignCenter);

        alarm_model->setItem(alarmIndex, 1, new QStandardItem(u8"网络连接")); //防区
        alarm_model->item(alarmIndex, 1)->setTextAlignment(Qt::AlignCenter);

        alarm_model->setItem(alarmIndex, 2,
                             new QStandardItem(u8"网络断开")); //报警状态

        alarm_model->item(alarmIndex, 2)->setTextAlignment(Qt::AlignCenter);
        alarm_model->item(alarmIndex, 2)->setForeground(QBrush(QColor(255, 0, 0)));

        QString alarmDateTimeStr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        alarm_model->setItem(alarmIndex, 3, new QStandardItem(alarmDateTimeStr)); //报警时间

        alarmIndex++;

        //2.联动地图
        for (QGraphicsItem *item : alarmScene->items()) {
            item->setVisible(true);
            //            break;
        }

        //3.联动语音报警
        if (tts->state() == QTextToSpeech::Ready) {
            QString alarmStr = "请注意主机网络断开!";
            tts->say(alarmStr);
            tts->say(alarmStr);
            tts->say(alarmStr);
        }
    }
}

/*
void AlarmWindows::analyseData()
{
    QTreeWidgetItemIterator it(ui.deviceTree);

    while (*it) {
        if ((*it)->parent() != nullptr) { //防区节点
            QVariant v = (*it)->data(0, Qt::UserRole);
            DefenceZone defence;

            if(v.canConvert<DefenceZone>()) {
                defence = v.value<DefenceZone>();
            }

            if(defence.armingState == 0) {
                int alarmSensor = 0;

                foreach (VibSensor vs, defence.vsList) {
                    //                    VibSensor vs = sensorStates[addr];

                    if (vs.alarmState == 1) { //报警状态, 0:正常, 1:报警
                        alarmSensor++;
                    }
                }

                //判断报警
                QTreeWidgetItem *item = defenceItems[defence.id];

                if(alarmSensor > 1) {
                    defence.alarmState = 1;

                    //0.通知APIServer
                    QByteArray alarmData;
                    alarmData.append(0xff);
                    alarmData.append(0xff);
                    alarmData.append(0x01);//报警
                    alarmData.append(defence.name.toLocal8Bit());
                    alarmData.append(0xee);
                    alarmData.append(0xee);
                    emit notify(alarmData);

                    //1.添加报警记录
                    QSqlQuery saveAlarm(db);
                    saveAlarm.prepare("INSERT INTO alarm_info ( sub_system, defence_name, alarm_status, alarm_datetime) VALUES( :sub_system, :defence_name, :alarm_status, :alarm_datetime)");

                    alarm_model->setItem(alarmIndex, 0, new QStandardItem(u8"震动报警")); //类型
                    alarm_model->item(alarmIndex, 0)->setTextAlignment(Qt::AlignCenter);
                    saveAlarm.bindValue(":sub_system", u8"震动报警");

                    alarm_model->setItem(alarmIndex, 1, new QStandardItem(defence.name)); //防区
                    alarm_model->item(alarmIndex, 1)->setTextAlignment(Qt::AlignCenter);
                    saveAlarm.bindValue(":defence_name", defence.name);

                    alarm_model->setItem(alarmIndex, 2, new QStandardItem(u8"报警")); //报警状态
                    alarm_model->item(alarmIndex, 2)->setTextAlignment(Qt::AlignCenter);
                    alarm_model->item(alarmIndex, 2)->setForeground(QBrush(QColor(255, 0, 0)));
                    saveAlarm.bindValue(":alarm_status", u8"报警");

                    QString alarmDateTimeStr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
                    alarm_model->setItem(alarmIndex, 3, new QStandardItem(alarmDateTimeStr)); //报警时间
                    saveAlarm.bindValue(":alarm_datetime", alarmDateTimeStr);

                    saveAlarm.exec();
                    alarmIndex++;

                    //2.联动地图报警
                    for (QGraphicsItem* item : alarmScene->items())
                    {
                        if (item->data(0).toInt() == defence.id)
                        {
                            item->setVisible(true);
                        }
                    }

                    //3.联动语音报警
                    if (tts->state() == QTextToSpeech::Ready)
                    {
                        QString alarmStr = "请注意" + defence.name + "报警!";
                        tts->say(alarmStr);
                        tts->say(alarmStr);
                        tts->say(alarmStr);
                    }
                }else {
                    defence.alarmState = 0;
                }

                //2.显示报警
                item->setText(1, armTable.value(1)); //布防状态
                item->setData(1, Qt::UserRole, 1);
                item->setBackground(1, armColor.value(1));

                item->setText(2, alarmTable.value(defence.alarmState));//报警状态
                item->setBackground(2, alarmColor.value(defence.alarmState));
            }
        }
        ++it;
    }
}

*/

void AlarmWindows::showMapMenu(const QPoint &pos)
{
    currentItem = ui.alarmView->itemAt(pos);
    if (currentItem) {
        itemMenu->exec(QCursor::pos());
    } else {
        alarmMenu->exec(QCursor::pos());
    }
}

void AlarmWindows::showTreeMenu(const QPoint& pos)
{
    //获取当前被点击的节点
    QTreeWidgetItem *item = ui.deviceTree->itemAt(pos);

    if (item == nullptr) //空白处点击
    {
        addGroupMenu->exec(QCursor::pos());
    }
    else if(item->isSelected() && item->parent() == nullptr) //分组点击
    {
        modifyGroupMenu->exec(QCursor::pos());
    }else if(item->isSelected()) {
        armingMenu->exec(QCursor::pos());
    }

}

void AlarmWindows::on_deviceTree_itemChanged(QTreeWidgetItem *item, int column)
{
    if (item->parent() == nullptr) //根节点,表示分组
    {
        int groupId = item->data(0, Qt::UserRole).toInt();

        QSqlQuery query(db);
        query.prepare("UPDATE defence_group SET name = :name WHERE id = :id");

        query.bindValue(":id", groupId);
        query.bindValue(":name", item->text(0));

        if(query.exec())
        {
            //            groups.insert(id, item->text(0));
        }else
        {
            //            GoGuiMessageBox::showWarningBox(this, u8"失败"), u8"修改分组失败，检查是否有重名分组。"));
        }

        query.finish();

        qDebug() << "### update group is : " << query.lastError();
    }
}

void AlarmWindows::addDefence()
{
    DefenceDialog dialog(this);

    if (QDialog::Accepted == dialog.exec())
    {
        //1.取值
        QString name = dialog.getName();
        int type = dialog.getType();
        QString ip = dialog.getHostAddress();
        float sens = dialog.getSens();
        int alarmTime = dialog.getAlarmTime();
        int alarmFreq = dialog.getAlarmFreq();
        QString rtsp = dialog.getRtspAddress();
        int groupId = ui.deviceTree->currentItem()->data(0, Qt::UserRole).toInt();

        //2.保存数据库
        QSqlQuery addDefenceQuery(db);
        addDefenceQuery.prepare(
            "INSERT INTO defence (name, type, sens, ip, alarm_time, rtspAddress, group_id) VALUES "
            "(:name, :type, :sens, :ip, :alarm_time, :rtsp, :group_id)");

        addDefenceQuery.bindValue(":name", name);
        addDefenceQuery.bindValue(":type", type);
        addDefenceQuery.bindValue(":sens", sens);
        addDefenceQuery.bindValue(":ip", ip);
        addDefenceQuery.bindValue(":alarm_time", alarmTime);
        addDefenceQuery.bindValue(":rtsp", rtsp);
        addDefenceQuery.bindValue(":group_id", groupId);

        if(!addDefenceQuery.exec()) {
            qDebug() << " DB: error is " << addDefenceQuery.lastQuery()
                     << addDefenceQuery.lastError();
            return;
        }

        int defenceId = addDefenceQuery.lastInsertId().toInt();

        if(defenceId > 0) {
            DefenceZone defence;
            defence.id = defenceId;
            defence.name = name;
            defence.type = type;
            defence.sens = sens;
            defence.address = ip;
            defence.armingState = 0;
            defence.alarmTime = alarmTime;
            defence.alarmFreq = alarmFreq;
            defence.groupId = groupId;

            //3. 保存传感器列表到数据库
            QSqlQuery addDeviceQuery(db);
            addDeviceQuery.prepare(
                "INSERT INTO device (addr, sens, freq, alarm_status, arm_status, "
                "defence_id) VALUES (:addr, :sens, :freq, :alarm_status, :arm_status, "
                ":defence_id)");

            QList<VibSensor> vsList = dialog.getDeviceList();
            qDebug() << "VSList count " << vsList.count();

            for (int i = 0; i < vsList.count(); i++) {
                VibSensor vs = vsList[i];
                addDeviceQuery.bindValue(":addr", vs.addr);
                addDeviceQuery.bindValue(":sens", vs.sens);
                addDeviceQuery.bindValue(":freq", vs.freq);
                addDeviceQuery.bindValue(":alarm_status", vs.alarmState);
                addDeviceQuery.bindValue(":arm_status", vs.armingState);
                addDeviceQuery.bindValue(":defence_id", defence.id);

                if (!addDeviceQuery.exec()) {
                    qDebug() << "Insert " << vs.addr << addDeviceQuery.lastQuery()
                             << " DB: " << addDeviceQuery.lastError();
                    return;
                }

                int deviceId = addDeviceQuery.lastInsertId().toInt();

                vs.id = deviceId;
                vsList[i] = vs;
            }

            defence.vsList = vsList;

            //4. 添加防区Item
            addDefenceItem(defence);
        }

        updateAddrList();
    }
}

void AlarmWindows::modifyDefence()
{
    QVariant v = ui.deviceTree->currentItem()->data(0, Qt::UserRole);
    DefenceZone defence;

    if(v.canConvert<DefenceZone>()) {
        defence = v.value<DefenceZone>();
    }

    //初始化dialog
    DefenceDialog dialog(this);
    dialog.setDefence(defence);

    if (QDialog::Accepted == dialog.exec())
    {
        //1.取值
        QString name = dialog.getName();
        int type = dialog.getType();
        QString ip = dialog.getHostAddress();
        float sens = dialog.getSens();
        short alarmTime = dialog.getAlarmTime();
        short alarmFreq = dialog.getAlarmFreq();
        QString rtsp = dialog.getRtspAddress();

        //2.保存数据库
        QSqlQuery updateDefenceQuery(db);
        updateDefenceQuery.prepare("UPDATE defence SET name = :name, ip = :ip, type = :type, "
                                   "sens = :sens, alarm_time = :alarm_time, alarm_freq = "
                                   ":alarm_freq, rtspAddress = :rtsp WHERE id = :id");

        updateDefenceQuery.bindValue(":name", name);
        updateDefenceQuery.bindValue(":ip", ip);
        updateDefenceQuery.bindValue(":type", type);
        updateDefenceQuery.bindValue(":sens", sens);
        updateDefenceQuery.bindValue(":alarm_time", alarmTime);
        updateDefenceQuery.bindValue(":alarm_freq", alarmFreq);
        updateDefenceQuery.bindValue(":rtsp", rtsp);
        updateDefenceQuery.bindValue(":id", defence.id);

        if(!updateDefenceQuery.exec()) {
            return;
        }

        //3.更新treeitem
        if(name != defence.name) {
            ui.deviceTree->currentItem()->setText(0, name);
            defence.name = name;
        }

        defence.type = type;
        defence.address = ip;
        defence.sens = sens;
        defence.alarmTime = alarmTime;
        defence.alarmFreq = alarmFreq;
        defence.rtspAddress = rtsp;

        //4. 保存传感器列表到数据库
        //4.1 删除旧数据
        QSqlQuery deleteDevice(db);
        deleteDevice.prepare("DELETE FROM device WHERE defence_id = :defence_id");
        deleteDevice.bindValue(":defence_id", defence.id);

        if (deleteDevice.exec()) {
            QSqlQuery addDeviceQuery(db);
            addDeviceQuery.prepare(
                "INSERT INTO device (addr, sens, freq, alarm_status, arm_status, "
                "defence_id) VALUES (:addr, :sens, :freq, :alarm_status, :arm_status, "
                ":defence_id)");

            QList<VibSensor> vsList = dialog.getDeviceList();
            qDebug() << "VSList count " << vsList.count();

            for (int i = 0; i < vsList.count(); i++) {
                VibSensor vs = vsList[i];
                addDeviceQuery.bindValue(":addr", vs.addr);
                addDeviceQuery.bindValue(":sens", vs.sens);
                addDeviceQuery.bindValue(":freq", vs.freq);
                addDeviceQuery.bindValue(":alarm_status", vs.alarmState);
                addDeviceQuery.bindValue(":arm_status", vs.armingState);
                addDeviceQuery.bindValue(":defence_id", defence.id);

                if (!addDeviceQuery.exec()) {
                    qDebug() << "Insert " << vs.addr << addDeviceQuery.lastQuery()
                             << " DB: " << addDeviceQuery.lastError();
                    return;
                }
            }
        }

        defence.vsList.clear();
        defence.vsList = loadDeviceList(defence.id);

        ui.deviceTree->currentItem()->setData(0, Qt::UserRole, QVariant::fromValue(defence));
        addrDefences[defence.address] = defence;

        emit distribDefenceInfo(defence.address, defence);
        emit distribDefenceInfo(defence.address, defence);
    }
}

void AlarmWindows::deleteDefence()
{
    if (myHelper::ShowMessageBoxQuesion("确认要删除防区吗？") == QDialog::Rejected) {
        return;
    }

    QVariant v = ui.deviceTree->currentItem()->data(0, Qt::UserRole);
    DefenceZone defence;

    if(v.canConvert<DefenceZone>()) {
        defence = v.value<DefenceZone>();
    }

    QSqlQuery deleteDefence(db);
    deleteDefence.prepare("DELETE FROM defence WHERE id = :id");
    deleteDefence.bindValue(":id", defence.id);

    if(deleteDefence.exec())
    {
        ui.deviceTree->currentItem()->parent()->removeChild(ui.deviceTree->currentItem());

        updateAddrList();
    }
}

void AlarmWindows::updateAddrList()
{
    this->addrList.clear();
    this->sensorStates.clear();

    QTreeWidgetItemIterator it(ui.deviceTree);

    while (*it) {
        if ((*it)->parent() != nullptr) { //防区节点
            QVariant v = (*it)->data(0, Qt::UserRole);
            DefenceZone defence;

            if(v.canConvert<DefenceZone>()) {
                defence = v.value<DefenceZone>();
            }

            //            this->addrList.append(defence.addrList);
            //            this->netAddrList.insert(defence.ip, defence.addrList);
            this->netDeviceSens.insert(defence.address, defence.sens);

            //            foreach (VibSensor vs, defence.vsList) {
            //                sensorStates.insert(vs.addr, vs);
            //            }
        }

        ++it;
    }

    //更新work
    //    vsWorker->setWorkState(false);
    //    vsWorker->setAddrList(this->addrList);
    //    vsWorker->setWorkState(true);
}

void AlarmWindows::addAlarmItem()
{/*
    DefenceDialog dialog(this);

    QHashIterator<int, QString> i(groups);
    while (i.hasNext()) {
        i.next();
        dialog.addGorup(i.key(), i.value());
    }

    if (QDialog::Accepted == dialog.exec())
    {
        QPixmap pixmap(":/Resources/image/alarm/alarm_light.png");
        AlarmPixmapItem *alarmPixmapItem = new AlarmPixmapItem(pixmap);
        alarmPixmapItem->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);

        alarmPixmapItem->setData(1, dialog.getAddress());
        alarmPixmapItem->setData(2, dialog.getName());
        alarmPixmapItem->setData(3, dialog.getGroupId());

        alarmScene->addItem(alarmPixmapItem);
        alarmPixmapItem->setPos(QCursor::pos() - QPoint(0, pixmap.height()));

        QSqlQuery saveItem(db);
        saveItem.prepare("INSERT INTO device (address, name, map_x, map_y, group_id) "
                         "VALUES (:address, :name, :map_x, :map_y, :group_id)");

        saveItem.bindValue(":address", alarmPixmapItem->data(1));
        saveItem.bindValue(":name", alarmPixmapItem->data(2));
        saveItem.bindValue(":map_x", alarmPixmapItem->scenePos().x());
        saveItem.bindValue(":map_y", alarmPixmapItem->scenePos().y());
        saveItem.bindValue(":group_id", alarmPixmapItem->data(3));

        bool ok = saveItem.exec();

        if(ok) {
            int id = saveItem.lastInsertId().toInt();

            alarmPixmapItem->setData(0, id);
        }else {
            alarmScene->removeItem(alarmPixmapItem);
        }

        qDebug() << "### item " << alarmPixmapItem->scenePos() << " saved status is " << ok;
    }*/
}

void AlarmWindows::showAlarmItem()
{
    bool status = showAlarmAction->isChecked();

    for (QGraphicsItem* item : alarmScene->items())
    {
        item->setVisible(status);
    }
}

void AlarmWindows::deleteAlarmItem()
{
    //    if (Q_NULLPTR != currentItem)
    //    {
    //        QSqlQuery deleteItem(db);
    //        deleteItem.prepare("DELETE FROM device WHERE id = :id");
    //        deleteItem.bindValue(":id", currentItem->data(0));

    //        if(deleteItem.exec())
    //        {
    //            alarmScene->removeItem(currentItem);
    //            currentItem = Q_NULLPTR;
    //        }
    //    }
}

void AlarmWindows::modifyAlarmItem()
{/*
    if (Q_NULLPTR != currentItem)
    {
        DefenceDialog dialog(this);

        QHashIterator<int, QString> i(groups);
        while (i.hasNext()) {
            i.next();
            dialog.addGorup(i.key(), i.value());
        }

        dialog.setModifyMode(true);
        dialog.setDeviceId(currentItem->data(0).toInt());
        dialog.setAddress(currentItem->data(1).toString());
        dialog.setName(currentItem->data(2).toString());
        dialog.setGroup(groups.value(currentItem->data(3).toInt()));

        if (QDialog::Accepted == dialog.exec())
        {
            currentItem->setData(1, dialog.getAddress());
            currentItem->setData(2, dialog.getName());
        }
    }*/
}

void AlarmWindows::saveMap()
{
    QMessageBox box;
    box.setIcon(QMessageBox::Question);
    box.setWindowTitle(u8"保存地图");
    box.setText(u8"是否需要保存当前防区布局？");
    box.addButton(QMessageBox::Yes)->setText(u8"保存");
    box.addButton(QMessageBox::No)->setText(u8"取消");

    int ret = box.exec();

    QList<QGraphicsItem*> items = alarmScene->items();

    QSqlQuery updateItem(db);
    updateItem.prepare("UPDATE defence SET map_x = :map_x, map_y = :map_y WHERE id = :id");

    switch (ret)
    {
    case QMessageBox::Yes:

        for (QGraphicsItem* item : items)
        {
            QPointF location = item->scenePos();

            updateItem.bindValue(":id", item->data(0));
            updateItem.bindValue(":map_x", location.x());
            updateItem.bindValue(":map_y", location.y());

            bool result = updateItem.exec();

            qDebug() << "### item " << location << " update status is " << result;
        }

        updateItem.finish();

        showAlarmAction->setChecked(false);

        break;

    case QMessageBox::No:
        break;

    default:
        break;
    }
}

void AlarmWindows::delayLoad() {
    //设置与view相同
    alarmScene->setSceneRect(0, 0, ui.alarmView->size().width(), ui.alarmView->size().height());

    //    qDebug() << "### alarm scene rect is " << alarmScene->sceneRect();
    //    qDebug() << "### alarm view rect is " << ui.alarmView->rect();

    updateAddrList();

    return;
}

void AlarmWindows::on_searchAlarmButton_clicked()
{
    //清除现有记录
    alarm_model->removeRows(0, alarmIndex);
    alarmIndex = 0;

    //查询指定日期记录
    QSqlQuery searchAlarmInfo(db);
    searchAlarmInfo.prepare("SELECT sub_system, defence_name, alarm_status, alarm_datetime from alarm_info where alarm_datetime > :begin_datetime AND alarm_datetime < :end_datetime");

    searchAlarmInfo.bindValue(":begin_datetime", ui.beginDateEdit->date().toString("yyyy-MM-dd 00:00:00"));
    searchAlarmInfo.bindValue(":end_datetime", ui.endDateEdit->date().toString("yyyy-MM-dd 23:59:59"));

    if (searchAlarmInfo.exec()) {
        while (searchAlarmInfo.next())
        {
            alarm_model->setItem(alarmIndex, 0, new QStandardItem(searchAlarmInfo.value("sub_system").toString())); //类型
            alarm_model->item(alarmIndex, 0)->setTextAlignment(Qt::AlignCenter);

            alarm_model->setItem(alarmIndex, 1, new QStandardItem(searchAlarmInfo.value("defence_name").toString())); //防区
            alarm_model->item(alarmIndex, 1)->setTextAlignment(Qt::AlignCenter);

            alarm_model->setItem(alarmIndex, 2, new QStandardItem(searchAlarmInfo.value("alarm_status").toString())); //报警状态
            alarm_model->item(alarmIndex, 2)->setTextAlignment(Qt::AlignCenter);
            alarm_model->item(alarmIndex, 2)->setForeground(QBrush(QColor(255, 0, 0)));

            alarm_model->setItem(alarmIndex, 3, new QStandardItem(searchAlarmInfo.value("alarm_datetime").toString())); //报警时间

            alarmIndex++;

        }
    }
}

void AlarmWindows::on_exportButton_clicked()
{
    QString name = u8"报警记录_" + QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss") + ".pdf";
    QString filename = QFileDialog::getSaveFileName(this, u8"保存位置", name, "*.pdf");

    GoPDF pdf(filename);
    pdf.setCreator(u8"周界安全科技有限公司");
    pdf.setTitle(u8"报警记录报表");
    pdf.setPageSize(QPagedPaintDevice::A4);
    pdf.setResolution(300); //分辨率是300像素/英寸时,A4纸的尺寸的图像的像素是2480×3508;
    int margin = 15;
    pdf.setPageMargins(QMarginsF(margin, margin, margin, margin), QPageLayout::Millimeter);

    this->paintDocument(&pdf);
}

void AlarmWindows::on_printButton_clicked()
{
    //QPrinter printer;
    QPrintPreviewDialog previewDialog(this);

    connect(&previewDialog, &QPrintPreviewDialog::paintRequested, this, &AlarmWindows::printPrivew);

    previewDialog.showMaximized();
    previewDialog.exec();
}

void AlarmWindows::printPrivew(QPrinter *printer)
{
    printer->setPageSize(QPagedPaintDevice::A4);
    printer->setResolution(300); //分辨率是300像素/英寸时,A4纸的尺寸的图像的像素是2480×3508;
    int margin = 15;
    printer->setPageMargins(QMarginsF(margin, margin, margin, margin), QPageLayout::Millimeter);

    paintDocument(printer);
}

void AlarmWindows::paintDocument(QPagedPaintDevice *device)
{
    QPainter *painter = new QPainter(device);
    int pageW = device->width();
    int pageH = device->height();
    int penX = 0;
    int penY = 0;

    //绘制标题
    QFont titleFont("simhei.ttf", 18, QFont::Bold);
    //titleFont.setUnderline(true);
    painter->setFont(titleFont);
    QFontMetrics fm = painter->fontMetrics();

    QString title = u8"报警记录";
    painter->drawText(0, 0, pageW, fm.lineSpacing(), Qt::AlignCenter, title);
    penY += fm.lineSpacing() * 3;

    //绘制Header
    QFont textFont("simsun.ttf", 14, QFont::Normal);
    painter->setFont(textFont);
    QFontMetrics textFm = painter->fontMetrics();
    QRect colSpace = textFm.boundingRect(0, penY, pageW, textFm.lineSpacing(),
                                         Qt::AlignCenter, u8"空");
    QRect col1 = textFm.boundingRect(0, penY, pageW, textFm.lineSpacing(),
                                     Qt::AlignCenter, u8"序号");
    QRect col2 = textFm.boundingRect(0, penY, pageW, textFm.lineSpacing(),
                                     Qt::AlignCenter, u8"系统名称");
    QRect col3 = textFm.boundingRect(0, penY, pageW, textFm.lineSpacing(),
                                     Qt::AlignCenter, u8"防区名称");
    QRect col4 = textFm.boundingRect(0, penY, pageW, textFm.lineSpacing(),
                                     Qt::AlignCenter, u8"报警状态");
    QRect col5 = textFm.boundingRect(0, penY, pageW, textFm.lineSpacing(),
                                     Qt::AlignCenter, u8"报警时间");

    penX = 0; //新行置零
    penX += colSpace.width();
    painter->drawText(penX, penY, u8"序号");
    penX += col1.width() + colSpace.width() * 2;

    painter->drawText(penX, penY, u8"系统名称");
    penX += col2.width() + colSpace.width() * 2;

    painter->drawText(penX, penY, u8"防区名称");
    penX += col3.width() + colSpace.width() * 1.5;

    painter->drawText(penX, penY, u8"报警状态");
    penX += col4.width() + colSpace.width() * 3;

    penX += (pageW - penX)/2 - col5.width()/2;
    painter->drawText(penX, penY, u8"报警时间");

    penY += textFm.lineSpacing();

    QPen pen = painter->pen();
    pen.setWidth(5);
    painter->setPen(pen);
    painter->drawLine(0, penY, pageW, penY);
    penY += textFm.lineSpacing() * 2;

    //绘制内容
    pen.setWidth(1);
    painter->setPen(pen);
    col5 = textFm.boundingRect(0, penY, pageW, textFm.lineSpacing(),
                               Qt::AlignCenter, u8"yyyy-MM-dd hh:mm:ss");

    for (int i = 0; i < this->alarm_model->rowCount(); i++) {
        penX = 0; //新行置零
        penX += colSpace.width();
        painter->drawText(penX, penY, QString::number(i));
        penX += col1.width() + colSpace.width() * 1;

        painter->drawText(penX, penY, alarm_model->item(i, 0)->text());
        penX += col2.width() + colSpace.width() * 1;

        painter->drawText(penX, penY, alarm_model->item(i, 1)->text());
        penX += col3.width() + colSpace.width() * 1;

        painter->drawText(penX, penY, alarm_model->item(i, 2)->text());
        penX += col4.width() + colSpace.width() * 3;

        penX += (pageW - penX)/2 - col5.width()/2;
        painter->drawText(penX, penY, alarm_model->item(i, 3)->text());

        penY += textFm.lineSpacing();

        if(penY > pageH) {
            device->newPage();
            penY = 0;
        }
    }

    delete painter;
}

//布防
void AlarmWindows::arming()
{
    QTreeWidgetItem *item = ui.deviceTree->currentItem();
    QVariant v = item->data(0, Qt::UserRole);
    DefenceZone defence;

    if(v.canConvert<DefenceZone>()) {
        defence = v.value<DefenceZone>();
        defence.armingState = 1; //1：布防

        item->setText(1, armTable.value(1)); //1：布防
        item->setBackground(1, armColor.value(1));

        item->setData(0, Qt::UserRole, QVariant::fromValue(defence));

        qDebug() << "defence : " << defence.id << defence.armingState;

        //更新数据库
        QSqlQuery query(db);
        query.prepare("UPDATE defence SET arm_status = :arm_status WHERE id = :id");

        query.bindValue(":arm_status", defence.armingState);
        query.bindValue(":id", defence.id);

        query.exec();
        query.finish();

        qDebug() << query.lastQuery() << query.lastError();

        emit distribDefenceInfo(defence.address, defence);
        emit distribDefenceInfo(defence.address, defence);
    }
}

//撤防
void AlarmWindows::disArming()
{
    QTreeWidgetItem *item = ui.deviceTree->currentItem();
    QVariant v = item->data(0, Qt::UserRole);
    DefenceZone defence;

    if(v.canConvert<DefenceZone>()) {
        defence = v.value<DefenceZone>();
        defence.armingState = 0; //0:撤防

        item->setText(1, armTable.value(0)); //0:撤防
        item->setBackground(1, armColor.value(0));

        item->setData(0, Qt::UserRole, QVariant::fromValue(defence));

        qDebug() << "defence : " << defence.id << defence.armingState;

        //更新数据库
        QSqlQuery query(db);
        query.prepare("UPDATE defence SET arm_status = :arm_status WHERE id = :id");

        query.bindValue(":id", defence.id);
        query.bindValue(":arm_status", defence.armingState);

        query.exec();
        query.finish();

        qDebug() << query.lastQuery() << query.lastError() << defence.id << defence.armingState;

        emit distribDefenceInfo(defence.address, defence);
        emit distribDefenceInfo(defence.address, defence);
    }
}

//消警
void AlarmWindows::clearArming()
{
    QTreeWidgetItem *item = ui.deviceTree->currentItem();
    QVariant v = item->data(0, Qt::UserRole);
    DefenceZone defence;

    if(v.canConvert<DefenceZone>()) {
        defence = v.value<DefenceZone>();

        //0.通知APIServer
        QByteArray alarmData;
        alarmData.append(0xff);
        alarmData.append(0xff);
        alarmData.append(0xff); //正常
        alarmData[2] = 0;
        alarmData.append(defence.name.toLocal8Bit());
        alarmData.append(0xee);
        alarmData.append(0xee);
        emit notify(alarmData);
        qDebug() << "clear arming "<< alarmData.toHex() << defence.name;
    }

    //1.清除报警状态
    item->setText(2, alarmTable.value(0));//报警状态
    item->setData(2, Qt::UserRole, 0);
    item->setBackground(2, alarmColor.value(0));

    //2.联动地图报警
    QString alarmDefenceName = item->text(0);

    for (QGraphicsItem* item : alarmScene->items())
    {
        if (item->data(1).toString() == alarmDefenceName)
        {
            item->setVisible(false);
        }
    }

    //3.通知现场
    emit distribRemoteCmd(defence.address, 17);

    ui.deviceTree->clearFocus();
}

void AlarmWindows::initMenu()
{
    //0. 初始化分组菜单
    addGroupMenu = new QMenu(ui.deviceTree);
    connect(ui.deviceTree, &QTreeWidget::customContextMenuRequested, this, &AlarmWindows::showTreeMenu);

    QAction *addGroupAction = addGroupMenu->addAction(u8"添加分组");
    connect(addGroupAction, &QAction::triggered, [&](){
        QSqlQuery query(db);
        query.prepare("INSERT INTO defence_group (name) VALUES (:name)");

        query.bindValue(":name", u8"新建分组");

        bool result = query.exec();

        int groupId = query.lastInsertId().toInt();

        query.finish();

        qDebug() << "### Add group  is : " << result << query.lastError();

        if (result)
        {
            QTreeWidgetItem *newItem = new QTreeWidgetItem;
            newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
            newItem->setText(0, u8"新建分组");
            newItem->setData(0, Qt::UserRole, groupId);

            ui.deviceTree->addTopLevelItem(newItem);
            ui.deviceTree->editItem(newItem, 0);

            //            groups.insert(groupId, newItem->text(0));
        }else
        {
            //GoGuiMessageBox::showWarningBox(this, u8"失败"), u8"添加分组失败，检查是否有重名分组。"));
        }
    });

    modifyGroupMenu = new QMenu(ui.deviceTree);

    QAction *modifyGroupAction = modifyGroupMenu->addAction(u8"修改分组");
    connect(modifyGroupAction, &QAction::triggered, [&]()
    {
        QTreeWidgetItem *item = ui.deviceTree->currentItem();
        ui.deviceTree->editItem(item, 0);
    });

    QAction *delGroupAction = modifyGroupMenu->addAction(u8"删除分组");
    connect(delGroupAction, &QAction::triggered, [&]()
    {
        QTreeWidgetItem *item = ui.deviceTree->selectedItems()[0];
        int id = item->data(0, Qt::UserRole).toInt();

        //1.删除数据库
        QSqlQuery query(db);
        query.prepare("DELETE FROM defence_group WHERE id = :id");

        query.bindValue(":id", id);

        if (query.exec())
        {
            //2.删除item
            ui.deviceTree->takeTopLevelItem(ui.deviceTree->indexOfTopLevelItem(item));

            //            groups.remove(id);
            //            groupItems.remove(id);

        }else
        {
            qDebug() << "### delete group is : " << query.lastError();

            //GoGuiMessageBox::showWarningBox(this, u8"失败"), u8"删除分组失败，只允许删除空分组。"));
        }

    });

    modifyGroupMenu->addSeparator();

    QAction *addDefenceAction = modifyGroupMenu->addAction(u8"添加防区");
    connect(addDefenceAction, SIGNAL(triggered()), this, SLOT(addDefence()));


    //1. 初始化列表菜单
    armingMenu = new QMenu(ui.deviceTree);

    QAction* modifyDefenceAction = new QAction(u8"&修改防区", this);
    armingMenu->addAction(modifyDefenceAction);
    connect(modifyDefenceAction, SIGNAL(triggered()), this, SLOT(modifyDefence()));

    QAction* deleteDefenceAction = new QAction(u8"&删除防区", this);
    armingMenu->addAction(deleteDefenceAction);
    connect(deleteDefenceAction, SIGNAL(triggered()), this, SLOT(deleteDefence()));


    armingMenu->addSeparator();

    QAction* armingAction = new QAction(u8"&布防", this);
    armingMenu->addAction(armingAction);
    connect(armingAction, SIGNAL(triggered()), this, SLOT(arming()));

    QAction* disarmingAction = new QAction(u8"&撤防", this);
    armingMenu->addAction(disarmingAction);
    connect(disarmingAction, SIGNAL(triggered()), this, SLOT(disArming()));

    QAction* clearArmingAction = new QAction(u8"&消警", this);
    armingMenu->addAction(clearArmingAction);
    connect(clearArmingAction, SIGNAL(triggered()), this, SLOT(clearArming()));

    //2. 初始化地图菜单
    alarmMenu = new QMenu(ui.alarmView);

    //    addAlarmAction = new QAction(u8"&添加报警点", alarmMenu);
    //    alarmMenu->addAction(addAlarmAction);
    //    connect(addAlarmAction, SIGNAL(triggered()), this, SLOT(addAlarmItem()));

    showAlarmAction = new QAction(u8"&显示报警点", alarmMenu);
    showAlarmAction->setCheckable(true);
    showAlarmAction->setChecked(false);
    alarmMenu->addAction(showAlarmAction);
    connect(showAlarmAction, SIGNAL(triggered()), this, SLOT(showAlarmItem()));

    saveAction = new QAction(u8"&保存地图", alarmMenu);
    alarmMenu->addAction(saveAction);
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveMap()));

    connect(ui.alarmView,
            SIGNAL(customContextMenuRequested(QPoint)),
            this,
            SLOT(showMapMenu(QPoint)));

    //3. 初始化报警点菜单
    itemMenu = new QMenu(ui.alarmView);

    //    deleteAlarmAction = new QAction(u8"删除报警点", itemMenu);
    //    itemMenu->addAction(deleteAlarmAction);
    //    connect(deleteAlarmAction, &QAction::triggered, this, &AlarmWindows::deleteAlarmItem);

    //    modifyAlarmAction = new QAction(u8"修改报警点", itemMenu);
    //    itemMenu->addAction(modifyAlarmAction);
    //    connect(modifyAlarmAction, &QAction::triggered, this, &AlarmWindows::modifyAlarmItem);

}

void AlarmWindows::initDb()
{
    db = QSqlDatabase::addDatabase("QSQLITE", "pims");
    db.setDatabaseName("pims.db");

    if (db.open())
    {
        qDebug() << "### alarm windows open db success!";
    }
    else
    {
        qDebug() << "### alarm windows open db fail!";
    }
}

void AlarmWindows::initData()
{
    defenceItems.clear();
    addrList.clear();
    ui.deviceTree->clear();
    alarmScene->clear();
    //    alarmStatus.clear();

    ui.deviceTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    //1.加载分组
    QSqlQuery selectGroup("SELECT * FROM defence_group", db);

    if(db.isOpen() && !selectGroup.exec()) {
        qDebug() << "### select group is : " << selectGroup.lastError();
        return;
    }

    int idNo = selectGroup.record().indexOf("id");
    int nameNo = selectGroup.record().indexOf("name");

    while (selectGroup.next()) {
        int groupId = selectGroup.value(idNo).toInt();
        QString name = selectGroup.value(nameNo).toString();

        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setText(0, name);
        item->setData(0, Qt::UserRole, groupId);

        ui.deviceTree->addTopLevelItem(item); //添加为顶层节点
        groupItems.insert(groupId, item); //存入节点列表
    }

    ui.deviceTree->expandAll();

    //2. 加载防区
    QSqlQuery selectMap("SELECT * FROM defence ORDER BY cast(name as '9999')", db);

    int defIdNo = selectMap.record().indexOf("id");
    int defNameNo = selectMap.record().indexOf("name");
    int typeNo = selectMap.record().indexOf("type");
    int sensNo = selectMap.record().indexOf("sens");
    int ipNo = selectMap.record().indexOf("ip");
    int addressNo = selectMap.record().indexOf("address");
    int mapXNo = selectMap.record().indexOf("map_x");
    int mapYNo = selectMap.record().indexOf("map_y");
    int armStatusNo = selectMap.record().indexOf("arm_status");
    int alarmTimeNo = selectMap.record().indexOf("alarm_time");
    int alarmFreqNo = selectMap.record().indexOf("alarm_freq");
    int rtspNo = selectMap.record().indexOf("rtspAddress");
    int groupIdNo = selectMap.record().indexOf("group_id");

    while (selectMap.next()) {
        int defenceId = selectMap.value(defIdNo).toInt();
        QString name = selectMap.value(defNameNo).toString();
        int type = selectMap.value(typeNo).toInt();
        float sens = selectMap.value(sensNo).toFloat();
        QString ip = selectMap.value(ipNo).toString();
        QString address = selectMap.value(addressNo).toString();
        int x = selectMap.value(mapXNo).toInt();
        int y = selectMap.value(mapYNo).toInt();
        int armStatus = selectMap.value(armStatusNo).toInt();
        int alarmTime = selectMap.value(alarmTimeNo).toInt();
        int alarmFreq = selectMap.value(alarmFreqNo).toInt();
        QString rtsp = selectMap.value(rtspNo).toString();
        int groupId = selectMap.value(groupIdNo).toInt();

        DefenceZone defence;
        defence.id = defenceId;
        defence.name = name;
        defence.type = type;
        defence.sens = sens;
        defence.address = ip;
        defence.mapX = x;
        defence.mapY = y;
        defence.armingState = armStatus;
        defence.alarmTime = alarmTime;
        defence.alarmFreq = alarmFreq;
        defence.rtspAddress = rtsp;
        defence.groupId = groupId;

        //        QFile logFile("log/" + QString::fromLocal8Bit(defence.name.toLocal8Bit().data()) + "_"
        //                      + defence.address + "_log.txt");
        //        logFile.open(QIODevice::WriteOnly | QIODevice::Append);

        //2.1 加载防区设备列表
        defence.vsList = loadDeviceList(defence.id);

        //2.2 添加防区Item
        addDefenceItem(defence);

        //添加到地图
        QPixmap pixmap(":/Resources/image/alarm/alarm_light.png");
        AlarmPixmapItem *alarmPixmapItem = new AlarmPixmapItem(pixmap);
        alarmPixmapItem->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);

        alarmPixmapItem->setData(0, defenceId);
        alarmPixmapItem->setData(1, name);
        alarmPixmapItem->setToolTip(name);

        alarmScene->addItem(alarmPixmapItem);
        alarmPixmapItem->setPos(QPointF(x,y));
        alarmPixmapItem->setVisible(false);
    }

    //    updateAddrList();
}

void AlarmWindows::initList()
{
    //1.报警列表
    alarm_model = new QStandardItemModel();
    alarm_model->setHorizontalHeaderItem(0, new QStandardItem(u8"类型"));
    alarm_model->setHorizontalHeaderItem(1, new QStandardItem(u8"防区"));
    alarm_model->setHorizontalHeaderItem(2, new QStandardItem(u8"报警状态"));
    alarm_model->setHorizontalHeaderItem(3, new QStandardItem(u8"报警时间"));

    //利用setModel()方法将数据模型与QTableView绑定
    ui.eventView->setModel(alarm_model);

    //设置列宽
    ui.eventView->setColumnWidth(0, 150);
    ui.eventView->setColumnWidth(1, 200);
    ui.eventView->setColumnWidth(2, 200);
    ui.eventView->horizontalHeader()->setStretchLastSection(true);

    //设置选中时为整行选中
    ui.eventView->setSelectionBehavior(QAbstractItemView::SelectRows);

    //设置表格的单元为只读属性，即不能编辑
    ui.eventView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //如果你用在QTableView中使用右键菜单，需启用该属性
    ui.eventView->setContextMenuPolicy(Qt::CustomContextMenu);

    //2.设置当前日期
    ui.beginDateEdit->setDate(QDate::currentDate());
    ui.endDateEdit->setDate(QDate::currentDate());
}

void AlarmWindows::addDefenceItem(DefenceZone defence)
{
    QTreeWidgetItem *item = new QTreeWidgetItem;

    item->setText(0, defence.name);                               //名称
    item->setData(0, Qt::UserRole, QVariant::fromValue(defence)); //存储defence结构体

    item->setText(1, armTable.value(defence.armingState)); //布防撤防状态
    item->setBackground(1, armColor.value(defence.armingState));

    item->setText(2, alarmTable.value(0)); //开机为正常状态
    item->setBackground(2, alarmColor.value(0));

    item->setText(3, onlineTable.value(0)); //开机为离线状态
    item->setBackground(3, onlineColor.value(0));

    groupItems.value(defence.groupId)->addChild(item); //添加防区节点到顶层分组节点
    defenceItems.insert(defence.id, item);             //防区节点
    defenceNameItems.insert(defence.name, item);
    defenceIpItems.insert(defence.address, item);
    addrDefences.insert(defence.address, defence);
    //    alarmStatus.insert(defence.id, 0);
}

QList<VibSensor> AlarmWindows::loadDeviceList(int defenceId)
{
    QList<VibSensor> vsList;

    QSqlQuery selectDeviceMap("SELECT * FROM device WHERE defence_id = "
                                  + QString::number(defenceId) + " ORDER BY cast(addr as '9999')",
                              db);

    int devIdNo = selectDeviceMap.record().indexOf("id");
    int devAddrNo = selectDeviceMap.record().indexOf("addr");
    int devBaudRateNo = selectDeviceMap.record().indexOf("baudrate");
    int devSensNo = selectDeviceMap.record().indexOf("sens");
    int devFreqNo = selectDeviceMap.record().indexOf("freq");
    int devMapXNo = selectDeviceMap.record().indexOf("map_x");
    int devMapYNo = selectDeviceMap.record().indexOf("map_y");
    //    int devOnlineStatusNo = selectDeviceMap.record().indexOf("online_status");
    int devAlarmStatusNo = selectDeviceMap.record().indexOf("alarm_status");
    int devArmStatusNo = selectDeviceMap.record().indexOf("arm_status");

    while (selectDeviceMap.next()) {
        int devId = selectDeviceMap.value(devIdNo).toInt();
        QString devAddr = selectDeviceMap.value(devAddrNo).toString();
        int devBaudRate = selectDeviceMap.value(devBaudRateNo).toInt();
        float devSens = selectDeviceMap.value(devSensNo).toFloat();
        int devFreq = selectDeviceMap.value(devFreqNo).toInt();
        int devMapX = selectDeviceMap.value(devMapXNo).toInt();
        int devMapY = selectDeviceMap.value(devMapYNo).toInt();
        //        int devOnlineStatus = selectDeviceMap.value(devOnlineStatusNo).toInt();
        int devAlarmStatus = selectDeviceMap.value(devAlarmStatusNo).toInt();
        int devArmStatus = selectDeviceMap.value(devArmStatusNo).toInt();

        VibSensor vs;
        vs.id = devId;
        vs.addr = devAddr;
        vs.baudRate = devBaudRate;
        vs.sens = devSens;
        vs.freq = devFreq;
        vs.mapX = devMapX;
        vs.mapY = devMapY;
        //        vs.state = devOnlineStatus;
        vs.alarmState = devAlarmStatus;
        vs.armingState = devArmStatus;
        vs.xGyro = 0;
        vs.yGyro = 0;
        vs.zGyro = 0;
        vs.alarmCount = 0;

        vsList.append(vs);
    }

    return vsList;
}
