#ifndef QLEVELSSERIALIZER_H
#define QLEVELSSERIALIZER_H

#include <QObject>
#include <QJSValue>

#include "live/abstractcodeserializer.h"

class QLevelsSerializer : public lv::AbstractCodeSerializer{

    Q_OBJECT

public:
    explicit QLevelsSerializer(QObject* parent = 0);
    ~QLevelsSerializer();

    QString toCode(const QVariant& value, const lv::DocumentEditFragment* channel);
    QVariant fromCode(const QString& value, const lv::DocumentEditFragment* channel);
};

#endif // QLEVELSSERIALIZER_H
