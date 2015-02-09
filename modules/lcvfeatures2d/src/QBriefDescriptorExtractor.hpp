#ifndef QBRIEFDESCRIPTOREXTRACTOR_HPP
#define QBRIEFDESCRIPTOREXTRACTOR_HPP

#include "QDescriptorExtractor.hpp"

class QBriefDescriptorExtractor : public QDescriptorExtractor{

    Q_OBJECT

public:
    explicit QBriefDescriptorExtractor(QQuickItem *parent = 0);
    ~QBriefDescriptorExtractor();

signals:

public slots:

};

#endif // QBRIEFDESCRIPTOREXTRACTOR_HPP
