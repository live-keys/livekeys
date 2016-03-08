/****************************************************************************
**
** Copyright (C) 2014 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV application.
**
** GNU General Public License Usage
** 
** This file may be used under the terms of the GNU General Public License 
** version 3.0 as published by the Free Software Foundation and appearing 
** in the file LICENSE.GPL included in the packaging of this file.  Please 
** review the following information to ensure the GNU General Public License 
** version 3.0 requirements will be met: http://www.gnu.org/copyleft/gpl.html.
**
****************************************************************************/
#ifndef QGAUSSIANBLUR_HPP
#define QGAUSSIANBLUR_HPP

#include "QMatFilter.hpp"

class QGaussianBlur : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(QSize  ksize      READ ksize      WRITE setKsize      NOTIFY ksizeChanged)
    Q_PROPERTY(double sigmaX     READ sigmaX     WRITE setSigmaX     NOTIFY sigmaXChanged)
    Q_PROPERTY(double sigmaY     READ sigmaY     WRITE setSigmaY     NOTIFY sigmaYChanged)
    Q_PROPERTY(int    borderType READ borderType WRITE setBorderType NOTIFY borderTypeChanged)

public:
    explicit QGaussianBlur(QQuickItem *parent = 0);
    ~QGaussianBlur();

    virtual void transform(cv::Mat& in, cv::Mat& out);

    const QSize& ksize() const;
    double sigmaX() const;
    double sigmaY() const;
    int borderType() const;

    void setKsize(const QSize& arg);
    void setSigmaX(double arg);
    void setSigmaY(double arg);
    void setBorderType(int arg);

signals:
    void ksizeChanged();
    void sigmaXChanged();
    void sigmaYChanged();
    void borderTypeChanged();

private:
    QSize  m_ksize;
    double m_sigmaX;
    double m_sigmaY;
    int    m_borderType;
};

inline const QSize& QGaussianBlur::ksize() const{
    return m_ksize;
}

inline double QGaussianBlur::sigmaX() const{
    return m_sigmaX;
}

inline double QGaussianBlur::sigmaY() const{
    return m_sigmaY;
}

inline int QGaussianBlur::borderType() const{
    return m_borderType;
}

inline void QGaussianBlur::setKsize(const QSize& arg){
    if (m_ksize != arg) {
        m_ksize = arg;
        emit ksizeChanged();
        QMatFilter::transform();
    }
}

inline void QGaussianBlur::setSigmaX(double arg){
    if (m_sigmaX != arg) {
        m_sigmaX = arg;
        emit sigmaXChanged();
        QMatFilter::transform();
    }
}

inline void QGaussianBlur::setSigmaY(double arg){
    if (m_sigmaY != arg) {
        m_sigmaY = arg;
        emit sigmaYChanged();
        QMatFilter::transform();
    }
}

inline void QGaussianBlur::setBorderType(int arg){
    if (m_borderType != arg) {
        m_borderType = arg;
        emit borderTypeChanged();
        QMatFilter::transform();
    }
}

#endif // QGAUSSIANBLUR_HPP
