#ifndef QBRISKFEATUREDETECTOR_HPP
#define QBRISKFEATUREDETECTOR_HPP

#include "QFeatureDetector.hpp"

class QBriskFeatureDetector : public QFeatureDetector{

    Q_OBJECT
public:
    explicit QBriskFeatureDetector(QQuickItem *parent = 0);
    virtual ~QBriskFeatureDetector();

signals:

public slots:

};

#endif // QBRISKFEATUREDETECTOR_HPP
