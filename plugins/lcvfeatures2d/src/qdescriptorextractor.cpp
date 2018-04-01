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

#include "qdescriptorextractor.h"
#include "qkeypointvector.h"

QDescriptorExtractor::QDescriptorExtractor(QQuickItem *parent)
    : QQuickItem(parent)
    , m_extractor()
    , m_keypoints(0)
    , m_descriptors(new QMat)
    , m_isBinary(true)
{
}

QDescriptorExtractor::QDescriptorExtractor(cv::Ptr<cv::DescriptorExtractor> extractor, QQuickItem* parent)
    : QQuickItem(parent)
    , m_extractor(extractor)
    , m_keypoints(0)
    , m_descriptors(new QMat)
    , m_isBinary(true)
{
}

QDescriptorExtractor::~QDescriptorExtractor(){
    delete m_descriptors;
}

void QDescriptorExtractor::initialize(const QVariantMap &){}

void QDescriptorExtractor::initializeExtractor(cv::Ptr<cv::DescriptorExtractor> extractor){
    m_extractor = extractor;
    compute();
}

void QDescriptorExtractor::componentComplete(){
    QQuickItem::componentComplete();
    compute();
}

void QDescriptorExtractor::compute(){
    if ( m_extractor && m_keypoints && isComponentComplete() ){
        m_extractor->compute(m_keypoints->cvMat(), m_keypoints->keypoints(), *m_descriptors->cvMat());
        if ( (m_keypoints->cvMat().cols != 0 || m_keypoints->cvMat().rows != 0) && m_descriptors->cvMat()->cols == 0){
            m_descriptors->cvMat()->create(0, descriptorCols(), CV_8UC1);
        }
        emit descriptorsChanged();
    }
}
