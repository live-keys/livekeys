#ifndef QSIMPLEBLOBDETECTOR_H
#define QSIMPLEBLOBDETECTOR_H

#include "qfeaturedetector.h"

class QSimpleBlobDetector : public QFeatureDetector{

    Q_OBJECT

public:
    QSimpleBlobDetector(QQuickItem *parent = 0);
    ~QSimpleBlobDetector();

    void initialize(const QVariantMap& settings);

};

#endif // QSIMPLEBLOBDETECTOR_H
