#ifndef QTONEMAPDRAGO_H
#define QTONEMAPDRAGO_H

#include <QQuickItem>
#include "qtonemap.h"

class QTonemapDrago : public QTonemap{

    Q_OBJECT

public:
    QTonemapDrago(QQuickItem* parent = nullptr);
    virtual ~QTonemapDrago(){}

    void initialize(const QVariantMap &options);

};

#endif // QTONEMAPDRAGO_H
