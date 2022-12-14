/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
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

#include "qimageview.h"
#include "qmatnode.h"
#include "qmatshader.h"
#include "live/shared.h"
#include "live/visuallogqt.h"

#include <QSGSimpleMaterial>

QImageView::QImageView(QQuickItem *parent)
    : QQuickItem(parent)
    , m_mat(nullptr)
    , m_linearFilter(true)
{
    setFlag(ItemHasContents, true);
}

QImageView::~QImageView(){
}

QSGNode *QImageView::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *){
    QMatNode *n = static_cast<QMatNode*>(node);
    if (!node)
        n = new QMatNode();

    QSGGeometry::updateTexturedRectGeometry(n->geometry(), boundingRect(), QRectF(0, 0, 1, 1));
    QMatState* state = static_cast<QSGSimpleMaterial<QMatState>*>(n->material())->state();
    state->mat          = m_mat;
    state->textureSync  = false;
    state->linearFilter = m_linearFilter;
    n->markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);

    return n;
}

void QImageView::setImage(QMat *arg){
    if ( m_mat )
        lv::Shared::unref(m_mat);

    m_mat = arg;

    if ( arg != nullptr ){
        lv::Shared::ref(arg);
        cv::Mat* matData = arg->internalPtr();
        if ( static_cast<int>(implicitWidth()) != matData->cols || static_cast<int>(implicitHeight()) != matData->rows ){
            setImplicitWidth(matData->cols);
            setImplicitHeight(matData->rows);
        }
    }

    emit imageChanged();
    update();
}
