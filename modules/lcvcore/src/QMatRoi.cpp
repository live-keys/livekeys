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

#include "QMatRoi.hpp"

QMatRoi::QMatRoi(QQuickItem *parent) :
    QMatFilter(parent)
{
}

void QMatRoi::transform(cv::Mat &in, cv::Mat &out){
    if ( in.cols > m_regionX + m_regionWidth && in.rows > m_regionY + m_regionHeight )
        in(cv::Rect(m_regionX, m_regionY, m_regionWidth, m_regionHeight)).copyTo(out);
}
