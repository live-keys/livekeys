#ifndef QFREAKDESCRIPTOREXTRACTOR_HPP
#define QFREAKDESCRIPTOREXTRACTOR_HPP

#include "qdescriptorextractor.h"

class QFreakDescriptorExtractor : public QDescriptorExtractor{

    Q_OBJECT

public:
    explicit QFreakDescriptorExtractor(QQuickItem *parent = 0);
    ~QFreakDescriptorExtractor();

protected:
    void initialize(const QVariantMap& params);

};

#endif // QFREAKDESCRIPTOREXTRACTOR_HPP
