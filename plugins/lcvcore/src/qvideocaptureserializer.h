#ifndef QVIDEOCAPTURESERIALIZER_H
#define QVIDEOCAPTURESERIALIZER_H

#include <QObject>
#include <QJSValue>
#include "live/abstractcodeserializer.h"

class QVideoCaptureSerializer : public lv::AbstractCodeSerializer{

    Q_OBJECT

public:
    explicit QVideoCaptureSerializer(QObject *parent = 0);
    ~QVideoCaptureSerializer();

    QString toCode(const QVariant& value, const lv::DocumentEditFragment* channel);
    QVariant fromCode(const QString& value, const lv::DocumentEditFragment* channel);
};

#endif // QVIDEOCAPTURESERIALIZER_H
