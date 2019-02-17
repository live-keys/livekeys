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

#include "qmatempty.h"

/**
  *\class QMatEmpty
  *\internal
  *\brief Creates an empty matrix.
 */


QMatEmpty::QMatEmpty(QQuickItem *parent) :
    QMatDisplay(parent),
    m_matSize(0, 0),
    m_color(0, 0, 0),
    m_channels(1),
    m_type(QMat::CV8U),
    m_colorScalar(0)
{
}

/**
 *\fn void QMatEmpty::cacheScalarColor()
 *\brief Helper on caching the scalar color
 */



QMatEmpty::~QMatEmpty(){
}