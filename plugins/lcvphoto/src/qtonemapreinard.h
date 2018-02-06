#ifndef QTONEMAPREINARD_H
#define QTONEMAPREINARD_H

#include <QQuickItem>
#include "qtonemap.h"

class QTonemapReinard : public QTonemap{

    Q_OBJECT

public:
    QTonemapReinard(QQuickItem* parent = nullptr);
    virtual ~QTonemapReinard(){}

    void initialize(const QVariantMap &options);
};

#endif // QTONEMAPREINARD_H
