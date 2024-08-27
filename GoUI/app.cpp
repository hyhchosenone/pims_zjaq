#include "stdafx.h"
#include "app.h"
#include "myhelper.h"

QString App::AppName = "PIMS";
QString App::AppPath = "";
int App::DeskWidth = 1440;
int App::DeskHeight = 900;

int App::IcoMain = 0xf1aa;
int App::IcoMenu = 0xf0d7;
int App::IcoMin = 0xf068;
int App::IcoMax = 0xf079;
int App::IcoNormal = 0xf0b2;
int App::IcoClose = 0xf00d;

int App::FontSize = 9;
QString App::FontName = "Microsoft YaHei";
QString App::PaletteColor = "#000000";

bool App::SaveLog = false;

//注册信息
QString App::KeyData = "";
bool App::KeyUseDate = false;
QString App::KeyDate = "2015-01-01";
bool App::KeyUseRun = false;
int App::KeyRun = 1;
bool App::KeyUseCount = false;
int App::KeyCount = 10;

QString App::StyleName = ":/Resources/qss/blue.css";

QString App::Title = "微震动入侵探测系统";
QString App::Version = "V6.0";
QString App::Author = "周界安全科技有限公司";
bool App::UseInput = false;
QString App::InputPosition = "control";

void App::ReadConfig()
{
    QString fileName = QString("%1/config/%2_Config.ini").arg(App::AppPath).arg(App::AppName);

    //如果配置文件不存在或者内容为空,则以初始值继续运行,并生成配置文件
    QFile file(fileName);
    if (file.size() == 0) {
#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
        App::Title = App::Title.toLatin1();
        App::Author = App::Author.toLatin1();
#endif
        WriteConfig();
        return;
    }

    QSettings set(fileName, QSettings::IniFormat);
    set.beginGroup("AppConfig");

    App::StyleName = set.value("StyleName").toString();
    App::Title = set.value("Title").toString();
    App::Version = set.value("Version").toString();
    App::Author = set.value("Author").toString();

    set.endGroup();
}

void App::WriteConfig()
{
    QString fileName = QString("%1/config/%2_Config.ini").arg(App::AppPath).arg(App::AppName);
    QSettings set(fileName, QSettings::IniFormat);
    set.beginGroup("AppConfig");

    set.setValue("StyleName", App::StyleName);
    set.setValue("Title", App::Title);
    set.setValue("Version", App::Version);
    set.setValue("Author", App::Author);

    set.endGroup();
}

QList<QString> App::TaskList = QList<QString>();
QList<QString> App::SnapList = QList<QString>();
QList<QString> App::EmailList = QList<QString>();
QList<QString> App::MsgList = QList<QString>();

void App::AppendTask(QString task)
{
    QMutex mutex;
    mutex.lock();
    App::TaskList.append(task);
    mutex.unlock();
}

void App::AppendSnap(QString snap)
{
    QMutex mutex;
    mutex.lock();
    App::SnapList.append(snap);
    mutex.unlock();
}

void App::AppendEmail(QString email)
{
    QMutex mutex;
    mutex.lock();
    App::EmailList.append(email);
    mutex.unlock();
}

void App::AppendMsg(QString msg)
{
    QMutex mutex;
    mutex.lock();
    App::MsgList.append(msg);
    mutex.unlock();
}
