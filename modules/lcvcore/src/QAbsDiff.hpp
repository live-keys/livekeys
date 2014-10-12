#ifndef QABSDIFF_HPP
#define QABSDIFF_HPP

#include "QMatFilter.hpp"

class QAbsDiff : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(QMat* input2 READ input2 WRITE setInput2 NOTIFY input2Changed)

public:
    explicit QAbsDiff(QQuickItem *parent = 0);
    virtual ~QAbsDiff();

    QMat* input2();
    void setInput2(QMat* m);

    void transform(cv::Mat& in, cv::Mat& out);

signals:
    void input2Changed();

private:
    QMat* m_in2;
    QMat* m_in2Internal;

};

inline QMat *QAbsDiff::input2(){
    return m_in2;
}

inline void QAbsDiff::setInput2(QMat *m){
    m_in2 = m;
    emit input2Changed();
    if ( m_in2->cvMat()->size() == inputMat()->cvMat()->size() )
        QMatFilter::transform();
}

#endif // QABSDIFF_HPP
