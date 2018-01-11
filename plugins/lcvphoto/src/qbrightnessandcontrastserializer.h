#ifndef QBRIGHTNESSANDCONTRASTSERIALIZER_H
#define QBRIGHTNESSANDCONTRASTSERIALIZER_H

#include <QObject>
#include <QJSValue>

#include "live/abstractcodeserializer.h"

class QBrightnessAndContrastSerializer : public lv::AbstractCodeSerializer{

    Q_OBJECT

public:
    explicit QBrightnessAndContrastSerializer(QObject* parent = 0);
    ~QBrightnessAndContrastSerializer();

    QString toCode(const QVariant& value, const lv::DocumentEditFragment* channel);
    QVariant fromCode(const QString& value, const lv::DocumentEditFragment* channel);
};

#endif // QBRIGHTNESSANDCONTRASTSERIALIZER_H
