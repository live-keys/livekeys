#ifndef QDESCRIPTOREXTRACTOR_HPP
#define QDESCRIPTOREXTRACTOR_HPP

#include <QQuickItem>
#include "QMat.hpp"
#include "QKeyPointVector.hpp"

namespace cv{
class DescriptorExtractor;
}

class QKeyPointVector;
class QDescriptorExtractor : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QKeyPointVector* keypoints READ keypoints   WRITE  setKeypoints NOTIFY keypointsChanged)
    Q_PROPERTY(QMat* descriptors          READ descriptors NOTIFY descriptorsChanged)

public:
    explicit QDescriptorExtractor(QQuickItem *parent = 0);
    QDescriptorExtractor(cv::DescriptorExtractor* extractor, QQuickItem* parent = 0);
    ~QDescriptorExtractor();

    QKeyPointVector* keypoints();
    void setKeypoints(QKeyPointVector* keypoints);

    QMat* descriptors();

protected:
    cv::DescriptorExtractor* extractor();
    void initializeExtractor(cv::DescriptorExtractor* extractor);
    virtual void componentComplete();

signals:
    void keypointsChanged();
    void descriptorsChanged();

public slots:
    void compute();

private:
    cv::DescriptorExtractor* m_extractor;
    QKeyPointVector*         m_keypoints;
    QMat*                    m_descriptors;
};

inline QKeyPointVector* QDescriptorExtractor::keypoints(){
    return m_keypoints;
}

inline void QDescriptorExtractor::setKeypoints(QKeyPointVector* keypoints){
    if ( keypoints->keypoints().size() > 0 ){
        m_keypoints      = keypoints;
        emit keypointsChanged();
        compute();
    }
}

inline QMat*QDescriptorExtractor::descriptors(){
    return m_descriptors;
}

#endif // QDESCRIPTOREXTRACTOR_HPP
