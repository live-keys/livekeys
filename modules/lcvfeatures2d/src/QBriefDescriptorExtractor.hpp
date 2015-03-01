#ifndef QBRIEFDESCRIPTOREXTRACTOR_HPP
#define QBRIEFDESCRIPTOREXTRACTOR_HPP

#include "QDescriptorExtractor.hpp"

class QBriefDescriptorExtractor : public QDescriptorExtractor{

    Q_OBJECT

public:
    explicit QBriefDescriptorExtractor(QQuickItem *parent = 0);
    ~QBriefDescriptorExtractor();

public slots:
    void initialize(int bytes = 32);

};

#endif // QBRIEFDESCRIPTOREXTRACTOR_HPP
