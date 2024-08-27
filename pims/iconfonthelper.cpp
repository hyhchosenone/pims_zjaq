#include "iconfonthelper.h"
#include <QFontDatabase>

IconfontHelper *IconfontHelper::instance = nullptr;

IconfontHelper::IconfontHelper(QObject *parent)
    : QObject(parent)
{
    int fontId = QFontDatabase::addApplicationFont("://Resources/font/fontawesome_470.ttf");
    QString fontName = QFontDatabase::applicationFontFamilies(fontId).at(0);
    iconfont = QFont(fontName);
}

IconfontHelper::~IconfontHelper()
{
}

void IconfontHelper::setIcon(QLabel* lab, QChar c, int size)
{
    iconfont.setPointSize(size);
    lab->setFont(iconfont);
    lab->setText(c);
}

void IconfontHelper::setIcon(QAbstractButton* btn, QChar c, int size)
{
    iconfont.setPointSize(size);
    btn->setFont(iconfont);
    btn->setText(c);
}

void IconfontHelper::setIcon(QAbstractButton* btn, QChar icon, QString text, int size)
{
    iconfont.setPointSize(size);
    btn->setFont(iconfont);
    btn->setText(QString(icon) + " " + text);
}
