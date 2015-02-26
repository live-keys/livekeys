#ifndef QBRISKDESCRIPTOREXTRACTOR_HPP
#define QBRISKDESCRIPTOREXTRACTOR_HPP

#include "QDescriptorExtractor.hpp"

class QBriskDescriptorExtractor : public QDescriptorExtractor{

    Q_OBJECT

public:
    explicit QBriskDescriptorExtractor(QQuickItem *parent = 0);
    ~QBriskDescriptorExtractor();

signals:

public slots:

};

#endif // QBRISKDESCRIPTOREXTRACTOR_HPP
