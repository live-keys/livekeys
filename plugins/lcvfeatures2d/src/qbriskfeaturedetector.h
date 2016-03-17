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
    void initialize(int thresh = 30, int octaves = 3, float patternScale = 1.0f);

};

#endif // QBRISKFEATUREDETECTOR_HPP
