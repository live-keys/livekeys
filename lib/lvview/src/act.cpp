#include "act.h"
#include "workerthread.h"
#include <QMetaObject>
#include <QMetaProperty>
#include <QQmlProperty>

namespace lv{

Act::Act(QObject *parent)
    : QObject(parent)
    , m_isComponentComplete(false)
    , m_workerThread(nullptr)
{
}

Act::~Act(){
}

void Act::use(Shared::ReadScope *locker, const std::function<void ()> &cb, const std::function<void ()> &rs){
    if ( locker ){
        if( !locker->reserved() ){
            delete locker;
            return;
        }
    }

    if ( workerThread() ){
        workerThread()->postWork(cb, rs, locker);
    } else {
        cb();
        rs();
        delete locker;
    }
}

void Act::componentComplete(){
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
