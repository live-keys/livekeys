/****************************************************************************
**
** Copyright (C) 2014-2016 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

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

        qDebug() << "Keypoints to Scene: " << m_keypointsToScene->size();

        for ( int i = 0; i < m_keypointsToScene->size(); ++i ){
            if ( i >= m_objectCorners.size() )
                break;

            QKeyPointToSceneMap::ObjectKeypointToScene* os = m_keypointsToScene->mappingAt(i);

            qDebug() << "Scene points:" << os->scenePoints.size() << os->objectPoints.size();
            QString scenePData;
            QString objectPData;
//            for ( int i = 0; i < 10; ++i ){
//                scenePData  += "(" + QString::number(os->scenePoints[i].x) + "," + QString::number(os->scenePoints[i].y) + ")";
//                objectPData += "(" + QString::number(os->objectPoints[i].x) + "," + QString::number(os->objectPoints[i].y) + ")";
//            }
            qDebug() << "Scene point data:" <<  scenePData;
            qDebug() << "Object point data:" << objectPData;

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
                    cv::line(*surface, sceneCorners[si], sceneCorners[si + 1], colorAt(i), 4);
                }
                if ( sceneCorners.size() > 1 )
                    cv::line(*surface, sceneCorners[sceneCorners.size() - 1], sceneCorners[0], colorAt(i), 4);
            }
        }

    }

    return QMatDisplay::updatePaintNode(node, nodeData);
}

cv::Scalar QKeypointHomography::colorAt(int i) const{
    if ( m_cachedObjectColors.size() == 0 )
        return cv::Scalar(0, 255, 0);
    return m_cachedObjectColors[i % m_cachedObjectColors.size()];
}

