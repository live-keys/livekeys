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

#include "qfeaturedetector.h"
#include "qkeypointvector.h"
#include "qmatnode.h"
#include "qmatshader.h"
#include "opencv2/features2d/features2d.hpp"

// null keypoints
// null descriptors
// defined keypoints
// defined descriptors


QFeatureDetector::QFeatureDetector(QQuickItem *parent)
    : QQuickItem(parent)
    , m_detector(0)
    , m_keypoints(new QKeyPointVector)
    , m_in(QMat::nullMat())
    , m_output(new QMat)
    , m_mask(QMat::nullMat())
    , m_outputDirty(false)
{
    setFlag(ItemHasContents, true);
}

QFeatureDetector::QFeatureDetector(cv::FeatureDetector* detector, QQuickItem* parent)
    : QQuickItem(parent)
    , m_detector(detector)
    , m_keypoints(new QKeyPointVector)
    , m_in(QMat::nullMat())
    , m_output(new QMat)
    , m_mask(QMat::nullMat())
    , m_outputDirty(false)
{
    setFlag(ItemHasContents, true);
}

QFeatureDetector::~QFeatureDetector(){
    delete m_detector;
    delete m_output;
}

cv::FeatureDetector *QFeatureDetector::detector(){
    return m_detector;
}

void QFeatureDetector::initializeDetector(cv::FeatureDetector *detector){
    delete m_detector;
    m_detector = detector;
    detect();
}

void QFeatureDetector::detect(){
    if ( m_detector != 0 && !m_in->cvMat()->empty() && isComponentComplete() ){
        m_detector->detect(*m_in->cvMat(), m_keypoints->keypoints(), *m_mask->cvMat());
        cv::Mat inClone = m_in->cvMat()->clone();
        m_keypoints->setMat(inClone);
        emit keypointsChanged();
        m_outputDirty = true;
        emit outputChanged();
        update();
    }
}

void QFeatureDetector::componentComplete(){
    QQuickItem::componentComplete();
    detect();
}

QSGNode* QFeatureDetector::updatePaintNode(QSGNode* node, QQuickItem::UpdatePaintNodeData*){
    if ( m_outputDirty )
        drawKeypoints();

    QMatNode *n = static_cast<QMatNode*>(node);
    if (!node)
        n = new QMatNode();

    QSGGeometry::updateTexturedRectGeometry(n->geometry(), boundingRect(), QRectF(0, 0, 1, 1));
    QMatState* state = static_cast<QSGSimpleMaterial<QMatState>*>(n->material())->state();
    state->mat          = m_output;
    state->textureSync  = false;
    state->linearFilter = false;
    n->markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);

    return n;
}

void QFeatureDetector::drawKeypoints(){
    if ( !inputMat()->cvMat()->empty() ){
        cv::drawKeypoints(
            *m_in->cvMat(),
             m_keypoints->keypoints(),
            *m_output->cvMat(),
             cv::Scalar::all(-1),
             cv::DrawMatchesFlags::DEFAULT
        );
    }
    m_outputDirty = false;
}
