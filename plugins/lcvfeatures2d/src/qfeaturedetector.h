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

#ifndef QFEATUREDETECTOR_H
#define QFEATUREDETECTOR_H

#include "qmatdisplay.h"
#include "qlcvfeatures2dglobal.h"
#include "opencv2/features2d.hpp"

class QKeyPointVector;
class Q_LCVFEATURES2D_EXPORT QFeatureDetector : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QMat* input                READ inputMat  WRITE setInputMat  NOTIFY inputChanged)
    Q_PROPERTY(QMat* output               READ output    NOTIFY outputChanged)
    Q_PROPERTY(QMat* mask                 READ mask      WRITE setMask      NOTIFY maskChanged)
    Q_PROPERTY(QVariantMap params         READ params    WRITE setParams    NOTIFY paramsChanged)
    Q_PROPERTY(QKeyPointVector* keypoints READ keypoints WRITE setKeypoints NOTIFY keypointsChanged)

public:
    explicit QFeatureDetector(QQuickItem *parent = 0);
    QFeatureDetector(cv::Ptr<cv::FeatureDetector> detector, QQuickItem* parent = 0);
    virtual ~QFeatureDetector();

public:
    QMat* inputMat();
    void setInputMat(QMat* mat);

    QMat* output();

    QMat* mask();
    void setMask(QMat* mat);

    QKeyPointVector* keypoints();
    void setKeypoints(QKeyPointVector* keypoints);

    const QVariantMap &params() const;

protected:
    virtual void initialize(const QVariantMap& params);

    cv::FeatureDetector* detector();
    void initializeDetector(cv::Ptr<cv::FeatureDetector> detector);
    void detect();
    virtual void componentComplete();

public:
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *);

public slots:
    void setParams(const QVariantMap& arg);

signals:
    void inputChanged();
    void maskChanged();
    void keypointsChanged();
    void outputChanged();
    void paramsChanged();

private:
    void drawKeypoints();

    cv::Ptr<cv::FeatureDetector> m_detector;
    QKeyPointVector*     m_keypoints;

    QMat* m_in;
    QMat* m_output;
    QMat* m_mask;
    QVariantMap m_params;

    bool  m_outputDirty;
};

inline QMat *QFeatureDetector::inputMat(){
    return m_in;
}

inline void QFeatureDetector::setInputMat(QMat *mat){
    if ( mat == 0 )
        return;

    cv::Mat* matData = mat->cvMat();
    if ( implicitWidth() != matData->cols || implicitHeight() != matData->rows ){
        setImplicitWidth(matData->cols);
        setImplicitHeight(matData->rows);
    }
    m_in = mat;
    emit inputChanged();
    detect();
}

inline QMat* QFeatureDetector::output(){
    if ( m_outputDirty )
        drawKeypoints();
    return m_output;
}

inline QMat* QFeatureDetector::mask(){
    return m_mask;
}

inline void QFeatureDetector::setMask(QMat* mat){
    if ( mat == 0 )
        return;

    m_mask = mat;
    emit maskChanged();
    detect();
}

inline QKeyPointVector* QFeatureDetector::keypoints(){
    return m_keypoints;
}

inline void QFeatureDetector::setKeypoints(QKeyPointVector* keypoints){
    m_keypoints = keypoints;
    emit keypointsChanged();
}

inline void QFeatureDetector::initialize(const QVariantMap &){}

inline const QVariantMap& QFeatureDetector::params() const{
    return m_params;
}

inline void QFeatureDetector::setParams(const QVariantMap &arg){
    if (m_params == arg)
        return;

    m_params = arg;

    initialize(arg);
    emit paramsChanged();
    detect();
}

#endif // QFEATUREDETECTOR_H
