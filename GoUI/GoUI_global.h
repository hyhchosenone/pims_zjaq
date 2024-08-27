#ifndef GOUI_GLOBAL_H
#define GOUI_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GOUI_LIBRARY)
#  define GOUI_EXPORT Q_DECL_EXPORT
#else
#  define GOUI_EXPORT Q_DECL_IMPORT
#endif

#endif // GOUI_GLOBAL_H
