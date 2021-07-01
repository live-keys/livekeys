#include "qimageview.h"
#include "qmatnode.h"
#include "qmatshader.h"
#include "live/shared.h"
#include <QSGSimpleMaterial>

QImageView::QImageView(QQuickItem *parent)
    : QQuickItem(parent)
    , m_mat(QMat::nullMat())
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
    if ( arg == nullptr )
        return;

    cv::Mat* matData = arg->internalPtr();
    if ( static_cast<int>(implicitWidth()) != matData->cols || static_cast<int>(implicitHeight()) != matData->rows ){
        setImplicitWidth(matData->cols);
        setImplicitHeight(matData->rows);
    }

    if ( m_mat )
        lv::Shared::unref(m_mat);

    m_mat = arg;
    lv::Shared::ref(arg);

    emit imageChanged();
    update();
}
