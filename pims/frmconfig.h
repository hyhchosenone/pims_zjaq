#ifndef FRMCONFIG_H
#define FRMCONFIG_H

#include <QWidget>
#include <QDateTime>

namespace Ui
{
    class frmConfig;
}

class frmConfig : public QWidget
{
    Q_OBJECT

public:
    explicit frmConfig(QWidget *parent = 0);
    ~frmConfig();

private slots:
    void SaveConfig();
    void button_clicked();
    void on_btnSetInfo_clicked();
    void on_btnSetSrv_clicked();
	void on_btnSavePrinter_clicked();
	void on_btnSelectMap_clicked();
    void on_btnSaveCom_clicked();

private:
    Ui::frmConfig *ui;

    void InitForm();

};

#endif // FRMCONFIG_H
