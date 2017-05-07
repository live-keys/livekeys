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
#ifndef QERODE_H
#define QERODE_H

#include "qmatfilter.h"

class QErode : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(QMat*  kernel      READ kernel      WRITE setKernel      NOTIFY kernelChanged)
    Q_PROPERTY(QPoint anchor      READ anchor      WRITE setAnchor      NOTIFY anchorChanged)
    Q_PROPERTY(int    iterations  READ iterations  WRITE setIterations  NOTIFY iterationsChanged)
    Q_PROPERTY(int    borderType  READ borderType  WRITE setBorderType  NOTIFY borderTypeChanged)
    Q_PROPERTY(QColor borderValue READ borderValue WRITE setBorderValue NOTIFY borderValueChanged)

public:
    QErode(QQuickItem* parent = 0);
    ~QErode();

signals:
    void kernelChanged();
    void anchorChanged();
    void iterationsChanged();
    void borderTypeChanged();
    void borderValueChanged();

public:
    QMat* kernel() const;
    void setKernel(QMat* kernel);

    const QPoint& anchor() const;
    void setAnchor(const QPoint& anchor);

    int iterations() const;
    void setIterations(int iterations);

    int borderType() const;
    void setBorderType(int borderType);

    const QColor& borderValue() const;
    void setBorderValue(const QColor& borderValue);

    void transform(cv::Mat& in, cv::Mat& out);

private:
    QMat*      m_kernel;
    QPoint     m_anchor;
    cv::Point  m_anchorCv;
    int        m_iterations;
    int        m_borderType;
    QColor     m_borderValue;
};

inline const QColor& QErode::borderValue() const{
    return m_borderValue;
}

inline void QErode::setBorderValue(const QColor& borderValue){
    if (m_borderValue != borderValue){
        m_borderValue = borderValue;
        emit borderValueChanged();
    }
}

inline int QErode::borderType() const{
    return m_borderType;
}

inline void QErode::setBorderType(int borderType){
    if (m_borderType != borderType){
        m_borderType = borderType;
        emit borderTypeChanged();
    }
}

inline int QErode::iterations() const{
    return m_iterations;
}

inline void QErode::setIterations(int iterations){
    if (m_iterations != iterations){
        m_iterations = iterations;
        emit iterationsChanged();
    }
}

inline const QPoint& QErode::anchor() const{
    return m_anchor;
}

inline void QErode::setAnchor(const QPoint& anchor){
    if (m_anchor != anchor){
        m_anchor   = anchor;
        m_anchorCv = cv::Point(anchor.x(), anchor.y());
        emit anchorChanged();
    }
}

inline QMat* QErode::kernel() const{
    return m_kernel;
}

inline void QErode::setKernel(QMat*kernel){
    if (m_kernel != kernel){
        m_kernel = kernel;
        emit kernelChanged();
    }
}

#endif // QERODE_H
