#include "lvaddcontainer.h"

namespace lv{

LvAddContainer::LvAddContainer(int addPosition, const QStringList &objectType, QObject *parent)
    : QObject(parent)
    , m_objectTypePath(objectType)
    , m_componentModel(new LvComponentModel(addPosition, this))
    , m_propertyModel(new LvPropertyModel(addPosition, this))
{
}

LvAddContainer::~LvAddContainer(){
    delete m_propertyModel;
    delete m_componentModel;
}

} // namespace
