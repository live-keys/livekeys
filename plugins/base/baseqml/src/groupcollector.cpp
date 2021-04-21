#include "groupcollector.h"
#include "live/group.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/exception.h"

#include <QQmlProperty>

namespace lv{

GroupCollector::GroupCollector(QObject *parent)
    : QObject(parent)
    , m_isComponentComplete(false)
    , m_source(nullptr)
    , m_current(nullptr)
    , m_output(nullptr)
{
}

GroupCollector::~GroupCollector(){
    clearEntries();
}

void GroupCollector::componentComplete(){
    m_isComponentComplete = true;
    emit complete();
}

void GroupCollector::reset(){
    if ( !m_source )
        return;

    QObject* obj = m_source->create(m_source->creationContext());
    if ( !obj ){
        Exception e = CREATE_EXCEPTION(
            lv::Exception, "Failed to create source component.", Exception::toCode("component")
        );
        ViewContext::instance().engine()->throwError(&e, this);
    }

    Group* group = qobject_cast<Group*>(obj);
    if ( !group ){
        Exception e = CREATE_EXCEPTION(
            lv::Exception, "Component not of group type.", Exception::toCode("~Group")
        );
        ViewContext::instance().engine()->throwError(&e, this);
    }

    clearEntries();

    const QMetaObject *meta = group->metaObject();

    for (int i = 0; i < meta->propertyCount(); i++){
        QMetaProperty property = meta->property(i);
        QByteArray name = property.name();
        if ( name != "objectName" ){

            QQmlProperty pp(group, name);
            if ( pp.hasNotifySignal() ){
                auto entry = new GroupCollectorEntry(name, this);
                m_properties.append(entry);
                pp.connectNotifySignal(entry, SLOT(propertyChanged()));
            }
        }
    }

    m_current = group;
    emit currentChanged();
}

void GroupCollector::release(){
    m_output = m_current;
    m_current = nullptr;

    reset();

    emit outputChanged();
}

void GroupCollector::clearEntries(){
    for ( auto it = m_properties.begin(); it != m_properties.end(); ++it )
        delete *it;
    m_properties.clear();
}

void GroupCollector::groupPropertyChanged(GroupCollectorEntry *){
    bool allPropertiesChanged = true;
    for ( auto it = m_properties.begin(); it != m_properties.end(); ++it ){
        GroupCollectorEntry* gce = *it;
        if ( !gce->changed() ){
            allPropertiesChanged = false;
            return;
        }
    }

    if ( allPropertiesChanged ){
        release();
    }
}

}// namespace
