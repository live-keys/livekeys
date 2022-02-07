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

#ifndef QFILTERINGOPERATIONS_H
#define QFILTERINGOPERATIONS_H

#include <QObject>
#include "qmat.h"
#include "opencv2/imgproc.hpp"

class QFilteringOperations: public QObject
{
    Q_OBJECT
    Q_ENUMS(BorderType)
    Q_ENUMS(ElementShape)
public:
    enum BorderType {
        BORDER_CONSTANT    = cv::BORDER_CONSTANT,
        BORDER_REPLICATE   = cv::BORDER_REPLICATE,
        BORDER_REFLECT     = cv::BORDER_REFLECT,
        BORDER_WRAP        = cv::BORDER_WRAP,
        BORDER_REFLECT_101 = cv::BORDER_REFLECT_101,
        BORDER_TRANSPARENT = cv::BORDER_TRANSPARENT,

        BORDER_REFLECT101  = BORDER_REFLECT_101,
        BORDER_DEFAULT     = BORDER_REFLECT_101,
        BORDER_ISOLATED    = cv::BORDER_ISOLATED
    };

    enum ElementShape{
        MORPH_RECT    = cv::MORPH_RECT,
        MORPH_ELLIPSE = cv::MORPH_ELLIPSE,
        MORPH_CROSS   = cv::MORPH_CROSS
    };

    QFilteringOperations(QObject* parent = nullptr);
public slots:
    QMat* blur(QMat *input, QSize size, QPoint point, int borderType = BORDER_DEFAULT);
    QMat* canny(QMat* input, double threshold1, double threshold2,
                             int apertureSize = 3, bool L2gradient = false);
    QMat* dilate(QMat* input, QMat* kernel,
                 QPoint anchor, int iterations = 1,
                 int borderType = BORDER_CONSTANT, QColor borderValue = QColor());
    QMat* erode(QMat* input, QMat* kernel,
                QPoint anchor, int iterations = 1,
                int borderType = BORDER_CONSTANT, QColor borderValue = QColor());
    QMat* filter2D(QMat* src, int ddepth,
                   QMat* kernel, QPoint anchor = QPoint(1, 1),
                   double delta = 0, int borderType = BORDER_DEFAULT);
    QMat* gaussianBlur(QMat* src, QSize size,
                       double sigmaX, double sigmaY = 0,
                       int borderType = BORDER_DEFAULT);
    QMat* sobel(QMat* input, int ddepth, int xorder, int yorder,
                int ksize = 3,
                double scale = 1,
                double delta = 0,
                int borderType = BORDER_DEFAULT);
    QMat* getStructuringElement(int shape, QSize size, QPoint anchor);
};

#endif // QFILTERINGOPERATIONS_H
