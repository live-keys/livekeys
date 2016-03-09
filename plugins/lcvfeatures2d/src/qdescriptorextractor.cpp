#include "qdescriptorextractor.h"
#include "qkeypointvector.h"

QDescriptorExtractor::QDescriptorExtractor(QQuickItem *parent)
    : QQuickItem(parent)
    , m_extractor(0)
    , m_keypoints(0)
    , m_descriptors(new QMat)
{
}

QDescriptorExtractor::QDescriptorExtractor(cv::DescriptorExtractor* extractor, QQuickItem* parent)
    : QQuickItem(parent)
    , m_extractor(extractor)
    , m_keypoints(0)
    , m_descriptors(new QMat)
{
}

QDescriptorExtractor::~QDescriptorExtractor(){
    delete m_descriptors;
}

void QDescriptorExtractor::initializeExtractor(cv::DescriptorExtractor* extractor){
    delete m_extractor;
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
        emit descriptorsChanged();
    }
}
