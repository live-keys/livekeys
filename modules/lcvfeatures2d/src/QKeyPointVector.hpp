#ifndef QKEYPOINTVECTOR_HPP
#define QKEYPOINTVECTOR_HPP

#include <QQuickItem>
#include "QLCVFeatures2DGlobal.hpp"

#include "opencv2/features2d/features2d.hpp"

class Q_LCVFEATURES2D_EXPORT QKeyPointVector : public QQuickItem{

    Q_OBJECT

public:
    explicit QKeyPointVector(QQuickItem *parent = 0);
    ~QKeyPointVector();

    std::vector<cv::KeyPoint>& keypoints();
    const std::vector<cv::KeyPoint>& keypoints() const;

    void setMat(cv::Mat& mat);
    const cv::Mat& cvMat();

public slots:
    QKeyPointVector* createOwnedObject();

private:
    std::vector<cv::KeyPoint> m_keyPoints;
    cv::Mat                   m_mat;
};

inline const std::vector<cv::KeyPoint>&QKeyPointVector::keypoints() const{
    return m_keyPoints;
}

inline std::vector<cv::KeyPoint>& QKeyPointVector::keypoints(){
    return m_keyPoints;
}

inline const cv::Mat &QKeyPointVector::cvMat(){
    return m_mat;
}

inline void QKeyPointVector::setMat(cv::Mat &mat){
    m_mat = mat;
}

#endif // QKEYPOINTVECTOR_HPP
