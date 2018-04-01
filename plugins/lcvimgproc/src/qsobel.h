/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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

#ifndef QSOBEL_H
#define QSOBEL_H

#include "qmatfilter.h"

//- Sobel operator - QSobel Class

class QSobel : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(QMat::Type ddepth     READ ddepth     WRITE setDdepth     NOTIFY ddepthChanged)
    Q_PROPERTY(int        xorder     READ xorder     WRITE setXOrder     NOTIFY xorderChanged)
    Q_PROPERTY(int        yorder     READ yorder     WRITE setYOrder     NOTIFY yorderChanged)
    Q_PROPERTY(int        ksize      READ ksize      WRITE setKsize      NOTIFY ksizeChanged)
    Q_PROPERTY(double     scale      READ scale      WRITE setScale      NOTIFY scaleChagned)
    Q_PROPERTY(double     delta      READ delta      WRITE setDelta      NOTIFY deltaChanged)
    Q_PROPERTY(int        borderType READ borderType WRITE setBorderType NOTIFY borderTypeChanged)

public:
    explicit QSobel(QQuickItem *parent = 0);
    virtual ~QSobel();

    virtual QSGNode *updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *nodeData);
    virtual void transform(const cv::Mat& in, cv::Mat& out);

    QMat::Type ddepth() const;
    int    xorder() const;
    int    yorder() const;
    int    ksize() const;
    double scale() const;
    double delta() const;
    int    borderType() const;

    void setDdepth(QMat::Type arg);
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
    QMat::Type m_ddepth;
    int        m_xorder;
    int        m_yorder;
    int        m_ksize;
    double     m_scale;
    double     m_delta;
    int        m_borderType;

    QMat*      m_display;

};

inline QMat::Type QSobel::ddepth() const{
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

inline void QSobel::setDdepth(QMat::Type arg){
    if (m_ddepth != arg) {
        m_ddepth = arg;
        emit ddepthChanged();
        QMatFilter::transform();
    }
}

inline void QSobel::setXOrder(int arg){
    if (m_xorder != arg) {
        m_xorder = arg;
        emit xorderChanged();
        QMatFilter::transform();
    }
}

inline void QSobel::setYOrder(int arg){
    if (m_yorder != arg) {
        m_yorder = arg;
        emit yorderChanged();
        QMatFilter::transform();
    }
}

inline void QSobel::setKsize(int arg){
    if (m_ksize != arg) {
        m_ksize = arg;
        emit ksizeChanged();
        QMatFilter::transform();
    }
}

inline void QSobel::setScale(double arg){
    if (m_scale != arg) {
        m_scale = arg;
        emit scaleChagned();
        QMatFilter::transform();
    }
}

inline void QSobel::setDelta(double arg){
    if (m_delta != arg) {
        m_delta = arg;
        emit deltaChanged();
        QMatFilter::transform();
    }
}

inline void QSobel::setBorderType(int arg){
    if (m_borderType != arg) {
        m_borderType = arg;
        emit borderTypeChanged();
        QMatFilter::transform();
    }
}

#endif // QSOBEL_H
