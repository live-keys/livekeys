#include "qwritablemat.h"
#include "qmat.h"
#include "qmatop.h"
#include <QQmlEngine>

QWritableMat::QWritableMat(QObject *parent)
    : QObject(parent)
    , m_internal(new cv::Mat)
{
}

QWritableMat::QWritableMat(cv::Mat *mat, QObject *parent)
    : QObject(parent)
    , m_internal(mat)
{
}

QWritableMat::~QWritableMat(){
    delete m_internal;
}

const cv::Mat &QWritableMat::internal() const{
    return *m_internal;
}

cv::Mat &QWritableMat::internal(){
    return *m_internal;
}

QByteArray QWritableMat::buffer(){
    return QByteArray::fromRawData(
        reinterpret_cast<const char*>(m_internal->data),
        static_cast<int>(m_internal->total() * m_internal->elemSize())
                );
}

int QWritableMat::channels() const{
    return m_internal->channels();
}

int QWritableMat::depth() const{
    return m_internal->depth();
}

QSize QWritableMat::dimensions() const{
    return QSize(m_internal->cols, m_internal->rows);
}

QMat *QWritableMat::toMat() const{
    cv::Mat* m = new cv::Mat;
    m_internal->copyTo(*m);
    QMat* mwrap = new QMat(m);
    return mwrap;
}

void QWritableMat::fill(const QColor &color, QMat *mask){
    if ( mask ){
        m_internal->setTo(QMatOp::toScalar(color), mask->internal());
    } else {
        m_internal->setTo(QMatOp::toScalar(color));
    }
}
