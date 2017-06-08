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

/****************************************************************************
**
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
#include "qfastnlmeansdenoisingmulti.h"
#include "opencv2/photo.hpp"

using namespace cv;

/*!
  \qmltype FastNlMeansDenoisingMulti
  \instantiates QFastNlMeansDenoisingMulti
  \inqmlmodule lcvphoto
  \inherits FastNlMeansDenoising
  \brief Denoises a grayscale or color image using multiple frames.

  Variant of FastNlMeansDenoising using a history of frames.
  Implemented with a sliding window, which makes it suitable for video input.

  All FastNlMeansDenoising options also apply to FastNlMeansDenoisingMulti.

  Usage notes for video input:
  \list
  \li The algorithm is rather slow, which can be a problem for live video processing.
      Tweak the input framerate, searchWindowSize and temporalWindowSize as required.
  \li The output will lag (temporalWindowSize-1)/2 frames behind the input.
  \li Output will only start when the buffer is filled; expect temporalWindowSize-1
      black output frames directly after recompiling the QML.
  \endlist
*/

/*!
  \class QFastNlMeansDenoisingMulti
  \inmodule lcvphoto_cpp
  \internal
  \brief Denoises a grayscale or color image using multiple frames.
 */

/*!
  \brief QFastNlMeansDenoisingMulti constructor

  Parameters :
  \a parent
 */
QFastNlMeansDenoisingMulti::QFastNlMeansDenoisingMulti(QQuickItem *parent) :
    QFastNlMeansDenoising(parent),
    m_temporalWindowSize(3){
}

/*!
  \brief QFastNlMeansDenoisingMulti destructor
 */
QFastNlMeansDenoisingMulti::~QFastNlMeansDenoisingMulti(){
}

/*!
  \qmlproperty int FastNlMeansDenoisingMulti::temporalWindowSize

  Size of the sliding window in frames. Higher values keep a longer history,
  which requires more memory and processing power and increases output delay.
  Must be an odd number. Defaults to 3.
 */
void QFastNlMeansDenoisingMulti::setTemporalWindowSize(int temporalWindowSize){
    if ( (temporalWindowSize & 1) == 0 )
        --temporalWindowSize; // Must be odd
    if ( temporalWindowSize < 1 )
        temporalWindowSize = 1;
    if ( m_temporalWindowSize != temporalWindowSize ){
        m_frameHistory.reserve(temporalWindowSize);
        trimFrameHistory(temporalWindowSize);
        m_temporalWindowSize = temporalWindowSize;
        emit temporalWindowSizeChanged();
        QMatFilter::transform();
    }
}

void QFastNlMeansDenoisingMulti::trimFrameHistory(std::size_t size){
    if (m_frameHistory.size() > size){
        int elementsToDelete = static_cast<int>(m_frameHistory.size() - size);
        m_frameHistory.erase(m_frameHistory.begin(), m_frameHistory.begin() + elementsToDelete);
    }
}

/*!
  \brief Filter function.

  Parameters:
  \a in
  \a out
 */
void QFastNlMeansDenoisingMulti::transform(const Mat &in, Mat &out){
    if ( !in.empty() ){ // fastNlMeansDenoising hangs on empty Mat
        trimFrameHistory(temporalWindowSize() - 1);
        m_frameHistory.push_back(in);
        if ( m_frameHistory.size() == static_cast<std::size_t>(temporalWindowSize()) ){
            bool colorEnabled = colorAlgorithm();
            if ( autoDetectColor() ){
                colorEnabled = (in.channels() > 1);
            }
            if ( colorEnabled ){
                fastNlMeansDenoisingColoredMulti(m_frameHistory, out, temporalWindowSize() / 2, temporalWindowSize(), h(), hColor(), templateWindowSize(), searchWindowSize());
            }
            else{
                fastNlMeansDenoisingMulti(m_frameHistory, out, temporalWindowSize() / 2, temporalWindowSize(), h(), templateWindowSize(), searchWindowSize());
            }
        }
    }
}
