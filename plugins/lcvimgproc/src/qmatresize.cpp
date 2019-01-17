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

#include "qmatresize.h"
#include "opencv2/imgproc.hpp"

using namespace cv;

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
void QMatResize::transform(const cv::Mat& in, cv::Mat& out){
    if ( !in.empty() && (m_matSize != Size(0, 0) || m_fx != 0 || m_fy != 0) ){
        resize(in, out, m_matSize, m_fx, m_fy, m_interpolation);
        if ( out.rows != implicitHeight() )
            setImplicitHeight(out.rows);
        if ( out.cols != implicitWidth() )
            setImplicitWidth(out.cols);
    }
}
