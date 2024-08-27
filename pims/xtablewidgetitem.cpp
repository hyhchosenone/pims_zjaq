#include "xtablewidgetitem.h"

#include <QBrush>
#include <QTimer>

XTableWidgetItem::XTableWidgetItem()
{

}

void XTableWidgetItem::setFlashText(const QString &text, int flashTime, QColor flashColor)
{
    if(this->text() != text) {
        setText(text);
        setBackground(flashColor);

        QTimer::singleShot(flashTime, this, [&](){
            setBackgroundColor(Qt::white);
        });
    }
}
