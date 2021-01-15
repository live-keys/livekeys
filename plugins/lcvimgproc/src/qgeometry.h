#ifndef QGEOMETRY_H
#define QGEOMETRY_H

#include <QObject>
#include "opencv2/imgproc.hpp"
#include "qmat.h"

class QGeometry : public QObject{

    Q_OBJECT
    Q_ENUMS(Interpolation)
    Q_ENUMS(BorderTypes)

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

    enum BorderTypes {
        BORDER_CONSTANT    = cv::BORDER_CONSTANT, //!< `iiiiii|abcdefgh|iiiiiii`  with some specified `i`
        BORDER_REPLICATE   = cv::BORDER_REPLICATE, //!< `aaaaaa|abcdefgh|hhhhhhh`
        BORDER_REFLECT     = cv::BORDER_REFLECT, //!< `fedcba|abcdefgh|hgfedcb`
        BORDER_WRAP        = cv::BORDER_WRAP, //!< `cdefgh|abcdefgh|abcdefg`
        BORDER_REFLECT_101 = cv::BORDER_REFLECT_101, //!< `gfedcb|abcdefgh|gfedcba`
        BORDER_TRANSPARENT = cv::BORDER_TRANSPARENT, //!< `uvwxyz|abcdefgh|ijklmno`

        BORDER_REFLECT101  = cv::BORDER_REFLECT101, //!< same as BORDER_REFLECT_101
        BORDER_DEFAULT     = cv::BORDER_DEFAULT, //!< same as BORDER_REFLECT_101
        BORDER_ISOLATED    = cv::BORDER_ISOLATED //!< do not look outside of ROI
    };


public:
    explicit QGeometry(QObject *parent = nullptr);
    ~QGeometry(){}

public slots:
    QMat* resize(QMat* input, QSize size, int interpolation);
    QMat* scale(QMat* input, double fx, double fy, int interpolation);
    QMat* resizeBy(QMat* input, QJSValue ob, int interpolation);
    QMat* rotate(QMat* input, double degrees);
    QMat* transform(QMat* input, QMat* m);

    QMat* getPerspectiveTransform(QJSValue src, QJSValue dst);
    QMat* getPerspectiveTransform(QVariantList src, QVariantList dst);
    QMat* getPerspectiveTransform(std::vector<cv::Point2f> src, std::vector<cv::Point2f> dst);

    QMat* warpPerspective(QMat* input, QMat* transform, QSize size, int flags, int borderMode);

    QVariantList applyPerspectiveTransform(QVariantList points, QMat* warp);
    void warpTriangles(QMat* src, QMat* dst, QVariantList triangles1, QVariantList triangles2);
};

#endif // QGEOMETRY_H
