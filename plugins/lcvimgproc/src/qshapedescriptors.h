#ifndef QSHAPEPREDICTORS_H
#define QSHAPEPREDICTORS_H

#include <QObject>
#include <QVariantList>
#include <QSize>

class QMat;

class QShapeDescriptors : public QObject
{
    Q_OBJECT
public:
    explicit QShapeDescriptors(QObject *parent = nullptr);
public slots:
    QVariantList convexHullIndices(QVariantList points);
    QVariantList DelaunayTriangles(QSize rectSize, QVariantList points);
    QMat* getMaskFromHullPoints(QVariantList points, QSize size, int depth);
};

#endif // QSHAPEPREDICTORS_H
