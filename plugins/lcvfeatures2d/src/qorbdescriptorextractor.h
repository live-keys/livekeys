#ifndef QORBDESCRIPTOREXTRACTOR_HPP
#define QORBDESCRIPTOREXTRACTOR_HPP

#include "qdescriptorextractor.h"

class QOrbDescriptorExtractor : public QDescriptorExtractor{

    Q_OBJECT
public:
    explicit QOrbDescriptorExtractor(QQuickItem *parent = 0);
    ~QOrbDescriptorExtractor();

public slots:
    void initialize(int patchSize = 32);

};

#endif // QORBDESCRIPTOREXTRACTOR_HPP
