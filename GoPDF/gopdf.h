#ifndef GOPDF_H
#define GOPDF_H

#include "GoPDF_global.h"
#include <QObject>
#include <QPdfWriter>

class GOPDF_EXPORT GoPDF : public QPdfWriter
{
    Q_OBJECT
public:
    GoPDF(const QString &filename);
    ~GoPDF();


};

#endif // GOPDF_H
