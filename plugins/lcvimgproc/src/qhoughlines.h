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

#ifndef QHOUGHLINES_H
#define QHOUGHLINES_H

#include "qmatfilter.h"

class QHoughLinesPrivate;
class QHoughLines : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(double rho           READ rho           WRITE setRho           NOTIFY rhoChanged)
    Q_PROPERTY(double theta         READ theta         WRITE setTheta         NOTIFY thetaChanged)
    Q_PROPERTY(int    threshold     READ threshold     WRITE setThreshold     NOTIFY thresholdChanged)
    Q_PROPERTY(double srn           READ srn           WRITE setSrn           NOTIFY srnChanged)
    Q_PROPERTY(double stn           READ stn           WRITE setStn           NOTIFY stnChanged)
    Q_PROPERTY(QColor lineColor     READ lineColor     WRITE setLineColor     NOTIFY lineColorChanged)
    Q_PROPERTY(int    lineThickness READ lineThickness WRITE setLineThickness NOTIFY lineThicknessChanged)

public:
    explicit QHoughLines(QQuickItem *parent = 0);
    ~QHoughLines();

    virtual void     transform(cv::Mat& in, cv::Mat& out);
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *nodeData);

    double rho() const;
    double theta() const;
    int    threshold() const;
    double srn() const;
    double stn() const;
    const QColor& lineColor() const;
    int lineThickness() const;

    void setRho(double arg);
    void setTheta(double arg);
    void setThreshold(int arg);
    void setSrn(double srn);
    void setStn(double stn);
    void setLineColor(const QColor& lineColor);
    void setLineThickness(int lineThickness);

signals:
    void rhoChanged();
    void thetaChanged();
    void thresholdChanged();
    void srnChanged();
    void stnChanged();
    void lineColorChanged();
    void lineThicknessChanged();

private:
    QHoughLines(const QHoughLines& other);
    QHoughLines& operator= (const QHoughLines& other);

    double m_rho;
    double m_theta;
    int    m_threshold;
    double m_srn;
    double m_stn;
    QColor m_lineColor;
    int    m_lineThickness;

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

inline double QHoughLines::srn() const{
    return m_srn;
}

inline double QHoughLines::stn() const{
    return m_stn;
}

inline const QColor&QHoughLines::lineColor() const{
    return m_lineColor;
}

inline int QHoughLines::lineThickness() const{
    return m_lineThickness;
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

inline void QHoughLines::setSrn(double srn){
    if ( m_srn != srn ){
        m_srn = srn;
        emit srnChanged();
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

inline void QHoughLines::setLineColor(const QColor& lineColor){
    if ( m_lineColor != lineColor ){
        m_lineColor = lineColor;
        emit lineColorChanged();
        update();
    }
}

inline void QHoughLines::setLineThickness(int lineThickness){
    if ( m_lineThickness != lineThickness ){
        m_lineThickness = lineThickness;
        emit lineThicknessChanged();
        update();
    }
}

#endif // QHOUGHLINES_H
