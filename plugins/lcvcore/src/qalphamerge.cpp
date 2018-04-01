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
#include "qalphamerge.h"


/*!
  \qmltype AlphaMerge
  \instantiates QAlphaMerge
  \inqmlmodule lcvcore
  \inherits MatFilter
  \brief Merges an alpha channel to a matrix.

  Alpha merge is used to merge an alpha channel to a 1 or 3 channel matrix. The alpha channel is a single channel image
  or mask that is loaded within the mask property of this class.

  In the sample at samples/imgproc/alphamerge.qml a loaded image is merged with a drawn circle.

  \quotefile imgproc/alphamerge.qml
*/

QAlphaMerge::QAlphaMerge(QQuickItem *parent) :
    QMatFilter(parent),
    m_mask(0)
{
}

/*!
  \qmlproperty Mat AlphaMerge::mask

  Mask to merge the input with.
 */


void QAlphaMerge::transform(const cv::Mat &in, cv::Mat &out){
    if ( mask() )
        mergeMask(in, *mask()->cvMat(), out);
}

void QAlphaMerge::mergeMask(const cv::Mat &input, cv::Mat &mask, cv::Mat &output){
    if ( input.cols != mask.cols || input.rows != mask.rows ){
        qWarning("Open CV Error: Different mask size.");
        return;
    }
    output.create(input.size(), CV_8UC4);
    const uchar* pi;
    uchar *pm, *po;
    if ( input.channels() == 1 ){
        for ( int i = 0; i < input.rows; ++i ){
            pi = input.ptr<uchar>(i);
            pm = mask.ptr<uchar>(i);
            po = output.ptr<uchar>(i);
            for ( int j = 0; j < input.cols; ++j ){
                po[j * 4 + 0] = pi[j];
                po[j * 4 + 1] = pi[j];
                po[j * 4 + 2] = pi[j];
                po[j * 4 + 3] = pm[j];
            }
        }
    } else {
        for ( int i = 0; i < input.rows; ++i ){
            pi = input.ptr<uchar>(i);
            pm = mask.ptr<uchar>(i);
            po = output.ptr<uchar>(i);
            for ( int j = 0; j < input.cols; ++j ){
                po[j * 4 + 0] = pi[j * 3 + 0];
                po[j * 4 + 1] = pi[j * 3 + 1];
                po[j * 4 + 2] = pi[j * 3 + 2];
                po[j * 4 + 3] = pm[j];
            }
        }
    }
}
