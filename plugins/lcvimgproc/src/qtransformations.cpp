#include "qtransformations.h"
#include "opencv2/imgproc.hpp"

QTransformations::QTransformations(QObject *parent) : QObject(parent)
{

}

QMat* QTransformations::threshold(QMat* input, double thresh, double maxVal, int type)
{
    if (!input) return nullptr;
    qDebug() << "test: " << input->internal().depth() << QMat::CV8U;
    QMat* output = new QMat(input->dimensions().width(), input->dimensions().height(), QMat::CV8U, 3);
    cv::threshold(input->internal(), output->internal(), thresh, maxVal, type);
    return output;
}
