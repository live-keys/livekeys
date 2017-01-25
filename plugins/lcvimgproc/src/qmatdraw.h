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

#ifndef QMATDRAW_H
#define QMATDRAW_H

#include "qlcvglobal.h"
#include "qmatdisplay.h"

class QMatDraw : public QMatDisplay{

    Q_OBJECT
    Q_PROPERTY(QMat* input READ inputMat WRITE setInputMat NOTIFY inChanged)

public:
    explicit QMatDraw(QQuickItem *parent = 0);
    virtual ~QMatDraw();

public slots:
    void cleanUp();
    void line(
            const QPoint& p1,
            const QPoint& p2,
            const QColor& color,
            int thickness = 1,
            int lineType  = 8,
            int shift     = 0);
    void rectangle(
            const QPoint& p1,
            const QPoint& p2,
            const QColor& color,
            int thickness = 1,
            int lineType  = 8,
            int shift     = 0);
    void circle(
            const QPoint& center,
            int radius,
            const QColor& color,
            int thickness = 1,
            int lineType  = 8,
            int shift     = 0);
    void ellipse(
            const QPoint& center,
            const QSize& axes,
            double angle,
            double startAngle,
            double endAngle,
            const QColor& color,
            int thickness = 1,
            int lineType  = 8,
            int shift     = 0 );
    void fillPoly(
            const QVariantList& points,
            const QColor& color,
            int lineType = 8,
            int shift    = 0,
            const QPoint& offset = QPoint());

    QMat* inputMat();
    void setInputMat(QMat* mat);

signals:
    void inChanged();

private:
    QMat* m_in;

};


inline QMat *QMatDraw::inputMat(){
    return m_in;
}

inline void QMatDraw::setInputMat(QMat *mat){
    if ( mat == 0 )
        return;

    cv::Mat* matData = mat->cvMat();
    if ( implicitWidth() != matData->cols || implicitHeight() != matData->rows ){
        setImplicitWidth(matData->cols);
        setImplicitHeight(matData->rows);
    }
    m_in = mat;
    cleanUp();
    emit inChanged();
}

#endif // QMATDRAW_H
