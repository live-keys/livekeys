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
#include "QFilter2D.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

QFilter2D::QFilter2D(QQuickItem *parent)
    : QMatFilter(parent)
    , m_anchorCv(-1, -1)
    , m_borderType(BORDER_DEFAULT)
{
}

QFilter2D::~QFilter2D(){
}

void QFilter2D::transform(cv::Mat &in, cv::Mat &out){
    if ( m_kernel ){
        filter2D(in, out, m_ddepth, *(m_kernel->cvMat()), m_anchorCv, m_borderType);
    }
}
