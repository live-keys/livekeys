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

#include "qdescriptormatcher.h"
#include "opencv2/features2d.hpp"

/**
 * \class QDescriptorMatcher
 * \brief Abstract base class for matching keypoint descriptors.
 * \ingroup plugin-lcvfeatures2d
 */

/**
 * \brief QDescriptorMatcher main constructor
 */
QDescriptorMatcher::QDescriptorMatcher(QQuickItem *parent)
    : QQuickItem(parent)
    , m_matcher(0)
    , m_matches(new QDMatchVector)
    , m_queryDescriptors(QMat::nullMat())
    , m_knn(-1)
{
}

/**
 * \brief Constructor containing the actual matcher
 */
QDescriptorMatcher::QDescriptorMatcher(cv::DescriptorMatcher* matcher, QQuickItem *parent)
    : QQuickItem(parent)
    , m_matcher(matcher)
    , m_matches(new QDMatchVector)
    , m_queryDescriptors(QMat::nullMat())
    , m_knn(-1)
{
}

/**
 * \brief QDescriptorMatcher destructor
 */
QDescriptorMatcher::~QDescriptorMatcher(){
    delete m_matcher;
}

/**
*\brief Returns a number of matches
*/
QDMatchVector* QDescriptorMatcher::matches(){
    return m_matches;
}
/**
 * \brief Adds a set of descriptors
 */
void QDescriptorMatcher::add(QMat* descriptors){
    if ( !descriptors )
        return;
    if ( descriptors->cvMat()->cols == 0 )
        return;
    if ( m_matcher ){
        std::vector<cv::Mat> descriptorVector;
        descriptorVector.push_back(*descriptors->cvMat());
        m_matcher->add(descriptorVector);
    } else {
        qWarning("Descriptor Matcher Add: No matcher defined. You need to initialize the matcher manually.");
    }
}

/**
 * \brief Trains the matcher
 */
void QDescriptorMatcher::train(){
    if ( m_matcher ){
        try{
            m_matcher->train();
            callMatch();
        } catch ( cv::Exception& e ){
            qCritical("%s", qPrintable( QString( QString("Descriptor matcher train: ") + e.what()) ) );
        }
    } else {
        qWarning("Descriptor Matcher Train: No matcher defined. You need to initialize the matcher manually.");
    }
}
/**
*\brief Initializes matcher
*/
void QDescriptorMatcher::callMatch(){
    if ( m_matcher && isComponentComplete() ){
        match(m_queryDescriptors, m_matches);
        emit matchesChanged();
    }
}

void QDescriptorMatcher::match(QMat* queryDescriptors, QDMatchVector* matches){
    if ( m_knn != -1 ){
        knnMatch(queryDescriptors, matches, m_knn);
    } else {
        if ( m_matcher ){
            try{
                if ( matches->matches().size() != 1)
                    matches->matches().resize(1);
                if ( queryDescriptors->cvMat()->cols == 0 )
                    return;
                m_matcher->match(*queryDescriptors->cvMat(), matches->matches()[0] );
                m_matches->setType(QDMatchVector::BEST_MATCH);
            } catch ( cv::Exception& e ){
                qCritical("Descriptor matcher: %s", e.what());
            }
        }
    }
}

/**
 * \brief Finds best matches for each descriptor 
 */
void QDescriptorMatcher::knnMatch(QMat *queryDescriptors, QDMatchVector *matches, int k){
    if ( m_matcher ){
        try{
            if ( queryDescriptors->cvMat()->cols == 0 )
                return;
            m_matcher->knnMatch(*queryDescriptors->cvMat(), matches->matches(), k);
            m_matches->setType(QDMatchVector::KNN);
        } catch ( cv::Exception& e ){
            qCritical("Descriptor matcher knn match: %s", qPrintable(e.what()));
        }
    }
}

/**
*\brief Override from QQuickItem.Calls the matcher.
*/
void QDescriptorMatcher::componentComplete(){
    QQuickItem::componentComplete();
    callMatch();
}
/**
*\brief Initializes any internal parameters
*/
void QDescriptorMatcher::initialize(const QVariantMap &){
}

/**
*\brief Function used for extending Descriptor matcher
*/
void QDescriptorMatcher::initializeMatcher(cv::DescriptorMatcher* matcher){
    delete m_matcher;
    m_matcher = matcher;
    callMatch();
}

/**
*\fn lv::QDescriptorMatcher::match()
*/

/**
*\fn lv::QDescriptorMatcher::params()
*\brief Given parameters
*/

/**
*\fn lv::QDescriptorMatcher::setParams()
*\brief Setter for the params
*/

/**
*\fn lv::QDescriptorMatcher::paramsChanged()
*\brief Triggered when params are changed
*/

/**
*\fn lv::QDescriptorMatcher::queryDescriptors()
*\brief Inputs
*/

/**
*\fn lv::QDescriptorMatcher::queryDescriptorsChanged()
*\brief Triggered when queryDescriptors are changed
*/

/**
*\fn lv::QDescriptorMatcher::matchesChanged()
*\brief Triggered when matches are changed
*/

/**
*\fn lv::QDescriptorMatcher::setKnn()
*\brief Setter for the knn
*/

/**
*\fn lv::QDescriptorMatcher::knn()
*\brief Count of best matches found per each query descriptor or less if a query descriptor has less than k possible matches in total.
*/


/**
*\fn lv::QDescriptorMatcher::knnChanged()
*\brief Triggered when knn is changed
*/

/**
*\fn lv::QDescriptorMatcher::setQueryDescriptors()
*\brief Triggered when queryDescriptors are changed
*/
