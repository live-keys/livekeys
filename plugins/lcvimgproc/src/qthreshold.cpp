/****************************************************************************
**
** Copyright (C) 2014-2016 Dinu SV.
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
#include "qthreshold.h"
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

  \quotefile imgproc/threshold.qml
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
  \qmlproperty enumeration Threshold::Type

  Extrapolation method border type:
  \list
  \li Threshold.BINARY if ( input(x, y) > thresh ) output(x, y) = maxVal; else output(x, y) = 0
  \li Threshold.BINARY_INV if ( input(x, y) > thresh ) output(x, y) = 0; else output(x, y) = maxVal
  \li Threshold.TRUNC if ( input(x, y) > thresh ) output(x, y) = threshold; else output(x, y) = input(x, y)
  \li Threshold.TOZERO if ( input(x, y) > thresh ) output(x, y) = input(x, y); else output(x, y) = 0
  \li Threshold.TOZERO_INV if ( input(x, y) > thresh ) output(x, y) = 0; else output(x, y) = input(x, y)
  \li Threshold.OTSU The function determines the optimal threshold value using the Otsu’s algorithm and uses it instead
   of the specified thresh
  \endlist
*/


/*!
  \enum QThreshold::Type

  \value BINARY
  \value BINARY_INV
  \value TRUNC
  \value TOZERO
  \value TOZERO_INV
  \value OTSU
 */

/*!
  \property QThreshold::thresh
  \sa Threshold::thresh
 */

/*!
  \qmlproperty int Threshold::thresh

  Threshold value
 */

/*!
  \property QThreshold::maxVal
  \sa Threshold::maxVal
 */

/*!
  \qmlproperty int Threshold::maxVal

  Maximum value to use with the THRESH_BINARY and THRESH_BINARY_INV thresholding types.
 */

/*!
  \property QThreshold::thresholdType
  \sa Threshold::thresholdType
 */

/*!
  \qmlproperty Threshold::Type Threshold::thresholdType

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
