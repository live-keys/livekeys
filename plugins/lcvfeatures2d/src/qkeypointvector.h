/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

#ifndef QKEYPOINTVECTOR_HPP
#define QKEYPOINTVECTOR_HPP

#include <QQuickItem>
#include "qlcvfeatures2dglobal.h"
#include "opencv2/features2d.hpp"

class QKeyPoint;
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
    QList<QObject*> keyPointData();
    void setKeyPointData(const QList<QObject*> data);
    void appendKeyPoint(QKeyPoint* pt);
    void removeKeyPoint(int position);
    QKeyPoint *createKeyPoint();
    int size();

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

inline int QKeyPointVector::size(){
    return static_cast<int>(m_keyPoints.size());
}

#endif // QKEYPOINTVECTOR_HPP
