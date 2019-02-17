#include "call.h"
#include <QMetaObject>
#include <QMetaProperty>
#include <QQmlProperty>

namespace lv{

Call::Call(QObject *parent)
    : QObject(parent)
    , m_isComponentComplete(false)
{
}

Call::~Call(){
}

void Call::componentComplete(){
    m_isComponentComplete = true;

    const QMetaObject* meta = metaObject();

    for ( int i = 0; i < meta->propertyCount(); i++ ){
        QMetaProperty property = meta->property(i);
        QByteArray name = property.name();
        if ( name != "objectName" && name != "result" ){
            QQmlProperty pp(this, name);
            pp.connectNotifySignal(this, SIGNAL(trigger()));
        }
    }

    emit complete();
}

}// namespace
