#include "qmlpropertywatcher.h"

namespace lv{

QmlPropertyWatcher::QmlPropertyWatcher(QObject *object, const QString &propertyName, QObject* parent)
    : QObject(parent)
    , m_property(object, propertyName)
{
    if ( m_property.isValid() )
        m_property.connectNotifySignal(this, SLOT(propertyChanged()));
}

QmlPropertyWatcher::QmlPropertyWatcher(const QQmlProperty &p, QObject *parent)
    : QObject(parent)
    , m_property(p)
{
    if ( m_property.isValid() )
        m_property.connectNotifySignal(this, SLOT(propertyChanged()));
}

QmlPropertyWatcher::~QmlPropertyWatcher(){
}

void QmlPropertyWatcher::propertyChanged() const{
    if ( m_propertyChange )
        m_propertyChange(*this);
}

}// namespace
