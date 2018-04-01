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

#include "qmatroi.h"


/*!
  \qmltype MatRoi
  \instantiates QMatRoi
  \inqmlmodule lcvcore
  \inherits MatFilter
  \brief Selects a region of interest (ROI).

  Select a region from an image for further processing. The 'PanAndZoom' component shows how to use a MatRoi to select
  a region from an image, then use a MatRead to read the regions values.

  \quotefile imgproc/panandzoom.qml
*/

/*!
  \class QMatRoi
  \internal
  \brief Performs a selection of a matrixes region of interest for further processing.
 */

/*!
  \brief QMatRoi constructor

  Parameters:
  \a parent
 */
QMatRoi::QMatRoi(QQuickItem *parent)
    : QMatFilter(parent)
    , m_regionX(0)
    , m_regionY(0)
{
}


/*!
  \qmlproperty int MatRoi::regionX

  The x coordinate of the most top-left point of the region.
 */


/*!
  \qmlproperty int MatRoi::regionY

  The y coordinate of the most top-left point of the region.
 */


/*!
  \qmlproperty int MatRoi::regionWidth

  The width of the seleted region.
 */


/*!
  \qmlproperty int MatRoi::regionHeight

  The height of the seleted region.
 */

void QMatRoi::transform(const cv::Mat &in, cv::Mat &out){
    if ( in.cols >= m_regionX + m_regionWidth && in.rows >= m_regionY + m_regionHeight )
        in(cv::Rect(m_regionX, m_regionY, m_regionWidth, m_regionHeight)).copyTo(out);
}
