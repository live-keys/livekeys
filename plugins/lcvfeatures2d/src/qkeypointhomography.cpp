#include "qkeypointhomography.h"
#include "opencv2/calib3d/calib3d.hpp"

QKeypointHomography::QKeypointHomography(QQuickItem* parent)
    : QMatDisplay(parent)
    , m_keypointsToScene(0)
    , m_queryImage(0)
{
    setFlag(QQuickItem::ItemHasContents, true);
}

QKeypointHomography::~QKeypointHomography(){
}

QSGNode *QKeypointHomography::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *nodeData){
    if ( m_keypointsToScene && m_queryImage ){

        cv::Mat* surface = output()->cvMat();
        m_queryImage->cvMat()->copyTo(*surface);

        for ( int i = 0; i < m_keypointsToScene->size(); ++i ){
            if ( i >= m_objectCorners.size() )
                break;

            QKeyPointToSceneMap::ObjectKeypointToScene* os = m_keypointsToScene->mappingAt(i);
            if ( os->scenePoints.size() > 10 ){
                cv::Mat H = cv::findHomography(os->objectPoints, os->scenePoints, CV_RANSAC);

                QVariantList corners = m_objectCorners[i].toList();
                std::vector<cv::Point2f> currentCorners(corners.size());
                for ( int ci = 0; ci < corners.size(); ++ci ){
                    QPoint p = corners[ci].toPoint();
                    currentCorners[ci] = cvPoint(p.x(), p.y());
                }

                std::vector<cv::Point2f> sceneCorners(corners.size());

                perspectiveTransform(currentCorners, sceneCorners, H);

                for ( size_t si = 0; si < sceneCorners.size() - 1; ++si ){
                    cv::line(*surface, sceneCorners[si], sceneCorners[si + 1], cv::Scalar(40 + si * 40, 255, 0), 4);
                }
                if ( sceneCorners.size() > 1 )
                    cv::line(*surface, sceneCorners[sceneCorners.size() - 1], sceneCorners[0], cv::Scalar(0, 255, 0), 4);
            }
        }

    }

    return QMatDisplay::updatePaintNode(node, nodeData);
}

