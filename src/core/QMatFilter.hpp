#ifndef QMATTRANSFORMATION_HPP
#define QMATTRANSFORMATION_HPP

#include "QMatDisplay.hpp"

class QMatFilter : public QMatDisplay{

    Q_OBJECT
    Q_PROPERTY(QMat* input READ inputMat WRITE setInputMat NOTIFY inChanged)

public:
    explicit QMatFilter(QQuickItem *parent = 0);
    virtual ~QMatFilter();
    
    QMat* inputMat();
    void setInputMat(QMat* mat);

    void transform();
    virtual void transform(cv::Mat& in, cv::Mat& out);

signals:
    void inChanged();

private:
    QMat* m_in;
    
};

inline QMat *QMatFilter::inputMat(){
    return m_in;
}

inline void QMatFilter::setInputMat(QMat *mat){
    m_in = mat;
    emit inChanged();
    transform();
}

inline void QMatFilter::transform(){
    transform(*inputMat()->data(), *output()->data());
    emit outChanged();
    update();
}

#endif // QMATTRANSFORMATION_HPP
