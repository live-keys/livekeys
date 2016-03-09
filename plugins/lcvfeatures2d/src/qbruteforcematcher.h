#ifndef QBRUTEFORCEMATCHER_HPP
#define QBRUTEFORCEMATCHER_HPP

#include "qdescriptormatcher.h"

class QBruteForceMatcher : public QDescriptorMatcher{

    Q_OBJECT

public:
    QBruteForceMatcher(QQuickItem* parent = 0);
    virtual ~QBruteForceMatcher();

};

#endif // QBRUTEFORCEMATCHER_HPP
