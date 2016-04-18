#ifndef QMSERFEATUREDETECTOR_H
#define QMSERFEATUREDETECTOR_H

#include "qfeaturedetector.h"

class QMSerFeatureDetector : public QFeatureDetector{

    Q_OBJECT

public:
    QMSerFeatureDetector(QQuickItem* parent = 0);
    ~QMSerFeatureDetector();

public slots:
    void initialize(const QVariantMap& settings);
};


#endif // QMSERFEATUREDETECTOR_H
