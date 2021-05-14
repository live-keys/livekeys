#include "qgradient.h"
#include <QVector3D>
#include "qwritablemat.h"
#include "opencv2/core.hpp"
#include <opencv2/core/matx.hpp>
#include <QVariant>

QGradient::QGradient(QObject *parent) : QObject(parent)
{

}

void QGradient::draw(QWritableMat *result, QPointF p1, QPointF p2, QJSValue list)
{
    if (p1 == p2) return;

    auto mat = result->internal();
    if (mat.rows == 0 || mat.cols == 0) return;

    QVariantList vList = list.toVariant().toList();
    if (vList.empty()) return;

    double prev = -1.0;
    colorList.clear();
    for (int i = 0; i < vList.length(); ++i)
    {
        auto elem = vList.at(i);
        QVariantList pvList = elem.toList();
        if (pvList.empty() || pvList.length() != 2) return;

        double t = pvList.at(0).toDouble();
        if (t < prev || qFuzzyCompare(t, prev)) return; // the array should be increasing
        prev = t;
        QColor c = pvList.at(1).value<QColor>();

        colorList.push_back(std::make_pair(t, c));
    }

    if (colorList.size() < 2) return;
    if (!qFuzzyCompare(colorList[0].first, 0.0)) return; // the first array element should be 0
    if (!qFuzzyCompare(colorList[colorList.size() - 1].first, 1.0)) return; // the last array element should be 1

    calculateLineParams(p2, p1);

    for (int y = 0; y < mat.rows; ++y)
    for (int x = 0; x < mat.cols; ++x)
    {
        uchar* p = mat.ptr<uchar>(y);
        qreal dist = calculateSignedDistance(QPointF(x,y));
        qreal param = 0.5 + 0.5 * dist / radius;
        if (param < 0.0) param = 0.0;
        if (param > 1.0) param = 1.0;
        QColor colorAt = weightedColor(param);
        p[x*mat.channels() + 0] = colorAt.blue();
        p[x*mat.channels() + 1] = colorAt.green();
        p[x*mat.channels() + 2] = colorAt.red();
    }
}

void QGradient::calculateLineParams(QPointF start, QPointF end)
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
    int i = 0;
    for (; i < colorList.size() - 1; ++i)
        if (colorList[i].first <= t && t <= colorList[i+1].first) break;

    QColor startColor = colorList[i].second;
    QColor endColor = colorList[i+1].second;
    double t1 = colorList[i].first;
    double t2 = colorList[i+1].first;

    QVector3D linearSC = linearFromRGB(startColor);
    QVector3D linearEC = linearFromRGB(endColor);
    qreal brightness1 = pow(linearSC.x()+linearSC.y()+linearSC.z(), Gamma);
    qreal brightness2 = pow(linearEC.x()+linearEC.y()+linearEC.z(), Gamma);

    qreal scaledT = (t-t2)/(t1 - t2);

    qreal brightness = pow(weightedValue(brightness1, brightness2, scaledT), 1/Gamma);
    qreal c1 = weightedValue(linearSC.x(), linearEC.x(), scaledT);
    qreal c2 = weightedValue(linearSC.y(), linearEC.y(), scaledT);
    qreal c3 = weightedValue(linearSC.z(), linearEC.z(), scaledT);

    qreal total = c1 + c2 + c3;
    if (total != 0){
        c1 *= brightness / total;
        c2 *= brightness / total;
        c3 *= brightness / total;
    }

    QColor result = RGBFromLinear(QVector3D(c1, c2, c3));

    return result;
}

