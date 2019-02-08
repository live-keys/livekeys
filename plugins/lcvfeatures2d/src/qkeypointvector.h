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

#ifndef QKEYPOINTVECTOR_H
#define QKEYPOINTVECTOR_H

#include <QQuickItem>
#include "qlcvfeatures2dglobal.h"
#include "opencv2/features2d.hpp"

class QKeyPoint;
class Q_LCVFEATURES2D_EXPORT QKeyPointVector : public QQuickItem{

    Q_OBJECT

public:
    explicit QKeyPointVector(QQuickItem *parent = 0);
    ~QKeyPointVector();

    /**
    *\brief 
    */
    std::vector<cv::KeyPoint>& keypoints();
    const std::vector<cv::KeyPoint>& keypoints() const;

    /**
    *\brief Matrix where keypoint are detected from.
    */
    void setMat(cv::Mat& mat);
    const cv::Mat& cvMat();

public slots:
    /**
    *\brief Creates copy of the vector.
    */
    QKeyPointVector* createOwnedObject();
    /**
    *\Getting the KeyPointData as the list of objects.
    */
    QList<QObject*> keyPointData();
    /**
    *\brief add this
    */
    void setKeyPointData(const QList<QObject*> data);
    /**
    *\brief Appends a keypoint.
    */
    void appendKeyPoint(QKeyPoint* pt);
    /**
    *\brief Removes a keypoint.
    */
    void removeKeyPoint(int position);
    /**
    *\brief Creates a keypoint
    */
    QKeyPoint *createKeyPoint();
    /**
    *\brief Size of the vector.
    */
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

#endif // QKEYPOINTVECTOR_H
