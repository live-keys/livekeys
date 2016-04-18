#ifndef QORBFEATUREDETECTOR_HPP
#define QORBFEATUREDETECTOR_HPP

#include "qfeaturedetector.h"

class QOrbFeatureDetector : public QFeatureDetector{

    Q_OBJECT
    Q_ENUMS(ScoreType)

public:
    enum ScoreType{
        kBytes = 32,
        HARRIS_SCORE=0,
        FAST_SCORE=1
    };

public:
    explicit QOrbFeatureDetector(QQuickItem *parent = 0);
    virtual ~QOrbFeatureDetector();

public slots:
    void initialize(const QVariantMap& settings);

};

#endif // QORBFEATUREDETECTOR_HPP
