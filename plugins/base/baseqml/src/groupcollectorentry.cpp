#include "groupcollectorentry.h"
#include "groupcollector.h"

namespace lv{

GroupCollectorEntry::GroupCollectorEntry(const QString &property, GroupCollector *parent)
    : QObject(parent)
    , m_propertyName(property)
    , m_changed(false)
{
}

GroupCollectorEntry::~GroupCollectorEntry(){
}

GroupCollector *GroupCollectorEntry::groupCollector(){
    return qobject_cast<GroupCollector*>(parent());
}

void GroupCollectorEntry::propertyChanged(){
    m_changed = true;
    groupCollector()->groupPropertyChanged(this);
}

}// namespace
