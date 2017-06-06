#ifndef QABSTRACTCODESERIALIZER_H
#define QABSTRACTCODESERIALIZER_H

#include <QObject>
#include <QJSValue>
#include "qlcveditorglobal.h"

namespace lcv{

class Q_LCVEDITOR_EXPORT QAbstractCodeSerializer : public QObject{

    Q_OBJECT

public:
    explicit QAbstractCodeSerializer(QObject *parent = 0);
    virtual ~QAbstractCodeSerializer();

    virtual QString toCode(const QVariant& value) = 0;
    virtual QVariant fromCode(const QString& value) = 0;

};

}// namespace

#endif // QABSTRACTCODESERIALIZER_H
