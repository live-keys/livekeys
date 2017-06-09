#include "qstaticloaderproperty.h"
#include <QQmlEngine>
#include <QtQml>


/*!
   \class lcv::QStaticLoaderProperty
   \inmodule live_cpp
   \internal
 */

/*!
  \qmltype StaticLoaderProperty
  \instantiates lcv::QStaticLoaderProperty
  \inqmlmodule live
  \inherits QtObject
  \brief Stores a StaticLoader item property and manages it's lifetime (Owns the property if
  its an object)
 */

/*!
  \qmlproperty variant StaticLoaderProperty::value

  Value to store.
 */

QStaticLoaderProperty::QStaticLoaderProperty(QObject *parent)
    : QObject(parent)
{
}

QStaticLoaderProperty::~QStaticLoaderProperty(){
    if ( m_value.canConvert<QObject*>() ){
        QObject* valueObj = m_value.value<QObject*>();
        valueObj->deleteLater();
    }
}

void QStaticLoaderProperty::setValue(const QVariant& value){
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

