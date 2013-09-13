#ifndef QMAT_HPP
#define QMAT_HPP

#include <QQuickItem>
#include "opencv2/core/core.hpp"

class QMat : public QQuickItem{

    Q_OBJECT
    Q_ENUMS(Type)

public:
    enum Type{
        CV8U  = CV_8U,
        CV8S  = CV_8S,
        CV16U = CV_16U,
        CV16S = CV_16S,
        CV32S = CV_32S,
        CV32F = CV_32F,
        CV64F = CV_64F
    };

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
