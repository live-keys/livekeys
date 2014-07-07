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
#include "QCopyMakeBorder.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

QCopyMakeBorder::QCopyMakeBorder(QQuickItem *parent)
    : QMatFilter(parent)
    , m_color()
{
}

QCopyMakeBorder::~QCopyMakeBorder(){
}

void QCopyMakeBorder::transform(cv::Mat &in, cv::Mat &out){
    Scalar value;
    if ( m_color.isValid() ){
        if ( out.channels() == 1 )
            value = m_color.red();
        else if ( out.channels() == 3 )
            value = Scalar(m_color.blue(), m_color.green(), m_color.red());
    }
    copyMakeBorder(in, out, m_top, m_bottom, m_left, m_right, m_borderType, value);
}
