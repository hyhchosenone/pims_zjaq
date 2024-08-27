#include "stdafx.h"
#include "appinit.h"
#include "myhelper.h"

AppInit::AppInit(QObject *parent) : QObject(parent)
{
    mousePressed = false;
    qApp->installEventFilter(this);
}

bool AppInit::eventFilter(QObject *obj, QEvent *evt)
{
    QWidget *w = (QWidget *)obj;
    if (!w->property("CanMove").toBool()) {
        return QObject::eventFilter(obj, evt);
    }

    QMouseEvent *event = static_cast<QMouseEvent *>(evt);
    if (event->type() == QEvent::MouseButtonPress) {
        if (event->button() == Qt::LeftButton) {
            mousePressed = true;
            mousePoint = event->globalPos() - w->pos();
            return true;
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        mousePressed = false;
        return true;
    } else if (event->type() == QEvent::MouseMove) {
        if (mousePressed && (event->buttons() && Qt::LeftButton)) {
            w->move(event->globalPos() - mousePoint);
            return true;
        }
    }

    return QObject::eventFilter(obj, evt);
}

void AppInit::Load()
{
    myHelper::SetUTF8Code();
    myHelper::SetChinese();

    App::AppPath = QApplication::applicationDirPath();
    App::DeskWidth = qApp->desktop()->availableGeometry().width();
    App::DeskHeight = qApp->desktop()->availableGeometry().height();
    App::ReadConfig();
    myHelper::SetFont();
    myHelper::SetStyle(App::StyleName);

    /*
    QFile key(App::AppPath + "/key.dll");
    key.open(QFile::ReadOnly);
    App::KeyData = key.readLine();
    key.close();

    //将从注册码文件中的密文解密,与当前时间比较是否到期
    App::KeyData = myHelper::getXorEncryptDecrypt(App::KeyData, 110);
    QStringList data = App::KeyData.split("|");
    if (data.count() != 6) {
        myHelper::ShowMessageBoxError("注册码文件已损坏,程序将自动关闭!");
        exit(0);
    }

    App::KeyUseDate = (data.at(0) == "1" ? true : false);
    App::KeyDate = data.at(1);
    App::KeyUseRun = (data.at(2) == "1" ? true : false);
    App::KeyRun = data.at(3).toInt();
    App::KeyUseCount = (data.at(4) == "1" ? true : false);
    App::KeyCount = data.at(5).toInt();

    //如果启用了时间限制
    if (App::KeyUseDate) {
        QString nowDate = QDate::currentDate().toString("yyyy-MM-dd");
        if (nowDate > App::KeyDate) {
            myHelper::ShowMessageBoxError("软件已到期,请联系供应商更新注册码!");
            exit(0);
        }
    }
    */

    //程序启动后打开数据库连接,在任何窗体需要的地方调用数据库
//    QSqlDatabase DbConn;
//    DbConn = QSqlDatabase::addDatabase("QSQLITE");
//    QString dbFile = App::AppPath + "/pims.db";

//    if (!myHelper::FileIsExist(dbFile)) {
//        myHelper::ShowMessageBoxError("数据库文件不存在!");
//        exit(0);
//    }

//    DbConn.setDatabaseName(dbFile);
//    if (!DbConn.open()) {
//        myHelper::ShowMessageBoxError("数据库文件打开失败!");
//        exit(0);
//    }
}
