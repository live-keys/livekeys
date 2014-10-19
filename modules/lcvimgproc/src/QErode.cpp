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
#include "QErode.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

/*!
  \qmltype Erode
  \instantiates QErode
  \inqmlmodule lcvimgproc
  \inherits MatFilter
  \brief

  \quotefile imgproc/.qml
*/

/*!
  \class QErode
  \inmodule lcvimgproc_cpp
  \brief
 */


/*!
  \brief QErode constructor

  Parameters:
  \a parent
 */
QErode::QErode(QQuickItem *parent)
    : QMatFilter(parent)
    , m_kernel(0)
    , m_anchorCv(-1, -1)
    , m_iterations(1)
    , m_borderType(BORDER_CONSTANT)
{
}

/*!
  \brief QErode destructor
 */
QErode::~QErode(){
}


/*!
  \property QErode::kernel
  \sa Erode::kernel
 */

/*!
  \qmlproperty Mat Erode::kernel

  Structuring element used for erosion; if element=Mat() , a 3x3 rectangular structuring element is used.
 */


/*!
  \property QErode::anchor
  \sa Erode::anchor
 */

/*!
  \qmlproperty Point Erode::anchor

  Position of the anchor within the element; default value (-1, -1) means that the anchor is at the element center.
 */

/*!
  \property QErode::iterations
  \sa Erode::iterations
 */

/*!
  \qmlproperty int Erode::iterations

  Number of times erosion is applied.
 */

/*!
  \property QErode::borderType
  \sa Erode::borderType
 */

/*!
  \qmlproperty int Erode::borderType

  Pixel extrapolation method.

  \sa CopyMakeBorder::BorderType
 */

/*!
  \property QErode::borderValue
  \sa Erode::borderValue
 */

/*!
  \qmlproperty color Erode::borderValue

  Border value in case of a constant border

  \sa CopyMakeBorder::BorderType
 */

/*!
  \brief Filtering function.

  Parameters:
  \a in
  \a out
 */
void QErode::transform(cv::Mat &in, cv::Mat &out){
    if ( m_kernel ){
        Scalar borderValue = morphologyDefaultBorderValue();
        if ( m_borderValue.isValid() ){
            if ( in.channels() == 3 )
                borderValue = Scalar(m_borderValue.blue(), m_borderValue.green(), m_borderValue.red());
            else
                borderValue = Scalar(m_borderValue.red());
        }
        erode(in, out, *(m_kernel->cvMat()), m_anchorCv, m_iterations, m_borderType, borderValue);
    }
}
