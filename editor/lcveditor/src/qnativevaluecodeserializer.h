#ifndef QNATIVEVALUECODESERIALIZER_H
#define QNATIVEVALUECODESERIALIZER_H

#include <QObject>
#include <QJSValue>
#include "qlcveditorglobal.h"
#include "qabstractcodeserializer.h"

namespace lcv{

class Q_LCVEDITOR_EXPORT QNativeValueCodeSerializer : public QAbstractCodeSerializer{

    Q_OBJECT

public:
    explicit QNativeValueCodeSerializer(QObject *parent = 0);
    ~QNativeValueCodeSerializer();

    QString toCode(const QVariant& value);
    QVariant fromCode(const QString& value);
};

}// namespace

#endif // QNATIVEVALUECODESERIALIZER_H
