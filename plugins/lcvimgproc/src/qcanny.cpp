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
#include "qcanny.h"
#include "opencv2/imgproc/imgproc.hpp"

/*!
  \qmltype Canny
  \instantiates QCanny
  \inqmlmodule lcvimgproc
  \inherits MatFilter
  \brief Finds edges within an image.

  Canny finds edges in the input image image and marks them in the output map edges using the Canny algorithm.
  The smallest value between threshold1 and threshold2 is used for edge linking. The largest value is used to find
  initial segments of strong edges

  \quotefile imgproc/canny.qml
*/

/*!
  \class QCanny
  \inmodule lcvimgproc_cpp
  \brief Finds edges within an image.
 */

/*!
  \brief QCanny constructor

  Parameters :
  \a parent
 */
QCanny::QCanny(QQuickItem *parent) :
    QMatFilter(parent),
    m_apertureSize(3),
    m_l2gradient(false){
}

/*!
  \property QCanny::threshold1
  \sa Canny::threshold1
 */

/*!
  \qmlproperty int Canny::threshold1

  First threshold for the hysteresis procedure.
 */

/*!
  \property QCanny::threshold2
  \sa Canny::threshold2
 */

/*!
  \qmlproperty Size Canny::threshold2

  Second threshold for the hysteresis procedure.
 */

/*!
  \property QCanny::apertureSize
  \sa Canny::apertureSize
 */

/*!
  \qmlproperty Size Canny::apertureSize

  Aperture size for the Sobel() operator.
 */

/*!
  \property QCanny::l2gradient
  \sa Canny::l2gradient
 */

/*!
  \qmlproperty Size Canny::l2gradient

  A flag indicating whether a more accurate L2 norm should be used to calculate the image gradient magnitude.
 */


/*!
  \brief QCanny destructor
 */
QCanny::~QCanny(){
}

/*!
  \brief Filter function.

  Parameters:
  \a in
  \a out
 */
void QCanny::transform(cv::Mat &in, cv::Mat &out){
    cv::Canny(in, out, m_threshold1, m_threshold2, m_apertureSize, m_l2gradient);
}
