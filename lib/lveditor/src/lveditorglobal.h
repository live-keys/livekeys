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

#ifndef LVEDITORGLOBAL_H
#define LVEDITORGLOBAL_H

#include <qglobal.h>

/**
  \defgroup lveditor lveditor Library
  \brief Contains editor functionality and extension hooks.
  */

#ifndef LV_EDITOR_STATIC
#  ifdef LV_EDITOR_LIB
#    define LV_EDITOR_EXPORT Q_DECL_EXPORT
#  else
#    define LV_EDITOR_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define LV_EDITOR_EXPORT
#endif

// #define LV_EDITOR_DEBUG

#endif //LVEDITORGLOBAL_H
