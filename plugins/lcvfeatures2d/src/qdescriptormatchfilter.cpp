#include "qdescriptormatchfilter.h"
#include <QDebug>

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

void QDescriptorMatchFilter::filterMatches(
        const std::vector<std::vector<cv::DMatch> > &src,
        std::vector<std::vector<cv::DMatch> > &dst
){

    //TODO: Solve for BEST_MATCH vs KNN_MATCH scenarios

    if ( !isFilterSet() ){
        copyMatches(src, dst);
        return;
    }

    if ( src.size() == 0 )
        return;

    dst.resize(1);
    dst[0].clear();
    std::vector<cv::DMatch>& dstItem = dst[0];

    qDebug() << "FIRST SIZE:" << src.size();
    qDebug() << "SECOND SIZE:" << src[0].size();

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
}

void QDescriptorMatchFilter::copyMatches(
        const std::vector<std::vector<cv::DMatch> > &src,
        std::vector<std::vector<cv::DMatch> > &dst
){
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

void QDescriptorMatchFilter::componentComplete(){
    QQuickItem::componentComplete();
    callFilterMatches();
}


QDMatchVector *QDescriptorMatchFilter::matches1to2() const{
    return m_matches1to2;
}

void QDescriptorMatchFilter::callFilterMatches(){
    if ( isComponentComplete() && m_matches1to2 ){
        filterMatches(m_matches1to2->matches(), m_matches1to2Out->matches());
        emit matches1to2OutChanged();
    }
}
