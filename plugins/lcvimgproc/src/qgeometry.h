#ifndef QGEOMETRY_H
#define QGEOMETRY_H

#include <QObject>
#include "opencv2/imgproc.hpp"
#include "qmat.h"

class QGeometry : public QObject{

    Q_OBJECT
    Q_ENUMS(Interpolation)

public:
    enum Interpolation{
        INTER_NEAREST    = CV_INTER_NN, //!< nearest neighbor interpolation
        INTER_LINEAR     = CV_INTER_LINEAR, //!< bilinear interpolation
        INTER_CUBIC      = CV_INTER_CUBIC, //!< bicubic interpolation
        INTER_AREA       = CV_INTER_AREA, //!< area-based (or super) interpolation
        INTER_LANCZOS4   = CV_INTER_LANCZOS4, //!< Lanczos interpolation over 8x8 neighborhood
        INTER_MAX        = 7,
        WARP_INVERSE_MAP = CV_WARP_INVERSE_MAP
    };

public:
    explicit QGeometry(QObject *parent = nullptr);
    ~QGeometry(){}

public slots:
    QMat* resize(QMat* input, QSize size, int interpolation);
    QMat* scale(QMat* input, double fx, double fy, int interpolation);
    QMat* resizeBy(QMat* input, QJSValue ob, int interpolation);

};

#endif // QGEOMETRY_H
