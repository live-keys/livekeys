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
#include "QGaussianBlur.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

/*!
  \qmltype GaussianBlur
  \instantiates QGaussianBlur
  \inqmlmodule lcvimgproc
  \inherits MatFilter
  \brief

  \quotefile imgproc/.qml
*/

/*!
  \class QGaussianBlur
  \inmodule lcvimgproc_cpp
  \brief
 */

/*!
  \brief QGaussianBlur constructor

  Parameters:
  \a parent
 */
QGaussianBlur::QGaussianBlur(QQuickItem *parent)
    : QMatFilter(parent)
    , m_ksize(QSize(3, 3))
    , m_borderType(BORDER_DEFAULT)
{
}

/*!
  \brief QGaussianBlur destructor
 */
QGaussianBlur::~QGaussianBlur(){
}


/*!
  \property QGaussianBlur::ksize
  \sa GaussianBlur::ksize
 */

/*!
  \qmlproperty size GaussianBlur::ksize

  Gaussian kernel size. ksize.width and ksize.height can differ but they both must be positive and odd. Or, they can be
  zero’s and then they are computed from sigma.
 */

/*!
  \property QGaussianBlur::sigmaX
  \sa GaussianBlur::sigmaX
 */

/*!
  \qmlproperty real GaussianBlur::sigmaX

  Gaussian kernel standard deviation in X direction.
 */

/*!
  \property QGaussianBlur::sigmaY
  \sa GaussianBlur::sigmaY
 */

/*!
  \qmlproperty real GaussianBlur::sigmaY

  Gaussian kernel standard deviation in Y direction.
 */

/*!
  \property QGaussianBlur::borderType
  \sa GaussianBlur::borderType
 */

/*!
  \qmlproperty int GaussianBlur::borderType

  Pixel extrapolation method (see \l{CopyMakeBorder::BorderType}{CopyMakeBorder::BorderType} for details).
 */

/*!
  \brief Filtering function.

  Parameters:
  \a in
  \a out
 */
void QGaussianBlur::transform(cv::Mat &in, cv::Mat &out){
    GaussianBlur(in, out, cv::Size(m_ksize.width(), m_ksize.height()), m_sigmaX, m_sigmaY, m_borderType);
}
