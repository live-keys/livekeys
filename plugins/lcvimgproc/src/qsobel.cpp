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
#include "qsobel.h"
#include "opencv2/imgproc.hpp"
#include "qmatnode.h"
#include <QSGSimpleMaterial>

using namespace cv;

/*!
  \class QSobel
  \inmodule lcvimgproc_cpp
  \internal
  \brief Calculates the first, second, third, or mixed image derivatives using an extended Sobel operator.
 */

QSobel::QSobel(QQuickItem *parent)
    : QMatFilter(parent)
    , m_ddepth(QMat::CV16S)
    , m_xorder(1)
    , m_yorder(0)
    , m_ksize(3)
    , m_scale(1)
    , m_delta(0)
    , m_borderType(BORDER_DEFAULT)
    , m_display(new QMat)
{
}


QSobel::~QSobel(){
    delete m_display;
}

/*!
  \fn virtual QSGNode* QSobel::updatePaintNode(QSGNode*, QQuickItem::UpdatePaintNodeData*)

  \brief Updates the scene graph node with the drawn lines.

  Parameters :
  \a node
  \a nodeData
 */

QSGNode *QSobel::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *nodeData){
    if ( output()->cvMat()->size() != Size(0, 0) )
        convertScaleAbs(*output()->cvMat(), *m_display->cvMat());

    QMatNode *n = static_cast<QMatNode*>(QMatDisplay::updatePaintNode(node, nodeData));
    static_cast<QSGSimpleMaterial<QMatState>*>(n->material())->state()->mat = m_display;
    return n;
}

/*!
  \brief Filtering function.

  Parameters:
  \a in
  \a out
 */
void QSobel::transform(const cv::Mat &in, cv::Mat &out){
    if ( in.size() != Size(0, 0) )
        Sobel(in, out, m_ddepth, m_xorder, m_yorder, m_ksize, m_scale, m_delta, m_borderType);
}
