#include "qmlbindingchannel.h"
#include "live/codepalette.h"
#include "live/qmleditfragment.h"
#include "live/runnable.h"
#include "live/hookcontainer.h"
#include "documentqmlinfo.h"
#include "qmlbuilder.h"

#include <QQmlContext>

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

QmlBindingChannel::Ptr QmlBindingChannel::create(
        QmlBindingPath::Ptr bindingPath,
        Runnable *runnable,
        const QQmlProperty &property,
        const QMetaMethod &method)
{
    QmlBindingChannel::Ptr bc = QmlBindingChannel::create(bindingPath, runnable);
    bc->updateConnection(property, method);
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

void QmlBindingChannel::rebuild(){
    HookContainer* hooks = qobject_cast<HookContainer*>(
        m_runnable->viewContext()->contextProperty("hooks").value<QObject*>()
    );

    if ( !hooks )
        return;

    QString file = m_bindingPath->rootFile();
    QmlBindingPath::ComponentNode* cn = (QmlBindingPath::ComponentNode*)m_bindingPath->root()->child;
    QString builderId = cn->name;

    QList<QObject*> builders = hooks->entriesFor(file, builderId);

    for ( QObject* ob : builders ){
        QmlBuilder* builder = qobject_cast<QmlBuilder*>(ob);
        if ( builder ){
            builder->rebuild();
        }
    }
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
