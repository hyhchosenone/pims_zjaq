#include "pims.h"
#include "apiworker.h"
#include "frmabout.h"
#include "frmconfig.h"
#include "iconhelper.h"
#include "netalarmserver.h"
#include "tcpclient.h"
#include "vibsensorworker.h"
#include <app.h>
#include <QDesktopWidget>
#include <QSettings>
#include <QTimer>

PIMS::PIMS(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    this->InitStyle();
    //this->InitDb();
    this->InitForm();
    this->setGeometry(qApp->desktop()->availableGeometry());

    QTimer::singleShot(3000, this, SLOT(delayStart()));
}

PIMS::~PIMS()
{
    delete alarmWindws;
    delete dataWindows;
    delete configWindows;
}

void PIMS::InitStyle()
{
    ui.lab_Title->setText(App::Title);
    this->setProperty("Form", true);
    this->setGeometry(qApp->desktop()->availableGeometry());
    this->setWindowTitle(ui.lab_Title->text());
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
    IconHelper::Instance()->SetIcoMain(ui.lab_Ico, 40);
    ui.lab_Title->setFont(QFont(App::FontName, 20));
}

void PIMS::InitForm()
{
    QFont font = QFont(App::FontName, App::FontSize - 1);
    QList<QLabel *> labs = ui.widget_bottom->findChildren<QLabel *>();
    foreach(QLabel * lab, labs) {
        lab->setFont(font);
    }

    ui.labWelcom->setText(QString("  欢迎使用%1 %2  |  ").arg(App::Title).arg(App::Version));
    ui.labAuthor->setText(QString("版权所有 : %1  |  ").arg(App::Author));

    QList<QToolButton *> btns = ui.widget_top->findChildren<QToolButton *>();
    foreach(QToolButton * btn, btns) {
        connect(btn, SIGNAL(clicked()), this, SLOT(button_clicked()));
    }

    alarmWindws = new AlarmWindows();
    ui.stackedWidget->addWidget(alarmWindws);

    dataWindows = new DataWindows();
    ui.stackedWidget->addWidget(dataWindows);

    configWindows = new frmConfig;
    ui.stackedWidget->addWidget(configWindows);

    ui.btnAlarmCenter->click();

}

void PIMS::InitDb()
{
    /*
    this->db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("127.0.0.1");
    db.setDatabaseName("pims");
    db.setUserName("postgres");
    db.setPassword("p0st45576");
    bool ok = db.open();
    */
}

void PIMS::startTcpClient()
{
    //server setting
    QString fileName = QString("%1/config/%2_Config.ini").arg(App::AppPath).arg(App::AppName);
    QSettings set(fileName, QSettings::IniFormat);
    set.beginGroup("Server");
    QString srvIP = set.value("ip").toString();
    int srvPort = set.value("port").toInt();
    set.endGroup();

    dataThread = new QThread();
    TcpClient *client = new TcpClient();
    client->setIpAndPort(srvIP, srvPort);
    client->moveToThread(dataThread);

    connect(dataThread, &QThread::finished, client, &QObject::deleteLater);
    connect(dataThread, &QThread::started, client, &TcpClient::doWork);
    //    connect(client, &TcpClient::uploadData, alarmWindws, &AlarmWindows::reciveData);

    dataThread->start();
}

void PIMS::delayStart()
{
    //    startNetAlarmServerThread();
//    startAPIServerThread();
}

void PIMS::startAPIServerThread()
{
    apiThread = new QThread();
    APIWorker *apiWorker = new APIWorker();
    apiWorker->moveToThread(apiThread);

    connect(apiThread, &QThread::finished, apiWorker, &QObject::deleteLater);
    connect(apiThread, &QThread::started, apiWorker, &APIWorker::doWork);
    connect(alarmWindws, &AlarmWindows::notify, apiWorker, &APIWorker::notifyClient);

    apiThread->start();

    qDebug() << "API Thread starting...";
}

void PIMS::startNetAlarmServerThread()
{
    alarmServerThread = new QThread();
    NetAlarmServer *netAlarmSrv = new NetAlarmServer();
    netAlarmSrv->moveToThread(alarmServerThread);

    connect(alarmServerThread, &QThread::finished, netAlarmSrv, &QObject::deleteLater);
    connect(alarmServerThread, &QThread::started, netAlarmSrv, &NetAlarmServer::doWork);
    //    connect(alarmWindws, &AlarmWindows::notify, netAlarmSrv, &APIWorker::notifyClient);

    alarmServerThread->start();

    qDebug() << "API Thread starting...";
}

void PIMS::button_clicked()
{
    QToolButton *btn = (QToolButton *)sender();
    QString name = btn->text();

if (name == "警情中心") {
        ui.stackedWidget->setCurrentIndex(0);
        ui.btnAlarmCenter->setChecked(true);
        ui.btnRealtimeData->setChecked(false);
        ui.btnConfig->setChecked(false);
    }
else if (name == "运行数据") {
    ui.stackedWidget->setCurrentIndex(1);
    ui.btnAlarmCenter->setChecked(false);
    ui.btnRealtimeData->setChecked(true);
    ui.btnConfig->setChecked(false);
}
    else if (name == "系统设置") {
        ui.stackedWidget->setCurrentIndex(2);
        ui.btnAlarmCenter->setChecked(false);
        ui.btnRealtimeData->setChecked(false);
        ui.btnConfig->setChecked(true);
    }
    else if (name == "退出") {
        QApplication::exit();
    }
}
