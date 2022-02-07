/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#ifndef LVLIVEGLOBAL_H
#define LVLIVEGLOBAL_H

#include <qglobal.h>

/**
  \defgroup plugin-live live Plugin

  Plugin oriented towards live coding.
  */

#ifndef Q_LIVE_STATIC
#  ifdef Q_LIVE_LIB
#    define LV_LIVE_EXPORT Q_DECL_EXPORT
#  else
#    define LV_LIVE_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define LV_LIVE_EXPORT
#endif

#endif // LVLIVEGLOBAL_H
