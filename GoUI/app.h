#ifndef APP_H
#define APP_H

#include "GoUI_global.h"
#include <QList>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

class GOUI_EXPORT App
{
public:
    //0.系统运行环境设置,该部分在设置界面不体现，需通过编程修改
    static QString AppName;         //应用程序英文名称
    static QString AppPath;         //应用程序路径
    static int DeskWidth;           //运行环境桌面宽度
    static int DeskHeight;          //运行环境桌面高度
    static int FontSize;            //应用程序字体大小
    static QString FontName;        //应用程序字体名称
    static QString PaletteColor;    //画板背景色

    static int IcoMain;             //左上角图标
    static int IcoMenu;             //下来菜单图标
    static int IcoMin;              //最小化图标
    static int IcoMax;              //最大化图标
    static int IcoNormal;           //正常图标
    static int IcoClose;            //关闭图标

    static bool SaveLog;            //输出日志文件
    static bool UseInput;           //是否启用输入法
    static QString InputPosition;   //输入法显示位置

    static QString KeyData;         //注册码密文
    static bool KeyUseDate;         //是否启用运行日期时间限制
    static QString KeyDate;         //到期时间字符串
    static bool KeyUseRun;          //是否启用可运行时间限制
    static int KeyRun;              //可运行时间
    static bool KeyUseCount;        //是否启用设备数量限制
    static int KeyCount;            //设备限制数量

    //1.系统设置 -- 基本设置
    static QString Title;           //应用程序标题
    static QString Version;         //应用程序版本
    static QString StyleName;       //应用程序样式
    static QString Author;          //版权所有

    static QString ServerIP;		//中心服务器IP
    static int ServerPort;			//中心服务器端口

    //3.车牌识别
    static QString EnterIP;           //入口设备IP
    static QString EnterDeviceType;   //入口设备类型
    static QString EnterName;		  //入口名称
    static QString EnterSavePath;     //入口抓拍存储路径

    static QString ExitIP;           //出口设备IP
    static QString ExitDeviceType;   //出口设备类型
    static QString ExitName;		 //出口名称
    static QString ExitSavePath;     //出口抓拍存储路径

    static QString VLPRType;		 //识别库类型
    static QString NamePriority;	 //省份优先级


    static void ReadConfig();       //读取配置文件,在main函数最开始加载程序载入
    static void WriteConfig();      //写入配置文件,在更改配置文件程序关闭时调用

    static QList<QString> TaskList;             //待处理操作任务链表
    static QList<QString> SnapList;             //待保存的抓拍告警图像名称
    static QList<QString> EmailList;            //待发送告警邮件链表
    static QList<QString> MsgList;              //待发送告警邮件链表
    static void AppendTask(QString task);       //插入待处理操作任务
    static void AppendSnap(QString snap);       //插入待保存告警图像名称队列
    static void AppendEmail(QString email);     //插入待发送告警邮件链表
    static void AppendMsg(QString msg);         //插入待发送告警邮件链表

};

#endif // APP_H
