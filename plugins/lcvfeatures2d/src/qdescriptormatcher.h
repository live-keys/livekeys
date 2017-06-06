/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

#ifndef QDESCRIPTORMATCHER_H
#define QDESCRIPTORMATCHER_H

#include <QQuickItem>
#include "qmat.h"
#include "qlcvfeatures2dglobal.h"
#include "qdmatchvector.h"

namespace cv{
    class DescriptorMatcher;
}

class Q_LCVFEATURES2D_EXPORT QDescriptorMatcher : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QMat*          queryDescriptors READ queryDescriptors WRITE  setQueryDescriptors NOTIFY queryDescriptorsChanged)
    Q_PROPERTY(QDMatchVector* matches          READ matches          NOTIFY matchesChanged)
    Q_PROPERTY(QVariantMap    params           READ params           WRITE  setParams           NOTIFY paramsChanged)
    Q_PROPERTY(int            knn              READ knn              WRITE  setKnn              NOTIFY knnChanged)

public:
    explicit QDescriptorMatcher(QQuickItem *parent = 0);
    QDescriptorMatcher(cv::DescriptorMatcher* matcher, QQuickItem *parent = 0);
    virtual ~QDescriptorMatcher();

    void setQueryDescriptors(QMat* descriptors);
    QMat* queryDescriptors();

    void setKnn(int knn);
    int knn() const;

    QDMatchVector* matches();

    const QVariantMap& params() const;

signals:
    void queryDescriptorsChanged();
    void matchesChanged();
    void knnChanged();

    void paramsChanged();

public slots:
    void add(QMat* descriptors);
    void train();

    void match(QMat* queryDescriptors, QDMatchVector* matches);
    void knnMatch(QMat* queryDescriptors, QDMatchVector* matches, int k = 2);

    void setParams(const QVariantMap &arg);

protected:
    virtual void componentComplete();
    virtual void initialize(const QVariantMap& params);

    void initializeMatcher(cv::DescriptorMatcher* matcher);
    void callMatch();

private:
    cv::DescriptorMatcher* m_matcher;

    QDMatchVector*         m_matches;
    QMat*                  m_queryDescriptors;

    int                    m_knn;
    QVariantMap            m_params;
};

inline void QDescriptorMatcher::setQueryDescriptors(QMat* descriptors){
    if ( descriptors == 0 )
        return;

    m_queryDescriptors = descriptors;
    emit queryDescriptorsChanged();
    callMatch();
}

inline QMat* QDescriptorMatcher::queryDescriptors(){
    return m_queryDescriptors;
}

inline void QDescriptorMatcher::setKnn(int knn){
    if ( m_knn != knn ){
        m_knn = knn;
        emit knnChanged();
        callMatch();
    }
}

inline int QDescriptorMatcher::knn() const{
    return m_knn;
}

inline const QVariantMap& QDescriptorMatcher::params() const{
    return m_params;
}

inline void QDescriptorMatcher::setParams(const QVariantMap& arg){
    if (m_params == arg)
        return;

    initialize(arg);
    m_params = arg;
    emit paramsChanged();
}

#endif // QDESCRIPTORMATCHER_H
