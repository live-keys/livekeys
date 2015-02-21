#ifndef QKEYPOINTVECTOR_HPP
#define QKEYPOINTVECTOR_HPP

#include <QQuickItem>
#include "opencv2/features2d/features2d.hpp"

class QKeyPointVector : public QQuickItem{

    Q_OBJECT

public:
    explicit QKeyPointVector(QQuickItem *parent = 0);
    ~QKeyPointVector();

    std::vector<cv::KeyPoint>& keypoints();
    const std::vector<cv::KeyPoint>& keypoints() const;

public slots:
    QKeyPointVector* createOwnedObject();

private:
    std::vector<cv::KeyPoint> m_keyPoints;
};

inline const std::vector<cv::KeyPoint>&QKeyPointVector::keypoints() const{
    return m_keyPoints;
}

inline std::vector<cv::KeyPoint>& QKeyPointVector::keypoints(){
    return m_keyPoints;
}

#endif // QKEYPOINTVECTOR_HPP
