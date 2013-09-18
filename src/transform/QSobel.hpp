#ifndef QSOBEL_HPP
#define QSOBEL_HPP

#include "QMatFilter.hpp"

class QSobel : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(int    ddepth     READ ddepth     WRITE setDdepth     NOTIFY ddepthChanged)
    Q_PROPERTY(int    xorder     READ xorder     WRITE setXOrder     NOTIFY xorderChanged)
    Q_PROPERTY(int    yorder     READ yorder     WRITE setYOrder     NOTIFY yorderChanged)
    Q_PROPERTY(int    ksize      READ ksize      WRITE setKsize      NOTIFY ksizeChanged)
    Q_PROPERTY(double scale      READ scale      WRITE setScale      NOTIFY scaleChagned)
    Q_PROPERTY(double delta      READ delta      WRITE setDelta      NOTIFY deltaChanged)
    Q_PROPERTY(int    borderType READ borderType WRITE setBorderType NOTIFY borderTypeChanged)

public:
    explicit QSobel(QQuickItem *parent = 0);
    ~QSobel();

    virtual void transform(cv::Mat& in, cv::Mat& out);

    int ddepth() const;
    int xorder() const;
    int yorder() const;
    int ksize() const;
    double scale() const;
    double delta() const;
    int borderType() const;

    void setDdepth(int arg);
    void setXOrder(int arg);
    void setYOrder(int arg);
    void setKsize(int arg);
    void setScale(double arg);
    void setDelta(double arg);
    void setBorderType(int arg);

signals:
    void ddepthChanged();
    void xorderChanged();
    void yorderChanged();
    void ksizeChanged();
    void scaleChagned();
    void deltaChanged();
    void borderTypeChanged();

private:
    int    m_ddepth;
    int    m_xorder;
    int    m_yorder;
    int    m_ksize;
    double m_scale;
    double m_delta;
    int    m_borderType;

};

inline int QSobel::ddepth() const{
    return m_ddepth;
}

inline int QSobel::xorder() const{
    return m_xorder;
}

inline int QSobel::yorder() const{
    return m_yorder;
}

inline int QSobel::ksize() const{
    return m_ksize;
}

inline double QSobel::scale() const{
    return m_scale;
}

inline double QSobel::delta() const{
    return m_delta;
}

inline int QSobel::borderType() const{
    return m_borderType;
}

inline void QSobel::setDdepth(int arg){
    if (m_ddepth != arg) {
        m_ddepth = arg;
        emit ddepthChanged();
    }
}

inline void QSobel::setXOrder(int arg){
    if (m_xorder != arg) {
        m_xorder = arg;
        emit xorderChanged();
    }
}

inline void QSobel::setYOrder(int arg){
    if (m_yorder != arg) {
        m_yorder = arg;
        emit yorderChanged();
    }
}

inline void QSobel::setKsize(int arg){
    if (m_ksize != arg) {
        m_ksize = arg;
        emit ksizeChanged();
    }
}

inline void QSobel::setScale(double arg){
    if (m_scale != arg) {
        m_scale = arg;
        emit scaleChagned();
    }
}

inline void QSobel::setDelta(double arg){
    if (m_delta != arg) {
        m_delta = arg;
        emit deltaChanged();
    }
}

inline void QSobel::setBorderType(int arg){
    if (m_borderType != arg) {
        m_borderType = arg;
        emit borderTypeChanged();
    }
}

#endif // QSOBEL_HPP
