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
#include "QThreshold.hpp"
#include "opencv2/imgproc/imgproc.hpp"

/*!
  \qmltype Threshold
  \instantiates QThreshold
  \inqmlmodule lcvimgproc
  \inherits MatFilter
  \brief Applies a fixed-level threshold to each element

  The function applies fixed-level thresholding to a single-channel array. The function is typically used to get a
  bi-level (binary) image out of a grayscale image ( compare() could be also used for this purpose) or for removing a
  noise, that is, filtering out pixels with too small or too large values.

  \quotefile imgproc/canny.qml
*/

/*!
  \class QThreshold
  \inmodule lcvimgproc_cpp
  \brief Finds edges within an image.
 */

/*!
  \brief QThreshold constructor

  Parameters
  \a parent
 */
QThreshold::QThreshold(QQuickItem *parent) :
    QMatFilter(parent)
{
}

/*!
  \brief QThreshold destructor
 */
QThreshold::~QThreshold()
{
}

/*!
  \property QThreshold::thresh
  \sa Threshold::thresh
 */

/*!
  \qmlproperty int Threshold::thresh

  Threshold value
 */

/*!
  \property QThreshold::maxval
  \sa Threshold::maxval
 */

/*!
  \qmlproperty int Threshold::maxval

  Maximum value to use with the THRESH_BINARY and THRESH_BINARY_INV thresholding types.
 */

/*!
  \property QThreshold::type
  \sa Threshold::type
 */

/*!
  \qmlproperty Threshold::Type Threshold::type

  Maximum value to use with the THRESH_BINARY and THRESH_BINARY_INV thresholding types.
 */

//THRESH_BINARY

//    \texttt{dst} (x,y) = \fork{\texttt{maxval}}{if $\texttt{src}(x,y) > \texttt{thresh}$}{0}{otherwise}

//THRESH_BINARY_INV

//    \texttt{dst} (x,y) = \fork{0}{if $\texttt{src}(x,y) > \texttt{thresh}$}{\texttt{maxval}}{otherwise}

//THRESH_TRUNC

//    \texttt{dst} (x,y) = \fork{\texttt{threshold}}{if $\texttt{src}(x,y) > \texttt{thresh}$}{\texttt{src}(x,y)}{otherwise}

//THRESH_TOZERO

//    \texttt{dst} (x,y) = \fork{\texttt{src}(x,y)}{if $\texttt{src}(x,y) > \texttt{thresh}$}{0}{otherwise}

//THRESH_TOZERO_INV

//    \texttt{dst} (x,y) = \fork{0}{if $\texttt{src}(x,y) > \texttt{thresh}$}{\texttt{src}(x,y)}{otherwise}




/*!
  \brief Filtering function.

  Parameters:
  \a in
  \a out
 */
void QThreshold::transform(cv::Mat &in, cv::Mat &out){
    cv::threshold(in, out, m_thresh, m_maxVal, m_type);
}
