#include "stdafx.h"
#include "datawindows.h"
#include "xtablewidgetitem.h"
#include "xsqlquerymodel.h"

#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>

DataWindows::DataWindows(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    ui.hvdataTable->setColumnWidth(0, 200);

    initDb();
    initForm();

    connect(ui.btnSearchData, &QPushButton::clicked, this, &DataWindows::searchHVData);

    dbDataTimer = new QTimer(this);
    connect(dbDataTimer, SIGNAL(timeout()), this, SLOT(updateData()));
    dbDataTimer->start(3000);
}

DataWindows::~DataWindows()
{
}

bool DataWindows::initDb()
{
    //server setting
    QString fileName = QString("%1/config/%2_Config.ini").arg(App::AppPath).arg(App::AppName);
    QSettings set(fileName, QSettings::IniFormat);
    set.beginGroup("Server");
    QString srvIP = set.value("ip").toString();
    int srvPort = set.value("port").toInt();
    set.endGroup();

    qDebug() << "srvIp : " << srvIP;
    qDebug() << "before db.connectionName:"<<db.connectionName();

   // this->db = QSqlDatabase::addDatabase("QPSQL");

    if(QSqlDatabase::contains("qt_sql_default_connection"))
    {
        this->db = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        this->db = QSqlDatabase::addDatabase("QPSQL");
    }
    qDebug() << "after db.connectionName:"<<db.connectionName();

    db.setHostName(srvIP);
    db.setPort(srvPort);
    db.setDatabaseName("hvdata");
    db.setUserName("hvuser");
    db.setPassword("zja@1234");



    bool openStatus = db.open();

    if(!openStatus) {

        qDebug() << "打开数据库失败"<<db.lastError().text();

    }

    return openStatus;
}

void DataWindows::initForm()
{
    //0.初始化界面
    ui.dteStart->setDateTime(QDateTime(QDate::currentDate(), QTime(0,0,0)));
    ui.dteEnd->setDateTime(QDateTime(QDate::currentDate(), QTime(23,59,59)));

    //1.初始化TableWidget, 设置列
    QStringList headerTitle;
    int defenceCount;

    if(this->db.isOpen()) {
        QSqlQuery sql("SELECT name FROM defence ORDER BY id", this->db);

        while (sql.next())
        {
            defenceCount = ui.hvdataTable->columnCount();

            qDebug() << defenceCount;
            ui.hvdataTable->insertColumn(defenceCount);

            QString name = sql.value("name").toString();
            headerTitle << name;

            headerCols[name] = defenceCount;

            qDebug() << name << " : " << defenceCount;
        }
    }

    ui.hvdataTable->setHorizontalHeaderLabels(headerTitle);
    ui.cmbDefence->addItems(headerTitle);

    //1.1.设置行
    QStringList rowHeaderTitle = {"A相电压(V)", "B相电压(V)", "AB电压(V)", "A相电流(mA)", "B相电流(mA)", "输入电流(mA)"};
    int rowCount = rowHeaderTitle.count();
    ui.hvdataTable->setRowCount(rowCount);

    ui.hvdataTable->setVerticalHeaderLabels(rowHeaderTitle);

    //1.2.设置单元格
    for(int row = 0; row < rowCount; row++) {
        for(int col = 0; col <= defenceCount; col++) {
            XTableWidgetItem *item = new XTableWidgetItem();
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui.hvdataTable->setItem(row, col, item);
        }
    }

    //1.3.设置更改事件
    //    connect(ui.hvdataTable, &QTableWidget::itemChanged, this, &DataWindows::itemUpdated);
    //    connect(ui.hvdataTable, &QTableWidget::cellChanged, this, &DataWindows::cellUpdated);
}

QString DataWindows::buildSqlModelFilter()
{
    //0.构造where语句
    QString where = " where 1 = 1 "
            + (ui.cmbDefence->currentIndex() > 0 ? " and name = '" + ui.cmbDefence->currentText() + "'" : "");

    QString filter = QString("select * from (select * from realtime_data " + where + " order by id desc limit 10000) where time > '%1' and time < '%2' order by id desc limit %3")
            .arg(ui.dteStart->dateTime().toString("yyyy-MM-dd hh:mm:ss"))
            .arg(ui.dteEnd->dateTime().toString("yyyy-MM-dd hh:mm:ss"))
            .arg(ui.ledLimit->text());

    qDebug() << filter;

    return filter;
}

void DataWindows::showData(HVData data)
{
    int zone = data.chn;
    int col = zone + 1;

    //ui.hvdataTable->item(0, col)->setText(QString::number(data.hua));
    //ui.hvdataTable->item(1, col)->setText(QString::number(data.hub));
    ui.hvdataTable->item(0, col)->setText(QString::number(data.huab));
    //ui.hvdataTable->item(3, col)->setText(QString::number(data.hia));
    //ui.hvdataTable->item(4, col)->setText(QString::number(data.hib));
    ui.hvdataTable->item(1, col)->setText(QString::number(data.hiab));
    /*
    ui.hvdataTable->item(6, col)->setText(QString::number(data.lua));
    ui.hvdataTable->item(7, col)->setText(QString::number(data.lub));
    ui.hvdataTable->item(8, col)->setText(QString::number(data.lia));
    ui.hvdataTable->item(9, col)->setText(QString::number(data.lib));
    ui.hvdataTable->item(10, col)->setText(QString::number(data.tempra));
    ui.hvdataTable->item(11, col)->setText(QString::number(data.wetratio));
    ui.hvdataTable->item(12, col)->setText(QString::number(data.iu));
    ui.hvdataTable->item(13, col)->setText(QString::number(data.ii));
    ui.hvdataTable->item(14, col)->setText(QString::number(data.oia));
    ui.hvdataTable->item(15, col)->setText(QString::number(data.oib));
    */
}

void DataWindows::updateData()
{
    if(!this->db.isOpen()) {
        qDebug() << "数据库重连";

        if(!initDb()) {
            return;
        }
    }

    QSqlQuery sql("SELECT a_line_v, a_line_i, b_line_v, b_line_i, source_i, name FROM realtime_data ORDER BY id DESC LIMIT 22", this->db);

    while (sql.next())
    {
        int av = sql.value("a_line_v").toInt();
        int ai = sql.value("a_line_i").toInt();
        int bv = sql.value("b_line_v").toInt();
        int bi = sql.value("b_line_i").toInt();
        int si = sql.value("source_i").toInt();
        QString colName = sql.value("name").toString();

        int col = headerCols[colName];

        //        qDebug() << colName << " : " << col;

        XTableWidgetItem *itemA = (XTableWidgetItem*) ui.hvdataTable->item(0, col);
        itemA->setFlashText(QString::number(av), 1000, Qt::GlobalColor::green);

        XTableWidgetItem *itemB = (XTableWidgetItem*) ui.hvdataTable->item(1, col);
        itemB->setFlashText(QString::number(bv), 1000, Qt::GlobalColor::green);

        XTableWidgetItem *itemAB = (XTableWidgetItem*) ui.hvdataTable->item(2, col);
        itemAB->setFlashText(QString::number(av + bv), 1000, Qt::GlobalColor::green);

        XTableWidgetItem *itemAI = (XTableWidgetItem*) ui.hvdataTable->item(3, col);
        itemAI->setFlashText(QString::number(ai), 1000, Qt::GlobalColor::green);

        XTableWidgetItem *itemBI = (XTableWidgetItem*) ui.hvdataTable->item(4, col);
        itemBI->setFlashText(QString::number(bi), 1000, Qt::GlobalColor::green);

        XTableWidgetItem *itemSI = (XTableWidgetItem*) ui.hvdataTable->item(5, col);
        itemSI->setFlashText(QString::number(si), 1000, Qt::GlobalColor::green);
    }
}

void DataWindows::searchHVData()
{
    //1.设置model
    if(db.isOpen()) {
        XSqlQueryModel *oldModel = (XSqlQueryModel*) ui.hvdataView->model();

        if(oldModel != nullptr) {
            oldModel->clear();
            delete oldModel;
        }

        XSqlQueryModel *hvdataModel = new XSqlQueryModel(this);
        hvdataModel->setQuery(buildSqlModelFilter(), db);

        hvdataModel->setHeaderData(1,Qt::Horizontal,"防区");
        hvdataModel->setHeaderData(2,Qt::Horizontal,"状态");
        hvdataModel->setHeaderData(3,Qt::Horizontal,"A相电压");
        hvdataModel->setHeaderData(5,Qt::Horizontal,"B相电压");
        hvdataModel->setHeaderData(4,Qt::Horizontal,"A相电流");
        hvdataModel->setHeaderData(6,Qt::Horizontal,"B相电流");
        hvdataModel->setHeaderData(7,Qt::Horizontal,"低压电压");
        hvdataModel->setHeaderData(8,Qt::Horizontal,"低压电流");
        hvdataModel->setHeaderData(9,Qt::Horizontal,"时间");

        //        QSqlTableModel *hvdataModel = new QSqlTableModel(this, db);
        //        hvdataModel->setTable("realtime_data");
        //        hvdataModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

        //        hvdataModel->setFilter(buildSqlModelFilter());
        //        hvdataModel->select();

        //        hvdataModel->setHeaderData(hvdataModel->fieldIndex("name"),Qt::Horizontal,"防区");
        //        hvdataModel->setHeaderData(hvdataModel->fieldIndex("a_line_v"),Qt::Horizontal,"A相电压");
        //        hvdataModel->setHeaderData(hvdataModel->fieldIndex("b_line_v"),Qt::Horizontal,"B相电压");
        //        hvdataModel->setHeaderData(hvdataModel->fieldIndex("a_line_i"),Qt::Horizontal,"A相电流");
        //        hvdataModel->setHeaderData(hvdataModel->fieldIndex("b_line_i"),Qt::Horizontal,"B相电流");
        //        hvdataModel->setHeaderData(hvdataModel->fieldIndex("source_v"),Qt::Horizontal,"输入电压");
        //        hvdataModel->setHeaderData(hvdataModel->fieldIndex("time"),Qt::Horizontal,"时间");

        ui.hvdataView->setModel(hvdataModel);
        ui.hvdataView->hideColumn(0); //隐藏id
        ui.hvdataView->hideColumn(10); //隐藏metadata
        ui.hvdataView->show();
    }
}
