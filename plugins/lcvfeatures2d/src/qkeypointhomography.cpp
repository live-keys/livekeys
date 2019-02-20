/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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
#include "opencv2/calib3d.hpp"
#include "opencv2/imgproc.hpp"

/**
 * \class QKeypointHomography
 * \brief Maps the keypoints in one image to the corresponding keypoints in the other image
 * \ingroup plugin-lcvfeatures2d
 */

/**
 *\brief QKeypointHomography constructor
 */
QKeypointHomography::QKeypointHomography(QQuickItem* parent)
    : QMatDisplay(parent)
    , m_keypointsToScene(0)
    , m_queryImage(0)
{
    setFlag(QQuickItem::ItemHasContents, true);
}

/**
 *\brief QKeypointHomography desctructor
 */
QKeypointHomography::~QKeypointHomography(){
}

/**
*\fn lv::QKeypointHomography::queryImage()
*\brief Getter for The image that is used 
*/

/**
*\fn lv::QKeypointHomography::setQueryImage()
*\brief Setter for the image that is used 
*/

/**
*\fn lv::QKeypointHomography::objectCorners()
*\brief Specifies object corners 
*/

/**
*\fn lv::QKeypointHomography::objectColors()
*\brief Specifies object colors
*/

/**
*\fn lv::QKeypointHomography::setObjectColors()
*\brief Setter for the object colors
*/

QSGNode *QKeypointHomography::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *nodeData){
    if ( m_keypointsToScene && m_queryImage ){

        cv::Mat* surface = output()->cvMat();
        m_queryImage->cvMat()->copyTo(*surface);

        for ( int i = 0; i < m_keypointsToScene->size(); ++i ){
            if ( i >= m_objectCorners.size() )
                break;

            QKeyPointToSceneMap::ObjectKeypointToScene* os = m_keypointsToScene->mappingAt(i);

            if ( os->scenePoints.size() > 10 ){
                std::vector<uchar> outlierMask;
                cv::Mat H = cv::findHomography(os->objectPoints, os->scenePoints, outlierMask, CV_RANSAC, 4);

                size_t outlierCount = 0;
                for ( size_t k = 0; k < outlierMask.size(); ++k ){
                    if ( !outlierMask[k] )
                        outlierCount++;
                }

//                if ( outlierMask.size() / 2 > outlierCount ){

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
//                }
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

