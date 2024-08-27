#ifndef GOPDF_GLOBAL_H
#define GOPDF_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GOPDF_LIBRARY)
#  define GOPDF_EXPORT Q_DECL_EXPORT
#else
#  define GOPDF_EXPORT Q_DECL_IMPORT
#endif

#endif // GOPDF_GLOBAL_H
