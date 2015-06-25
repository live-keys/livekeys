#include "QFeatureDetector.hpp"
#include "QKeyPointVector.hpp"
#include "opencv2/features2d/features2d.hpp"

#include "opencv2/highgui/highgui.hpp"

QFeatureDetector::QFeatureDetector(QQuickItem *parent)
    : QMatDisplay(parent)
    , m_detector(0)
    , m_keypoints(new QKeyPointVector)
    , m_in(QMat::nullMat())
    , m_mask(QMat::nullMat())
{
}

QFeatureDetector::QFeatureDetector(cv::FeatureDetector* detector, QQuickItem* parent)
    : QMatDisplay(parent)
    , m_detector(detector)
    , m_keypoints(new QKeyPointVector)
    , m_in(QMat::nullMat())
    , m_mask(QMat::nullMat())
{
}

QFeatureDetector::~QFeatureDetector(){
    delete m_detector;
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
    if ( m_detector != 0 && isComponentComplete() ){
        m_detector->detect(*m_in->cvMat(), m_keypoints->keypoints(), *m_mask->cvMat());
        cv::Mat inClone = m_in->cvMat()->clone();
        m_keypoints->setMat(inClone);
        emit keypointsChanged();
        update();
    }
}

void QFeatureDetector::componentComplete(){
    QQuickItem::componentComplete();
    detect();
}

QSGNode* QFeatureDetector::updatePaintNode(QSGNode* node, QQuickItem::UpdatePaintNodeData* nodeData){
    if ( m_keypoints->keypoints().size() > 0 && !inputMat()->cvMat()->empty() ){

        cv::drawKeypoints(
                    *m_in->cvMat(),
                     m_keypoints->keypoints(),
                    *output()->cvMat(),
                     cv::Scalar::all(-1),
                     cv::DrawMatchesFlags::DEFAULT);

        emit outputChanged();
    }
    return QMatDisplay::updatePaintNode(node, nodeData);
}
