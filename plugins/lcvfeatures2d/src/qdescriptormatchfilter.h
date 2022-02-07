/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
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

#ifndef QMATCHFILTER_H
#define QMATCHFILTER_H

#include <QQuickItem>
#include "qdmatchvector.h"
#include "qlcvfeatures2dglobal.h"

class Q_LCVFEATURES2D_EXPORT QDescriptorMatchFilter : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QDMatchVector* matches1to2      READ matches1to2      WRITE  setMatches1to2      NOTIFY matches1to2Changed)
    Q_PROPERTY(QDMatchVector* matches1to2Out   READ matches1to2Out   NOTIFY matches1to2OutChanged)
    Q_PROPERTY(float          minDistanceCoeff READ minDistanceCoeff WRITE  setMinDistanceCoeff NOTIFY minDistanceCoeffChanged)
    Q_PROPERTY(float          maxDistance      READ maxDistance      WRITE  setMaxDistance      NOTIFY maxDistanceChanged)
    Q_PROPERTY(float          nndrRatio        READ nndrRatio        WRITE  setNndrRatio        NOTIFY nndrRatioChanged)

public:
    QDescriptorMatchFilter(QQuickItem* parent = 0);
    virtual ~QDescriptorMatchFilter();

    QDMatchVector* matches1to2() const;
    void setMatches1to2(QDMatchVector* arg);

    QDMatchVector* matches1to2Out() const;
    
    float minDistanceCoeff() const;
    void setMinDistanceCoeff(float arg);

    float maxDistance() const;
    void setMaxDistance(float arg);

    float nndrRatio() const;
    void setNndrRatio(float arg);

signals:
    void matches1to2Changed();
    void matches1to2OutChanged();
    void minDistanceCoeffChanged();
    void maxDistanceChanged();
    void nndrRatioChanged();

protected:
    /// \private
    void callFilterMatches();
    /// \private
    QDMatchVector::Type filterKnnMatches(
        const std::vector<std::vector<cv::DMatch> >& src,
        std::vector<std::vector<cv::DMatch> >& dst
    );
    /// \private
    QDMatchVector::Type filterBestMatches(
        const std::vector<std::vector<cv::DMatch> >& src,
        std::vector<std::vector<cv::DMatch> >& dst
    );
    /// \private
    void copyMatches(
        const std::vector<std::vector<cv::DMatch> >& src,
        std::vector<std::vector<cv::DMatch> >& dst
    );
    /// \private
    virtual void componentComplete();

private:
    bool isFilterSet();
    bool isMinDistanceFilterSet();
    bool isMaxDistanceFilterSet();
    bool isNndrRatioSet();

    QDMatchVector* m_matches1to2;
    QDMatchVector* m_matches1to2Out;
    float          m_minDistanceCoeff;
    float          m_maxDistance;
    float          m_nndrRatio;
};

inline void QDescriptorMatchFilter::setMatches1to2(QDMatchVector *arg){
    m_matches1to2 = arg;
    emit matches1to2Changed();
    callFilterMatches();
}

inline QDMatchVector *QDescriptorMatchFilter::matches1to2Out() const{
    return m_matches1to2Out;
}

inline float QDescriptorMatchFilter::minDistanceCoeff() const{
    return m_minDistanceCoeff;
}

inline void QDescriptorMatchFilter::setMinDistanceCoeff(float arg){
    if (m_minDistanceCoeff == arg)
        return;

    m_minDistanceCoeff = arg;
    emit minDistanceCoeffChanged();
    callFilterMatches();
}

inline float QDescriptorMatchFilter::maxDistance() const{
    return m_maxDistance;
}

inline void QDescriptorMatchFilter::setMaxDistance(float arg){
    if (m_maxDistance == arg)
        return;

    m_maxDistance = arg;
    emit maxDistanceChanged();
    callFilterMatches();
}

inline float QDescriptorMatchFilter::nndrRatio() const{
    return m_nndrRatio;
}

inline void QDescriptorMatchFilter::setNndrRatio(float arg){
    if (m_nndrRatio == arg)
        return;

    m_nndrRatio = arg;
    emit nndrRatioChanged();
    callFilterMatches();
}

inline bool QDescriptorMatchFilter::isFilterSet(){
    return isMinDistanceFilterSet() || isMaxDistanceFilterSet() || isNndrRatioSet();
}

inline bool QDescriptorMatchFilter::isMinDistanceFilterSet(){
    return m_minDistanceCoeff > -1;
}

inline bool QDescriptorMatchFilter::isMaxDistanceFilterSet(){
    return m_maxDistance > -1;
}

inline bool QDescriptorMatchFilter::isNndrRatioSet(){
    return m_nndrRatio > -1;
}

#endif // QMATCHFILTER_H
