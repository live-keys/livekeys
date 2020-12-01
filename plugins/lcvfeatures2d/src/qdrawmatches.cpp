/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Livekeys Application.
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

#include "qdrawmatches.h"
#include "opencv2/features2d.hpp"

#include <iostream>

/**
 * \class QDrawMatches
 * \brief Draws the found matches of keypoints from two images.
 * \ingroup plugin-lcvfeatures2d
 */

/**
 *\brief QDrawMatches constructor 
 */
QDrawMatches::QDrawMatches(QQuickItem *parent)
    : QMatDisplay(parent)
    , m_keypoints1(0)
    , m_keypoints2(0)
    , m_matches(0)
    , m_matchSurfaceDirty(false)
    , m_matchIndex(-1)
{
}

/**
*\brief QDrawMatches desctructor
*/
QDrawMatches::~QDrawMatches(){
}

/**
* \fn lv::QDrawMatches::keypoints1()
* \brief Keypoint from the first source   
*/

/**
* \fn lv::QDrawMatches::setKeypoints1()
* \brief Setter for the keypoints1   
*/

/**
* \fn lv::QDrawMatches::keypoints1Changed()
* \brief Triggered when the keypoints1 is changed   
*/

/**
* \fn lv::QDrawMatches::keypoints2()
* \brief Keypoint from the second source   
*/

/**
* \fn lv::QDrawMatches::setKeypoints2()
* \brief Setter for the keypoints2   
*/

/**
* \fn lv::QDrawMatches::keypoints2Changed()
* \brief Triggered when the keypoints2 is changed   
*/

/**
* \fn lv::QDrawMatches::matches1to2()
* \brief Matches from the first image to the second one.
*/

/**
* \fn lv::QDrawMatches::matches1to2Changed()
* \brief Triggered when matches1to2 is changed
*/


/**
* \fn lv::QDrawMatches::drawExtractedMatches()
* \brief Filter only specific image.
*/

/**
* \fn lv::QDrawMatches::matchIndex()
* \brief Filter only specific image.
*/

/**
* \fn lv::QDrawMatches::setMatchIndex()
* \brief Setter for the matchIndex
*/

/**
* \fn lv::QDrawMatches::matchIndexChanged()
* \brief Triggered when the matchIndex is changed.
*/

/**
 * \brief Setter for the matches1to2
 */
void QDrawMatches::setMatches1to2(QDMatchVector *matches1to2){
    m_matches = matches1to2;
    emit matches1to2Changed();
    m_matchSurfaceDirty = true;
    update();
}
/**
*\brief Implementation from QQuickItem
*/
QSGNode *QDrawMatches::updatePaintNode(QSGNode *node, UpdatePaintNodeData *nodeData){

    if ( m_keypoints1 &&
         m_keypoints2 &&
         m_matches &&
         m_matchSurfaceDirty &&
         m_matchIndex != -1 &&
         m_matches->matches().size() > 0
    ){
        if ( m_matches->type() == QDMatchVector::BEST_MATCH ){
            drawExtractedMatches(m_matches->matches()[0]);
        } else {
            std::vector<cv::DMatch> selectedMatches;
            selectedMatches.resize(m_matches->matches().size());
            for ( size_t i = 0; i < m_matches->matches().size(); ++i )
                selectedMatches[i] = m_matches->matches()[i][0];
            drawExtractedMatches(selectedMatches);
        }

    }
    return QMatDisplay::updatePaintNode(node, nodeData);
}

void QDrawMatches::drawExtractedMatches(const std::vector<cv::DMatch> matches){
    m_mask.resize(matches.size());
    std::fill(m_mask.begin(), m_mask.end(), 0);
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
            matches,
            *(output()->internalPtr()),
            cv::Scalar::all(-1),
            cv::Scalar::all(-1),
            m_mask
        );
        setImplicitSize(output()->internalPtr()->cols, output()->internalPtr()->rows);

    } catch(cv::Exception& e){
        qWarning("%s", e.what());
    }

    m_matchSurfaceDirty = false;
}
