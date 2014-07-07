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
#include "QStructuringElement.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

QStructuringElement::QStructuringElement(QQuickItem *parent)
    : QQuickItem(parent)
    , m_output(new QMat)
    , m_outputDirty(false)
{
    setFlag(ItemHasContents, false);
}

QMat* QStructuringElement::output() const{
    if ( m_outputDirty )
        *m_output->cvMat() = getStructuringElement(
                m_shape,
                Size(m_ksize.width(), m_ksize.height()),
                Point(m_anchor.x(), m_anchor.y()));

    return m_output;
}

QStructuringElement::~QStructuringElement(){
    delete m_output;
}
