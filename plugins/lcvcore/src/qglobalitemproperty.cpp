#include "qglobalitemproperty.h"
#include <QQmlEngine>

QGlobalItemProperty::QGlobalItemProperty(QQuickItem *parent) : QQuickItem(parent)
{

}

QGlobalItemProperty::~QGlobalItemProperty(){
    if ( m_value.canConvert<QObject*>() ){
        QObject* valueObj = m_value.value<QObject*>();
        valueObj->deleteLater();
    }
}

void QGlobalItemProperty::setValue(const QVariant& value){
    if (m_value == value)
        return;

    if ( m_value.canConvert<QObject*>() ){
        QObject* valueObj = m_value.value<QObject*>();
        qmlEngine(this)->setObjectOwnership(valueObj, QQmlEngine::JavaScriptOwnership);
    }

    if ( value.canConvert<QObject*>() ){
        QObject* valueObj = value.value<QObject*>();
        qmlEngine(this)->setObjectOwnership(valueObj, QQmlEngine::CppOwnership);
    }
    m_value = value;

    emit valueChanged();
}
