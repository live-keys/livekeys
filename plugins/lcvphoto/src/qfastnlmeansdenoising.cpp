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
#include "qfastnlmeansdenoising.h"
#include "opencv2/photo/photo.hpp"

using namespace cv;

/*!
  \qmltype FastNlMeansDenoising
  \instantiates QFastNlMeansDenoising
  \inqmlmodule lcvphoto
  \inherits MatFilter
  \brief Denoises a grayscale or color image.

  Performs denoising using the Non-local Means Denoising algorithm.

  \quotefile photo/fastnlmeansdenoising.qml
*/

/*!
  \class QFastNlMeansDenoising
  \inmodule lcvphoto_cpp
  \brief Performs denoising using the Non-local Means Denoising algorithm.
 */

/*!
  \brief QFastNlMeansDenoising constructor

  Parameters :
  \a parent
 */
QFastNlMeansDenoising::QFastNlMeansDenoising(QQuickItem *parent) :
    QMatFilter(parent),
    m_useColorAlgorithm(false),
    m_autoDetectColor(true),
    m_h(3),
    m_hColor(3),
    m_templateWindowSize(7),
    m_searchWindowSize(21){
}

/*!
  \brief QFastNlMeansDenoising destructor
 */
QFastNlMeansDenoising::~QFastNlMeansDenoising(){
}

/*!
  \property QFastNlMeansDenoising::colorAlgorithm
  \sa FastNlMeansDenoising::colorAlgorithm
 */

/*!
  \qmlproperty bool FastNlMeansDenoising::colorAlgorithm

  False to use fastNlMeansDenoising, true to use fastNlMeansDenoisingColored.
  By default, this is autodetected based on the number of channels in the source.
 */

/*!
  \property QFastNlMeansDenoising::h
  \sa FastNlMeansDenoising::h
 */

/*!
  \qmlproperty float FastNlMeansDenoising::h

  Parameter regulating filter strength. Defaults to 3.
 */

/*!
  \property QFastNlMeansDenoising::hColor
  \sa FastNlMeansDenoising::hColor
 */

/*!
  \qmlproperty float FastNlMeansDenoising::hColor

  The same as h but for color components. Defaults to 3.
  Only relevant when using the color algorithm (see colorAlgorithm).
 */

/*!
  \property QFastNlMeansDenoising::templateWindowSize
  \sa FastNlMeansDenoising::templateWindowSize
 */

/*!
  \qmlproperty int FastNlMeansDenoising::templateWindowSize

  Size in pixels of the template patch that is used to compute weights.
  Should be odd. Defaults to 7.
 */

/*!
  \property QFastNlMeansDenoising::searchWindowSize
  \sa FastNlMeansDenoising::searchWindowSize
 */

/*!
  \qmlproperty int FastNlMeansDenoising::searchWindowSize

  Size in pixels of the window that is used to compute weighted average for given pixel.
  Has a large performance impact. Should be odd. Defaults to 21.
 */

/*!
  \brief Filter function.

  Parameters:
  \a in
  \a out
 */
void QFastNlMeansDenoising::transform(Mat &in, Mat &out){
    if ( !in.empty() ){ // fastNlMeansDenoising hangs on empty Mat
        bool colorEnabled = m_useColorAlgorithm;
        if ( m_autoDetectColor ){
            colorEnabled = (in.channels() > 1);
        }
        if ( colorEnabled ){
            fastNlMeansDenoisingColored(in, out, m_h, m_hColor, m_templateWindowSize, m_searchWindowSize);
        }
        else{
            fastNlMeansDenoising(in, out, m_h, m_templateWindowSize, m_searchWindowSize);
        }
    }
}
