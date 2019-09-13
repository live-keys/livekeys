/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#ifndef QLCVVIDEOGLOBAL_H
#define QLCVVIDEOGLOBAL_H

#include <qglobal.h>

/**
  \defgroup plugin-lcvvideo lcvvideo Plugin

  Open CV video processing plugin.
*/

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
