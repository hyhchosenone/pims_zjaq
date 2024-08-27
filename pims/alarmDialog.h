#ifndef ALARMWINDOWS_H
#define ALARMWINDOWS_H

#include "ui_alarmDialog.h"
#include <QAuthenticator>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QtWidgets/QWidget>

class AlarmDialog : public QWidget
{
    Q_OBJECT
public:
    explicit AlarmDialog(QWidget *parent = nullptr);
    ~AlarmDialog() override;

signals:

public slots:
    void setAlarmInfo(const QString &source, const QString &type, const QString &datetime, const QString &level, const QString &detail, const QUrl &mapLink, const QString &videoLink);

    void authenticationResponse(QNetworkReply *reply, QAuthenticator *authenticator);

protected:
    bool eventFilter(QObject *obj, QEvent *ev) override;

private:
    void initStyle();

    Ui::AlarmDialog *ui;
    QPoint mousePoint;  //鼠标拖动时的坐标
    bool mousePressed;  //鼠标按下状态
    QString alarmTemplate; //警情信息模板
    QNetworkAccessManager manager;
};

#endif // ALARMWINDOWS_H
