#include "qwarpperspective.h"

#include "opencv2/imgproc.hpp"

#include "qmatnode.h"
#include "qmatshader.h"

QWarpPerspective::QWarpPerspective(QQuickItem *parent)
    : QQuickItem(parent)
    , m_in(QMat::nullMat())
    , m_output(QMat::nullMat())
    , m_m(QMat::nullMat())
    , m_interpolation(cv::INTER_LINEAR)
    , m_borderMode(cv::BORDER_CONSTANT)
    , m_borderValueInternal(cv::Scalar())
{
    setFlag(ItemHasContents, true);
}

QWarpPerspective::~QWarpPerspective(){
}

void QWarpPerspective::transform(){
    if ( isComponentComplete() ){
        try{
            transform(*input()->internalPtr(), *output()->internalPtr());
            emit outputChanged();
            update();
        } catch (cv::Exception& e ){
            qCritical("%s", e.msg.c_str());
        }
    }
}

void QWarpPerspective::transform(const cv::Mat &in, cv::Mat &out){
    if ( in.empty() )
        return;

    cv::warpPerspective(in, out, m_m->internal(), out.size(), m_interpolation, m_borderMode, m_borderValueInternal);
}

QSGNode *QWarpPerspective::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *){
    QMatNode *n = static_cast<QMatNode*>(node);
    if (!node)
        n = new QMatNode();

    QSGGeometry::updateTexturedRectGeometry(n->geometry(), boundingRect(), QRectF(0, 0, 1, 1));
    QMatState* state = static_cast<QSGSimpleMaterial<QMatState>*>(n->material())->state();
    state->mat          = m_output;
    state->textureSync  = false;
    state->linearFilter = false;
    n->markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);

    return n;
}

void QWarpPerspective::componentComplete(){
    QQuickItem::componentComplete();
    transform();
}
