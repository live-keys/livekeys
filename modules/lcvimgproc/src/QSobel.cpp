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
#include "QSobel.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "QMatNode.hpp"
#include <QSGSimpleMaterial>

using namespace cv;

/**
 * @brief QSobel::QSobel
 * @param parent
 */
QSobel::QSobel(QQuickItem *parent)
    : QMatFilter(parent)
    , m_display(new QMat){
}

QSobel::~QSobel(){
    delete m_display;
}

QSGNode *QSobel::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *nodeData){
    if ( output()->cvMat()->size() != Size(0, 0) )
        convertScaleAbs(*output()->cvMat(), *m_display->cvMat());

    QMatNode *n = static_cast<QMatNode*>(QMatDisplay::updatePaintNode(node, nodeData));
    static_cast<QSGSimpleMaterial<QMatState>*>(n->material())->state()->mat = m_display;
    return n;
}

void QSobel::transform(cv::Mat &in, cv::Mat &out){
    if ( in.size() != Size(0, 0) )
        Sobel(in, out, m_ddepth, m_xorder, m_yorder, m_ksize, m_scale, m_delta, m_borderType);
}
