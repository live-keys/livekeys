#include "qmatview.h"
#include "qmatnode.h"
#include "qmatshader.h"
#include <QSGSimpleMaterial>

QMatView::QMatView(QQuickItem *parent)
    : QQuickItem(parent)
    , m_mat(QMat::nullMat())
    , m_linearFilter(true)
{
    setFlag(ItemHasContents, true);
}

QMatView::~QMatView(){
}

QSGNode *QMatView::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *){
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

