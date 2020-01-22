#include "qmlbindingchannel.h"
#include "live/codepalette.h"
#include "live/qmleditfragment.h"
#include "live/runnable.h"
#include "documentqmlinfo.h"

namespace lv{

QmlBindingChannel::Ptr QmlBindingChannel::create(QmlBindingPath::Ptr bindingPath, Runnable *runnable){
    return QmlBindingChannel::Ptr(new QmlBindingChannel(bindingPath, runnable));
}

QmlBindingChannel::Ptr QmlBindingChannel::create(
        QmlBindingPath::Ptr bindingPath, Runnable *runnable, const QQmlProperty &property, int listIndex)
{
    QmlBindingChannel::Ptr bc = QmlBindingChannel::create(bindingPath, runnable);
    bc->updateConnection(property, listIndex);
    return bc;
}

QmlBindingChannel::QmlBindingChannel(QmlBindingPath::Ptr bindingPath, Runnable *runnable, QObject *parent)
    : QObject(parent)
    , m_bindingPath(bindingPath)
    , m_runnable(runnable)
    , m_listIndex(-1)
    , m_enabled(false)
{
    connect(runnable, &Runnable::objectReady, this, &QmlBindingChannel::__runableReady);
}

QmlBindingChannel::~QmlBindingChannel(){
}

void QmlBindingChannel::__runableReady(){
    QmlBindingChannel::Ptr bc = DocumentQmlInfo::traverseBindingPath(m_bindingPath, m_runnable);
    if ( bc ){
        m_property = bc->m_property;
        m_listIndex = bc->m_listIndex;
    } else {
        m_property = QQmlProperty();
        m_listIndex = -1;
    }

    emit runnableObjectReady();
}


}// namespace
