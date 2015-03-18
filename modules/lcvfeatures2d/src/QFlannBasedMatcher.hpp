#ifndef QFLANNBASEDMATCHER_HPP
#define QFLANNBASEDMATCHER_HPP

#include "QDescriptorMatcher.hpp"

class QFlannBasedMatcher : public QDescriptorMatcher{

    Q_OBJECT

public:
    explicit QFlannBasedMatcher(QQuickItem *parent = 0);
    ~QFlannBasedMatcher();

public slots:
    void initialize(const QJsonObject& initparams);

};

#endif // QFLANNBASEDMATCHER_HPP
