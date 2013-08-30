#ifndef QMATTRANSFORMATION_HPP
#define QMATTRANSFORMATION_HPP

#include "QMatDisplay.hpp"

class QMatTransformation : public QMatDisplay{

    Q_OBJECT
    Q_PROPERTY(QMat* input READ inputMat WRITE setInputMat NOTIFY inChanged)

public:
    explicit QMatTransformation(QQuickItem *parent = 0);
    virtual ~QMatTransformation();
    
    QMat* inputMat();
    void setInputMat(QMat* mat);

    void transform();
    virtual void transform(cv::Mat& in, cv::Mat& out);

signals:
    void inChanged();

private:
    QMat* m_in;
    
};

inline QMat *QMatTransformation::inputMat(){
    return m_in;
}

inline void QMatTransformation::setInputMat(QMat *mat){
    m_in = mat;
    emit inChanged();
    transform();
}

inline void QMatTransformation::transform(){
    transform(*inputMat()->data(), *output()->data());
    emit outChanged();
    update();
}

#endif // QMATTRANSFORMATION_HPP
