#ifndef QHOUGHLINES_HPP
#define QHOUGHLINES_HPP

#include "QMatFilter.hpp"

class QHoughLinesPrivate;
class QHoughLines : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(double rho           READ rho           WRITE setRho           NOTIFY rhoChanged)
    Q_PROPERTY(double theta         READ theta         WRITE setTheta         NOTIFY thetaChanged)
    Q_PROPERTY(int    threshold     READ threshold     WRITE setThreshold     NOTIFY thresholdChanged)
    Q_PROPERTY(double sm            READ sm            WRITE setSm            NOTIFY smChanged)
    Q_PROPERTY(double stn           READ stn           WRITE setStn           NOTIFY stnChanged)

public:
    explicit QHoughLines(QQuickItem *parent = 0);
    ~QHoughLines();

    virtual void     transform(cv::Mat& in, cv::Mat&);
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *nodeData);

    double rho() const;
    double theta() const;
    int    threshold() const;
    double sm() const;
    double stn() const;

    void setRho(double arg);
    void setTheta(double arg);
    void setThreshold(int arg);
    void setSm(double sm);
    void setStn(double stn);

signals:
    void rhoChanged();
    void thetaChanged();
    void thresholdChanged();
    void smChanged();
    void stnChanged();

private:
    QHoughLines(const QHoughLines& other);
    QHoughLines& operator= (const QHoughLines& other);

    double m_rho;
    double m_theta;
    int    m_threshold;
    double m_sm;
    double m_stn;

    QHoughLinesPrivate* const d_ptr;

    Q_DECLARE_PRIVATE(QHoughLines)

};

inline double QHoughLines::rho() const{
    return m_rho;
}

inline double QHoughLines::theta() const{
    return m_theta;
}

inline int QHoughLines::threshold() const{
    return m_threshold;
}

inline double QHoughLines::sm() const{
    return m_sm;
}

inline double QHoughLines::stn() const{
    return m_stn;
}

inline void QHoughLines::setRho(double arg){
    if (m_rho != arg) {
        m_rho = arg;
        emit rhoChanged();
        QMatFilter::transform();
    }
}

inline void QHoughLines::setTheta(double arg){
    if (m_theta != arg) {
        m_theta = arg;
        emit thetaChanged();
    }
}

inline void QHoughLines::setThreshold(int arg){
    if (m_threshold != arg) {
        m_threshold = arg;
        emit thresholdChanged();
        QMatFilter::transform();
    }
}

inline void QHoughLines::setSm(double sm){
    if ( m_sm != sm ){
        m_sm = sm;
        emit smChanged();
        QMatFilter::transform();
    }
}

inline void QHoughLines::setStn(double stn){
    if ( m_stn != stn ){
        m_stn = stn;
        emit stnChanged();
        QMatFilter::transform();
    }
}

#endif // QHOUGHLINES_HPP
