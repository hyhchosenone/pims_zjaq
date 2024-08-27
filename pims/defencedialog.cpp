#include "defencedialog.h"
#include "rtsplayer.h"
#include "stdafx.h"

DefenceDialog::DefenceDialog(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    ui.tableWidget->setRowCount(64);

    //connect(ui.saveButton, &QPushButton::click, this, &DefenceDialog::on_saveButton_clicked);
    //connect(ui.cancelButton, &QPushButton::click, this, &DefenceDialog::on_cancelButton_clicked);
}

DefenceDialog::~DefenceDialog()
{
//    if (player != nullptr) {
//        player->stop();
//        delete player;
//    }
}

int DefenceDialog::getDeviceId()
{
    return deviceId;
}

void DefenceDialog::setDeviceId(int id)
{
    this->deviceId = id;
}

QString DefenceDialog::getHostAddress()
{
    return ui.hostAddress->text().trimmed();
}

void DefenceDialog::setHostAddress(QString addr)
{
    ui.hostAddress->setText(addr);
}

QString DefenceDialog::getAddress()
{
    //    return  ui.addr->toPlainText().trimmed();
    return "";
}

void DefenceDialog::setAddress(QString address)
{
    //    ui.addr->setText(address);
}

QString DefenceDialog::getName()
{
    return  ui.name->text().trimmed();
}

void DefenceDialog::setName(QString name)
{
    ui.name->setText(name);
}

void DefenceDialog::setModifyMode(bool modifyFlag)
{
    //    ui.addr->setReadOnly(modifyFlag);
}

float DefenceDialog::getSens()
{
    float sens = ui.sens->text().trimmed().toFloat();
    sens = sens > 1.0 ? 1.0 : sens;
    sens = sens < 0 ? 0.0 : sens;

    return sens;
}

void DefenceDialog::setSens(float sens)
{
    qDebug() << "setSens is " << sens;
    ui.sens->setText(QString::number(sens, 'f', 2));
}

int DefenceDialog::getAlarmTime()
{
    short time = ui.alarmTime->text().trimmed().toShort();
    time = time > 255 ? 255 : time;
    time = time < 0 ? 0 : time;

    return time;
}

void DefenceDialog::setAlarmTime(int time)
{
    ui.alarmTime->setText(QString::number(time));
}

int DefenceDialog::getAlarmFreq()
{
    short freq = ui.alarmFreq->text().trimmed().toShort();
    freq = freq > 5 ? 5 : freq;
    freq = freq < 0 ? 1 : freq;

    return freq;
}

void DefenceDialog::setAlarmFreq(int freq)
{
    ui.alarmFreq->setText(QString::number(freq));
}

QList<VibSensor> DefenceDialog::getDeviceList()
{
    QList<VibSensor> vsList;
    for (int i = 0; i < ui.tableWidget->rowCount(); i++) {
        QTableWidgetItem *addrItem = ui.tableWidget->item(i, 0);

        if (addrItem == nullptr) {
            continue;
        }

        QString addr = addrItem->text().trimmed();
        if (addr.length() > 0) { //地址存在
            VibSensor vs;
            vs.addr = addr;

            QTableWidgetItem *sensItem = ui.tableWidget->item(i, 1); //灵敏度
            float sens = sensItem != nullptr ? sensItem->text().toFloat() : 0.0;
            vs.sens = sens < 0 ? 0.0 : sens;
            vs.sens = vs.sens > 1.0 ? 1.0 : vs.sens;

            QTableWidgetItem *onlineItem = ui.tableWidget->item(i, 2); //频率阈值
            vs.freq = onlineItem != nullptr ? onlineItem->text().toUInt() : 0;

            QTableWidgetItem *alarmItem = ui.tableWidget->item(i, 3); //在线状态
            vs.state = alarmItem != nullptr ? alarmItem->text().toUInt() : 0;

            QTableWidgetItem *armItem = ui.tableWidget->item(i, 4);
            vs.armingState = armItem != nullptr ? armItem->text().toUInt() : 0;

            vsList.append(vs);
        }
    }

    return vsList;
}

void DefenceDialog::setDeviceList(QList<VibSensor> vsList)
{
    for (int i = 0; i < vsList.count(); i++) {
        VibSensor vs = vsList[i];

        QTableWidgetItem *addrItem = new QTableWidgetItem(vs.addr);
        addrItem->setTextAlignment(Qt::AlignCenter);
        ui.tableWidget->setItem(i, 0, addrItem);

        QTableWidgetItem *sensItem = new QTableWidgetItem(QString::number(vs.sens, 'f', 2));
        sensItem->setTextAlignment(Qt::AlignCenter);
        ui.tableWidget->setItem(i, 1, sensItem);

        QTableWidgetItem *freqItem = new QTableWidgetItem(QString::number(vs.freq));
        freqItem->setTextAlignment(Qt::AlignCenter);
        ui.tableWidget->setItem(i, 2, freqItem);

        //        QTableWidgetItem *statusItem = new QTableWidgetItem(QString::number(vs.state));
        QTableWidgetItem *statusItem = new QTableWidgetItem("正常");
        statusItem->setTextAlignment(Qt::AlignCenter);
        ui.tableWidget->setItem(i, 3, statusItem);

        QTableWidgetItem *armItem = new QTableWidgetItem(QString::number(vs.armingState));
        armItem->setTextAlignment(Qt::AlignCenter);
        ui.tableWidget->setItem(i, 4, armItem);

        //        qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
        //        quint8 num = (quint8) qrand() % 100;

        float tem = QRandomGenerator::global()->bounded(1.1);

        QTableWidgetItem *tempItem = new QTableWidgetItem(QString::number(34 + tem, 'f', 1));
        tempItem->setTextAlignment(Qt::AlignCenter);
        ui.tableWidget->setItem(i, 5, tempItem);
    }
}

void DefenceDialog::setDefence(DefenceZone defence)
{
    this->defence = defence;

    setName(defence.name);
    setType(defence.type);
    setHostAddress(defence.address);
    setSens(defence.sens);
    setAlarmTime(defence.alarmTime);
    setAlarmFreq(defence.alarmFreq);
    setDeviceList(defence.vsList);
    setRtspAddress(defence.rtspAddress);
}

QString DefenceDialog::getRtspAddress()
{
    return ui.rtspAddress->text().trimmed();
}

void DefenceDialog::setRtspAddress(QString rtsp)
{
    ui.rtspAddress->setText(rtsp);
}

int DefenceDialog::getType()
{
    return ui.type->currentIndex();
}

void DefenceDialog::setType(int type)
{
    ui.type->setCurrentIndex(type);
}

void DefenceDialog::on_saveButton_clicked()
{
    if (ui.name->text().isEmpty()) {
        myHelper::ShowMessageBoxInfo("防区名称不能为空");

        return;
    } else {
        if (myHelper::ShowMessageBoxQuesion("确认要保存修改吗？") == QDialog::Rejected) {
            return;
        }

        accept();
    }
}

void DefenceDialog::on_cancelButton_clicked()
{
    reject();
}

void DefenceDialog::on_testRtsp_clicked()
{
//    if (player == nullptr) {
//        player = new RTSPlayer(this);
//        //        player->setWindowFlags(player->windowFlags() | Qt::Dialog);
//    }

//    player->play(getRtspAddress());
//    player->show();
}
