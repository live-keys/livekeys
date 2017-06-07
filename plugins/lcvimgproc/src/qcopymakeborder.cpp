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
#include "qcopymakeborder.h"
#include "opencv2/imgproc.hpp"

using namespace cv;

/*!
  \qmltype CopyMakeBorder
  \instantiates QCopyMakeBorder
  \inqmlmodule lcvimgproc
  \inherits MatFilter
  \brief Copies the source image into the middle of the destination image.

  The areas to the left, to the right, above and below the copied source image will be filled with extrapolated pixels

  \quotefile imgproc/copymakeborder.qml
*/

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
  \qmlproperty enumeration CopyMakeBorder::BorderType

  Extrapolation method border type:
  \list
  \li CopyMakeBorder.BORDER_REPLICATE
  \li CopyMakeBorder.BORDER_CONSTANT
  \li CopyMakeBorder.BORDER_REFLECT
  \li CopyMakeBorder.BORDER_WRAP
  \li CopyMakeBorder.BORDER_REFLECT_101
  \li CopyMakeBorder.BORDER_TRANSPARENT
  \li CopyMakeBorder.BORDER_DEFAULT
  \li CopyMakeBorder.BORDER_ISOLATED
  \endlist
*/


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
  \qmlproperty int CopyMakeBorder::top

  Property specifying how many pixels in the top direction from the source image rectangle to extrapolate.
 */


/*!
  \qmlproperty int CopyMakeBorder::bottom

  Property specifying how many pixels in the bottom direction from the source image rectangle to extrapolate.
 */


/*!
  \qmlproperty int CopyMakeBorder::left

  Property specifying how many pixels in the left direction from the source image rectangle to extrapolate.
 */


/*!
  \qmlproperty int CopyMakeBorder::right

  Property specifying how many pixels in the right direction from the source image rectangle to extrapolate.
 */

/*!
  \qmlproperty int CopyMakeBorder::borderType

  Border type.
  \sa CopyMakeBorder::BorderType
 */

/*!
  \qmlproperty color CopyMakeBorder::color

  Border value if borderType==CopyMakeBorder.BORDER_CONSTANT.
  \sa CopyMakeBorder::BorderType
 */

/*!
  \brief Filtering function.

  Parameters:
  \a in
  \a out
 */
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
