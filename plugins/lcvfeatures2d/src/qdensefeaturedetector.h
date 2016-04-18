#ifndef QDENSEFEATUREDETECTOR_HPP
#define QDENSEFEATUREDETECTOR_HPP

#include "qfeaturedetector.h"

class QDenseFeatureDetector : public QFeatureDetector{

    Q_OBJECT

public:
    explicit QDenseFeatureDetector(QQuickItem *parent = 0);
    virtual ~QDenseFeatureDetector();

public slots:
    void initialize(const QVariantMap& settings);

};

#endif // QDENSEFEATUREDETECTOR_HPP
