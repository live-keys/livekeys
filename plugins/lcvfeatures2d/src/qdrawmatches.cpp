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

#include "qdrawmatches.h"
#include "opencv2/features2d.hpp"

#include <iostream>

QDrawMatches::QDrawMatches(QQuickItem *parent)
    : QMatDisplay(parent)
    , m_keypoints1(0)
    , m_keypoints2(0)
    , m_matches(0)
    , m_matchSurfaceDirty(false)
    , m_matchIndex(-1)
{
}

QDrawMatches::~QDrawMatches(){
}

void QDrawMatches::setMatches1to2(QDMatchVector *matches1to2){
    m_matches = matches1to2;
    emit matches1to2Changed();
    m_matchSurfaceDirty = true;
    update();
}

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
