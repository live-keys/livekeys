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
