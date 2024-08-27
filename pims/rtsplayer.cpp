#include "rtsplayer.h"
#include "iconfonthelper.h"
#include "ui_rtsplayer.h"

RTSPlayer::RTSPlayer(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RTSPlayer)
{
    ui->setupUi(this);

    initStyle();
    initVLC();
}

RTSPlayer::~RTSPlayer()
{
    delete ui;

    libvlc_media_player_release (vlcPlayer);
    libvlc_release (vlc);
}

void RTSPlayer::setRTSPUrl(const int brand, const QString address, const int port, const QString username, const QString password)
{
    QString rtsp;

    switch (brand) {
    case 0: //周界安
        break;
    case 1: //海康
        rtsp = QString("rtsp://%1:%2@%3:%4/h265/ch1/sub/av_stream").arg(username).arg(password).arg(address).arg(QString::number(port));

        break;
    default:
        break;
    }

    this->setRTSPUrl(rtsp);
}

void RTSPlayer::setRTSPUrl(const QString rtspUrl)
{
    ui->toolWidget->setEnabled(true);

    this->rtspUrl = rtspUrl;

    //2.创建media资源
    libvlc_media_t *media = libvlc_media_new_location (vlc, rtspUrl.toLocal8Bit().data());

    //2.1 设置media参数
    libvlc_media_add_option(media, ":network-caching=1000 :live-caching=1500;");

    //3. 设置vlc播放器
    libvlc_media_player_set_media(vlcPlayer, media);

    //4. 释放media
    libvlc_media_release (media);
}

QString RTSPlayer::getRTSPUrl()
{
    return rtspUrl;
}

void RTSPlayer::play(QString rtspUrl)
{
    setRTSPUrl(rtspUrl);
    play();
}

void RTSPlayer::play(QString rtspUrl, int volume)
{
    play(rtspUrl);

    if (volume > 0) {
        libvlc_audio_set_volume(vlcPlayer, volume);
        IconfontHelper::Instance()->setIcon(ui->voiceButton, QChar(0xf028), 11);

        muteFlag = false;
    } else {
        libvlc_audio_set_volume(vlcPlayer, 0);
        IconfontHelper::Instance()->setIcon(ui->voiceButton, QChar(0xf026), 11);

        muteFlag = true;
    }
}

void RTSPlayer::play()
{
    this->on_playButton_clicked();
}

void RTSPlayer::pause()
{
    this->on_pauseButton_clicked();
}

void RTSPlayer::stop()
{
    this->on_stopButton_clicked();
}

void RTSPlayer::record()
{
    this->on_recordButton_clicked();
}

void RTSPlayer::capture()
{
    this->on_captureButton_clicked();
}

void RTSPlayer::on_playButton_clicked()
{
    libvlc_media_player_set_hwnd(vlcPlayer, (void *) ui->previewWidget->winId());
    libvlc_audio_set_volume(vlcPlayer, 0);
    libvlc_media_player_play(vlcPlayer);
}

void RTSPlayer::on_pauseButton_clicked()
{
    //libvlc_media_player_pause(vlcPlayer);
    if(libvlc_media_player_is_playing(vlcPlayer)) {

        libvlc_media_player_set_pause(vlcPlayer, 1);
    }
    //    else {

    //        libvlc_media_player_set_pause(vlcPlayer, 0);
    //    }
}

void RTSPlayer::on_stopButton_clicked()
{
    //ui->toolWidget->setEnabled(false);

    libvlc_media_player_stop (vlcPlayer);
}

void RTSPlayer::on_recordButton_clicked()
{
    if(ui->recordButton->isChecked()) {
        QPalette pal = ui->recordButton->palette();
        pal.setColor(QPalette::ButtonText, Qt::red);
        ui->recordButton->setPalette(pal);
    }else {
        QPalette pal = ui->recordButton->palette();
        pal.setColor(QPalette::ButtonText, Qt::white);
        ui->recordButton->setPalette(pal);
    }
}

void RTSPlayer::on_captureButton_clicked()
{
}

void RTSPlayer::on_voiceButton_clicked()
{
    if (muteFlag) {
        libvlc_audio_set_volume(vlcPlayer, 100);
        IconfontHelper::Instance()->setIcon(ui->voiceButton, QChar(0xf028), 11);
    } else {
        libvlc_audio_set_volume(vlcPlayer, 0);
        IconfontHelper::Instance()->setIcon(ui->voiceButton, QChar(0xf026), 11);
    }

    muteFlag = !muteFlag;
}

void RTSPlayer::initStyle()
{
    ui->previewWidget->setStyleSheet("background-image: url(://Resource/image/video-off.png);"
                                     "background-repeat: no-repeat;"
                                     "background-position: center;"
                                     "border: 1px solid #36393E;");

    //    ui->previewWidget->setStyleSheet("border: 1px solid #36393E;");

    IconfontHelper::Instance()->setIcon(ui->playButton, QChar(0xf04b), 11);
    IconfontHelper::Instance()->setIcon(ui->pauseButton, QChar(0xf04c), 11);
    IconfontHelper::Instance()->setIcon(ui->stopButton, QChar(0xf04d), 11);
    IconfontHelper::Instance()->setIcon(ui->recordButton, QChar(0xf03d), 11);
    IconfontHelper::Instance()->setIcon(ui->captureButton, QChar(0xf030), 11);
    IconfontHelper::Instance()->setIcon(ui->voiceButton, QChar(0xf026), 11);

    ui->toolWidget->setEnabled(false);
}

void RTSPlayer::initVLC()
{
    //1.初始化vlc实例
    vlc = libvlc_new (0, nullptr);

    //2.创建media资源
//    libvlc_media_t *media;

//    media = libvlc_media_new_location (vlc, "rtsp://admin:czjds123@192.168.1.64:554/h265/ch1/main/av_stream");

    //2.1 设置media参数
//    libvlc_media_add_option(media, ":network-caching=100 :live-caching=100;");

    //3. 创建vlc播放器
    vlcPlayer = libvlc_media_player_new (vlc);

    //4. 释放media
//    libvlc_media_release (media);
}
