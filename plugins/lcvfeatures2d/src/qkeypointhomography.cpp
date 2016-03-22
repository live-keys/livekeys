#include "qkeypointhomography.h"
#include "opencv2/calib3d/calib3d.hpp"

QKeypointHomography::QKeypointHomography(QQuickItem* parent)
    : QMatDisplay(parent)
    , m_keypointsToScene(0)
{
}

QKeypointHomography::~QKeypointHomography(){
}

QSGNode *QKeypointHomography::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *nodeData){
    if ( m_keypointsToScene ){
        QKeyPointToSceneMap::ObjectKeypointToScene* os = m_keypointsToScene->mappingAt(0);
        qDebug() << os->objectPoints.size();
        qDebug() << os->scenePoints.size();
        cv::Mat H = cv::findHomography( os->objectPoints, os->scenePoints, CV_RANSAC );


    }

    return QMatDisplay::updatePaintNode(node, nodeData);
}

