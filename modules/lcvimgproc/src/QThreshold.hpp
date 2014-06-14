#ifndef QTHRESHOLD_HPP
#define QTHRESHOLD_HPP

#include "QMatFilter.hpp"

class QThreshold : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(double           thresh        READ thresh        WRITE setThresh        NOTIFY threshChanged)
    Q_PROPERTY(double           maxVal        READ maxVal        WRITE setMaxVal        NOTIFY maxValChanged)
    Q_PROPERTY(QThreshold::Type thresholdType READ thresholdType WRITE setThresholdType NOTIFY thresholdTypeChanged)
    Q_ENUMS(Type)

public:
    enum Type{
        BINARY      = 0,
        BINARY_INV  = 1,
        TRUNC       = 2,
        TOZERO      = 3,
        TOZERO_INV  = 4,
        MASK        = 7,
        OTSU        = 8
    };

public:
    explicit QThreshold(QQuickItem *parent = 0);
    virtual ~QThreshold();

    double thresh() const;
    void setThresh(double thresh);

    double maxVal() const;
    void setMaxVal(double maxVal);

    QThreshold::Type thresholdType() const;
    void setThresholdType(QThreshold::Type type);

    virtual void transform(cv::Mat &in, cv::Mat &out);

signals:
    void threshChanged();
    void maxValChanged();
    void thresholdTypeChanged();

private:
    double           m_thresh;
    double           m_maxVal;
    QThreshold::Type m_type;

};

inline double QThreshold::thresh() const{
    return m_thresh;
}

inline void QThreshold::setThresh(double thresh){
    if ( thresh != m_thresh ){
        m_thresh = thresh;
        emit threshChanged();
        QMatFilter::transform();
    }
}

inline double QThreshold::maxVal() const{
    return m_maxVal;
}

inline void QThreshold::setMaxVal(double maxVal){
    if ( m_maxVal != maxVal ){
        m_maxVal = maxVal;
        emit maxValChanged();
        QMatFilter::transform();
    }
}

inline QThreshold::Type QThreshold::thresholdType() const{
    return m_type;
}

inline void QThreshold::setThresholdType(QThreshold::Type type){
    if ( m_type != type ){
        m_type = type;
        emit thresholdTypeChanged();
        QMatFilter::transform();
    }
}

#endif // QTHRESHOLD_HPP
