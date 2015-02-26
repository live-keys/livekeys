#ifndef QORBFEATUREDETECTOR_HPP
#define QORBFEATUREDETECTOR_HPP

#include "QFeatureDetector.hpp"

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
    void initialize(
            int nfeatures = 500, float scaleFactor = 1.2f, int nlevels = 8, int edgeThreshold = 31, int firstLevel = 0,
            int WTA_K = 2, int scoreType = ScoreType::HARRIS_SCORE);

};

#endif // QORBFEATUREDETECTOR_HPP
