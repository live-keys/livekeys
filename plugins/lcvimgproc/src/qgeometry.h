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
        INTER_NEAREST    = cv::INTER_NEAREST, //!< nearest neighbor interpolation
        INTER_LINEAR     = cv::INTER_LINEAR, //!< bilinear interpolation
        INTER_CUBIC      = cv::INTER_CUBIC, //!< bicubic interpolation
        INTER_AREA       = cv::INTER_AREA, //!< area-based (or super) interpolation
        INTER_LANCZOS4   = cv::INTER_LANCZOS4, //!< Lanczos interpolation over 8x8 neighborhood
        INTER_MAX        = 7,
        WARP_INVERSE_MAP = cv::WARP_INVERSE_MAP
    };

public:
    explicit QGeometry(QObject *parent = nullptr);
    ~QGeometry(){}

public slots:
    QMat* resize(QMat* input, QSize size, int interpolation);
    QMat* scale(QMat* input, double fx, double fy, int interpolation);
    QMat* resizeBy(QMat* input, QJSValue ob, int interpolation);
    QMat* rotate(QMat* input, double degrees);

    QMat* getPerspectiveTransform(QVariantList src, QVariantList dst);
    QVariantList applyPerspectiveTransform(QVariantList points, QMat* warp);
    void warpTriangles(QMat* src, QMat* dst, QVariantList triangles1, QVariantList triangles2);
};

#endif // QGEOMETRY_H
