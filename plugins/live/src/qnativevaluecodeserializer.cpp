#include "qnativevaluecodeserializer.h"
#include <QVariant>
#include <QDebug>

namespace lcv{

QNativeValueCodeSerializer::QNativeValueCodeSerializer(QObject *parent)
    : QAbstractCodeSerializer(parent)
{
}

QNativeValueCodeSerializer::~QNativeValueCodeSerializer(){
}

QString QNativeValueCodeSerializer::toCode(const QVariant &value){
    if ( value.type() >= QVariant::Bool && value.type() <= QVariant::Double )
        return value.toString();
    else if ( value.type() == QVariant::Char ||
              value.type() == QVariant::String ||
              value.type() == QVariant::ByteArray )
        return "\'" + value.toString() + "\'";

    qWarning("Failed to convert value: %s", qPrintable(value.toString()));
    return "";
}

QVariant QNativeValueCodeSerializer::fromCode(const QString &value){
    QString val = value.trimmed();
    if ( val.length() > 1 && (
             (val.startsWith('"') && val.endsWith('"')) ||
             (val.startsWith('\'') && val.endsWith('\'') )
        ))
    {
        return QVariant(val.mid(1, value.length() - 2));
    } else if ( val == "true" ){
        return QVariant(true);
    } else if ( val == "false" ){
        return QVariant(false);
    } else {
        bool ok;
        qint64 number = value.toLongLong(&ok);
        if (ok)
            return QVariant(number);

        return QVariant(val.toDouble());
    }

    return QVariant();
}

}// namespace
