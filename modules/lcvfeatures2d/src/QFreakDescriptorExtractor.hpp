#ifndef QFREAKDESCRIPTOREXTRACTOR_HPP
#define QFREAKDESCRIPTOREXTRACTOR_HPP

#include "QDescriptorExtractor.hpp"

class QFreakDescriptorExtractor : public QDescriptorExtractor{

    Q_OBJECT

public:
    explicit QFreakDescriptorExtractor(QQuickItem *parent = 0);
    ~QFreakDescriptorExtractor();

public slots:
    void initialize(
        bool orientationNormalized = true, bool scaleNormalized = true, float patternScale = 22.0f, int nOctaves = 4
    );

};

#endif // QFREAKDESCRIPTOREXTRACTOR_HPP
