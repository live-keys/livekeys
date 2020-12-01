/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Livekeys Application.
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
  \class QStructuringElement
  \inmodule lcvimgproc_cpp
  \internal
  \brief Creates a structuring element of the specified size and shape for morphological operations.
 */

QStructuringElement::QStructuringElement(QQuickItem *parent)
    : QQuickItem(parent)
    , m_output(new QMat)
    , m_outputDirty(false)
{
    setFlag(ItemHasContents, false);
}

QStructuringElement::~QStructuringElement(){
    delete m_output;
}

/*!
  \enum QStructuringElement::ElementShape

  \value MORPH_RECT
  \value MORPH_ELLIPSE
  \value MORPH_CROSS
 */


QMat* QStructuringElement::output() const{
    if ( m_outputDirty )
        *m_output->internalPtr() = getStructuringElement(
                m_shape,
                Size(m_ksize.width(), m_ksize.height()),
                Point(m_anchor.x(), m_anchor.y()));

    return m_output;
}
