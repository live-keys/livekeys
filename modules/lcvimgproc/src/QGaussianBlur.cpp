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

QGaussianBlur::QGaussianBlur(QQuickItem *parent)
    : QMatFilter(parent)
    , m_ksize(QSize(3, 3))
    , m_borderType(BORDER_DEFAULT)
{
}

QGaussianBlur::~QGaussianBlur(){
}

void QGaussianBlur::transform(cv::Mat &in, cv::Mat &out){
    GaussianBlur(in, out, cv::Size(m_ksize.width(), m_ksize.height()), m_sigmaX, m_sigmaY, m_borderType);
}
