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
#include "qstructuringelement.h"
#include "opencv2/imgproc.hpp"

using namespace cv;


/*!
  \qmltype StructuringElement
  \instantiates QStructuringElement
  \inqmlmodule lcvimgproc
  \inherits Item
  \brief Creates a structuring element of the specified size and shape for morphological operations.

  \quotefile imgproc/erodedilate.qml
*/

/*!
  \class QStructuringElement
  \inmodule lcvimgproc_cpp
  \brief Creates a structuring element of the specified size and shape for morphological operations.
 */

/*!
  \brief QStructuringElement constructor

  Parameters
  \a parent
 */
QStructuringElement::QStructuringElement(QQuickItem *parent)
    : QQuickItem(parent)
    , m_output(new QMat)
    , m_outputDirty(false)
{
    setFlag(ItemHasContents, false);
}

/*!
  \brief QSobel destructor
 */
QStructuringElement::~QStructuringElement(){
    delete m_output;
}


/*!
  \property QStructuringElement::shape
  \sa StructuringElement::shape
 */

/*!
  \qmlproperty StructuringElement::ElementShape StructuringElement::shape

  Element shape.
 */

/*!
  \property QStructuringElement::ksize
  \sa StructuringElement::ksize
 */

/*!
  \qmlproperty size StructuringElement::ksize

  Size of the structuring element.
 */

/*!
  \property QStructuringElement::anchor
  \sa StructuringElement::anchor
 */

/*!
  \qmlproperty point StructuringElement::anchor

  Anchor position within the element. The default value (-1, -1) means that the anchor is at the center. Note that only
  the shape of a cross-shaped element depends on the anchor position. In other cases the anchor just regulates how much
  the result of the morphological operation is shifted
 */

/*!
  \property QStructuringElement::output
  \sa StructuringElement::output
 */

/*!
  \qmlproperty Mat StructuringElement::output

  Created element.
 */


/*!
  \qmlproperty enumeration StructuringElement::ElementShape

  Extrapolation method border type:
  \list
  \li StructuringElement.MORPH_RECT A rectangluar structuring element.
  \li StructuringElement.MORPH_ELLIPSE  An elliptic structuring element, that is, a filled ellipse inscribed into the
  rectangle Rect(0, 0, ksize.width, ksize.height)
  \li StructuringElement.MORPH_CROSS A cross-shaped structuring element.
  \endlist
*/


/*!
  \enum QStructuringElement::ElementShape

  \value MORPH_RECT
  \value MORPH_ELLIPSE
  \value MORPH_CROSS
 */


QMat* QStructuringElement::output() const{
    if ( m_outputDirty )
        *m_output->cvMat() = getStructuringElement(
                m_shape,
                Size(m_ksize.width(), m_ksize.height()),
                Point(m_anchor.x(), m_anchor.y()));

    return m_output;
}
