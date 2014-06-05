/****************************************************************************
**
** Copyright (C) 2014 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV application.
**
** GNU General Public License Usage
** 
** This file may be used under the terms of the GNU General Public License 
** version 3.0 as published by the Free Software Foundation and appearing 
** in the file LICENSE.GPL included in the packaging of this file.  Please 
** review the following information to ensure the GNU General Public License 
** version 3.0 requirements will be met: http://www.gnu.org/copyleft/gpl.html.
**
****************************************************************************/

#include "QMatEmpty.hpp"

QMatEmpty::QMatEmpty(QQuickItem *parent) :
    QMatDisplay(parent),
    m_matSize(0, 0),
    m_color(0, 0, 0),
    m_channels(1),
    m_type(QMat::CV8U),
    m_colorScalar(0)
{
}

QMatEmpty::~QMatEmpty(){
}
