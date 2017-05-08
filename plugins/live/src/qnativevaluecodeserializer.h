#ifndef QVALUECODESERIALIZER_H
#define QVALUECODESERIALIZER_H

#include <QObject>
#include <QJSValue>
#include "qliveglobal.h"
#include "qabstractcodeserializer.h"

namespace lcv{

class Q_LIVE_EXPORT QNativeValueCodeSerializer : public QAbstractCodeSerializer{

    Q_OBJECT

public:
    explicit QNativeValueCodeSerializer(QObject *parent = 0);
    ~QNativeValueCodeSerializer();

    QString toCode(const QJSValue& value);
    QJSValue fromCode(const QString& value);
};

}// namespace

#endif // QVALUECODESERIALIZER_H
