#ifndef QSIMPLEBLOBDETECTOR_H
#define QSIMPLEBLOBDETECTOR_H

#include "QFeatureDetector.hpp"

class QSimpleBlobDetector : public QFeatureDetector{

    Q_OBJECT

public:
    QSimpleBlobDetector(QQuickItem *parent = 0);
    ~QSimpleBlobDetector();

public slots:
    void initialize(const QVariantMap& settings);

};

#endif // QSIMPLEBLOBDETECTOR_H
