#ifndef QTONEMAPDURAND_H
#define QTONEMAPDURAND_H

#include <QQuickItem>
#include "qtonemap.h"

class QTonemapDurand : public QTonemap{

    Q_OBJECT

public:
    QTonemapDurand(QQuickItem* parent = nullptr);
    virtual ~QTonemapDurand(){}

    void initialize(const QVariantMap &options);
};

#endif // QTONEMAPDURAND_H
