#ifndef QTONEMAPMANTIUK_H
#define QTONEMAPMANTIUK_H

#include <QQuickItem>
#include "qtonemap.h"

class QTonemapMantiuk : public QTonemap{

    Q_OBJECT

public:
    QTonemapMantiuk(QQuickItem* parent = nullptr);
    virtual ~QTonemapMantiuk(){}

    void initialize(const QVariantMap& options);
};

#endif // QTONEMAPMANTIUK_H
