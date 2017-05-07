#ifndef QLCVVIDEOGLOBAL_H
#define QLCVVIDEOGLOBAL_H

#include <qglobal.h>

#ifndef Q_LCVVIDEO_STATIC
#  ifdef Q_LCVVIDEO_LIB
#    define Q_LCVVIDEO_EXPORT Q_DECL_EXPORT
#  else
#    define Q_LCVVIDEO_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define Q_LCV_EXPORT
#endif

#endif // QLCVVIDEOGLOBAL_H
