#include <stdafx.h>
#include "frmconfig.h"
#include "ui_frmconfig.h"
#include <QPrinter>
#include <QPrinterInfo>
#include <QSerialPortInfo>

frmConfig::frmConfig(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::frmConfig)
{
	ui->setupUi(this);
	this->InitForm();
}

frmConfig::~frmConfig()
{
	delete ui;
}

void frmConfig::InitForm()
{
	QList<QPushButton *> btns = ui->widget_left->findChildren<QPushButton *>();
	foreach(QPushButton * btn, btns) {
		connect(btn, SIGNAL(clicked()), this, SLOT(button_clicked()));
	}

	ui->btnConfig->click();

	ui->txtTitle->setText(App::Title);
	connect(ui->txtTitle, SIGNAL(textChanged(QString)), this, SLOT(SaveConfig()));

	ui->txtVersion->setText(App::Version);
	connect(ui->txtVersion, SIGNAL(textChanged(QString)), this, SLOT(SaveConfig()));

	QStringList qssName;
	qssName << "黑色风格" << "淡蓝色风格";
	ui->cboxStyleName->addItems(qssName);

	if (App::StyleName == ":/Resources/qss/black.css") {
		ui->cboxStyleName->setCurrentIndex(0);
	}
	else if (App::StyleName == ":/Resources/qss/blue.css") {
		ui->cboxStyleName->setCurrentIndex(1);
	}

	connect(ui->cboxStyleName, SIGNAL(currentIndexChanged(int)), this, SLOT(SaveConfig()));

	ui->txtAuthor->setText(App::Author);
	connect(ui->txtAuthor, SIGNAL(textChanged(QString)), this, SLOT(SaveConfig()));

    //配置文件
    QString fileName = QString("%1/config/%2_Config.ini").arg(App::AppPath).arg(App::AppName);
    QSettings config(fileName, QSettings::IniFormat);

    //检测串口
    foreach (QSerialPortInfo info, QSerialPortInfo::availablePorts()) {
        ui->cmbCom->addItem(info.portName());
    }

    //设置串口
    config.beginGroup("SerialPort");
    QString com = config.value("com").toString();
    ui->cmbCom->setCurrentText(com);
    config.endGroup();


    //服务器设置
	config.beginGroup("Server");
	ui->txtSrvIP->setText(config.value("ip").toString());
	ui->txtSrvPort->setText(config.value("port").toString());
	config.endGroup();

	//地图设置
	config.beginGroup("Map");
	ui->mapFile->setText(config.value("mapFile").toString());
	config.endGroup();
	
    //检测打印机
    QPrinterInfo info;
    QStringList list = info.availablePrinterNames();

    for (QString name : list)
    {
        ui->cmbAlarmPrinter->addItem(name, name);
        ui->cmbRecordPrinter->addItem(name, name);
    }

	//打印机设置
	config.beginGroup("Printer");
	ui->cmbAlarmPrinter->setCurrentText(config.value("alarmPrinter").toString());
	ui->cmbRecordPrinter->setCurrentText(config.value("recordPrinter").toString());
	config.endGroup();
		
}

void frmConfig::button_clicked()
{
	QPushButton *btn = (QPushButton *)sender();
	QString name = btn->text();

	QList<QPushButton *> btns = ui->widget_left->findChildren<QPushButton *>();

	foreach(QPushButton * b, btns) {
		b->setChecked(false);
	}

	btn->setChecked(true);

    if (name == "系统设置") {
        ui->stackedWidget->setCurrentIndex(0);
    } else if (name == "高压电网") {
        ui->stackedWidget->setCurrentIndex(1);
    }
}

void frmConfig::on_btnSetInfo_clicked()
{
    QString title = ui->txtTitle->text();

    if (title.length() == 0 || title.length() == 0) {
        myHelper::ShowMessageBoxError("标题不能为空!");
        return;
    }

    QString version = ui->txtVersion->text();
    if (version.length() == 0 || version.length() == 0) {
        myHelper::ShowMessageBoxError("版本不能为空!");
        return;
    }

    QString author = ui->txtAuthor->text();
    if (author.length() == 0 || author.length() == 0) {
        myHelper::ShowMessageBoxError("版权不能为空!");
        return;
    }

    QString fileName = QString("%1/config/%2_Config.ini").arg(App::AppPath).arg(App::AppName);
    QSettings set(fileName, QSettings::IniFormat);

    set.beginGroup("AppConfig");

    set.setValue("Title", title);
    set.setValue("Version", version);
    set.setValue("Author", author);

    set.endGroup();
}

void frmConfig::SaveConfig()
{
    //	App::Title = ui->txtTitle->text();
    //	App::Version = ui->txtVersion->text();

    //	QString style;
    //	int styleIndex = ui->cboxStyleName->currentIndex();
    //	if (styleIndex == 0) {
    //		style = ":/Resources/qss/black.css";
    //	}
    //	else if (styleIndex == 1) {
    //		style = ":/Resources/qss/blue.css";
    //	}

    //	if (style != App::StyleName) {
    //		App::StyleName = style;
    //		myHelper::SetStyle(App::StyleName);
    //	}

    //	App::Author = ui->txtAuthor->text();

    //		//调用保存配置文件函数
    //	App::WriteConfig();
}

void frmConfig::on_btnSetSrv_clicked()
{
	QString srvIP = ui->txtSrvIP->text();
	QString srvPort = ui->txtSrvPort->text();

	if (srvIP.length() == 0 || srvPort.length() == 0) {
		myHelper::ShowMessageBoxError("网络参数不能为空!");
		return;
	}

	if (!myHelper::IsIP(srvIP)) {
		myHelper::ShowMessageBoxError("IP地址不合法!");
		ui->txtSrvIP->setFocus();
		return;
	}

	QString fileName = QString("%1/config/%2_Config.ini").arg(App::AppPath).arg(App::AppName);
	QSettings set(fileName, QSettings::IniFormat);
	
	set.beginGroup("Server");

	set.setValue("ip", srvIP);
	set.setValue("port", srvPort);

	set.endGroup();
}

void frmConfig::on_btnSavePrinter_clicked()
{
	QString fileName = QString("%1/config/%2_Config.ini").arg(App::AppPath).arg(App::AppName);
	QSettings set(fileName, QSettings::IniFormat);
	
	set.beginGroup("Printer");

	set.setValue("alarmPrinter", ui->cmbAlarmPrinter->currentText());
	set.setValue("recordPrinter", ui->cmbRecordPrinter->currentText());

	set.endGroup();
}

void frmConfig::on_btnSelectMap_clicked()
{
	//定义文件对话框类
	QFileDialog *fileDialog = new QFileDialog(this);
	
	//定义文件对话框标题
	fileDialog->setWindowTitle(tr("打开文件"));
	
	//设置默认文件路径
	fileDialog->setDirectory(".");
	
	//设置文件过滤器
	fileDialog->setNameFilter(tr("Images(*.png *.jpg *.jpeg *.bmp)"));
	
	//设置视图模式
	fileDialog->setViewMode(QFileDialog::Detail);
	
	//打印所有选择的文件的路径
	QStringList fileNames;
	
	if (fileDialog->exec())
	{
		ui->mapFile->setText(fileDialog->selectedFiles()[0]);

		QString fileName = QString("%1/config/%2_Config.ini").arg(App::AppPath).arg(App::AppName);
		QSettings set(fileName, QSettings::IniFormat);

		set.beginGroup("Map");

		set.setValue("mapFile", ui->mapFile->text());

		set.endGroup();
    }
}

void frmConfig::on_btnSaveCom_clicked()
{
    QString fileName = QString("%1/config/%2_Config.ini").arg(App::AppPath).arg(App::AppName);
    QSettings set(fileName, QSettings::IniFormat);

    set.beginGroup("SerialPort");

    set.setValue("com", ui->cmbCom->currentText());

    set.endGroup();
}
