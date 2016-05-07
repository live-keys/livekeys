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

#include "qmatchestolocalkeypoint.h"

QMatchesToLocalKeypoint::QMatchesToLocalKeypoint(QQuickItem *parent)
    : QQuickItem(parent)
    , m_matches1to2(0)
    , m_queryKeypointVector(0)
    , m_output(new QKeyPointToSceneMap)
{
}

QMatchesToLocalKeypoint::~QMatchesToLocalKeypoint(){
    delete m_output;
}

void QMatchesToLocalKeypoint::componentComplete(){
    QQuickItem::componentComplete();
    mapValues();
}

void QMatchesToLocalKeypoint::mapValues(){
    if ( !isComponentComplete() )
        return;
    if ( m_matches1to2 == 0 || m_queryKeypointVector == 0 )
        return;
    if ( !m_matches1to2->matches().size() )
        return;

    m_output = new QKeyPointToSceneMap;
    m_output->resize(m_trainKeypointVectors.size());

    std::vector<cv::DMatch>& matches = m_matches1to2->matches()[0];

    // EDIT

//    qDebug() << "Mapping values";
//    QString top;
//    QString bottom;
//    for ( int i = 0; i < 10; ++i ){
//        cv::DMatch& match = matches[i];
//        top    += QString::number(match.trainIdx) + " ";
//        bottom += QString::number(match.queryIdx) + " ";
//    }
//    qDebug() << top;
//    qDebug() << bottom;

    int j = 0;

    QString p1line;
    QString p2line;

    // EDIT END

    for ( std::vector<cv::DMatch>::iterator it = matches.begin(); it != matches.end(); ++it ){
        cv::DMatch& match = *it;
        if ( match.imgIdx >= m_trainKeypointVectors.size() )
            return;
        QKeyPointVector* trainVector = qobject_cast<QKeyPointVector*>(m_trainKeypointVectors[match.imgIdx]);
        if ( !trainVector ){
            qWarning("Invalid keypoint vector given at %d", match.imgIdx);
            return;
        }

        // EDIT

        if ( j++ < 10 ){
            QString pt1 = "(" + QString::number(trainVector->keypoints().at(match.trainIdx).pt.x) + "," + QString::number(trainVector->keypoints().at(match.trainIdx).pt.y) + ")";
            QString pt2 = "(" + QString::number(m_queryKeypointVector->keypoints().at(match.queryIdx).pt.x) + "," + QString::number(m_queryKeypointVector->keypoints().at(match.queryIdx).pt.y) + ")";
//            qDebug() << trainVector->keypoints().at(match.trainIdx).pt << m_queryKeypointVector->keypoints().at(match.queryIdx).pt;
            p1line += pt1;
            p2line += pt2;
        }

        m_output->mappingAt(match.imgIdx)->objectPoints.push_back(
            trainVector->keypoints().at(match.trainIdx).pt
        );
        m_output->mappingAt(match.imgIdx)->scenePoints.push_back(
            m_queryKeypointVector->keypoints().at(match.queryIdx).pt
        );

    }

    qDebug() << "-------Train Points" << p1line;
    qDebug() << "-------Query Points" << p2line;

    emit outputChanged();
}

