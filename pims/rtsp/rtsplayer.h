#ifndef RTSPLAYER_H
#define RTSPLAYER_H

#include <QWidget>
#include <QtUiPlugin/QDesignerExportWidget>
#include "vlc/vlc.h"

namespace Ui {
class RTSPlayer;
}

//class QDESIGNER_WIDGET_EXPORT RTSPlayer : public QWidget
class RTSPlayer : public QWidget
{
    Q_OBJECT

public:
    explicit RTSPlayer(QWidget *parent = nullptr);
    ~RTSPlayer();

    void setRTSPUrl(const int brand, const QString address, const int port, const QString username, const QString password);
    void setRTSPUrl(const QString rtspUrl);
    QString getRTSPUrl();

public slots:
    void play(QString rtspUrl);
    void play();
    void pause();
    void stop();
    void record();
    void capture();

private slots:
    void on_playButton_clicked();
    void on_pauseButton_clicked();
    void on_stopButton_clicked();
    void on_recordButton_clicked();
    void on_captureButton_clicked();

private:
    void initStyle();
    void initVLC();

    Ui::RTSPlayer *ui;
    QString rtspUrl;
    libvlc_instance_t * vlc;
    libvlc_media_player_t *vlcPlayer;
};

#endif // RTSPLAYER_H
