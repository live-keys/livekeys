#ifndef QFEATUREDETECTOR_HPP
#define QFEATUREDETECTOR_HPP

#include "QMatDisplay.hpp"


namespace cv{
class FeatureDetector;
}

class QKeyPointVector;
class QFeatureDetector : public QMatDisplay{

    Q_OBJECT
    Q_PROPERTY(QMat* input                READ inputMat  WRITE setInputMat  NOTIFY inputChanged)
    Q_PROPERTY(QMat* mask                 READ mask      WRITE setMask      NOTIFY maskChanged)
    Q_PROPERTY(QKeyPointVector* keypoints READ keypoints WRITE setKeypoints NOTIFY keypointsChanged)

public:
    explicit QFeatureDetector(QQuickItem *parent = 0);
    QFeatureDetector(cv::FeatureDetector* detector, QQuickItem* parent = 0);
    virtual ~QFeatureDetector();

public:
    QMat* inputMat();
    void setInputMat(QMat* mat);

    QMat* mask();
    void setMask(QMat* mat);

    QKeyPointVector* keypoints();
    void setKeypoints(QKeyPointVector* keypoints);

protected:
    cv::FeatureDetector* const detector();
    void initializeDetector(cv::FeatureDetector* detector);
    void detect();

public:
    virtual void componentComplete();
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *nodeData);

signals:
    void inputChanged();
    void maskChanged();
    void keypointsChanged();

private:
    cv::FeatureDetector* m_detector;
    QKeyPointVector*     m_keypoints;

    QMat* m_in;
    QMat* m_inInternal;
    QMat* m_mask;
    QMat* m_maskInternal;
};

inline QMat *QFeatureDetector::inputMat(){
    return m_in;
}

inline void QFeatureDetector::setInputMat(QMat *mat){
    cv::Mat* matData = mat->cvMat();
    if ( implicitWidth() != matData->cols || implicitHeight() != matData->rows ){
        setImplicitWidth(matData->cols);
        setImplicitHeight(matData->rows);
    }
    m_in = mat;
    emit inputChanged();
    detect();
}

inline QMat* QFeatureDetector::mask(){
    return m_mask;
}

inline void QFeatureDetector::setMask(QMat* mat){
    m_mask = mat;
    emit maskChanged();
    detect();
}

inline QKeyPointVector* QFeatureDetector::keypoints(){
    return m_keypoints;
}

inline void QFeatureDetector::setKeypoints(QKeyPointVector* keypoints){
    m_keypoints = keypoints;
    emit keypointsChanged();
}

#endif // QFEATUREDETECTOR_HPP
