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

#ifndef QTRANSFORMATIONS_H
#define QTRANSFORMATIONS_H

#include <QObject>
#include "qmat.h"

class QTransformations : public QObject
{
    Q_OBJECT
public:
    explicit QTransformations(QObject *parent = nullptr);

    enum ThresholdType {
        BINARY      = 0,
        BINARY_INV  = 1,
        TRUNC       = 2,
        TOZERO      = 3,
        TOZERO_INV  = 4,
        OTSU        = 8
    };
    Q_ENUMS(ThresholdType);
public slots:
    QMat* threshold(QMat* in, double thresh, double maxVal, int type);
    QMat* blend(QMat* src1, QMat* src2, QMat* mask);
signals:

};


#endif // QTRANSFORMATIONS_H
