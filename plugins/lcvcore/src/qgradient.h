#ifndef QGRADIENT_H
#define QGRADIENT_H

#include <QObject>
#include <QPointF>
#include <QColor>
#include <QVector3D>

class QWritableMat;

class QGradient : public QObject{

    Q_OBJECT

public:
    explicit QGradient(QObject *parent = nullptr);

signals:

public slots:

    void draw(QWritableMat* result, QPointF p1, QPointF p2, QColor c1, QColor c2);

private:
    const double Gamma = 0.43;

    QPointF start, end;
    qreal A, B, C;
    qreal radius;
    QColor startColor, endColor;
    QVector3D linearSC, linearEC;
    qreal brightness1, brightness2;
    qreal correctiveSign;

    void calculateLineParams();
    qreal calculateSignedDistance(QPointF p);
    qreal linearFromComponent(qreal x);
    qreal componentFromLinear(qreal x);
    QVector3D linearFromRGB(QColor c);
    QColor RGBFromLinear(QVector3D v);
    qreal weightedValue(qreal v1, qreal v2, qreal t);
    QColor weightedColor(qreal t);
};

#endif // QGRADIENT_H
