#ifndef QBRISKDESCRIPTOREXTRACTOR_HPP
#define QBRISKDESCRIPTOREXTRACTOR_HPP

#include "qdescriptorextractor.h"

class QBriskDescriptorExtractor : public QDescriptorExtractor{

    Q_OBJECT

public:
    explicit QBriskDescriptorExtractor(QQuickItem *parent = 0);
    ~QBriskDescriptorExtractor();

};

#endif // QBRISKDESCRIPTOREXTRACTOR_HPP
