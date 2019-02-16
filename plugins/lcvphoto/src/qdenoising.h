#ifndef QDENOISING_H
#define QDENOISING_H

#include <QObject>
#include "qmat.h"
#include "live/filter.h"

class QDenoising : public QObject{

    Q_OBJECT

public:
    explicit QDenoising(QObject *parent = nullptr);

public slots:
    void fastNlMeansAsync(
        lv::Filter* context,
        QMat* input,
        float h = 3,
        int templateWindowSize = 7,
        int searchWindowSize = 21,
        const QJSValue& allocator = QJSValue()
    );

    void fastNlMeansColoredAsync(
        lv::Filter* context,
        QMat* input,
        float h = 3,
        int templateWindowSize = 7,
        int searchWindowSize = 21,
        const QJSValue& allocator = QJSValue()
    );

};

#endif // QDENOISING_H
