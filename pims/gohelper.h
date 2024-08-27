#ifndef GOHELPER_H
#define GOHELPER_H

#include <QObject>

class GoHelper : public QObject
{
    Q_OBJECT

public:
    explicit GoHelper(QObject *parent = 0);

    static QByteArray QString2Hex(QString str);    //16进制转换函数
    static quint8 convertUChar2Hex(const quint8 ch);
    static quint8 convertInt2UChar(const int i); //整形转换为字符型
    static quint16 CRC16Modbus(const QByteArray &data);
    static quint8 crcSum(const int sum); //返回0xff与sum的低字节的差

signals:
    
public slots:
    
};

#endif // GOHELPER_H
