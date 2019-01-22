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
#include "qgaussianblur.h"
#include "opencv2/imgproc.hpp"

using namespace cv;


/*!
  \class QGaussianBlur
  \internal
  \inmodule lcvimgproc_cpp
  \brief Blurs an image using a Gaussian filter.
 */

/*!
  \brief QGaussianBlur constructor

  Parameters:
  \a parent
 */
QGaussianBlur::QGaussianBlur(QQuickItem *parent)
    : QMatFilter(parent)
    , m_ksize(QSize(3, 3))
    , m_sigmaX(0)
    , m_sigmaY(0)
    , m_borderType(BORDER_DEFAULT)
{
}

/*!
  \brief QGaussianBlur destructor
 */
QGaussianBlur::~QGaussianBlur(){
}

/*!
  \brief Filtering function.

  Parameters:
  \a in
  \a out
 */
void QGaussianBlur::transform(const cv::Mat &in, cv::Mat &out){
    GaussianBlur(in, out, cv::Size(m_ksize.width(), m_ksize.height()), m_sigmaX, m_sigmaY, m_borderType);
}
