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

#include "qdenoisetvl1.h"
#include "opencv2/photo.hpp"

using namespace cv;

/*!
  \qmltype DenoiseTvl1
  \instantiates QDenoiseTvl1
  \inqmlmodule lcvphoto
  \inherits MatFilter
  \brief Denoises a grayscale or color image.

  Performs denoising using the primal-dual algorithm.
*/

/*!
  \class QDenoiseTvl1
  \inmodule lcvphoto_cpp
  \internal
  \brief Performs denoising using the primal-dual algorithm.
 */

/*!
  \qmlproperty int DenoiseTvl1::bufferSize

  Number of observations or noised versions of the image to be restored.
 */

/*!
  \qmlproperty real DenoiseTvl1::lambda

  As it is enlarged, the smooth (blurred) images are treated more favorably than detailed
  (but maybe more noised) ones. Roughly speaking, as it becomes smaller, the result will be
  more blur but more sever outliers will be removed.
 */

/*!
  \qmlproperty int DenoiseTvl1::nIters

  Number of iterations that the algorithm will run. The more iterations the better, but it is
  hard to quantitatively refine this statement, so just use the default and increase it if the
  results are poor.
 */

QDenoiseTvl1::QDenoiseTvl1(QQuickItem *parent) :
    QMatFilter(parent),
    m_matBuffer(),
    m_bufferSize(1),
    m_lambda(1.0),
    m_nIters(30)
{
}

void QDenoiseTvl1::setBufferSize(int bufferSize){
    if ( bufferSize < 1 )
        bufferSize = 1;
    if ( m_bufferSize != bufferSize ){
        m_matBuffer.reserve(bufferSize);
        trimBuffer(bufferSize);
        m_bufferSize = bufferSize;
        emit bufferSizeChanged();
        QMatFilter::transform();
    }
}

void QDenoiseTvl1::trimBuffer(int size){
    if (m_matBuffer.size() > static_cast<std::size_t>(size)){
        size_t elementsToDelete = m_matBuffer.size() - size;
        m_matBuffer.erase(m_matBuffer.begin(), m_matBuffer.begin() + elementsToDelete);
    }
}


QDenoiseTvl1::~QDenoiseTvl1(){
}

/*!
  \brief Filter function.

  Parameters:
  \a in
  \a out
 */
void QDenoiseTvl1::transform(const Mat &in, Mat &out){
    if ( !in.empty() ) {
        trimBuffer(m_bufferSize - 1);
        m_matBuffer.push_back(in);
        denoise_TVL1(m_matBuffer, out, m_lambda, m_nIters);
    }
}
