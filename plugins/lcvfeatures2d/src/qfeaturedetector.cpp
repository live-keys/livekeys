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

#include "qfeaturedetector.h"
#include "qkeypointvector.h"
#include "qmatnode.h"
#include "qmatshader.h"
#include "opencv2/features2d.hpp"

/**
 * \class QFeatureDetector
 * \brief Detects features in an image
 * \ingroup plugin-lcvfeatures2d
 */

 /**
  *\brief QFeatureDetector constructor
  */
QFeatureDetector::QFeatureDetector(QQuickItem *parent)
    : QQuickItem(parent)
    , m_detector(cv::Ptr<cv::FeatureDetector>())
    , m_keypoints(new QKeyPointVector)
    , m_in(QMat::nullMat())
    , m_output(new QMat)
    , m_mask(QMat::nullMat())
    , m_outputDirty(false)
{
    setFlag(ItemHasContents, true);
}

/**
 * \fn lv::QFeatureDetector::inputMat()
 * \brief The actual image
*/

/**
 * \fn lv::QFeatureDetector::setInputMat()
 * \brief Setter for the image
*/

/**
 * \fn lv::QFeatureDetector::output()
 * \brief Drawn features from the input to another image
*/

/**
 * \fn lv::QFeatureDetector::mask()
 * \brief Limited amount of detections for a specific area
*/

/**
 * \fn lv::QFeatureDetector::setMask()
 * \brief Setter for mask
*/

/**
 * \fn lv::QFeatureDetector::maskChanged()
 * \brief Triggered when mask is changed
*/
    
/**
 * \fn lv::QFeatureDetector::keypoints()
 * \brief Actual keypoints
*/

/**
 * \fn lv::QFeatureDetector::setKeypoints()
 * \brief Setter for keypoints
*/

/**
 * \fn lv::QFeatureDetector::keypointsChanged()
 * \brief Triggered when the keypoints are changed
*/

/**
 * \fn lv::QFeatureDetector::params()
 * \brief Given parameters
*/

/**
 * \fn lv::QFeatureDetector::setParams()
 * \brief Setter for given parameters
*/

/**
 * \fn lv::QFeatureDetector::paramsChanged()
 * \brief Triggered when params are changed
*/

/**
 * \fn lv::QFeatureDetector::outputChanged()
 * \brief Triggered when output is changed
*/

/**
 * \fn lv::QFeatureDetector::inputChanged()
 * \brief Triggered when input is changed
*/

/**
 * \fn lv::QFeatureDetector::initialize()
 * \brief todo
*/

 /**
  *\brief QFeatureDetector constructor
  */
QFeatureDetector::QFeatureDetector(cv::Ptr<cv::FeatureDetector> detector, QQuickItem* parent)
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

/**
 *\brief QFeatureDetector destructor
 */
QFeatureDetector::~QFeatureDetector(){
    delete m_output;
}

/**
 *\brief todo
 */
cv::FeatureDetector *QFeatureDetector::detector(){
    return m_detector;
}
/**
 *\brief todo
 */
void QFeatureDetector::initializeDetector(cv::Ptr<cv::FeatureDetector> detector){
    m_detector = detector;
    detect();
}
/**
 *\brief todo
 */
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
/**
 *\brief todo
 */
void QFeatureDetector::componentComplete(){
    QQuickItem::componentComplete();
    detect();
}

/**
 *\brief todo
 */
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
