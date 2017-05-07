#include "qnativevaluecodeserializer.h"
#include <QVariant>

namespace lcv{

QNativeValueCodeSerializer::QNativeValueCodeSerializer(QObject *parent)
    : QAbstractCodeSerializer(parent)
{
}

QNativeValueCodeSerializer::~QNativeValueCodeSerializer(){
}

QString QNativeValueCodeSerializer::toCode(const QJSValue &value){
    if ( value.isBool() || value.isNumber() || value.isString() )
        return value.toString();

    qWarning("Failed to convert value: %s", qPrintable(value.toString()));
    return "";
}

QJSValue QNativeValueCodeSerializer::fromCode(const QString &value){
    QString val = value.trimmed();
    if ( val.length() > 1 && (
             (val.startsWith('"') && val.endsWith('"')) ||
             (val.startsWith('\'') && val.endsWith('\'') )
        ))
    {
        return QJSValue(val.mid(1, value.length() - 2));
    } else if ( val == "true" ){
        return QJSValue(true);
    } else if ( val == "false" ){
        return QJSValue(false);
    } else {
        return QJSValue(val.toDouble());
    }
}

}// namespace
