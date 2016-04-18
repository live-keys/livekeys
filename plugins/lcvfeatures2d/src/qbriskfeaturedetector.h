#ifndef QBRISKFEATUREDETECTOR_HPP
#define QBRISKFEATUREDETECTOR_HPP

#include "qfeaturedetector.h"

class QBriskFeatureDetector : public QFeatureDetector{

    Q_OBJECT

public:
    explicit QBriskFeatureDetector(QQuickItem *parent = 0);
    virtual ~QBriskFeatureDetector();

signals:

public slots:
    void initialize(const QVariantMap& settings);

};

#endif // QBRISKFEATUREDETECTOR_HPP
