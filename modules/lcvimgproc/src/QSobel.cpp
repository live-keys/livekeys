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
    if ( output()->data()->size() != Size(0, 0) )
        convertScaleAbs(*output()->data(), *m_display->data());

    QMatNode *n = static_cast<QMatNode*>(QMatDisplay::updatePaintNode(node, nodeData));
    static_cast<QSGSimpleMaterial<QMatState>*>(n->material())->state()->mat = m_display;
    return n;
}

void QSobel::transform(cv::Mat &in, cv::Mat &out){
    if ( in.size() != Size(0, 0) )
        Sobel(in, out, m_ddepth, m_xorder, m_yorder, m_ksize, m_scale, m_delta, m_borderType);
}
