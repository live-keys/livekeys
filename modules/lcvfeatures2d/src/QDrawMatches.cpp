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

//        const std::vector<cv::DMatch>& matches = m_matches->matches()[0];
        const std::vector<cv::DMatch>& matches = m_matches->matches()[0];

//        double max_dist = 0;
//        double min_dist = 100;

//        //-- Quick calculation of max and min distances between keypoints
//        for( size_t i = 0; i < matches.size(); ++i ){
//            double dist = matches[i].distance;
//            if( dist < min_dist )
//                min_dist = dist;
//            if( dist > max_dist )
//                max_dist = dist;
//        }

////        printf("-- Max dist : %f \n", max_dist );
////        printf("-- Min dist : %f \n", min_dist );

//        //-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist )
//        //-- PS.- radiusMatch can also be used here.
//        std::vector<cv::DMatch> good_matches;

//        for( size_t i = 0; i < matches.size(); i++ ){
////            if( matches[i].distance < 2.5 * min_dist ){
//                good_matches.push_back( matches[i] );
////            }
//        }

//        m_mask.resize(good_matches.size());
//        fill(m_mask.begin(), m_mask.end(), 0);
//        for ( size_t i = 0; i < good_matches.size(); ++i ){
//            if ( good_matches[i].imgIdx == m_matchIndex )
//                m_mask[i] = 1;
//        }

        m_mask.resize(matches.size());
        fill(m_mask.begin(), m_mask.end(), 0);
        for ( size_t i = 0; i < matches.size(); ++i ){
            if ( matches[i].imgIdx == m_matchIndex )
                m_mask[i] = 1;
        }

        qDebug() << matches.size();

        try{
            cv::drawMatches(
                m_keypoints1->cvMat(),
                m_keypoints1->keypoints(),
                m_keypoints2->cvMat(),
                m_keypoints2->keypoints(),
                matches,
                *(output()->cvMat()),
                cv::Scalar::all(-1),
                cv::Scalar::all(-1),
                m_mask
            );
            setImplicitSize(output()->cvMat()->cols, output()->cvMat()->rows);

        } catch(cv::Exception& e){
            qWarning("%s", e.what());
        }

        m_matchSurfaceDirty = false;
    }
    return QMatDisplay::updatePaintNode(node, nodeData);
}
