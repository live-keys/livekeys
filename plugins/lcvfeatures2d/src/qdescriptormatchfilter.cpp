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

#include "qdescriptormatchfilter.h"

QDescriptorMatchFilter::QDescriptorMatchFilter(QQuickItem* parent)
    : QQuickItem(parent)
    , m_matches1to2(0)
    , m_matches1to2Out(new QDMatchVector)
    , m_minDistanceCoeff(-1)
    , m_maxDistance(-1)
    , m_nndrRatio(-1)
{
    QQuickItem::setFlag(QQuickItem::ItemHasContents, false);
}

QDescriptorMatchFilter::~QDescriptorMatchFilter(){
}

QDMatchVector::Type QDescriptorMatchFilter::filterKnnMatches(
        const std::vector<std::vector<cv::DMatch> > &src,
        std::vector<std::vector<cv::DMatch> > &dst)
{
    if ( !isFilterSet() ){
        copyMatches(src, dst);
        return QDMatchVector::KNN;
    }

    if ( src.size() == 0 )
        return QDMatchVector::KNN;

    dst.resize(1);
    dst[0].clear();
    std::vector<cv::DMatch>& dstItem = dst[0];

    float minDistance = 100;
    if ( isMinDistanceFilterSet() ){
        for( size_t i = 0; i < src.size(); ++i ){
            if ( src[i].size() > 0 ){
                if ( src[i][0].distance < minDistance )
                    minDistance = src[i][0].distance;
            }
        }
    }

    bool nndrRatioCheck = isNndrRatioSet() && (src[0].size() > 1);

    for( size_t i = 0; i < src.size(); ++i ){
        if ( src[i].size() != 0 ){

            const cv::DMatch& srcMatch = src[i][0];

            bool srcValid = true;
            if ( isMaxDistanceFilterSet() )
                if ( srcMatch.distance < m_maxDistance )
                    srcValid = false;

            if ( isMinDistanceFilterSet() )
                if ( srcMatch.distance > m_minDistanceCoeff * minDistance)
                    srcValid = false;

            if ( nndrRatioCheck )
                if ( srcMatch.distance >= m_nndrRatio * src[i][1].distance )
                    srcValid = false;

            if ( srcValid )
                dstItem.push_back(srcMatch);
        }
    }

    return QDMatchVector::BEST_MATCH;
}

void QDescriptorMatchFilter::copyMatches(
    const std::vector<std::vector<cv::DMatch> > &src,
    std::vector<std::vector<cv::DMatch> > &dst)
{
    if ( src.size() != dst.size() )
        dst.resize(src.size());

    for ( size_t i = 0; i < src.size(); ++i ){
        const std::vector<cv::DMatch>& srcItem = src[i];
        std::vector<cv::DMatch>& dstItem       = dst[i];
        if ( srcItem.size() != dstItem.size() )
            dstItem.resize(srcItem.size());

        for ( size_t j = 0; j < srcItem.size(); ++j ){
            dstItem[j] = srcItem[j];
        }
    }
}

QDMatchVector::Type QDescriptorMatchFilter::filterBestMatches(
    const std::vector<std::vector<cv::DMatch> > &src,
    std::vector<std::vector<cv::DMatch> > &dst)
{
    if ( !isFilterSet() ){
        copyMatches(src, dst);
        return QDMatchVector::BEST_MATCH;
    }

    if ( src.size() == 0 )
        return QDMatchVector::BEST_MATCH;
    if ( src[0].size() == 0 )
        return QDMatchVector::BEST_MATCH;

    dst.resize(1);
    dst[0].clear();
    const std::vector<cv::DMatch>& srcItem = src[0];
    std::vector<cv::DMatch>& dstItem       = dst[0];

    float minDistance = 100;
    for( size_t i = 0; i < srcItem.size(); ++i ){
        if ( srcItem[i].distance < minDistance )
            minDistance = srcItem[i].distance;
    }

    for( size_t i = 0; i < srcItem.size(); ++i ){
        const cv::DMatch& srcMatch = srcItem[i];
        bool srcValid = true;
        if ( isMaxDistanceFilterSet() )
            if ( srcMatch.distance < m_maxDistance )
                srcValid = false;

        if ( isMinDistanceFilterSet() )
            if ( srcMatch.distance > m_minDistanceCoeff * minDistance)
                srcValid = false;

        if ( srcValid )
            dstItem.push_back(srcMatch);
    }

    return QDMatchVector::BEST_MATCH;
}

void QDescriptorMatchFilter::componentComplete(){
    QQuickItem::componentComplete();
    callFilterMatches();
}


QDMatchVector *QDescriptorMatchFilter::matches1to2() const{
    return m_matches1to2;
}

void QDescriptorMatchFilter::callFilterMatches(){
    if ( isComponentComplete() && m_matches1to2 ){
        if ( m_matches1to2->type() == QDMatchVector::BEST_MATCH )
            m_matches1to2Out->setType(filterBestMatches(m_matches1to2->matches(), m_matches1to2Out->matches()));
        else
            m_matches1to2Out->setType(filterKnnMatches(m_matches1to2->matches(), m_matches1to2Out->matches()));
        emit matches1to2OutChanged();
    }
}
