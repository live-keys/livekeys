/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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
#include "qcopymakeborder.h"
#include "opencv2/imgproc.hpp"

using namespace cv;

/*!
  \class QCopyMakeBorder
  \internal
  \inmodule lcvimgproc_cpp
  \brief Copies the source image into the middle of the destination image.
 */
QCopyMakeBorder::QCopyMakeBorder(QQuickItem *parent)
    : QMatFilter(parent)
    , m_color()
{
}

QCopyMakeBorder::~QCopyMakeBorder(){
}

/*!
  \enum QCopyMakeBorder::BorderType

  \value BORDER_REPLICATE
  \value BORDER_CONSTANT
  \value BORDER_REFLECT
  \value BORDER_WRAP
  \value BORDER_REFLECT_101
  \value BORDER_TRANSPARENT
  \value BORDER_DEFAULT
  \value BORDER_ISOLATED
 */

/*!
  \brief Filtering function.

  Parameters:
  \a in
  \a out
 */
void QCopyMakeBorder::transform(const cv::Mat &in, cv::Mat &out){
    Scalar value;
    if ( m_color.isValid() ){
        if ( out.channels() == 1 )
            value = m_color.red();
        else if ( out.channels() == 3 )
            value = Scalar(m_color.blue(), m_color.green(), m_color.red());
    }
    copyMakeBorder(in, out, m_top, m_bottom, m_left, m_right, m_borderType, value);
}
