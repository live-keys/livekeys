/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#ifndef QCANNY_H
#define QCANNY_H

#include "qmatfilter.h"

class QCanny : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(double threshold1   READ threshold1   WRITE setThreshold1   NOTIFY threshold1Changed)
    Q_PROPERTY(double threshold2   READ threshold2   WRITE setThreshold2   NOTIFY threshold2Changed)
    Q_PROPERTY(int    apertureSize READ apertureSize WRITE setApertureSize NOTIFY apertureSizeChanged)
    Q_PROPERTY(bool   l2gradient   READ l2gradient   WRITE setL2gradient   NOTIFY l2gradientChanged)

public:
    explicit QCanny(QQuickItem *parent = 0);
    ~QCanny();

    virtual void transform(cv::Mat &in, cv::Mat &out);

    double threshold1()  const;
    double threshold2()  const;
    int    apertureSize()const;
    bool   l2gradient()  const;

    void setThreshold1( double threshold );
    void setThreshold2( double threshold );
    void setApertureSize( int aperture );
    void setL2gradient( bool gradient );

signals:
    void threshold1Changed();
    void threshold2Changed();
    void apertureSizeChanged();
    void l2gradientChanged();

private:
    double m_threshold1;
    double m_threshold2;
    int    m_apertureSize;
    bool   m_l2gradient;
    
};

inline double QCanny::threshold1() const{
    return m_threshold1;
}

inline double QCanny::threshold2() const{
    return m_threshold2;
}

inline int QCanny::apertureSize() const{
    return m_apertureSize;
}

inline bool QCanny::l2gradient() const{
    return m_l2gradient;
}

inline void QCanny::setThreshold1(double threshold){
    if ( m_threshold1 != threshold ){
        m_threshold1 = threshold;
        emit threshold1Changed();
        QMatFilter::transform();
    }
}

inline void QCanny::setThreshold2(double threshold){
    if ( m_threshold2 != threshold ){
        m_threshold2 = threshold;
        emit threshold2Changed();
        QMatFilter::transform();
    }
}

inline void QCanny::setApertureSize(int aperture){
    if ( m_apertureSize != aperture ){
        m_apertureSize = aperture;
        emit apertureSizeChanged();
        QMatFilter::transform();
    }
}

inline void QCanny::setL2gradient(bool gradient){
    if( m_l2gradient != gradient){
        m_l2gradient = gradient;
        emit l2gradientChanged();
        QMatFilter::transform();
    }
}

#endif // QCANNY_H
