#include "alarmDialog.h"

#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPainter>
#include <QStyleOption>
//#include "iconfonthelper.h"

AlarmDialog::AlarmDialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AlarmDialog)
{
    ui->setupUi(this);

    initStyle();

    this->alarmTemplate = QStringLiteral("事件源头:\n\t%1\n\n")
            + QStringLiteral("事件类型:\n\t%2\n\n")
            + QStringLiteral("报警时间:\n\t%3\n\n")
            + QStringLiteral("事件级别:\n\t%4\n\n")
            + QStringLiteral("事件细节:\n\t%5");

    connect(&manager,
            &QNetworkAccessManager::authenticationRequired,
            this,
            &AlarmDialog::authenticationResponse);
}

AlarmDialog::~AlarmDialog()
{
    delete ui;
}

void AlarmDialog::setAlarmInfo(const QString &source,
                               const QString &type,
                               const QString &datetime,
                               const QString &level,
                               const QString &detail,
                               const QUrl &mapLink,
                               const QString &videoLink)
{
    //1.事件信息
    ui->eventTextEdit->setText(this->alarmTemplate.arg(source).arg(type).arg(datetime).arg(level).arg(detail));

    //2.报警点视频
    ui->rtsplayer->setRTSPUrl(videoLink);
    ui->rtsplayer->play();

    //3.报警点地图
    QNetworkReply *reply = manager.get(QNetworkRequest(mapLink));

    //请求结束并下载完成后，退出子事件循环
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));

    //开启子事件循环
    loop.exec();

    QByteArray jpegData = reply->readAll();
    QPixmap pixmap;
    pixmap.loadFromData(jpegData);

    ui->mapLabel->setPixmap(pixmap); // 你在QLabel显示图片
    reply->deleteLater();
}

void AlarmDialog::authenticationResponse(QNetworkReply *reply, QAuthenticator *authenticator)
{
    authenticator->setUser("admin");
    authenticator->setPassword("czjds123");
}

bool AlarmDialog::eventFilter(QObject *obj, QEvent *ev)
{
    switch (ev->type())
    {
    case QEvent::MouseButtonPress:
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(ev);

        if (Qt::LeftButton == mouseEvent->button()) //检测是否为左键
        {

            if (obj == ui->appClose) //关闭按钮事件
            {
                this->hide();

                return true;
            }
            else if (obj == ui->title)
            {
                mousePressed = true;
                mousePoint = mouseEvent->globalPos() - this->pos();
                return true;
            }
        }
    }
        break;

    case QEvent::MouseMove:
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(ev);

        if (obj == ui->title && mousePressed)
        {
            this->move(mouseEvent->globalPos() - mousePoint);

            return true;
        }
    }
        break;

    case QEvent::MouseButtonRelease:
        if (obj == ui->title && mousePressed)
        {
            mousePressed = false;

            return true;
        }
        break;

    }

    return QObject::eventFilter(obj, ev);
}

void AlarmDialog::initStyle()
{
    //隐藏原生标题栏
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);

    QFile qss(":/alarmCenter/Resources/qss/black.css");
    if (qss.open(QFile::ReadOnly)) {
        QString style = QLatin1String(qss.readAll());
        this->setStyleSheet(style);
        qss.close();
    }

    //    IconfontHelper::Instance()->setIcon(ui->appClose, QChar(0xf00d));
    ui->appClose->installEventFilter(this); //关闭

    ui->title->installEventFilter(this); //拖动
}
