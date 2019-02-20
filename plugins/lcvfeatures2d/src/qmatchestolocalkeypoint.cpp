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

#include "qmatchestolocalkeypoint.h"
#include <stdexcept>

/**
 * \class QMatchesToLocalKeypoint
 * \brief add brief
 * \ingroup plugin-lcvfeatures2d
 */

/**
*\brief QMatchesToLocalKeypoint constructor
*/
QMatchesToLocalKeypoint::QMatchesToLocalKeypoint(QQuickItem *parent)
    : QQuickItem(parent)
    , m_matches1to2(0)
    , m_queryKeypointVector(0)
    , m_output(new QKeyPointToSceneMap)
{
}

/**
*\brief QMatchesToLocalKeypoint destructor
*/
QMatchesToLocalKeypoint::~QMatchesToLocalKeypoint(){
    delete m_output;
}

void QMatchesToLocalKeypoint::componentComplete(){
    QQuickItem::componentComplete();
    mapValues();
}

void QMatchesToLocalKeypoint::setQueryWithMatches(QKeyPointVector *query, QDMatchVector *matches){
    m_queryKeypointVector = query;
    m_matches1to2         = matches;

    emit matches1to2Changed();
    emit queryKeypointVectorChanged();

    mapValues();
}

void QMatchesToLocalKeypoint::mapValues(){
    if ( !isComponentComplete() )
        return;
    if ( m_matches1to2 == 0 || m_queryKeypointVector == 0 )
        return;
    if ( !m_matches1to2->matches().size() )
        return;

    m_output->resize(m_trainKeypointVectors.size());
    std::vector<cv::DMatch>& matches = m_matches1to2->matches()[0];

    try{
        for ( std::vector<cv::DMatch>::iterator it = matches.begin(); it != matches.end(); ++it ){
            cv::DMatch& match = *it;
            if ( match.imgIdx >= m_trainKeypointVectors.size() )
                return;
            QKeyPointVector* trainVector = qobject_cast<QKeyPointVector*>(m_trainKeypointVectors[match.imgIdx]);
            if ( !trainVector ){
                qWarning("Invalid keypoint vector given at %d", match.imgIdx);
                return;
            }

            m_output->mappingAt(match.imgIdx)->objectPoints.push_back(
                trainVector->keypoints().at(match.trainIdx).pt
            );
            m_output->mappingAt(match.imgIdx)->scenePoints.push_back(
                m_queryKeypointVector->keypoints().at(match.queryIdx).pt
            );

        }

    } catch ( std::out_of_range& ){
        qCritical("Out of range reached when selecting query descriptor id. Is the query image synced with the matches?");
        m_output->resize(0);
    }

    emit outputChanged();
}

/**
 * \fn lv::QMatchesToLocalKeypoint::matches1to2()
 * \brief Matches from the first image to the second one
*/

/**
 * \fn lv::QMatchesToLocalKeypoint::setMatches1to2()
 * \brief Setter for the matches1to2
*/

/**
 * \fn lv::QMatchesToLocalKeypoint::matches1to2Changed()
 * \brief Triggered when matches1to2 is changed
*/

/**
 * \fn lv::QMatchesToLocalKeypoint::trainKeypointVectors()
 * \brief Triggers training of the keypoint vectors.
*/

/**
 * \fn lv::QMatchesToLocalKeypoint::setTrainKeypointVectors()
 * \brief Setter for the trainKeypointVectors
*/

/**
 * \fn lv::QMatchesToLocalKeypoint::trainKeypointVectorsChanged()
 * \brief Triggered when trainKeypointVectors are changed
*/

/**
 * \fn lv::QMatchesToLocalKeypoint::queryKeypointVector()
 * \brief Returns the query keypoins.
*/

/**
 * \fn lv::QMatchesToLocalKeypoint::setQueryKeypointVector()
 * \brief Setter for the queryKeypointVectors
*/

/**
 * \fn lv::QMatchesToLocalKeypoint::queryKeypointVectorChanged()
 * \brief Triggered when queryKeypointVectors are changed
*/


/**
 * \fn lv::QMatchesToLocalKeypoint::setQueryWithMatches()
 * \brief Setter for the queryWithMatches
*/

/**
 * \fn lv::QMatchesToLocalKeypoint::setMatches1to2()
 * \brief Setter for the matches1to2
*/

/**
 * \fn lv::QMatchesToLocalKeypoint::matches1to2Changed()
 * \brief Triggered when matches1to2 is changed
*/

/**
 * \fn lv::QMatchesToLocalKeypoint::output()
 * \brief Returns the output.
*/

/**
 * \fn lv::QMatchesToLocalKeypoint::outputChanged()
 * \brief Triggered when output is changed
*/

/**
 * \fn lv::QMatchesToLocalKeypoint::componentComplete()
 * \brief Implementation from QQuickItem. Triggers a mapping of the matches if the input has been set.
*/

