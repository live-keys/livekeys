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

#include "qdescriptorextractor.h"
#include "qkeypointvector.h"

/**
 * \class QDescriptorExtractor
 * \brief Abstract base class to compute descriptors for image keypoints.
 * \ingroup plugin-lcvfeatures2d
 */

/**
 *\brief QDescriptorExtractor constructor
 */
QDescriptorExtractor::QDescriptorExtractor(QQuickItem *parent)
    : QQuickItem(parent)
    , m_extractor()
    , m_keypoints(0)
    , m_descriptors(new QMat)
    , m_isBinary(true)
{
}
/**
 *\brief QDescriptorExtractor constructor
 */
QDescriptorExtractor::QDescriptorExtractor(cv::Ptr<cv::DescriptorExtractor> extractor, QQuickItem* parent)
    : QQuickItem(parent)
    , m_extractor(extractor)
    , m_keypoints(0)
    , m_descriptors(new QMat)
    , m_isBinary(true)
{
}

/**
 * QDescriptorExtractor destructor
 */
QDescriptorExtractor::~QDescriptorExtractor(){
    delete m_descriptors;
}
/**
 * \brief Initializes QDescriptorExtractor
 */
void QDescriptorExtractor::initialize(const QVariantMap &){}
/**
 * \brief Creates a descriptor extractor by name.
 */
void QDescriptorExtractor::initializeExtractor(cv::Ptr<cv::DescriptorExtractor> extractor){
    m_extractor = extractor;
    compute();
}

/**
*\brief Implementation from QQuickItem. Triggers a calculation of descriptors if the keypoints have been set.
*/
void QDescriptorExtractor::componentComplete(){
    QQuickItem::componentComplete();
    compute();
}

/**
 * \brief Computes the descriptors for a set of keypoints detected in an image or image set.
 */
void QDescriptorExtractor::compute(){
    if ( m_extractor && m_keypoints && isComponentComplete() ){
        m_extractor->compute(m_keypoints->cvMat(), m_keypoints->keypoints(), *m_descriptors->internalPtr());
        if ( (m_keypoints->cvMat().cols != 0 || m_keypoints->cvMat().rows != 0) && m_descriptors->internalPtr()->cols == 0){
            m_descriptors->internalPtr()->create(0, descriptorCols(), CV_8UC1);
        }
        emit descriptorsChanged();
    }
}

/**
 *\fn lv::QDescriptorExtractor::setParams()
 *\brief Setter for parameters
 */

/**
 *\fn lv::QDescriptorExtractor::keypointsChanged()
 *\brief Triggered when the keypoints are changed
 */

/**
 *\fn lv::QDescriptorExtractor::descriptorsChanged()
 *\brief Triggered when descriptors are changed
 */

/**
 *\fn lv::QDescriptorExtractor::paramsChanged()
 *\brief Triggered when params are changed
 */

 /**
 *\fn lv::QDescriptorExtractor::keypoints()
 *\brief Keypoints getter
 */

/**
 *\fn lv::QDescriptorExtractor::setKeypoints()
 *\brief Setter for the keypoints
 */

 /**
 *\fn lv::QDescriptorExtractor::descriptors()
 * Getter for descriptors
 */

/**
 *\fn lv::QDescriptorExtractor::params()
 *\brief Getter for the params.
 */

 /**
 *\fn lv::QDescriptorExtractor::isBinary()
 * Returns true if this descriptor extractor is binary
 */

 /**
 *\fn lv::QDescriptorExtractor::descriptorCols()
 * Returns the number of cols for the descriptor.
 */

 /**
 *\fn lv::QDescriptorExtractor::extractor()
 * Returns the internal extractor.
 */







