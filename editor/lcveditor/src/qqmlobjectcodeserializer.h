#ifndef QQMLOBJECTCODESERIALIZER_H
#define QQMLOBJECTCODESERIALIZER_H

#include <QObject>
#include "qlcveditorglobal.h"
#include "qabstractcodeserializer.h"

namespace lcv{

class Q_LCVEDITOR_EXPORT QQmlObjectCodeSerializer : public QAbstractCodeSerializer{

public:
    explicit QQmlObjectCodeSerializer(QObject* parent = 0);
    ~QQmlObjectCodeSerializer();

    QString toCode(const QVariant &value, const QDocumentEditFragment* channel) Q_DECL_OVERRIDE;
    QVariant fromCode(const QString& value, const QDocumentEditFragment* channel) Q_DECL_OVERRIDE;

};

}// namespace

#endif // QQMLOBJECTCODESERIALIZER_H
