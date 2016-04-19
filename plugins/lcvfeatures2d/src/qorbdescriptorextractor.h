#ifndef QORBDESCRIPTOREXTRACTOR_HPP
#define QORBDESCRIPTOREXTRACTOR_HPP

#include "qdescriptorextractor.h"

class QOrbDescriptorExtractor : public QDescriptorExtractor{

    Q_OBJECT
public:
    explicit QOrbDescriptorExtractor(QQuickItem *parent = 0);
    ~QOrbDescriptorExtractor();

protected:
    void initialize(const QVariantMap& params);

};

#endif // QORBDESCRIPTOREXTRACTOR_HPP
