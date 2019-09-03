#include "qgradient.h"
#include <QVector3D>
#include "qmat.h"
#include "opencv2/core.hpp"
#include <opencv2/core/matx.hpp>

#define GAMMA 0.43

QGradient::QGradient(QObject *parent) : QObject(parent)
{

}

void QGradient::draw(QMat* result, QPointF p1, QPointF p2, QColor c1, QColor c2)
{
    if (p1 == p2) return;

    auto mat = result->cvMat();
    if (mat->rows == 0 || mat->cols == 0) return;


    start = p1; end = p2;
    startColor = c1; endColor = c2;

    calculateLineParams();

    linearSC = linearFromRGB(startColor);
    linearEC = linearFromRGB(endColor);
    brightness1 = pow(linearSC.x()+linearSC.y()+linearSC.z(), GAMMA);
    brightness2 = pow(linearEC.x()+linearEC.y()+linearEC.z(), GAMMA);

    for (int y = 0; y < mat->rows; ++y)
    for (int x = 0; x < mat->cols; ++x)
    {
        cv::Vec3b& color = mat->at<cv::Vec3b>(cv::Point(x,y));
        qreal dist = calculateSignedDistance(QPointF(x,y));
        qreal param = 0.5 + 0.5 * dist / radius;
        if (param < 0.0) param = 0.0;
        if (param > 1.0) param = 1.0;
        QColor colorAt = weightedColor(param);
        color[2] = colorAt.red();
        color[1] = colorAt.green();
        color[0] = colorAt.blue();
    }
}

void QGradient::calculateLineParams()
{

    A = end.x() - start.x();
    B = end.y() - start.y();
    C = 0.5*(start.x() * start.x() + start.y() * start.y()
           - end.x() * end.x() - end.y() * end.y());
    correctiveSign = (A*start.x() + B*start.y() + C > 0) ? 1.0 : -1.0;
    radius = abs(calculateSignedDistance(start));
}

qreal QGradient::calculateSignedDistance(QPointF p)
{
    return correctiveSign*(A*p.x() + B*p.y() + C) / sqrt(A*A + B*B);
}

qreal QGradient::linearFromComponent(qreal x)
{
    qreal result;
    if (x <= 0.04045) result = x / 12.92;
    else result = pow((x+0.055)/1.055, 2.4);

    return result;
}

qreal QGradient::componentFromLinear(qreal x)
{
    qreal result;

    if (x<=0.0031308) result = 12.92*x;
    else result = 1.055 * pow(x, 1.0/2.4) - 0.055;

    return result;
}

QVector3D QGradient::linearFromRGB(QColor c)
{
    return QVector3D(
        linearFromComponent(c.redF()),
        linearFromComponent(c.greenF()),
        linearFromComponent(c.blueF())
    );
}

QColor QGradient::RGBFromLinear(QVector3D v)
{
    QColor result;
    result.setRedF(componentFromLinear(v.x()));
    result.setGreenF(componentFromLinear(v.y()));
    result.setBlueF(componentFromLinear(v.z()));

    return result;
}

qreal QGradient::weightedValue(qreal v1, qreal v2, qreal t)
{
    return v2*(1-t) + v1*t;
}

QColor QGradient::weightedColor(qreal t)
{
    qreal brightness = pow(weightedValue(brightness1, brightness2, t), 1/GAMMA);
    qreal c1 = weightedValue(linearSC.x(), linearEC.x(), t);
    qreal c2 = weightedValue(linearSC.y(), linearEC.y(), t);
    qreal c3 = weightedValue(linearSC.z(), linearEC.z(), t);

    qreal total = c1 + c2 + c3;
    if (total != 0){
        c1 *= brightness / total;
        c2 *= brightness / total;
        c3 *= brightness / total;
    }

    QColor result = RGBFromLinear(QVector3D(c1, c2, c3));

    return result;
}

