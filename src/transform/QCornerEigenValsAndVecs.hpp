#ifndef QCORNEREIGENVALSANDVECS_HPP
#define QCORNEREIGENVALSANDVECS_HPP

#include <QQuickItem>
#include "QMatTransformation.hpp"

class QCornerEigenValsAndVecs : public QMatTransformation{

    Q_OBJECT
    Q_PROPERTY(int blockSize  READ blockSize  WRITE setBlockSize  NOTIFY blockSizeChanged)
    Q_PROPERTY(int ksize      READ ksize      WRITE setKsize      NOTIFY ksizeChanged)
    Q_PROPERTY(int borderType READ borderType WRITE setBorderType NOTIFY borderTypeChanged)

public:
    explicit QCornerEigenValsAndVecs(QQuickItem *parent = 0);

    int blockSize() const;
    void setBlockSize(int blockSize);

    int ksize() const;
    void setKsize(int ksize);

    int borderType() const;
    void setBorderType(int borderType);

    virtual void transform(cv::Mat &in, cv::Mat &out);

signals:
    void blockSizeChanged();
    void ksizeChanged();
    void borderTypeChanged();

private:
    int m_blockSize;
    int m_ksize;
    int m_borderType;

};

inline int QCornerEigenValsAndVecs::blockSize() const{
    return m_blockSize;
}

inline void QCornerEigenValsAndVecs::setBlockSize(int blockSize){
    m_blockSize = blockSize;
}

inline int QCornerEigenValsAndVecs::ksize() const{
    return m_ksize;
}

inline void QCornerEigenValsAndVecs::setKsize(int ksize){
    m_ksize = ksize;
}

inline int QCornerEigenValsAndVecs::borderType() const{
    return m_borderType;
}

inline void QCornerEigenValsAndVecs::setBorderType(int borderType){
    m_borderType = borderType;
}

#endif // QCORNEREIGENVALSANDVECS_HPP
