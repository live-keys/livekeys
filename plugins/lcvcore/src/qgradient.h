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

#ifndef QGRADIENT_H
#define QGRADIENT_H

#include <QObject>
#include <QPointF>
#include <QColor>
#include <QVector3D>
#include <QJSValue>

class QWritableMat;

class QGradient : public QObject{

    Q_OBJECT

public:
    explicit QGradient(QObject *parent = nullptr);

signals:

public slots:

    void draw(QWritableMat* result, QPointF p1, QPointF p2, QJSValue list);
private:
    const double Gamma = 0.43;

    qreal A, B, C;
    qreal radius;
    qreal correctiveSign;
    std::vector<std::pair<double, QColor>> colorList;

    void calculateLineParams(QPointF start, QPointF end);
    qreal calculateSignedDistance(QPointF p);
    qreal linearFromComponent(qreal x);
    qreal componentFromLinear(qreal x);
    QVector3D linearFromRGB(QColor c);
    QColor RGBFromLinear(QVector3D v);
    qreal weightedValue(qreal v1, qreal v2, qreal t);
    QColor weightedColor(qreal t);
};

#endif // QGRADIENT_H
