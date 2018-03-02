#ifndef QOVERLAPMAT_H
#define QOVERLAPMAT_H

#include <QQuickItem>
#include "qmatfilter.h"

class QOverlapMat : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(QMat* input2 READ input2 WRITE setInput2 NOTIFY input2Changed)
    Q_PROPERTY(QMat* mask   READ mask   WRITE setMask   NOTIFY maskChanged)

public:
    explicit QOverlapMat(QQuickItem* parent = 0);
    virtual ~QOverlapMat(){}

    QMat* mask();
    void setMask(QMat* m);

    QMat* input2();
    void setInput2(QMat* m);

    void transform(const cv::Mat& in, cv::Mat& out);

signals:
    void maskChanged();
    void input2Changed();

private:
    QMat* m_input2;
    QMat* m_mask;
};

inline QMat* QOverlapMat::mask(){
    return m_mask;
}

inline void QOverlapMat::setMask(QMat *m){
    if ( m == 0 )
        return;

    m_mask = m;
    emit maskChanged();

    QMatFilter::transform();
}

inline QMat *QOverlapMat::input2(){
    return m_input2;
}

inline void QOverlapMat::setInput2(QMat *m){
    if ( m == 0 )
        return;

    m_input2 = m;
    emit input2Changed();

    QMatFilter::transform();
}

#endif // QOVERLAPMAT_H
