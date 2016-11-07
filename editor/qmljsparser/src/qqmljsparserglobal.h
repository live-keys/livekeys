#ifndef QQMLJSPARSERGLOBAL_H
#define QQMLJSPARSERGLOBAL_H

#include <qglobal.h>

#ifndef Q_QMLJSPARSER_STATIC
#  ifdef Q_QMLJSPARSER_LIB
#    define Q_QMLJSPARSER_EXPORT Q_DECL_EXPORT
#  else
#    define Q_QMLJSPARSER_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define Q_LCV_EXPORT
#endif

#endif // QQMLJSPARSERGLOBAL_H
