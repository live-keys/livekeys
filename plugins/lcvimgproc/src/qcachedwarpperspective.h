/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
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

#ifndef QCACHEDWARPPERSPECTIVE_H
#define QCACHEDWARPPERSPECTIVE_H

#include <QQuickItem>
#include "qmat.h"

class QCachedWarpPerspective : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QMat* output       READ output        WRITE setOutput        NOTIFY outputChanged)
    Q_PROPERTY(QMat* input        READ input         WRITE setInput         NOTIFY inputChanged)
    Q_PROPERTY(QMat* m            READ m             WRITE setM             NOTIFY mChanged)
    Q_PROPERTY(int interpolation  READ interpolation WRITE setInterpolation NOTIFY interpolationChanged)

public:
    QCachedWarpPerspective(QQuickItem* parent = nullptr);
    ~QCachedWarpPerspective();

    QMat* m() const;
    int interpolation() const;
    QMat* input();
    QMat* output();

    void setM(QMat* m);
    void setInterpolation(int interpolation);
    void setInput(QMat* mat);
    void setOutput(QMat* mat);

    void transform();
    virtual void transform(const cv::Mat &in, cv::Mat &out);

signals:
    void outputChanged();
    void inputChanged();
    void mChanged();
    void interpolationChanged();

protected:
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *nodeData) Q_DECL_OVERRIDE;
    void componentComplete() Q_DECL_OVERRIDE;

private:
    QMat*      m_in;
    QMat*      m_output;
    QMat*      m_m;
    int        m_interpolation;
    bool       m_cacheDirty;
    cv::Mat*   m_cacheMap1;
    cv::Mat*   m_cacheMap2;
};

inline QMat *QCachedWarpPerspective::m() const{
    return m_m;
}

inline int QCachedWarpPerspective::interpolation() const{
    return m_interpolation;
}

inline QMat *QCachedWarpPerspective::input(){
    return m_in;
}

inline QMat *QCachedWarpPerspective::output(){
    return m_output;
}

inline void QCachedWarpPerspective::setM(QMat *m){
    m_m = m;
    emit mChanged();

    m_cacheDirty = true;
    transform();
}

inline void QCachedWarpPerspective::setInterpolation(int interpolation){
    if (m_interpolation == interpolation)
        return;

    m_interpolation = interpolation;
    emit interpolationChanged();
    transform();
}

inline void QCachedWarpPerspective::setInput(QMat *mat){
    if ( mat == 0 )
        return;

    if ( m_in->internal().size() != mat->internal().size() )
        m_cacheDirty = true;

    m_in = mat;
    emit inputChanged();
    transform();
}

inline void QCachedWarpPerspective::setOutput(QMat *mat){
    if ( mat == 0 )
        return;

    if ( m_output->internal().size() != mat->internal().size() )
        m_cacheDirty = true;

    cv::Mat* matData = mat->internalPtr();
    if ( implicitWidth() != matData->cols || implicitHeight() != matData->rows ){
        setImplicitWidth(matData->cols);
        setImplicitHeight(matData->rows);
    }
    m_output = mat;
    emit outputChanged();
    transform();
}

#endif // QCACHEDWARPPERSPECTIVE_H
