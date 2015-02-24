#include "QDrawMatches.hpp"
#include "opencv2/features2d/features2d.hpp"

QDrawMatches::QDrawMatches(QQuickItem *parent)
    : QMatDisplay(parent)
    , m_matches(0)
    , m_matchSurfaceDirty(false)
{
}

QDrawMatches::~QDrawMatches(){
}

QSGNode *QDrawMatches::updatePaintNode(QSGNode *node, UpdatePaintNodeData *nodeData){
    if ( m_keypoints1 && m_keypoints2 && m_matches && m_matchSurfaceDirty ){
        try{
            cv::drawMatches(
                        m_keypoints1->cvMat(),
                        m_keypoints1->keypoints(),
                        m_keypoints2->cvMat(),
                        m_keypoints2->keypoints(),
                        m_matches->matches(),
                        *(output()->cvMat())
                        );
            setImplicitSize(output()->cvMat()->cols, output()->cvMat()->rows);

        } catch(cv::Exception& e){
            qWarning(e.what());
        }
    }
    return QMatDisplay::updatePaintNode(node, nodeData);
}
