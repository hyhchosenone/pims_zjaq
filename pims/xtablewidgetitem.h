#ifndef XTABLEWIDGETITEM_H
#define XTABLEWIDGETITEM_H

#include <QTableWidgetItem>

class XTableWidgetItem : public QObject,  public QTableWidgetItem
{
    Q_OBJECT
public:
    XTableWidgetItem();

    void setFlashText(const QString &text, int flashTime, QColor flashColor);

private:
    QColor preBackgroundColor;
    QString qss;
};

#endif // XTABLEWIDGETITEM_H
