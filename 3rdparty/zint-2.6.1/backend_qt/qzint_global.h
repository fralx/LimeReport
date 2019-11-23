#ifndef QZINT_GLOBAL_H
#define QZINT_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef HAVE_STATIC_BUILD
#  define QZINTSHARED_EXPORT /**/
#else
#if defined(QZINT_LIBRARY)
#  define QZINTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QZINTSHARED_EXPORT Q_DECL_IMPORT
#endif
#endif

#endif // QZINT_GLOBAL_H
