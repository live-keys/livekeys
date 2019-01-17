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
#include "qthreshold.h"
#include "opencv2/imgproc.hpp"


/*!
  \class QThreshold
  \inmodule lcvimgproc_cpp
  \internal
  \brief Finds edges within an image.
 */

QThreshold::QThreshold(QQuickItem *parent) :
    QMatFilter(parent)
{
}

QThreshold::~QThreshold()
{
}


/*!
  \enum QThreshold::Type

  \value BINARY
  \value BINARY_INV
  \value TRUNC
  \value TOZERO
  \value TOZERO_INV
  \value OTSU
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
void QThreshold::transform(const cv::Mat &in, cv::Mat &out){
    cv::threshold(in, out, m_thresh, m_maxVal, m_type);
}
