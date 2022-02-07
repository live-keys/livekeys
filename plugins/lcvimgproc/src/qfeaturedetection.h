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

#ifndef QFEATUREDETECTION_H
#define QFEATUREDETECTION_H

#include <QObject>
#include "qmat.h"
#include "opencv2/imgproc.hpp"

class QFeatureDetection : public QObject
{
    Q_OBJECT
public:
    explicit QFeatureDetection(QObject *parent = nullptr);

public slots:
    QMat* houghLines(QMat* input, double rho, double theta, int threshold,
                     double srn = 0, double stn = 0);
    QMat* houghLinesP(QMat* input, double rho, double theta, int threshold,
                      double minLineLength = 0, double maxLineGap = 0);
};

#endif // QFEATUREDETECTION_H
