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

#ifndef LVELEMENTSGLOBAL_H
#define LVELEMENTSGLOBAL_H

#include "live/lvglobal.h"

/**
  \defgroup lvelements lvelements Library

  Contains the engine, compilers and main classes for Live Elements.
*/


#ifndef LV_ELEMENTS_ENGINE_STATIC
#  ifdef LV_ELEMENTS_ENGINE_LIB
#    define LV_ELEMENTS_ENGINE_EXPORT  LV_EXPORT
#  else
#    define LV_ELEMENTS_ENGINE_EXPORT  LV_IMPORT
#  endif
#else
#  define LV_ELEMENTS_EXPORT
#endif

#endif //LVELEMENTSGLOBAL
