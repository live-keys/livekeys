#ifndef QDENOISING_H
#define QDENOISING_H

#include <QObject>
#include "qmat.h"

/// \private
class QDenoising : public QObject{

    Q_OBJECT

public:
    Q_INVOKABLE QDenoising(QObject *parent = nullptr);

public slots:
    QObject *fastNlMeans(
        QObject *input,
        float h = 3,
        int templateWindowSize = 7,
        int searchWindowSize = 21
    );

    QObject *fastNlMeansColored(
        QObject* input,
        float h = 3,
        int templateWindowSize = 7,
        int searchWindowSize = 21
    );

};

#endif // QDENOISING_H
