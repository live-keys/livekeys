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

#include "qmatresize.h"
#include "opencv2/imgproc.hpp"

using namespace cv;

/*!
  \qmltype Resize
  \instantiates QMatResize
  \inqmlmodule lcvimgproc
  \inherits MatFilter
  \brief Resizes an image.

  Resizes an image according to the specified filter.

  \quotefile imgproc/resize.qml
*/

/*!
  \class QMatResize
  \inmodule lcvimgproc_cpp
  \internal
  \brief Finds edges within an image.
 */

/*!
  \brief QMatResize constructor
  Parameters:
  \a parent
 */
QMatResize::QMatResize(QQuickItem *parent)
    : QMatFilter(parent)
    , m_matSize(0, 0)
    , m_fx(0)
    , m_fy(0)
    , m_interpolation(INTER_LINEAR)
{
}

/*!
  \brief QMatResize destructor
 */
QMatResize::~QMatResize(){
}

/*!
  \qmlproperty Size Resize::matSize

  Size of the resized matrix. An unspecified size or a size of (0, 0) will make the element look into the
  fx and fy resize factors.
 */

/*!
  \qmlproperty real Resize::fx

  Factor by which to resize on x axis.
 */


/*!
  \qmlproperty real Resize::fy

  Factor by which to resize on y axis.
 */

/*!
  \qmlproperty Resize::Interpolation Resize::interpolation

  Interpolation method.
 */


/*!
  \qmlproperty enumeration Resize::Interpolation

  Interpolation method:
  \list
  \li Resize.INTER_NEAREST nearest neighbor interpolation
  \li Resize.INTER_LINEAR bilinear interpolation
  \li Resize.INTER_CUBIC bicubic interpolation
  \li Resize.INTER_AREA area-based (or super) interpolation
  \li Resize.INTER_LANCZOS4 Lanczos interpolation over 8x8 neighborhood
  \li Resize.INTER_MAX nearest neighbor interpolation
  \li Resize.WARP_INVERSE_MAP nearest neighbor interpolation
  \endlist
 */

/*!
  \enum QMatResize::Interpolation

  \value INTER_NEAREST
  \value INTER_LINEAR
  \value INTER_CUBIC
  \value INTER_AREA
  \value INTER_LANCZOS4
  \value INTER_MAX
  \value WARP_INVERSE_MAP
 */

/*!
  \brief Filtering function.

  Parameters:
  \a in
  \a out
 */
void QMatResize::transform(cv::Mat& in, cv::Mat& out){
    if ( !in.empty() && (m_matSize != Size(0, 0) || m_fx != 0 || m_fy != 0) ){
        resize(in, out, m_matSize, m_fx, m_fy, m_interpolation);
        if ( out.rows != implicitHeight() )
            setImplicitHeight(out.rows);
        if ( out.cols != implicitWidth() )
            setImplicitWidth(out.cols);
    }
}
