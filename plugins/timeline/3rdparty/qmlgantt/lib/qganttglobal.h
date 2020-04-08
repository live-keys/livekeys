/****************************************************************************
**
** Copyright (C) 2015-2016 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of QML Gantt library.
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

#ifndef QGANTTGLOBAL_H
#define QGANTTGLOBAL_H

#include <qglobal.h>

#define QML_GANTT_VERSION_MAJOR 0
#define QML_GANTT_VERSION_MINOR 1
#define QML_GANTT_VERSION_PATCH 0

#ifndef Q_GANTT_STATIC
#  ifdef Q_GANTT_LIB
#    define Q_GANTT_EXPORT Q_DECL_EXPORT
#  else
#    define Q_GANTT_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define Q_GANTT_EXPORT
#endif

#endif // QGANTTGLOBAL_H
