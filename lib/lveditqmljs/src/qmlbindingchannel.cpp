#include "qmlbindingchannel.h"
#include "live/codepalette.h"
#include "live/qmleditfragment.h"
#include "live/runnable.h"
#include "live/hookcontainer.h"
#include "documentqmlinfo.h"
#include "qmlbuilder.h"
#include "documentqmlchannels.h"

#include <QQmlContext>

namespace lv{

QmlBindingChannel::Ptr QmlBindingChannel::create(QmlBindingPath::Ptr bindingPath, Runnable *runnable, QObject *object){
    QmlBindingChannel::Ptr bc = QmlBindingChannel::Ptr(new QmlBindingChannel(bindingPath, runnable));
    bc->m_object = object;
    return bc;
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
    , m_object(nullptr)
    , m_listIndex(-1)
    , m_enabled(false)
    , m_isBuilder(false)
{
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

    QmlBindingPath::ComponentNode* cn = static_cast<QmlBindingPath::ComponentNode*>(m_bindingPath->lastNode());
    if ( !cn ){
        THROW_EXCEPTION(lv::Exception, "Failed to capture builder node in binding path.", Exception::toCode("~Builder"));
    }

    QString builderId = cn->name;
    QList<QObject*> builders = hooks->entriesFor(file, builderId);

    for ( QObject* ob : builders ){
        QmlBuilder* builder = qobject_cast<QmlBuilder*>(ob);
        if ( builder ){
            builder->rebuild();
        }
    }
}

QObject *QmlBindingChannel::object() const{
    if ( m_object )
        return m_object;

    if ( type() == QmlBindingChannel::Property ){
        return qobject_cast<QObject*>(m_property.read().value<QObject*>());
    }
    if ( type() != QmlBindingChannel::ListIndex )
        return nullptr;

    QQmlListReference ppref = qvariant_cast<QQmlListReference>(m_property.read());
    QObject* parent = ppref.count() > 0 ? ppref.at(0)->parent() : ppref.object();

    // create correct order for list reference
    QObjectList ordered;
    for (auto child: parent->children())
    {
        bool found = false;
        for (int i = 0; i < ppref.count(); ++i)
            if (child == ppref.at(i)){
                found = true;
                break;
            }
        if (found) ordered.push_back(child);
    }

    if ( listIndex() >= ordered.length() ){
        THROW_EXCEPTION(Exception, "Failed to find object in index: " + QString::number(listIndex()).toStdString() + " where list size is " + QString::number(ordered.length()).toStdString(), Exception::toCode("~Index"));
    }

    return ordered[listIndex()];
}


}// namespace
