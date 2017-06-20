#include "qqmlobjectcodeserializer.h"
#include "qdocumentqmlfragment.h"

namespace lcv{

QQmlObjectCodeSerializer::QQmlObjectCodeSerializer(QObject *parent)
    : QAbstractCodeSerializer(parent)
{
}

QQmlObjectCodeSerializer::~QQmlObjectCodeSerializer(){

}

QString QQmlObjectCodeSerializer::toCode(const QVariant &value, const QDocumentEditFragment* channel){
    const QDocumentQmlFragment* qmlfragment = static_cast<const QDocumentQmlFragment*>(channel);
    if ( qmlfragment ){
        if ( qmlfragment->property().propertyTypeCategory() == QQmlProperty::Object ){
            qmlfragment->property().write(value);
        }
    }

    return QString();
}

QVariant QQmlObjectCodeSerializer::fromCode(const QString &code, const QDocumentEditFragment *channel){
    const QDocumentQmlFragment* qmlfragment = static_cast<const QDocumentQmlFragment*>(channel);
    if ( qmlfragment ){
        if ( qmlfragment->property().propertyTypeCategory() == QQmlProperty::Object ){
            return qmlfragment->property().read();
        }
    }

    qWarning("Code section \'%s\' requires qml binding channel in order to modify value", qPrintable(code));
    return QVariant();
}

} // namespace
