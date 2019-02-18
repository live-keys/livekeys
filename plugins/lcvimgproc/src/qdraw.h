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

#ifndef QDRAW_H
#define QDRAW_H

#include <QObject>
#include <QPoint>
#include <QColor>
#include <QSize>
#include "qwritablemat.h"

class QDraw : public QObject{

    Q_OBJECT

public:
    explicit QDraw(QObject *parent = 0);
    virtual ~QDraw();

public slots:
    void line(
            QWritableMat* surface,
            const QPoint& p1,
            const QPoint& p2,
            const QColor& color,
            int thickness = 1,
            int lineType  = 8,
            int shift     = 0);
    void rectangle(
            QWritableMat* surface,
            const QPoint& p1,
            const QPoint& p2,
            const QColor& color,
            int thickness = 1,
            int lineType  = 8,
            int shift     = 0);
    void circle(
            QWritableMat* surface,
            const QPoint& center,
            int radius,
            const QColor& color,
            int thickness = 1,
            int lineType  = 8,
            int shift     = 0);
    void ellipse(
            QWritableMat* surface,
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
            QWritableMat* surface,
            const QVariantList& points,
            const QColor& color,
            int lineType = 8,
            int shift    = 0,
            const QPoint& offset = QPoint());

};

#endif // QDRAW_H
