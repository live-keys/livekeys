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

#ifndef QDESCRIPTOREXTRACTOR_H
#define QDESCRIPTOREXTRACTOR_H

#include <QQuickItem>
#include "qmat.h"
#include "qlcvfeatures2dglobal.h"
#include "qkeypointvector.h"

class QKeyPointVector;
class Q_LCVFEATURES2D_EXPORT QDescriptorExtractor : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QKeyPointVector* keypoints READ keypoints   WRITE setKeypoints NOTIFY keypointsChanged)
    Q_PROPERTY(QMat* descriptors          READ descriptors NOTIFY descriptorsChanged)
    Q_PROPERTY(QVariantMap params         READ params      WRITE setParams    NOTIFY paramsChanged)
    Q_PROPERTY(bool isBinary              READ isBinary)

public:
    explicit QDescriptorExtractor(QQuickItem *parent = 0);
    QDescriptorExtractor(cv::Ptr<cv::DescriptorExtractor> extractor, QQuickItem* parent = 0);
    virtual ~QDescriptorExtractor();

    QKeyPointVector* keypoints();
    void setKeypoints(QKeyPointVector* keypoints);

    QMat* descriptors();
    const QVariantMap& params() const;
    bool isBinary() const;

    virtual int descriptorCols() const;

protected:
    virtual void initialize(const QVariantMap& params);

    cv::Ptr<cv::DescriptorExtractor> extractor();
    void initializeExtractor(cv::Ptr<cv::DescriptorExtractor> extractor);
    virtual void componentComplete();

signals:
    void keypointsChanged();
    void descriptorsChanged();
    void paramsChanged();

public slots:
    void compute();
    void setParams(const QVariantMap &arg);

private:
    cv::Ptr<cv::DescriptorExtractor> m_extractor;

    QKeyPointVector*         m_keypoints;
    QMat*                    m_descriptors;
    QVariantMap              m_params;
    bool                     m_isBinary;
};

inline QKeyPointVector* QDescriptorExtractor::keypoints(){
    return m_keypoints;
}

inline void QDescriptorExtractor::setKeypoints(QKeyPointVector* keypoints){
    m_keypoints = keypoints;
    emit keypointsChanged();
    compute();
}

inline QMat*QDescriptorExtractor::descriptors(){
    return m_descriptors;
}

inline const QVariantMap &QDescriptorExtractor::params() const{
    return m_params;
}

inline bool QDescriptorExtractor::isBinary() const{
    return m_isBinary;
}

inline int QDescriptorExtractor::descriptorCols() const{
    return 32;
}

inline void QDescriptorExtractor::setParams(const QVariantMap &arg){
    if (m_params == arg)
        return;

    try {
        m_params = arg;
        initialize(m_params);
        emit paramsChanged();
    } catch ( cv::Exception& e ){
        qWarning("%s", e.what());
    }
}

#endif // QDESCRIPTOREXTRACTOR_H
