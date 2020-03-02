#include "qmladdcontainer.h"

namespace lv{

QmlAddContainer::QmlAddContainer(int addPosition, const QStringList &objectType, QObject *parent)
    : QObject(parent)
    , m_objectTypePath(objectType)
    , m_itemModel(new QmlItemModel(addPosition, this))
    , m_propertyModel(new QmlPropertyModel(addPosition, this))
    , m_eventModel(new QmlEventModel(addPosition, this))
{
}

QmlAddContainer::~QmlAddContainer(){
    delete m_propertyModel;
    delete m_itemModel;
    delete m_eventModel;
}

} // namespace
