#ifndef QMAT_HPP
#define QMAT_HPP

#include <QQuickItem>
#include "opencv2/core/core.hpp"

class QMat : public QQuickItem{

    Q_OBJECT

public:
    explicit QMat(QQuickItem *parent = 0);
    QMat(cv::Mat *mat, QQuickItem *parent = 0);
    ~QMat();

    cv::Mat* data();

public:
    cv::Mat* m_data;
    
};

inline cv::Mat *QMat::data(){
    return m_data;
}

#endif // QMAT_HPP
