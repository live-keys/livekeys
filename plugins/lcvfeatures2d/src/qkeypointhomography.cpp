#include "qkeypointhomography.h"
#include "opencv2/calib3d/calib3d.hpp"

QKeypointHomography::QKeypointHomography(QQuickItem* parent)
    : QMatDisplay(parent)
    , m_keypointsToScene(0)
{
    setFlag(QQuickItem::ItemHasContents, true);
}

QKeypointHomography::~QKeypointHomography(){
}

QSGNode *QKeypointHomography::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *nodeData){
    if ( m_keypointsToScene && m_queryImage ){
        QKeyPointToSceneMap::ObjectKeypointToScene* os = m_keypointsToScene->mappingAt(0);
        qDebug() << os->objectPoints.size();
        qDebug() << os->scenePoints.size();
        cv::Mat H = cv::findHomography( os->objectPoints, os->scenePoints, CV_RANSAC );

        std::vector<cv::Point2f> obj_corners(4);
        obj_corners[0] = cvPoint(0,0);
        obj_corners[1] = cvPoint( 100, 0 );
        obj_corners[2] = cvPoint( 100, 100 );
        obj_corners[3] = cvPoint( 0, 100 );

        std::vector<cv::Point2f> scene_corners(4);

        perspectiveTransform( obj_corners, scene_corners, H);

        cv::Mat img_matches = *m_queryImage->cvMat();
        line( img_matches, scene_corners[0] + cv::Point2f( 100, 0), scene_corners[1] + cv::Point2f( 100, 0), cv::Scalar(0, 255, 0), 4 );
        line( img_matches, scene_corners[1] + cv::Point2f( 100, 0), scene_corners[2] + cv::Point2f( 100, 0), cv::Scalar( 0, 255, 0), 4 );
        line( img_matches, scene_corners[2] + cv::Point2f( 100, 0), scene_corners[3] + cv::Point2f( 100, 0), cv::Scalar( 0, 255, 0), 4 );
        line( img_matches, scene_corners[3] + cv::Point2f( 100, 0), scene_corners[0] + cv::Point2f( 100, 0), cv::Scalar( 0, 255, 0), 4 );

        setOutput(m_queryImage);
    }

    return QMatDisplay::updatePaintNode(node, nodeData);
}

