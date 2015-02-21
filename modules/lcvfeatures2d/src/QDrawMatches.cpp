#include "QDrawMatches.hpp"
#include "opencv2/features2d/features2d.hpp"

QDrawMatches::QDrawMatches(QQuickItem *parent)
    : QMatDisplay(parent)
    , m_img1(new QMat)
    , m_img1Internal(m_img1)
    , m_img2(new QMat)
    , m_img2Internal(m_img2)
    , m_keypoints1(0)
    , m_keypoints2(0)
    , m_matches(0)
    , m_matchSurfaceDirty(false)
{
}

QDrawMatches::~QDrawMatches(){
    delete m_img1Internal;
    delete m_img2Internal;
}

QSGNode *QDrawMatches::updatePaintNode(QSGNode *node, UpdatePaintNodeData *nodeData){
    if ( m_keypoints1 && m_keypoints2 && m_matches && m_matchSurfaceDirty ){
        cv::drawMatches(
                    *(m_img1->cvMat()),
                    m_keypoints1->keypoints(),
                    *(m_img2->cvMat()),
                    m_keypoints2->keypoints(),
                    m_matches->matches(),
                    *(output()->cvMat())
        );
    }
    return QMatDisplay::updatePaintNode(node, nodeData);
}
