#ifndef QBRIEFDESCRIPTOREXTRACTOR_H
#define QBRIEFDESCRIPTOREXTRACTOR_H

#include "qdescriptorextractor.h"

class QBriefDescriptorExtractor : public QDescriptorExtractor{

    Q_OBJECT

public:
    explicit QBriefDescriptorExtractor(QQuickItem *parent = 0);
    ~QBriefDescriptorExtractor();

protected:
    void initialize(const QVariantMap& params);

};

#endif // QBRIEFDESCRIPTOREXTRACTOR_H
