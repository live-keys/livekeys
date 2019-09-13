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
#include "qdilate.h"
#include "opencv2/imgproc.hpp"

using namespace cv;

/*!
  \class QDilate
  \inmodule lcvimgproc_cpp
  \internal
  \brief Erodes an image by using a specific structuring element.
 */

/*!
  \brief QDilate constructor

  Parameters:
  \a parent
 */
QDilate::QDilate(QQuickItem *parent)
    : QMatFilter(parent)
    , m_kernel(0)
    , m_anchorCv(-1, -1)
    , m_iterations(1)
    , m_borderType(BORDER_CONSTANT)
{
}

/*!
  \brief QDilate destructor
 */
QDilate::~QDilate(){
}


/*!
  \property QDilate::kernel
  \sa Dilate::kernel
 */

/*!
  \property QDilate::anchor
  \sa Dilate::anchor
 */

/*!
  \property QDilate::iterations
  \sa Dilate::iterations
 */

/*!
  \property QDilate::borderType
  \sa Dilate::borderType
 */

/*!
  \property QDilate::borderValue
  \sa Dilate::borderValue
 */

/*!
  \brief Filtering function.

  Parameters:
  \a in
  \a out
 */
void QDilate::transform(const cv::Mat &in, cv::Mat &out){
    if ( m_kernel ){
        Scalar borderValue = morphologyDefaultBorderValue();
        if ( m_borderValue.isValid() ){
            if ( in.channels() == 3 )
                borderValue = Scalar(m_borderValue.blue(), m_borderValue.green(), m_borderValue.red());
            else
                borderValue = Scalar(m_borderValue.red());
        }
        dilate(in, out, *(m_kernel->cvMat()), m_anchorCv, m_iterations, m_borderType, borderValue);
    }
}
