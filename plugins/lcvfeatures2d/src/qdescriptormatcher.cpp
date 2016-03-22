#include "qdescriptormatcher.h"
#include "opencv2/features2d/features2d.hpp"

QDescriptorMatcher::QDescriptorMatcher(QQuickItem *parent)
    : QQuickItem(parent)
    , m_matcher(0)
    , m_matches(new QDMatchVector)
    , m_queryDescriptors(QMat::nullMat())
    , m_knn(-1)
{
}

QDescriptorMatcher::QDescriptorMatcher(cv::DescriptorMatcher* matcher, QQuickItem *parent)
    : QQuickItem(parent)
    , m_matcher(matcher)
    , m_matches(new QDMatchVector)
    , m_queryDescriptors(QMat::nullMat())
    , m_knn(-1)
{
}

QDescriptorMatcher::~QDescriptorMatcher(){
    delete m_matcher;
}

QDMatchVector* QDescriptorMatcher::matches(){
    return m_matches;
}

void QDescriptorMatcher::add(QMat* descriptors){
    if ( m_matcher ){
        std::vector<cv::Mat> descriptorVector;
        descriptorVector.push_back(*descriptors->cvMat());
        m_matcher->add(descriptorVector);
    } else {
        qWarning("Descriptor Matcher Add: No matcher defined. You need to initialize the matcher manually.");
    }
}

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
                m_matcher->match(*queryDescriptors->cvMat(), matches->matches()[0] );
                m_matches->setType(QDMatchVector::BEST_MATCH);
            } catch ( cv::Exception& e ){
                qCritical("%s", qPrintable( QString( QString("Descriptor matcher match: ") + e.what()) ) );
            }
        }
    }
}

void QDescriptorMatcher::knnMatch(QMat *queryDescriptors, QDMatchVector *matches, int k){
    if ( m_matcher ){
        try{
            m_matcher->knnMatch(*queryDescriptors->cvMat(), matches->matches(), k);
            m_matches->setType(QDMatchVector::KNN);
        } catch ( cv::Exception& e ){
            qCritical("%s", qPrintable( QString( QString("Descriptor matcher match: ") + e.what()) ) );
        }
    }
}

void QDescriptorMatcher::componentComplete(){
    QQuickItem::componentComplete();
    callMatch();
}

void QDescriptorMatcher::initializeMatcher(cv::DescriptorMatcher* matcher){
    delete m_matcher;
    m_matcher = matcher;
    callMatch();
}
