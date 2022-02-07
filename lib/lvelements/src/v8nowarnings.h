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

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)

#pragma warning(push)
#pragma warning(disable : 4100)
#define V8_COMPRESS_POINTERS

#include "v8.h"
#include "libplatform/libplatform.h"

#pragma warning(pop)

#elif defined(__clang__)

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

#define V8_COMPRESS_POINTERS

#include "v8.h"
#include "libplatform/libplatform.h"

#pragma clang diagnostic pop

#else

#include "v8.h"
#include "libplatform/libplatform.h"

#endif
