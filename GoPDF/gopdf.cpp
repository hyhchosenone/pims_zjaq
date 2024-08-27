#include "gopdf.h"

GoPDF::GoPDF(const QString &filename)
    : QPdfWriter(filename)
{

}

GoPDF::~GoPDF()
{

}
