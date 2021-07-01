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
