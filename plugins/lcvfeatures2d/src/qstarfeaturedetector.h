#ifndef QSTARFEATUREDETECTOR_H
#define QSTARFEATUREDETECTOR_H

#include "qfeaturedetector.h"

class QStarFeatureDetector : public QFeatureDetector{

    Q_OBJECT

public:
    explicit QStarFeatureDetector(QQuickItem *parent = 0);
    ~QStarFeatureDetector();

public slots:
    void initialize(const QVariantMap& settings);

};

#endif // QSTARFEATUREDETECTOR_H
