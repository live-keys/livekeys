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
#include "qcanny.h"
#include "opencv2/imgproc.hpp"

/*!
  \class QCanny
  \internal
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

QCanny::~QCanny(){
}

/*!
  \brief Filter function.

  Parameters:
  \a in
  \a out
 */
void QCanny::transform(const cv::Mat &in, cv::Mat &out){
    cv::Canny(in, out, m_threshold1, m_threshold2, m_apertureSize, m_l2gradient);
}
