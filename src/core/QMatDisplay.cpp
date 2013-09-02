#include "QMatDisplay.hpp"
#include "QMatNode.hpp"
#include "QMatShader.hpp"
#include <QSGSimpleMaterial>

QMatDisplay::QMatDisplay(QQuickItem *parent) :
    QQuickItem(parent),
    m_output(new QMat())
{
    setFlag(ItemHasContents, true);
}

QMatDisplay::QMatDisplay(QMat *output, QQuickItem *parent) :
    QQuickItem(parent),
    m_output(output)
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
    static_cast<QSGSimpleMaterial<QMatState>*>(n->material())->state()->mat         = m_output;
    static_cast<QSGSimpleMaterial<QMatState>*>(n->material())->state()->textureSync = false;
    n->markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);

    return n;
}
