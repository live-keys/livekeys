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

#ifndef QSHAPEPREDICTORS_H
#define QSHAPEPREDICTORS_H

#include <QObject>
#include <QVariantList>
#include <QSize>

class QMat;

class QShapeDescriptors : public QObject
{
    Q_OBJECT
public:
    explicit QShapeDescriptors(QObject *parent = nullptr);
public slots:
    QVariantList convexHullIndices(QVariantList points);
    QVariantList DelaunayTriangles(QSize rectSize, QVariantList points);
    QMat* getMaskFromHullPoints(QVariantList points, QSize size, int depth);
};

#endif // QSHAPEPREDICTORS_H
