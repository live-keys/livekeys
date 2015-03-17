#include "QDrawMatches.hpp"
#include "opencv2/features2d/features2d.hpp"

#include <iostream>

QDrawMatches::QDrawMatches(QQuickItem *parent)
    : QMatDisplay(parent)
    , m_matches(0)
    , m_matchSurfaceDirty(false)
    , m_matchIndex(-1)
{
}

QDrawMatches::~QDrawMatches(){
}

QSGNode *QDrawMatches::updatePaintNode(QSGNode *node, UpdatePaintNodeData *nodeData){

    if ( m_keypoints1 && m_keypoints2 && m_matches && m_matchSurfaceDirty ){

        m_mask.resize(m_matches->matches().size());
        fill(m_mask.begin(), m_mask.end(), 0);

        const std::vector<cv::DMatch>& matches = m_matches->matches();
        for ( size_t i = 0; i < matches.size(); ++i ){
            if ( matches[i].imgIdx == m_matchIndex )
                m_mask[i] = 1;
        }

        try{
            cv::drawMatches(
                        m_keypoints1->cvMat(),
                        m_keypoints1->keypoints(),
                        m_keypoints2->cvMat(),
                        m_keypoints2->keypoints(),
                        m_matches->matches(),
                        *(output()->cvMat()),
                        cv::Scalar::all(-1),
                        cv::Scalar::all(-1),
                        m_mask
                        );
            setImplicitSize(output()->cvMat()->cols, output()->cvMat()->rows);

        } catch(cv::Exception& e){
            qWarning(e.what());
        }

        m_matchSurfaceDirty = false;
    }
    return QMatDisplay::updatePaintNode(node, nodeData);
}
