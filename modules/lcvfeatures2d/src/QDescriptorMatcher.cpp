#include "QDescriptorMatcher.hpp"
#include "opencv2/features2d/features2d.hpp"

#include <QDebug>

QDescriptorMatcher::QDescriptorMatcher(QQuickItem *parent)
    : QQuickItem(parent)
    , m_matcher(0)
    , m_matches(new QDMatchVector)
{
}

QDescriptorMatcher::QDescriptorMatcher(cv::DescriptorMatcher* matcher, QQuickItem *parent)
    : QQuickItem(parent)
    , m_matcher(matcher)
    , m_matches(new QDMatchVector)
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
            match(m_queryDescriptors, m_matches);
        } catch ( cv::Exception& e ){
            qCritical(qPrintable( QString( QString("Descriptor matcher train: ") + e.what()) ) );
        }
    } else {
        qWarning("Descriptor Matcher Train: No matcher defined. You need to initialize the matcher manually.");
    }
}

void QDescriptorMatcher::match(QMat* queryDescriptors, QDMatchVector* matches){
    if ( m_matcher ){
        try{
            m_matcher->match(*queryDescriptors->cvMat(), matches->matches() );
        } catch ( cv::Exception& e ){
            qCritical(qPrintable( QString( QString("Descriptor matcher match: ") + e.what()) ) );
        }
    }
}

void QDescriptorMatcher::componentComplete(){
    QQuickItem::componentComplete();
    match(m_queryDescriptors, m_matches);
    emit matchesChanged();
}

void QDescriptorMatcher::initializeMatcher(cv::DescriptorMatcher* matcher){
    delete m_matcher;
    m_matcher = matcher;
    match(m_queryDescriptors, m_matches);
}
