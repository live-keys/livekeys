#ifndef QLCVEDITORGLOBAL_H
#define QLCVEDITORGLOBAL_H

#include <qglobal.h>

#ifndef Q_LCVEDITOR_STATIC
#  ifdef Q_LCVEDITOR_LIB
#    define Q_LCVEDITOR_EXPORT Q_DECL_EXPORT
#  else
#    define Q_LCVEDITOR_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define Q_LCVEDITOR_EXPORT
#endif

#endif //QLCVEDITORGLOBAL_H
