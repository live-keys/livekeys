/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#ifndef QLCVFEATURES2DGLOBAL_H
#define QLCVFEATURES2DGLOBAL_H
/// \private
#include <qglobal.h>

/**
  \defgroup plugin-lcvfeatures2d lcvfeatures2d Plugin

  Open CV features 2d plugin.
*/

#ifndef Q_LCVFEATURES2D_STATIC
#  ifdef Q_LCVFEATURES2D_LIB
#    define Q_LCVFEATURES2D_EXPORT Q_DECL_EXPORT
#  else
#    define Q_LCVFEATURES2D_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define Q_LCV_EXPORT
#endif

#endif // QLCVFEATURES2DGLOBAL_H

