#ifndef QDENOISING_H
#define QDENOISING_H

#include <QObject>
#include "qmat.h"
#include "live/act.h"

/// \private
class QDenoising : public QObject{

    Q_OBJECT

public:
    explicit QDenoising(QObject *parent = nullptr);

public slots:
    void fastNlMeans(
        lv::Act* context,
        QMat* input,
        float h = 3,
        int templateWindowSize = 7,
        int searchWindowSize = 21
    );

    void fastNlMeansColored(
        lv::Act* context,
        QMat* input,
        float h = 3,
        int templateWindowSize = 7,
        int searchWindowSize = 21
    );

};

#endif // QDENOISING_H
