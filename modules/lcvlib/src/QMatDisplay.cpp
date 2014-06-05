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

#include "QMatDisplay.hpp"
#include "QMatNode.hpp"
#include "QMatShader.hpp"
#include <QSGSimpleMaterial>

QMatDisplay::QMatDisplay(QQuickItem *parent)
    : QQuickItem(parent)
    , m_output(new QMat())
    , m_linearFilter(true)
{
    setFlag(ItemHasContents, true);
}

QMatDisplay::QMatDisplay(QMat *output, QQuickItem *parent)
    : QQuickItem(parent)
    , m_output(output)
    , m_linearFilter(true)
{
    setFlag(ItemHasContents, true);
}

QMatDisplay::~QMatDisplay(){
    delete m_output;
}

QSGNode *QMatDisplay::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *){
    QMatNode *n = static_cast<QMatNode*>(node);
    if (!node)
        n = new QMatNode();

    QSGGeometry::updateTexturedRectGeometry(n->geometry(), boundingRect(), QRectF(0, 0, 1, 1));
    QMatState* state = static_cast<QSGSimpleMaterial<QMatState>*>(n->material())->state();
    state->mat          = m_output;
    state->textureSync  = false;
    state->linearFilter = m_linearFilter;
    n->markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);

    return n;
}
