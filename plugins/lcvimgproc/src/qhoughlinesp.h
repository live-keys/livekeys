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

#ifndef QHOUGHTRANSFORMP_H
#define QHOUGHTRANSFORMP_H

#include "qmatfilter.h"

class QHoughLinesPPrivate;
class QHoughLinesP : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(double rho           READ rho           WRITE setRho           NOTIFY rhoChanged)
    Q_PROPERTY(double theta         READ theta         WRITE setTheta         NOTIFY thetaChanged)
    Q_PROPERTY(int    threshold     READ threshold     WRITE setThreshold     NOTIFY thresholdChanged)
    Q_PROPERTY(double minLineLength READ minLineLength WRITE setMinLineLength NOTIFY minLineLengthChanged)
    Q_PROPERTY(double maxLineGap    READ maxLineGap    WRITE setMaxLineGap    NOTIFY maxLineGapChanged)
    Q_PROPERTY(QColor lineColor     READ lineColor     WRITE setLineColor     NOTIFY lineColorChanged)
    Q_PROPERTY(int    lineThickness READ lineThickness WRITE setLineThickness NOTIFY lineThicknessChanged)

public:
    explicit QHoughLinesP(QQuickItem *parent = 0);
    ~QHoughLinesP();

    virtual void transform(cv::Mat& in, cv::Mat& out);
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *nodeData);

    double rho() const;
    double theta() const;
    int    threshold() const;
    double minLineLength() const;
    double maxLineGap() const;
    const QColor& lineColor() const;
    int lineThickness() const;

    void setRho(double arg);
    void setTheta(double arg);
    void setThreshold(int arg);
    void setMinLineLength(double arg);
    void setMaxLineGap(double arg);
    void setLineColor(const QColor& arg);
    void setLineThickness(int arg);

signals:
    void rhoChanged();
    void thetaChanged();
    void thresholdChanged();
    void minLineLengthChanged();
    void maxLineGapChanged();
    void lineColorChanged();
    void lineThicknessChanged();

private:
    QHoughLinesP(const QHoughLinesP& other);
    QHoughLinesP& operator= (const QHoughLinesP& other);

    double m_rho;
    double m_theta;
    int    m_threshold;
    double m_minLineLength;
    double m_maxLineGap;
    QColor m_lineColor;
    int    m_lineThickness;

    QHoughLinesPPrivate* const d_ptr;

    Q_DECLARE_PRIVATE(QHoughLinesP)
};

inline double QHoughLinesP::rho() const{
    return m_rho;
}

inline double QHoughLinesP::theta() const{
    return m_theta;
}

inline int QHoughLinesP::threshold() const{
    return m_threshold;
}

inline double QHoughLinesP::minLineLength() const{
    return m_minLineLength;
}

inline double QHoughLinesP::maxLineGap() const{
    return m_maxLineGap;
}

inline const QColor& QHoughLinesP::lineColor() const{
    return m_lineColor;
}

inline int QHoughLinesP::lineThickness() const{
    return m_lineThickness;
}

inline void QHoughLinesP::setRho(double arg){
    if (m_rho != arg) {
        m_rho = arg;
        emit rhoChanged();
        QMatFilter::transform();
    }
}

inline void QHoughLinesP::setTheta(double arg){
    if (m_theta != arg) {
        m_theta = arg;
        emit thetaChanged();
    }
}

inline void QHoughLinesP::setThreshold(int arg){
    if (m_threshold != arg) {
        m_threshold = arg;
        emit thresholdChanged();
        QMatFilter::transform();
    }
}

inline void QHoughLinesP::setMinLineLength(double arg){
    if (m_minLineLength != arg) {
        m_minLineLength = arg;
        emit minLineLengthChanged();
        QMatFilter::transform();
    }
}

inline void QHoughLinesP::setMaxLineGap(double arg){
    if (m_maxLineGap != arg) {
        m_maxLineGap = arg;
        emit maxLineGapChanged();
        QMatFilter::transform();
    }
}

inline void QHoughLinesP::setLineColor(const QColor& arg){
    if (m_lineColor != arg) {
        m_lineColor = arg;
        emit lineColorChanged();
    }
}

inline void QHoughLinesP::setLineThickness(int arg){
    if (m_lineThickness != arg) {
        m_lineThickness = arg;
        emit lineThicknessChanged();
    }
}

#endif // QHOUGHTRANSFORMP_H
