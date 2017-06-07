/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

#include "qabsdiff.h"

/*!
  \qmltype AbsDiff
  \instantiates QAbsDiff
  \inqmlmodule lcvcore
  \inherits MatFilter
  \brief Performs an absolute difference between two matrixes.

  The example in \b{samples/imgproc/framedifference.qml} shows differentiating two consecutive frames in a video to
  calculate the motion. It uses a MatBuffer to store the previous frame :

  \quotefile imgproc/framedifference.qml
*/


QAbsDiff::QAbsDiff(QQuickItem *parent)
    : QMatFilter(parent)
    , m_in2(QMat::nullMat())
{
}

QAbsDiff::~QAbsDiff(){
}

/*!
  \qmlproperty Mat AbsDiff::input2

  Second input for the subtraction.
 */

void QAbsDiff::transform(cv::Mat &in, cv::Mat &out){
    if ( in.size() == m_in2->cvMat()->size() )
        cv::absdiff(in, *m_in2->cvMat(), out);
}

