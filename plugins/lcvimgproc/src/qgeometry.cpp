#include "qgeometry.h"

QGeometry::QGeometry(QObject *parent)
    : QObject(parent)
{
}

QMat *QGeometry::resize(QMat *input, QSize size, int interpolation){
    if ( !input )
        return nullptr;
    QMat* m = new QMat;
    cv::resize(input->internal(), m->internal(), cv::Size(size.width(), size.height()), 0, 0, interpolation);
    return m;
}

QMat *QGeometry::scale(QMat *input, double fx, double fy, int interpolation){
    if ( !input )
        return nullptr;
    QMat* m = new QMat;
    cv::resize(input->internal(), m->internal(), cv::Size(), fx, fy, interpolation);
    return m;
}

QMat *QGeometry::resizeBy(QMat *input, QJSValue ob, int interpolation){
    if ( !input )
        return nullptr;

    if ( ob.hasOwnProperty("w") ){

        double s = ob.property("w").toInt() / input->internal().cols;
        return scale(input, s, s, interpolation);

    } else if ( ob.hasOwnProperty("h") ){

        double s = ob.property("h").toInt() / input->internal().cols;
        return scale(input, s, s, interpolation);
    }

    return nullptr;
}
