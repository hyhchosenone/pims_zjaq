#pragma once

#include <QSqlDatabase>
#include <QTimer>
#include <QWidget>
#include "ui_datawindows.h"
#include "hvdata.h"

class DataWindows : public QWidget
{
    Q_OBJECT

public:
    DataWindows(QWidget *parent = Q_NULLPTR);
    ~DataWindows();

public slots:
    void showData(HVData hvdata);
    void updateData();

signals:
    void updateAlarm(int zone, int alarm);

private slots:
    void searchHVData();
//    void itemUpdated(QTableWidgetItem *item);
//    void cellUpdated(int row, int col);

private:
    bool initDb(); //初始化数据库
    void initForm(); //初始化表格
    QString buildSqlModelFilter(); //构造filter

private:
    Ui::DataWindows ui;
    QSqlDatabase db;
    QTimer *dbDataTimer;
    QHash<QString, int> headerCols;
    QTableWidgetItem *preItem;
    int preRow = 0;
    int preCol = 0;
};
