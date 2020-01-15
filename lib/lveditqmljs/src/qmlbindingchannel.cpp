#include "qmlbindingchannel.h"
#include "live/codepalette.h"
#include "live/qmleditfragment.h"

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
}

QmlBindingChannel::~QmlBindingChannel(){
}


}// namespace
