/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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
#include "qfilter2d.h"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

/*!
  \qmltype Filter2D
  \instantiates QFilter2D
  \inqmlmodule lcvimgproc
  \inherits MatFilter
  \brief Converts an image with the specified kernel

  \quotefile imgproc/filter2D.qml
*/

/*!
  \class QFilter2D
  \inmodule lcvimgproc_cpp
  \brief Converts an image with the specified kernel
 */


/*!
  \brief QFilter2D constructor

  Parameters:
  \a parent
 */
QFilter2D::QFilter2D(QQuickItem *parent)
    : QMatFilter(parent)
    , m_kernel(0)
    , m_ddepth(-1)
    , m_anchorCv(-1, -1)
    , m_delta(0)
    , m_borderType(BORDER_DEFAULT)
{
}

/*!
  \brief QFilter2D destructor
 */
QFilter2D::~QFilter2D(){
}

/*!
  \property QFilter2D::ddepth
  \sa Filter2D::ddepth
 */

/*!
  \qmlproperty int Filter2D::ddepth

  Desired depth of the destination image; if it is negative, it will be the same as input.depth(); the following
  combinations of src.depth() and ddepth are supported:
        input.depth() = Mat.CV_8U, ddepth = -1/Mat.CV_16S/Mat.CV_32F/Mat.CV_64F
        input.depth() = Mat.CV_16U/CV_16S, ddepth = -1/Mat.CV_32F/Mat.CV_64F
        input.depth() = Mat.CV_32F, ddepth = -1/Mat.CV_32F/Mat.CV_64F
        input.depth() = Mat.CV_64F, ddepth = -1/Mat.CV_64F,

  where ddepth = -1 will yield an output the same as the source.
 */

/*!
  \property QFilter2D::kernel
  \sa Filter2D::kernel
 */

/*!
  \qmlproperty Mat Filter2D::kernel

  Convolution kernel (or rather a correlation kernel), a single-channel floating point matrix.
 */


/*!
  \property QFilter2D::anchor
  \sa Filter2D::anchor
 */

/*!
  \qmlproperty Point Filter2D::anchor

  Anchor of the kernel that indicates the relative position of a filtered point within the kernel; the anchor should lie
  within the kernel; default value (-1,-1) means that the anchor is at the kernel center.
 */

/*!
  \property QFilter2D::delta
  \sa Filter2D::delta
 */

/*!
  \qmlproperty real Filter2D::delta

  Optional value added to the filtered pixels before storing them in Filter2D.output.
 */

/*!
  \property QFilter2D::borderType
  \sa Filter2D::borderType
 */

/*!
  \qmlproperty int Filter2D::borderType

  Pixel extrapolation method (see \l{CopyMakeBorder::BorderType}{CopyMakeBorder::BorderType} for details).
 */


/*!
  \brief Filtering function.

  Parameters:
  \a in
  \a out
 */
void QFilter2D::transform(cv::Mat &in, cv::Mat &out){
    if ( m_kernel ){
        Mat* m = m_kernel->cvMat();
        if ( m->cols > 0 || m->rows > 0 ){
            filter2D(in, out, m_ddepth, *(m_kernel->cvMat()), m_anchorCv, m_delta, m_borderType);
        }
    }
}
